/*
 *  linux/include/linux/mmc/sdhci.h - Secure Digital Host Controller Interface
 *
 *  Copyright (C) 2005-2008 Pierre Ossman, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
#ifndef LINUX_MMC_SDHCI_H
#define LINUX_MMC_SDHCI_H

#include <linux/scatterlist.h>
#include <linux/compiler.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/ratelimit.h>
#include <linux/mmc/host.h>

struct sdhci_next {
	unsigned int sg_count;
	s32 cookie;
};

enum sdhci_power_policy {
	SDHCI_PERFORMANCE_MODE,
	SDHCI_POWER_SAVE_MODE,
	SDHCI_POWER_POLICY_NUM /* Always keep this one last */
};

struct sdhci_host {
	/* Data set by hardware interface driver */
	const char *hw_name;	/* Hardware bus name */

	unsigned int quirks;	/* Deviations from spec. */

/* Controller doesn't honor resets unless we touch the clock register */
#define SDHCI_QUIRK_CLOCK_BEFORE_RESET			(1<<0)
/* Controller has bad caps bits, but really supports DMA */
#define SDHCI_QUIRK_FORCE_DMA				(1<<1)
/* Controller doesn't like to be reset when there is no card inserted. */
#define SDHCI_QUIRK_NO_CARD_NO_RESET			(1<<2)
/* Controller doesn't like clearing the power reg before a change */
#define SDHCI_QUIRK_SINGLE_POWER_WRITE			(1<<3)
/* Controller has flaky internal state so reset it on each ios change */
#define SDHCI_QUIRK_RESET_CMD_DATA_ON_IOS		(1<<4)
/* Controller has an unusable DMA engine */
#define SDHCI_QUIRK_BROKEN_DMA				(1<<5)
/* Controller has an unusable ADMA engine */
#define SDHCI_QUIRK_BROKEN_ADMA				(1<<6)
/* Controller can only DMA from 32-bit aligned addresses */
#define SDHCI_QUIRK_32BIT_DMA_ADDR			(1<<7)
/* Controller can only DMA chunk sizes that are a multiple of 32 bits */
#define SDHCI_QUIRK_32BIT_DMA_SIZE			(1<<8)
/* Controller can only ADMA chunks that are a multiple of 32 bits */
#define SDHCI_QUIRK_32BIT_ADMA_SIZE			(1<<9)
/* Controller needs to be reset after each request to stay stable */
#define SDHCI_QUIRK_RESET_AFTER_REQUEST			(1<<10)
/* Controller needs voltage and power writes to happen separately */
#define SDHCI_QUIRK_NO_SIMULT_VDD_AND_POWER		(1<<11)
/* Controller provides an incorrect timeout value for transfers */
#define SDHCI_QUIRK_BROKEN_TIMEOUT_VAL			(1<<12)
/* Controller has an issue with buffer bits for small transfers */
#define SDHCI_QUIRK_BROKEN_SMALL_PIO			(1<<13)
/* Controller does not provide transfer-complete interrupt when not busy */
#define SDHCI_QUIRK_NO_BUSY_IRQ				(1<<14)
/* Controller has unreliable card detection */
#define SDHCI_QUIRK_BROKEN_CARD_DETECTION		(1<<15)
/* Controller reports inverted write-protect state */
#define SDHCI_QUIRK_INVERTED_WRITE_PROTECT		(1<<16)
/* Controller does not like fast PIO transfers */
#define SDHCI_QUIRK_PIO_NEEDS_DELAY			(1<<18)
/* Controller has to be forced to use block size of 2048 bytes */
#define SDHCI_QUIRK_FORCE_BLK_SZ_2048			(1<<20)
/* Controller cannot do multi-block transfers */
#define SDHCI_QUIRK_NO_MULTIBLOCK			(1<<21)
/* Controller can only handle 1-bit data transfers */
#define SDHCI_QUIRK_FORCE_1_BIT_DATA			(1<<22)
/* Controller needs 10ms delay between applying power and clock */
#define SDHCI_QUIRK_DELAY_AFTER_POWER			(1<<23)
/* Controller uses SDCLK instead of TMCLK for data timeouts */
#define SDHCI_QUIRK_DATA_TIMEOUT_USES_SDCLK		(1<<24)
/* Controller reports wrong base clock capability */
#define SDHCI_QUIRK_CAP_CLOCK_BASE_BROKEN		(1<<25)
/* Controller cannot support End Attribute in NOP ADMA descriptor */
#define SDHCI_QUIRK_NO_ENDATTR_IN_NOPDESC		(1<<26)
/* Controller is missing device caps. Use caps provided by host */
#define SDHCI_QUIRK_MISSING_CAPS			(1<<27)
/* Controller uses Auto CMD12 command to stop the transfer */
#define SDHCI_QUIRK_MULTIBLOCK_READ_ACMD12		(1<<28)
/* Controller doesn't have HISPD bit field in HI-SPEED SD card */
#define SDHCI_QUIRK_NO_HISPD_BIT			(1<<29)
/* Controller treats ADMA descriptors with length 0000h incorrectly */
#define SDHCI_QUIRK_BROKEN_ADMA_ZEROLEN_DESC		(1<<30)
/* The read-only detection via SDHCI_PRESENT_STATE register is unstable */
#define SDHCI_QUIRK_UNSTABLE_RO_DETECT			(1<<31)

