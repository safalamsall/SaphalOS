#ifndef SERVICES_AUDIO_H
#define SERVICES_AUDIO_H

#define DEFAULT_BOOT_SOUND "/etc/saphal/boot_sound.mp3"

void play_boot_sound_async(const char* sound_path);

#endif /* SERVICES_AUDIO_H */
