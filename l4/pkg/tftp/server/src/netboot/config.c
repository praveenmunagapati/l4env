/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */

#include	"../tftp_config.h"
#include	"grub.h"
#include	"pci.h"
#include	"isa.h"
#include	"nic.h"

#ifdef CONFIG_PCI
static int pci_probe(struct dev *dev, const char *type_name)
{
/*
 *	NIC probing is in pci device order, followed by the 
 *      link order of the drivers.  A driver that matches 
 *      on vendor and device id will supersede a driver
 *      that matches on pci class.
 *
 *	If you want to probe for another device behind the same pci
 *      device just increment index.  And the previous probe call
 *      will be repeated.
 */
	struct pci_probe_state *state = &dev->state.pci;
//	printf("Probing pci %s...\n", type_name);
	if (dev->how_probe == PROBE_FIRST) {
		state->advance    = 1;
		state->dev.driver = 0;
		state->dev.bus    = 0;
		state->dev.devfn  = 0;
		dev->index        = -1;
	}
	for(;;) {
		if ((dev->how_probe != PROBE_AWAKE) && state->advance) {
			find_pci(dev->type, &state->dev);
			dev->index = -1;
		}
		state->advance = 1;
		
		if (state->dev.driver == 0)
			break;
		
#if 0
		/* FIXME the romaddr code needs a total rethought to be useful */
		if (state->dev.romaddr != ((unsigned long) rom.rom_segment << 4)) {
			continue;
		}
#endif
		if (dev->how_probe != PROBE_AWAKE) {
			dev->type_index++;
		}
		dev->devid.bus_type = PCI_BUS_TYPE;
		dev->devid.vendor_id = htons(state->dev.vendor);
		dev->devid.device_id = htons(state->dev.dev_id);
		/* FIXME how do I handle dev->index + PROBE_AGAIN?? */
		
		printf("Found %s\n", state->dev.name);
		if (state->dev.driver->probe(dev, &state->dev)) {
			state->advance = (dev->index == -1);
			return PROBE_WORKED;
		}
		putchar('\n');
	}
	return PROBE_FAILED;
}
#else
#define pci_probe(d,tn) (PROBE_FAILED)
#endif

#ifdef CONFIG_ISA
static int isa_probe(struct dev *dev, const char *type_name)
{
/*
 *	NIC probing is in the order the drivers were linked togeter.
 *	If for some reason you want to change the order,
 *	just change the order you list the drivers in.
 */
	struct isa_probe_state *state = &dev->state.isa;
//	printf("Probing isa %s...\n", type_name);
	if (dev->how_probe == PROBE_FIRST) {
		state->advance = 0;
		state->driver  = isa_drivers;
		dev->index     = -1;
	}
	for(;;)
	{
		if ((dev->how_probe != PROBE_AWAKE) && state->advance) {
			state->driver++;
			dev->index = -1;
		}
		state->advance = 1;
		
		if (state->driver >= isa_drivers_end)
			break;

		if (state->driver->type != dev->type)
			continue;

		if (dev->how_probe != PROBE_AWAKE) {
			dev->type_index++;
		}
		printf("[%s]", state->driver->name);
		dev->devid.bus_type = ISA_BUS_TYPE;
		/* FIXME how do I handle dev->index + PROBE_AGAIN?? */
		/* driver will fill in vendor and device IDs */
		if (state->driver->probe(dev, state->driver->ioaddrs)) {
			state->advance = (dev->index == -1);
			return PROBE_WORKED;
		}
		putchar('\n');
	}
	return PROBE_FAILED;
}
#else
#define isa_probe(d,tn) (PROBE_FAILED)
#endif
#ifdef CONFIG_OSHKOSH
extern int oshkosh_probe(struct dev *dev, const char *type_name);
#endif

#ifdef CONFIG_ORE
extern int ore_probe(struct dev *dev, const char *typename);
#endif

static const char *driver_name[] = {
	"NIC", 
	"DISK", 
	"FLOPPY",
};
int probe(struct dev *dev)
{
	const char *type_name;

	EnterFunction("probe");

	type_name = "";
	if ((dev->type >= 0) && 
		(dev->type < sizeof(driver_name)/sizeof(driver_name[0]))) {
		type_name = driver_name[dev->type];
	}
#ifdef CONFIG_OSHKOSH
	if (dev->how_probe == PROBE_FIRST) {
		dev->to_probe = PROBE_OSHK;
		memset(&dev->state, 0, sizeof(dev->state));
	}
	if (dev->to_probe == PROBE_OSHK) {
		dev->how_probe = oshkosh_probe(dev, type_name);
		if (dev->how_probe == PROBE_FAILED) {
			dev->to_probe = PROBE_PCI;
		}
	}
#else 
#ifdef CONFIG_ORE
    if (dev->how_probe == PROBE_FIRST) {
        dev->to_probe = PROBE_ORE;
        memset(&dev->state, 0, sizeof(dev->state));
    }
    if (dev->to_probe == PROBE_ORE) {
        dev->how_probe = ore_probe(dev, type_name);
        if (dev->how_probe == PROBE_FAILED) {
            dev->to_probe = PROBE_PCI;
        }
    }
#else
	if (dev->how_probe == PROBE_FIRST) {
		dev->to_probe = PROBE_PCI;
		memset(&dev->state, 0, sizeof(dev->state));
	}
#endif
#endif
	if (dev->to_probe == PROBE_PCI) {
		dev->how_probe = pci_probe(dev, type_name);
		if (dev->how_probe == PROBE_FAILED) {
			dev->to_probe = PROBE_ISA;
		}
	}
	if (dev->to_probe == PROBE_ISA) {
		dev->how_probe = isa_probe(dev, type_name);
		if (dev->how_probe == PROBE_FAILED) {
			dev->to_probe = PROBE_NONE;
		}
	}
	if ((dev->to_probe != PROBE_PCI) &&
		(dev->to_probe != PROBE_ISA) &&
		(dev->to_probe != PROBE_OSHK) &&
		(dev->to_probe != PROBE_ORE)) {
		dev->how_probe = PROBE_FAILED;
	}

	LeaveFunction("probe");
	return dev->how_probe;
}

void disable(struct dev *dev)
{
	if (dev->disable) {
		dev->disable(dev);
		dev->disable = 0;
	}
}