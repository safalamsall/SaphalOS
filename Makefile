# ==============================================================================
# SAPHAL OS - Master Build Makefile
# Project: SAPHAL OS (Security-Aware Platform for Hardened Applications and Layered OS)
# Targets: Tier 1 QEMU Micro-Core & Tier 2 Smartphone Platform
# ==============================================================================

CC      := gcc
LD      := ld
NASM    := nasm

CFLAGS  := -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -O2 -Wall -Wextra -I.
LDFLAGS := -m elf_i386 -T kernel/linker.ld

BUILD_DIR := build
IMG       := $(BUILD_DIR)/saphal_os.img
INITRAMFS := $(BUILD_DIR)/initramfs.cpio.gz
BOOTIMG   := $(BUILD_DIR)/saphal_boot.img

DTB       := $(BUILD_DIR)/cattail.dtb

C_SRCS := \
	kernel/kernel.c \
	kernel/idt.c \
	kernel/pic.c \
	drivers/vga.c \
	drivers/serial.c \
	drivers/timer.c \
	drivers/keyboard.c \
	security/security.c \
	userspace/shell.c

C_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRCS))

ASM_OBJS := \
	$(BUILD_DIR)/boot/stage2_entry.o \
	$(BUILD_DIR)/kernel/idt_asm.o

.PHONY: all clean run test build run-headless userspace bootimg test-userspace test-all

all: build userspace bootimg

build: $(IMG)

$(BUILD_DIR)/boot/boot.bin: boot/boot.asm
	@mkdir -p $(BUILD_DIR)/boot
	$(NASM) -f bin $< -o $@

$(BUILD_DIR)/boot/stage2_entry.o: boot/stage2_entry.asm
	@mkdir -p $(BUILD_DIR)/boot
	$(NASM) -f elf32 $< -o $@

$(BUILD_DIR)/kernel/idt_asm.o: kernel/idt_asm.asm
	@mkdir -p $(BUILD_DIR)/kernel
	$(NASM) -f elf32 $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.bin: $(ASM_OBJS) $(C_OBJS) kernel/linker.ld
	$(LD) $(LDFLAGS) $(ASM_OBJS) $(C_OBJS) -o $@

$(IMG): $(BUILD_DIR)/boot/boot.bin $(BUILD_DIR)/kernel.bin
	cat $(BUILD_DIR)/boot/boot.bin $(BUILD_DIR)/kernel.bin > $@
	truncate -s 10M $@
	@echo "================================================="
	@echo "SAPHAL OS Disk Image Generated: $@"
	@echo "================================================="

userspace: $(INITRAMFS)

$(INITRAMFS): services/init.c services/secd.c ui/framebuffer.c ui/splash.c userspace/diagnostics.c userspace/cli/saphal_sh.c
	./scripts/build_initramfs.sh

$(DTB): arch/arm64/boot/dts/cattail.dts
	@mkdir -p $(BUILD_DIR)
	./tools/dtc/usr/bin/dtc -I dts -O dtb -o $@ $<

bootimg: $(BOOTIMG)

$(BOOTIMG): $(IMG) $(INITRAMFS) $(DTB)
	python3 scripts/generate_bootimg.py $(BUILD_DIR)/kernel.bin $(INITRAMFS) $@ $(DTB) "console=tty0 boot=saphal-init enforcing=1"

run: $(IMG)
	tools/qemu.sh -drive format=raw,file=$(IMG),index=0,media=disk -serial stdio

run-headless: $(IMG)
	tools/qemu.sh -drive format=raw,file=$(IMG),index=0,media=disk -serial stdio -display none

test: $(IMG)
	./scripts/test_boot.sh

test-userspace:
	./scripts/test_userspace.sh

test-all: test test-userspace
	@echo "================================================="
	@echo "  ALL TIER 1 & TIER 2 SYSTEM TESTS PASSED!"
	@echo "================================================="

clean:
	rm -rf $(BUILD_DIR)
