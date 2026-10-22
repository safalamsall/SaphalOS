#define _GNU_SOURCE
#include "init.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

static volatile int running = 1;

static void sig_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM || sig == SIGPWR) {
        printf("[SAPHAL-INIT] Received signal %d. System halting...\n", sig);
        running = 0;
    }
}

void init_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGPWR, &sa, NULL);

    // Ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);
}

void reap_zombies(void) {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        // Child reaped
    }
}

static int safe_mount(const char* src, const char* target, const char* fstype, unsigned long flags, const void* data) {
    mkdir(target, 0755);
    if (mount(src, target, fstype, flags, data) != 0) {
        if (errno != EBUSY) {
            fprintf(stderr, "[SAPHAL-INIT] Failed to mount %s on %s: %s\n", src, target, strerror(errno));
            return -1;
        }
    }
    return 0;
}

int mount_essential_filesystems(void) {
    printf("[SAPHAL-INIT] Mounting hardened virtual filesystems...\n");

    // /proc (procfs)
    safe_mount("proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC, NULL);

    // /sys (sysfs)
    safe_mount("sysfs", "/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC, NULL);

    // /dev (devtmpfs)
    safe_mount("devtmpfs", "/dev", "devtmpfs", MS_NOSUID | MS_NOEXEC, "mode=0755");

    // /dev/pts (devpts)
    mkdir("/dev/pts", 0755);
    safe_mount("devpts", "/dev/pts", "devpts", MS_NOSUID | MS_NOEXEC, "gid=5,mode=620");

    // /tmp (tmpfs) - hardened: noexec, nosuid, nodev
    safe_mount("tmpfs", "/tmp", "tmpfs", MS_NOSUID | MS_NODEV | MS_NOEXEC, "size=64m,mode=1777");

    // /run (tmpfs) - hardened: noexec, nosuid, nodev
    safe_mount("tmpfs", "/run", "tmpfs", MS_NOSUID | MS_NODEV | MS_NOEXEC, "size=32m,mode=0755");

    printf("[SAPHAL-INIT] Hardened virtual filesystems successfully mounted.\n");
    return 0;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Check if running as PID 1
    if (getpid() != 1) {
        printf("[SAPHAL-INIT] Warning: Not running as PID 1 (current PID: %d).\n", getpid());
        printf("[SAPHAL-INIT] Executing in standalone verification mode.\n");
    }

    printf("\n");
    printf("=================================================================\n");
    printf("  SAPHAL OS System Initializer (PID 1) - Release %s\n", SAPHAL_OS_RELEASE);
    printf("  Security-Aware Platform for Hardened Applications and Layered OS\n");
    printf("  Independent Non-Android Architecture\n");
    printf("=================================================================\n\n");

    // Set hostname
    sethostname(SAPHAL_HOSTNAME, strlen(SAPHAL_HOSTNAME));
    printf("[SAPHAL-INIT] Hostname configured: %s\n", SAPHAL_HOSTNAME);

    // Configure signal handling
    init_signals();

    // Mount virtual filesystems if running as PID 1
    if (getpid() == 1) {
        mount_essential_filesystems();
    }

    printf("[SAPHAL-INIT] Core subsystem initialization sequence complete.\n");
    printf("[SAPHAL-INIT] System status: READY\n");

    // If running in test harness, exit cleanly
    if (getenv("SAPHAL_TEST_MODE")) {
        printf("[SAPHAL-INIT] Test mode detected. Exiting cleanly.\n");
        return 0;
    }

    // Main supervisor loop
    while (running) {
        reap_zombies();
        sleep(1);
    }

    printf("[SAPHAL-INIT] System shutdown requested.\n");
    return 0;
}
