#include "gic.h"

static inline void write_gicd(uint32_t offset, uint32_t val) {
    volatile uint32_t* reg = (volatile uint32_t*)(GICD_BASE + offset);
    *reg = val;
}

static inline void write_gicc(uint32_t offset, uint32_t val) {
    volatile uint32_t* reg = (volatile uint32_t*)(GICC_BASE + offset);
    *reg = val;
}

void gic_init(void) {
    /* 1. Disable Distributor while configuring */
    write_gicd(GICD_CTLR, 0x00);

    /* 2. Priority Mask Register: Accept all interrupts (lowest priority 0xFF) */
    write_gicc(GICC_PMR, 0xFF);

    /* 3. Enable CPU Interface */
    write_gicc(GICC_CTLR, 0x01);

    /* 4. Enable Distributor */
    write_gicd(GICD_CTLR, 0x01);
}
