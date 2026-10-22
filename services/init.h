#ifndef SERVICES_INIT_H
#define SERVICES_INIT_H

#define SAPHAL_OS_RELEASE "0.1.0-alpha"
#define SAPHAL_HOSTNAME   "saphal-os"

int mount_essential_filesystems(void);
void init_signals(void);
void reap_zombies(void);

#endif /* SERVICES_INIT_H */