	unsigned int quirks2;	/* More deviations from spec. */

#define SDHCI_QUIRK2_HOST_OFF_CARD_ON			(1<<0)
#define SDHCI_QUIRK2_HOST_NO_CMD23			(1<<1)
/* The system physically doesn't support 1.8v, even if the host does */
#define SDHCI_QUIRK2_NO_1_8_V				(1<<2)
#define SDHCI_QUIRK2_PRESET_VALUE_BROKEN		(1<<3)
#define SDHCI_QUIRK2_CARD_ON_NEEDS_BUS_ON		(1<<4)
/* Controller has a non-standard host control register */
#define SDHCI_QUIRK2_BROKEN_HOST_CONTROL		(1<<5)
/* Controller does not support HS200 */
#define SDHCI_QUIRK2_BROKEN_HS200			(1<<6)
/* Controller does not support DDR50 */
#define SDHCI_QUIRK2_BROKEN_DDR50			(1<<7)
/* Stop command (CMD12) can set Transfer Complete when not using MMC_RSP_BUSY */
#define SDHCI_QUIRK2_STOP_WITH_TC			(1<<8)
/*
 * Read Transfer Active/ Write Transfer Active may be not
 * de-asserted after end of transaction. Issue reset for DAT line.
 */
#define SDHCI_QUIRK2_RDWR_TX_ACTIVE_EOT			(1<<9)
/*
 * Slow interrupt clearance at 400KHz may cause
 * host controller driver interrupt handler to
 * be called twice.
 */
#define SDHCI_QUIRK2_SLOW_INT_CLR			(1<<10)
/*
 * If the base clock can be scalable, then there should be no further
 * clock dividing as the input clock itself will be scaled down to
 * required frequency.
 */
#define SDHCI_QUIRK2_ALWAYS_USE_BASE_CLOCK		(1<<11)
/*
 * Ignore data timeout error for R1B commands as there will be no
 * data associated and the busy timeout value for these commands
 * could be lager than the maximum timeout value that controller
 * can handle.
 */
#define SDHCI_QUIRK2_IGNORE_DATATOUT_FOR_R1BCMD		(1<<12)
/*
 * The preset value registers are not properly initialized by
 * some hardware and hence preset value must not be enabled for
 * such controllers.
 */
#define SDHCI_QUIRK2_BROKEN_PRESET_VALUE		(1<<13)
/*
 * Some controllers define the usage of 0xF in data timeout counter
 * register (0x2E) which is actually a reserved bit as per
 * specification.
 */
#define SDHCI_QUIRK2_USE_RESERVED_MAX_TIMEOUT		(1<<14)
/*
 * This is applicable for controllers that advertize timeout clock
 * value in capabilities register (bit 5-0) as just 50MHz whereas the
 * base clock frequency is 200MHz. So, the controller internally
 * multiplies the value in timeout control register by 4 with the
 * assumption that driver always uses fixed timeout clock value from
 * capabilities register to calculate the timeout. But when the driver
 * uses SDHCI_QUIRK2_ALWAYS_USE_BASE_CLOCK base clock frequency is directly
 * controller by driver and it's rate varies upto max. 200MHz. This new quirk
 * will be used in such cases to avoid controller mulplication when timeout is
 * calculated based on the base clock.
 */
#define SDHCI_QUIRK2_DIVIDE_TOUT_BY_4			(1<<15)
/*
 * Some SDHC controllers are unable to handle data-end bit error in
 * 1-bit mode of SDIO.
 */
#define SDHCI_QUIRK2_IGN_DATA_END_BIT_ERROR             (1<<16)

/*
 * Some SDHC controllers do not require data buffers alignment, skip
 * the bounce buffer logic when preparing data
 */
#define SDHCI_QUIRK2_ADMA_SKIP_DATA_ALIGNMENT		(1<<17)

/* Controller has nonstandard clock management */
#define SDHCI_QUIRK_NONSTANDARD_CLOCK			(1<<18)
/* Capability register bit-63 indicates HS400 support */
#define SDHCI_QUIRK2_CAPS_BIT63_FOR_HS400		(1<<19)
/* Use reset workaround in case sdhci reset timeouts */
#define SDHCI_QUIRK2_USE_RESET_WORKAROUND		(1<<20)
/* Some controllers doesn't have have any LED control */
#define SDHCI_QUIRK2_BROKEN_LED_CONTROL			(1<<21)
/*
 * Some controllers doesn't follow the tuning procedure as defined in spec.
 * The tuning data has to be compared from SW driver to validate the correct
 * phase.
 */
#define SDHCI_QUIRK2_NON_STANDARD_TUNING (1 << 22)
/*
 * Some controllers may use PIO mode to workaround HW issues in ADMA for
 * eMMC tuning commands.
 */
#define SDHCI_QUIRK2_USE_PIO_FOR_EMMC_TUNING (1 << 23)

