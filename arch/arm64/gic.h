#ifndef ARCH_ARM64_GIC_H
#define ARCH_ARM64_GIC_H

#include <stdint.h>

/* MediaTek MT6769 GIC Base Addresses */
#define GICD_BASE 0x0C000000    /* GIC Distributor */
#define GICC_BASE 0x0C010000    /* GIC CPU Interface */

#define GICD_CTLR       0x000
#define GICD_TYPER      0x004
#define GICD_ISENABLER  0x100
#define GICD_ICENABLER  0x180
#define GICD_IPRIORITYR 0x400

#define GICC_CTLR       0x000
#define GICC_PMR        0x004
#define GICC_IAR        0x00C
#define GICC_EOIR       0x010

void gic_init(void);

#endif /* ARCH_ARM64_GIC_H */
