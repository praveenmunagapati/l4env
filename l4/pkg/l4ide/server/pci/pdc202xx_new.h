#ifndef PDC202XX_H
#define PDC202XX_H

#include <linux/config.h>
#include <linux/pci.h>
#include <linux/ide.h>

const static char *pdc_quirk_drives[] = {
	"QUANTUM FIREBALLlct08 08",
	"QUANTUM FIREBALLP KA6.4",
	"QUANTUM FIREBALLP KA9.1",
	"QUANTUM FIREBALLP LM20.4",
	"QUANTUM FIREBALLP KX13.6",
	"QUANTUM FIREBALLP KX20.5",
	"QUANTUM FIREBALLP KX27.3",
	"QUANTUM FIREBALLP LM20.5",
	NULL
};

#define set_2regs(a, b)					\
	do {						\
		hwif->OUTB((a + adj), indexreg);	\
		hwif->OUTB(b, datareg);			\
	} while(0)

#define set_ultra(a, b, c)				\
	do {						\
		set_2regs(0x10,(a));			\
		set_2regs(0x11,(b));			\
		set_2regs(0x12,(c));			\
	} while(0)

#define set_ata2(a, b)					\
	do {						\
		set_2regs(0x0e,(a));			\
		set_2regs(0x0f,(b));			\
	} while(0)

#define set_pio(a, b, c)				\
	do { 						\
		set_2regs(0x0c,(a));			\
		set_2regs(0x0d,(b));			\
		set_2regs(0x13,(c));			\
	} while(0)

#define DISPLAY_PDC202XX_TIMINGS

static void init_setup_pdcnew(struct pci_dev *, ide_pci_device_t *);
static void init_setup_pdc20270(struct pci_dev *, ide_pci_device_t *);
static void init_setup_pdc20276(struct pci_dev *dev, ide_pci_device_t *d);
static unsigned int init_chipset_pdcnew(struct pci_dev *, const char *);
static void init_hwif_pdc202new(ide_hwif_t *);

static ide_pci_device_t pdcnew_chipsets[] __devinitdata = {
	{	/* 0 */
		.vendor		= PCI_VENDOR_ID_PROMISE,
		.device		= PCI_DEVICE_ID_PROMISE_20268,
		.name		= "PDC20268",
		.init_setup	= init_setup_pdcnew,
		.init_chipset	= init_chipset_pdcnew,
		.init_iops	= NULL,
		.init_hwif	= init_hwif_pdc202new,
		.channels	= 2,
		.autodma	= AUTODMA,
		.enablebits	= {{0x00,0x00,0x00}, {0x00,0x00,0x00}},
		.bootable	= OFF_BOARD,
		.extra		= 0,
	},{	/* 1 */
		.vendor		= PCI_VENDOR_ID_PROMISE,
		.device		= PCI_DEVICE_ID_PROMISE_20269,
		.name		= "PDC20269",
		.init_setup	= init_setup_pdcnew,
		.init_chipset	= init_chipset_pdcnew,
		.init_iops	= NULL,
		.init_hwif	= init_hwif_pdc202new,
		.channels	= 2,
		.autodma	= AUTODMA,
		.enablebits	= {{0x00,0x00,0x00}, {0x00,0x00,0x00}},
		.bootable	= OFF_BOARD,
		.extra		= 0,
	},{	/* 2 */
		.vendor		= PCI_VENDOR_ID_PROMISE,
		.device		= PCI_DEVICE_ID_PROMISE_20270,
		.name		= "PDC20270",
		.init_setup	= init_setup_pdc20270,
		.init_chipset	= init_chipset_pdcnew,
		.init_iops	= NULL,
		.init_hwif	= init_hwif_pdc202new,
		.channels	= 2,
		.autodma	= AUTODMA,
#ifdef CONFIG_PDC202XX_FORCE
		.enablebits	= {{0x00,0x00,0x00}, {0x00,0x00,0x00}},
#else /* !CONFIG_PDC202XX_FORCE */
		.enablebits	= {{0x50,0x02,0x02}, {0x50,0x04,0x04}},
#endif
		.bootable	= OFF_BOARD,
		.extra		= 0,
	},{	/* 3 */
		.vendor		= PCI_VENDOR_ID_PROMISE,
		.device		= PCI_DEVICE_ID_PROMISE_20271,
		.name		= "PDC20271",
		.init_setup	= init_setup_pdcnew,
		.init_chipset	= init_chipset_pdcnew,
		.init_iops	= NULL,
		.init_hwif	= init_hwif_pdc202new,
		.channels	= 2,
		.autodma	= AUTODMA,
		.enablebits	= {{0x00,0x00,0x00}, {0x00,0x00,0x00}},
		.bootable	= OFF_BOARD,
		.extra		= 0,
	},{	/* 4 */
		.vendor		= PCI_VENDOR_ID_PROMISE,
		.device		= PCI_DEVICE_ID_PROMISE_20275,
		.name		= "PDC20275",
		.init_setup	= init_setup_pdcnew,
		.init_chipset	= init_chipset_pdcnew,
		.init_iops	= NULL,
		.init_hwif	= init_hwif_pdc202new,
		.channels	= 2,
		.autodma	= AUTODMA,
		.enablebits	= {{0x00,0x00,0x00}, {0x00,0x00,0x00}},
		.bootable	= OFF_BOARD,
		.extra		= 0,
	},{	/* 5 */
		.vendor		= PCI_VENDOR_ID_PROMISE,
		.device		= PCI_DEVICE_ID_PROMISE_20276,
		.name		= "PDC20276",
		.init_setup	= init_setup_pdc20276,
		.init_chipset	= init_chipset_pdcnew,
		.init_iops	= NULL,
		.init_hwif	= init_hwif_pdc202new,
		.channels	= 2,
		.autodma	= AUTODMA,
#ifdef CONFIG_PDC202XX_FORCE
		.enablebits	= {{0x00,0x00,0x00}, {0x00,0x00,0x00}},
#else /* !CONFIG_PDC202XX_FORCE */
		.enablebits	= {{0x50,0x02,0x02}, {0x50,0x04,0x04}},
#endif
		.bootable	= OFF_BOARD,
		.extra		= 0,
	},{	/* 6 */
		.vendor		= PCI_VENDOR_ID_PROMISE,
		.device		= PCI_DEVICE_ID_PROMISE_20277,
		.name		= "PDC20277",
		.init_setup	= init_setup_pdcnew,
		.init_chipset	= init_chipset_pdcnew,
		.init_iops	= NULL,
		.init_hwif	= init_hwif_pdc202new,
		.channels	= 2,
		.autodma	= AUTODMA,
		.enablebits	= {{0x00,0x00,0x00}, {0x00,0x00,0x00}},
		.bootable	= OFF_BOARD,
		.extra		= 0,
	},{
		.vendor		= 0,
		.device		= 0,
		.channels	= 0,
		.bootable	= EOL,
	}
};

#endif /* PDC202XX_H */
