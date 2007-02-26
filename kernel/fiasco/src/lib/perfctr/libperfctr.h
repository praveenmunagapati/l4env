/* $Id$
 * Library interface to Linux x86 Performance-Monitoring Counters.
 *
 * Copyright (C) 1999-2003  Mikael Pettersson
 */

#ifndef __LIB_PERFCTR_H
#define __LIB_PERFCTR_H

#define PERFCTR_X86_GENERIC	0	/* any x86 with rdtsc */
#define PERFCTR_X86_INTEL_P5	1	/* no rdpmc */
#define PERFCTR_X86_INTEL_P5MMX	2
#define PERFCTR_X86_INTEL_P6	3
#define PERFCTR_X86_INTEL_PII	4
#define PERFCTR_X86_INTEL_PIII	5
#define PERFCTR_X86_CYRIX_MII	6
#define PERFCTR_X86_WINCHIP_C6	7	/* no rdtsc */
#define PERFCTR_X86_WINCHIP_2	8	/* no rdtsc */
#define PERFCTR_X86_AMD_K7	9
#define PERFCTR_X86_VIA_C3	10	/* no pmc0 */
#define PERFCTR_X86_INTEL_P4	11	/* model 0 and 1 */
#define PERFCTR_X86_INTEL_P4M2	12	/* model 2 and above */
#define PERFCTR_X86_AMD_K8	13

/*
 * Descriptions of the events available for different processor types.
 */

enum perfctr_unit_mask_type {
    perfctr_um_type_fixed,	/* one fixed (required) value */
    perfctr_um_type_exclusive,	/* exactly one of N values */
    perfctr_um_type_bitmask,	/* bitwise 'or' of N power-of-2 values */
};

struct perfctr_unit_mask_value {
    unsigned int value;
    const char *description;	/* [NAME:]text */
};

struct perfctr_unit_mask {
    unsigned int default_value;
    enum perfctr_unit_mask_type type:16;
    unsigned short nvalues;
    struct perfctr_unit_mask_value values[1/*nvalues*/];
};

struct perfctr_event {
    unsigned int evntsel;
    unsigned int counters_set; /* P4 force this to be CPU-specific */
    const struct perfctr_unit_mask *unit_mask;
    const char *name;
    const char *description;
};

struct perfctr_event_set {
    unsigned int cpu_type;
    const char *event_prefix;
    const struct perfctr_event_set *include;
    unsigned int nevents;
    const struct perfctr_event *events;
};

#endif /* __LIB_PERFCTR_H */
