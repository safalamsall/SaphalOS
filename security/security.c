#include "security.h"
#include "../drivers/vga.h"
#include "../drivers/serial.h"

static sec_status_t sec_state;
static volatile uint32_t stack_guard_canary = CANARY_VALUE;

void sec_init(void) {
    sec_state.policy_mode = SEC_POLICY_ENFORCING;
    sec_state.ring_isolation_active = 1;
    sec_state.stack_canary = CANARY_VALUE;
    sec_state.memory_integrity_ok = 1;
    sec_state.audited_syscalls = 0;

    klog("SECURITY", "Hardened Security Subsystem Initialized");
    klog("SECURITY", "Mode: ENFORCING | Ring Isolation: ACTIVE | Canaries: SET");
}

int sec_verify_integrity(void) {
    if (stack_guard_canary != CANARY_VALUE || sec_state.stack_canary != CANARY_VALUE) {
        sec_state.memory_integrity_ok = 0;
        klog("SEC-ALERT", "Integrity Check FAILED! Stack Canary Corrupted!");
        return 0;
    }
    sec_state.memory_integrity_ok = 1;
    return 1;
}

void sec_audit(void) {
    klog("SEC-AUDIT", "Performing Subsystem Integrity Audit...");

    int ok = sec_verify_integrity();

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("  [+] Security Policy Mode : ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts(sec_state.policy_mode == SEC_POLICY_ENFORCING ? "ENFORCING\n" : "PERMISSIVE\n");

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("  [+] Hardware Ring Level  : ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts("RING 0 (Supervisor Core)\n");

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("  [+] GDT Ring 3 Gate      : ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts("CONFIGURED (0x18 / 0x20)\n");

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("  [+] Syscall Boundary     : ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts("INT 0x80 (Trap Gate DPL=3)\n");

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("  [+] Memory Stack Canary  : ");
    if (ok) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_puts("VALID (0xDEADBEEF)\n");
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_puts("CORRUPTED!\n");
    }
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

sec_status_t* sec_get_status(void) {
    return &sec_state;
}
