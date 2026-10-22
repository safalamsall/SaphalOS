#ifndef SERVICES_SECD_H
#define SERVICES_SECD_H

int sec_audit_mounts(void);
int sec_audit_cmdline(void);
int sec_audit_procs(void);
void sec_run_full_audit(void);

#endif /* SERVICES_SECD_H */
