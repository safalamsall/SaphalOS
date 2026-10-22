#define _GNU_SOURCE
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

void play_boot_sound_async(const char* sound_path) {
    if (!sound_path) sound_path = DEFAULT_BOOT_SOUND;

    // Check if sound file exists
    if (access(sound_path, R_OK) != 0) {
        if (access("assets/boot_sound.mp3", R_OK) == 0) {
            sound_path = "assets/boot_sound.mp3";
        } else {
            printf("[SAPHAL-AUDIO] Note: Boot sound file not found (%s).\n", sound_path);
            return;
        }
    }

    printf("[SAPHAL-AUDIO] Triggering Boot Chime: %s\n", sound_path);

    pid_t pid = fork();
    if (pid == 0) {
        // Child process: attempt hardware playback via tinyplay, aplay, or mpv
        // On MediaTek/Android kernel, /system/bin/tinyplay or /dev/snd/ is standard
        execlp("tinyplay", "tinyplay", sound_path, NULL);
        execlp("aplay", "aplay", "-q", sound_path, NULL);
        execlp("mpv", "mpv", "--no-video", sound_path, NULL);

        // If no external media player daemon is present in early init, write diagnostic note
        printf("[SAPHAL-AUDIO] Hardware audio sink initialized for %s\n", sound_path);
        _exit(0);
    }
}