	int irq;		/* Device IRQ */
	void __iomem *ioaddr;	/* Mapped address */

	const struct sdhci_ops *ops;	/* Low level hw interface */

	/* Internal data */
	struct mmc_host *mmc;	/* MMC structure */
	u64 dma_mask;		/* custom DMA mask */
	u64 coherent_dma_mask;

#if defined(CONFIG_LEDS_CLASS) || defined(CONFIG_LEDS_CLASS_MODULE)
	struct led_classdev led;	/* LED control */
	char led_name[32];
#endif

	spinlock_t lock;	/* Mutex */

	int flags;		/* Host attributes */
#define SDHCI_USE_SDMA		(1<<0)	/* Host is SDMA capable */
#define SDHCI_USE_ADMA		(1<<1)	/* Host is ADMA capable */
#define SDHCI_REQ_USE_DMA	(1<<2)	/* Use DMA for this req. */
#define SDHCI_DEVICE_DEAD	(1<<3)	/* Device unresponsive */
#define SDHCI_SDR50_NEEDS_TUNING (1<<4)	/* SDR50 needs tuning */
#define SDHCI_NEEDS_RETUNING	(1<<5)	/* Host needs retuning */
#define SDHCI_AUTO_CMD12	(1<<6)	/* Auto CMD12 support */
#define SDHCI_AUTO_CMD23	(1<<7)	/* Auto CMD23 support */
#define SDHCI_PV_ENABLED	(1<<8)	/* Preset value enabled */
#define SDHCI_SDIO_IRQ_ENABLED	(1<<9)	/* SDIO irq enabled */
#define SDHCI_SDR104_NEEDS_TUNING (1<<10)	/* SDR104/HS200 needs tuning */
#define SDHCI_USING_RETUNING_TIMER (1<<11)	/* Host is using a retuning timer for the card */
#define SDHCI_USE_ADMA_64BIT	 (1<<12)/* Host is 64-bit ADMA capable */
#define SDHCI_HOST_IRQ_STATUS	 (1<<13) /* host->irq status */

