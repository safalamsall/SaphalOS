#ifndef SECURITY_SECURITY_H
#define SECURITY_SECURITY_H

#include <stdint.h>

#define SEC_POLICY_ENFORCING  1
#define SEC_POLICY_PERMISSIVE 0

#define CANARY_VALUE 0xDEADBEEF

typedef struct {
    uint32_t policy_mode;
    uint32_t ring_isolation_active;
    uint32_t stack_canary;
    uint32_t memory_integrity_ok;
    uint32_t audited_syscalls;
} sec_status_t;

void sec_init(void);
int  sec_verify_integrity(void);
void sec_audit(void);
sec_status_t* sec_get_status(void);

#endif /* SECURITY_SECURITY_H */