	unsigned int version;	/* SDHCI spec. version */

	unsigned int max_clk;	/* Max possible freq (MHz) */
	unsigned int timeout_clk;	/* Timeout freq (KHz) */
	unsigned int clk_mul;	/* Clock Muliplier value */

	unsigned int clock;	/* Current clock (MHz) */
	u8 pwr;			/* Current voltage */

	bool runtime_suspended;	/* Host is runtime suspended */
	bool bus_on;		/* Bus power prevents runtime suspend */
	bool preset_enabled;	/* Preset is enabled */

	struct mmc_request *mrq;	/* Current request */
	struct mmc_command *cmd;	/* Current command */
	struct mmc_data *data;	/* Current data request */
	unsigned int data_early:1;	/* Data finished before cmd */
	unsigned int busy_handle:1;	/* Handling the order of Busy-end */

	struct sg_mapping_iter sg_miter;	/* SG state for PIO */
	unsigned int blocks;	/* remaining PIO blocks */

	int sg_count;		/* Mapped sg entries */

	u8 *adma_desc;		/* ADMA descriptor table */
	u8 *align_buffer;	/* Bounce buffer */

	unsigned int adma_desc_sz; /* ADMA descriptor table size */
	unsigned int adma_desc_line_sz; /* ADMA descriptor line size */
	unsigned int align_buf_sz; /* Bounce buffer size */
	unsigned int align_bytes; /* Alignment bytes (4/8 for 32-bit/64-bit) */
	unsigned int adma_max_desc; /* Max ADMA descriptos (max sg segments) */

	dma_addr_t adma_addr;	/* Mapped ADMA descr. table */
	dma_addr_t align_addr;	/* Mapped bounce buffer */

	struct tasklet_struct finish_tasklet;	/* Tasklet structures */

	struct timer_list timer;	/* Timer for timeouts */

	u32 caps;		/* Alternative CAPABILITY_0 */
	u32 caps1;		/* Alternative CAPABILITY_1 */

	unsigned int            ocr_avail_sdio;	/* OCR bit masks */
	unsigned int            ocr_avail_sd;
	unsigned int            ocr_avail_mmc;
	u32 ocr_mask;		/* available voltages */

	unsigned		timing;		/* Current timing */

	u32			thread_isr;

	/* cached registers */
	u32			ier;

	wait_queue_head_t	buf_ready_int;	/* Waitqueue for Buffer Read Ready interrupt */
	unsigned int		tuning_done;	/* Condition flag set when CMD19 succeeds */

	unsigned int		tuning_count;	/* Timer count for re-tuning */
	unsigned int		tuning_mode;	/* Re-tuning mode supported by host */
#define SDHCI_TUNING_MODE_1	0
	struct timer_list	tuning_timer;	/* Timer for tuning */

	struct sdhci_next next_data;
	ktime_t data_start_time;
	enum sdhci_power_policy power_policy;

	bool is_crypto_en;
	bool crypto_reset_reqd;
	bool sdio_irq_async_status;

	u32 auto_cmd_err_sts;
	struct ratelimit_state dbg_dump_rs;
	struct cmdq_host *cq_host;
	int reset_wa_applied; /* reset workaround status */
	ktime_t reset_wa_t; /* time when the reset workaround is applied */
	int reset_wa_cnt; /* total number of times workaround is used */

	u32 send_cmd_idx;
	s64 send_cmd_timestamp;
	u32 irq_cmd_idx;
	s64 irq_timestamp;
	s64 finish_tasklet_timestamp;

	unsigned long private[0] ____cacheline_aligned;
};
#endif /* LINUX_MMC_SDHCI_H */
