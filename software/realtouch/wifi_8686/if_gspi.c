/** @file if_gspi.c
  * @brief This file contains generic GSPI functions
  * 
  *  Copyright �Marvell International Ltd. and/or its affiliates, 2003-2007
  */
/********************************************************
Change log:
	09/26/05: Add Doxygen format comments
	10/20/05: Add GSPI8686 support
	01/30/06: Add kernel 2.6 support for GSPI8xxx/Bulverde
	
********************************************************/
#include "if_gspi.h"
#include "gspi_io.h"
#include "include.h"
#include <rtthread.h>
#include <dfs_posix.h>

extern void disable_wlan_interrupt(void);
extern void enable_wlan_interrupt(void);
#define HIC_DEFAULT_VALUE 0
/********************************************************
		Local Variables
********************************************************/

/********************************************************
		Global Variables
********************************************************/
extern int gspi_register_irq(int * irqnum);

extern int g_dummy_clk_reg;
extern int g_dummy_clk_ioport;

/********************************************************
		Local Functions
********************************************************/

static int gspi_read_reg32( u16 reg, u32 * data);
//static int gspi_read_host_int_status(u8 * curHIS);
static int gspi_read_event_scratch(WlanCard *card);

/** 
 *  @brief clear the auto generate interrupt bit in Host
 *  Interrupt Control register
 * 
 *  @param priv    A pointer to wlan_private structure
 *  @return 	   NA
 */
static void gspi_init_HOST_INT_CTRL_REG(void)
{
    gspi_write_reg(HOST_INT_CTRL_REG,HIC_DEFAULT_VALUE & ~(HIC_TxDnldAuto|HIC_RxUpldAuto | HIC_CmdDnldAuto|HIC_CmdUpldAuto));
}

/**
 *  @brief This function re-enable the interrupt of the mask bit.
 * 
 *  @param priv    A pointer to wlan_private structure
 *  @param mask    interrupt mask 	
 *  @return 	   WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
static int gspi_write_host_int_status(u16 mask)
{
    if (gspi_write_reg(HOST_INT_STATUS_REG, ~mask)) {
        return WLAN_STATUS_FAILURE;
    }
    return WLAN_STATUS_SUCCESS;
}

/** 
 *  @brief This function used to poll CmdDnLdRdy bit of Host Interrupt Status Register
 * 
 *  @param cardp   A pointer to gspi_card_rec structure
 *  @return 	   WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
static int wait_for_hostintstatus(void *card)
{
    #define MAX_WAIT_TRIES 1000
    int i = 0,y;
    u16 stat=0;

    for (i = 0; i < MAX_WAIT_TRIES; ++i) {
        gspi_read_reg(HOST_INT_STATUS_REG, &stat);

        if (stat & GHIS_CmdDnLdRdy)
        	return WLAN_STATUS_SUCCESS;
	  for(y=0;y<2000;y++);
    }

    return WLAN_STATUS_FAILURE;
}

/** 
 *  @brief This function read a 32bit value from GSPI register
 * 
 *  @param reg          hardware register
 *  @param data		A pointer to return the register value    
 *  @return 	        WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
static int gspi_read_reg32( u16 reg, u32 * data)
{
	u16 readdt[6];
	if (gspi_read_data_direct((u8 *) readdt, reg, 4) < 0) {
		return WLAN_STATUS_FAILURE;
	}
	rt_memcpy(data, readdt, 4);
	return WLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function read the host inerrrupt status register
 *  @param curHIS	A pointer to return the value of Host Interrupt Status register
 *  @return 	        WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int gspi_read_host_int_status(u8 * curHIS)
{
	u16 his;
	gspi_read_reg(HOST_INT_STATUS_REG, &his);
	*curHIS = (u8) his;
	return 0;
}

/**
 *  @brief Read Event cause from the event scratch register
 * 
 *  @param priv         A pointer to wlan_private structure
 *  @return 	        WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
static int gspi_read_event_scratch(WlanCard *card)
{
    int ret;

    ret = gspi_read_reg32(SCRATCH_3_REG, &card->EventCause);
    if (ret < 0) {
        WlanDebug(WlanMsg,"ERROR: Event Scratch Pad Register Read!\r\n");
        return ret;
    }
    WlanDebug(WlanMsg,"receive event is %x\n", card->EventCause);

    card->EventCause <<= 3;
    return WLAN_STATUS_SUCCESS;
}

/** 
 *  @brief This function enables the host interrupts mask
 *  
 *  @param priv    A pointer to wlan_private structure
 *  @param mask	   the interrupt mask
 *  @return 	   WLAN_STATUS_SUCCESS
 */
static int enable_host_int_mask(u8 mask)
{
    int ret = WLAN_STATUS_SUCCESS;
    // gspi_write_reg(HOST_INT_STATUS_MASK_REG,HISM_TxDnLdRdy | HISM_RxUpLdRdy | HISM_CmdDnLdRdy|HISM_CardEvent | HISM_CmdUpLdRdy);

    gspi_write_reg(HOST_INT_STATUS_MASK_REG, HISM_RxUpLdRdy | HISM_CmdDnLdRdy|HISM_CardEvent | HISM_CmdUpLdRdy);

    return ret;
}

/**  @brief This function disables the host interrupts mask.
 *  
 *  @param priv    A pointer to wlan_private structure
 *  @param mask	   the interrupt mask
 *  @return 	   WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
static int disable_host_int_mask(u8 int_mask)
{
    return gspi_write_reg(HOST_INT_STATUS_MASK_REG, 0x00);
}

/********************************************************
		Global Functions
********************************************************/
/** 
 *  @brief This is the interrupt handler for GSPI device
 *
 *  @param irq 	   The irq of GSPI device.
 *  @param dev_id  A pointer to net_device structure
 *  @param fp	   A pointer to pt_regs structure
 *  @return 	   n/a
 */
extern void disable_wlan_interrupt(void);
extern void enable_wlan_interrupt(void);
void sbi_interrupt(int dev)
{
  	disable_wlan_interrupt();
	wlan_interrupt();
}

#define IPFIELD_ALIGN_OFFSET    2

/** 
 *  @brief This function read the current interrupt status register.
 * 
 *  @param priv    A pointer to wlan_private structure
 *  @param ireg    A pointer to hold the return interrupt status value 	
 *  @return 	   WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int sbi_get_int_status(WlanCard *cardinfo, u8 *ireg)
{
	int ret = WLAN_STATUS_SUCCESS;
	rt_uint8_t tmp;
	WlanCard *card = cardinfo;

	/* disable host interrupt mask */
	disable_host_int_mask(HIM_DISABLE);
	/* read host interrupt status and clear it */
	gspi_read_host_int_status(&tmp);
	gspi_write_reg(HOST_INT_STATUS_REG, 0x0000);

	gspi_irq_clear();
	/* enable interrupt */
	enable_wlan_interrupt();

	/* re-map bit 0 and bit 1 for WLAN module since the definition is different */
	*ireg = tmp & (~(GHIS_TxDnLdRdy | GHIS_RxUpLdRdy));
	if (tmp & GHIS_TxDnLdRdy)
	{
		*ireg |= HIS_TxDnLdRdy;
	}
	if (tmp & GHIS_RxUpLdRdy)
	{
		*ireg |= HIS_RxUpLdRdy;
	}

	if (*ireg & HIS_RxUpLdRdy)
	{
		/* 16 Byte Align the IP fields *//*why the IPFIELD_ALIGN_OFFSET is 2 not 16 */
		card->TmpRxBufDataPtr = (unsigned char *) (((u32) card->TmpRxBuf + 3)
				& 0xfffffffc);
		if (sbi_card_to_host(MVMS_DAT, &card->TmpRxBuflen,
				card->TmpRxBufDataPtr, MRVDRV_ETH_RX_PACKET_BUFFER_SIZE) < 0)
		{
			WlanDebug(WlanMsg,"ERROR: Data Transfer from device failed\n");
			ret = WLAN_STATUS_FAILURE;
			goto done;
		}

		ProcessRxedPacket(card, card->TmpRxBufDataPtr, card->TmpRxBuflen);
	}

	if (*ireg & HIS_CmdUpLdRdy)
	{
		if (card->CmdResBuf != NULL)
		{
			if (sbi_card_to_host(MVMS_CMD, &card->CmdResLen, card->CmdResBuf,
					WLAN_UPLD_SIZE) < 0)
			{
				WlanDebug(WlanMsg,"ERROR: CMD Transfer from device failed\n");
				ret = WLAN_STATUS_FAILURE;
				goto done;
			}
		}
	}

done:
	enable_host_int_mask(HIM_ENABLE);

	return ret;
}

int WlanCardProbe(WlanCard * card)
{
    u16 host_int_mask;
    u16 chiprev;
    u32 irq_num;
   
    gspi_write_reg( SPU_BUS_MODE_REG, BUS_MODE_16_NO_DELAY);
    gspi_read_reg( CHIPREV_REG, &chiprev);
    card->ChipID = chiprev;
    WlanDebug(WlanMsg,"Chip ID 0x%x\n", chiprev);

    gspi_read_reg(HOST_INT_STATUS_REG, &host_int_mask);

    if (gspi_register_irq((int*)&irq_num) != GSPI_OK)
    {
         WlanDebug(WlanErr,"Wlan SPI interrupt register failed\r\n");
        return WLAN_STATUS_FAILURE;
    }

    enable_host_int_mask( HIM_ENABLE);
    gspi_init_HOST_INT_CTRL_REG();
    card->irqnum = irq_num;

    return WLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function check the firmware download status
 *   @return 	   WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int sbi_verify_fw_download(void)
{
    int i;
    u32 scr4;

    for (i = 0; i < MAX_FIRMWARE_POLL_TRIES; ++i) {

        if (gspi_read_reg32(SCRATCH_4_REG, &scr4) < 0) {
            WlanDebug(WlanErr,"Read from Scratch 4 failed !!!\n");
            return WLAN_STATUS_FAILURE;
        }

        if (scr4 == FIRMWARE_DNLD_OK) {
            WlanDebug(WlanMsg,"FW download successful !!!\n");	
            return WLAN_STATUS_SUCCESS;
        }

        rt_thread_delay(RT_TICK_PER_SECOND);
    }
	
    return WLAN_STATUS_FAILURE;
}

/** 
 *  @brief This function download the firmware to the hardware
 *  @param firmware     A pointer to firmware 
 *  @param firmwarelen  the len of firmware
 *  @return 	        WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int sbi_download_wlan_fw_image( const u8 * firmware,int firmwarelen)
{
    int ret,i;
    u16 len;
    u32 cnt = 0;
	
    do {
		for(i=0;i<1000;i++);
        	gspi_read_reg(SCRATCH_1_REG, &len);
    } while (!len);
   

    for (;;) {
        if (wait_for_hostintstatus(RT_NULL)) {
            WlanDebug(WlanErr,"FW download Phase2 failed\n");
            return WLAN_STATUS_FAILURE;
        }

        gspi_read_reg(SCRATCH_1_REG, &len);

        if (!len) {
            break;
        }

        if (len & 1) {
            WlanDebug(WlanMsg,"CRC Error\n");
            len &= ~1;
        } else {
            WlanDebug(WlanMsg,".");
        }

        gspi_write_data_direct((u8 *) (firmware + cnt), CMD_RDWRPORT_REG,
                               (len / 2) + 1);
        gspi_write_reg(HOST_INT_STATUS_REG, 0x0000);
        gspi_write_reg(CARD_INT_CAUSE_REG, CIC_CmdDnLdOvr);
        cnt += len;
    }
    ret = WLAN_STATUS_SUCCESS;
    return ret;
}

int sbi_download_wlan_fw_file(const char* filename)
{
    int ret,i;
    u16 len;
    u32 cnt = 0;
    int fd, ptr_length = 0;
    unsigned char* ptr = RT_NULL;

    do {
		for(i=0;i<1000;i++);
        	gspi_read_reg(SCRATCH_1_REG, &len);
    } while (!len);

    /* open firmware file */
    fd = open(filename, O_RDONLY, 0);
    if (fd < 0)
    {
    	rt_kprintf("no firmware:%s found.\n", filename);
    	return -1;
    }

    /* allocate buffer */
    ptr = rt_malloc(4096);
    if (ptr == NULL)
    {
    	rt_kprintf("out of memory.\n");
    	close(fd);
    	return -1;
    }

    for (;;)
    {
        if (wait_for_hostintstatus(RT_NULL))
        {
            WlanDebug(WlanErr,"FW download Phase2 failed\n");
            return WLAN_STATUS_FAILURE;
        }

        gspi_read_reg(SCRATCH_1_REG, &len);
        if (!len) {
            break;
        }

        if (len & 1) {
            WlanDebug(WlanMsg,"CRC Error\n");
            len &= ~1;
        } else {
            WlanDebug(WlanMsg,".");
        }

        if (len < 4096)
        {
        	int length;

        	length = read(fd, ptr, len);
        	if (length <= 0) break;
        }
        else
        {
        	rt_kprintf("warning: big length: %d\n", len);
        }

        gspi_write_data_direct(ptr, CMD_RDWRPORT_REG,
                               (len / 2) + 1);
        gspi_write_reg(HOST_INT_STATUS_REG, 0x0000);
        gspi_write_reg(CARD_INT_CAUSE_REG, CIC_CmdDnLdOvr);
    }
    ret = WLAN_STATUS_SUCCESS;

    if (ptr != NULL) rt_free(ptr);
    close(fd);

    return ret;
}

/**
 *  @brief This function is used to download firmware to hardware
 * 
 *  @param priv         	A pointer to wlan_private structure
 *  @param firmware 		A pointer to fimware
 *  @param firmwarelen		firmware length
 *  @return 	        	WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int sbi_prog_firmware_image(const u8 *firmware,int firmwarelen)
{
	int ret = WLAN_STATUS_SUCCESS;
	int fwblknow;

	for (fwblknow = 0; fwblknow < firmwarelen;fwblknow += FIRMWARE_DNLD_PCKCNT)
	{
		gspi_write_reg(SCRATCH_1_REG, FIRMWARE_DNLD_PCKCNT);

		if (wait_for_hostintstatus(RT_NULL)) {
			WlanDebug(WlanErr,"FW download Phase1 failed\n");
			return WLAN_STATUS_FAILURE;
		}
		gspi_write_data_direct((u8 *) (firmware + fwblknow), CMD_RDWRPORT_REG,
				(FIRMWARE_DNLD_PCKCNT / 2) + 1);
		gspi_write_reg(HOST_INT_STATUS_REG, 0x0000);
		gspi_write_reg(CARD_INT_CAUSE_REG, CIC_CmdDnLdOvr);
	}

	/* Writing 0 to Scr1 is to indicate the end of Firmware dwld */
	gspi_write_reg(SCRATCH_1_REG, FIRMWARE_DNLD_END);
	gspi_write_reg(HOST_INT_STATUS_REG, 0x0000);
	gspi_write_reg(CARD_INT_CAUSE_REG, CIC_CmdDnLdOvr);

	ret = WLAN_STATUS_SUCCESS;
	return ret;
}

int sbi_prog_firmware_file(const char* filename)
{
	int ret = WLAN_STATUS_SUCCESS;
	int fwblknow;
	unsigned char* ptr;
	int fd, len;

	fd = open(filename, O_RDONLY, 0);
	if (fd < 0)
	{
		rt_kprintf("no firmware found.\n");
		return -1;
	}
	ptr = rt_malloc(FIRMWARE_DNLD_PCKCNT);
	if (ptr == RT_NULL)
	{
		rt_kprintf("out of memory.\n");
		close(fd);

		return -1;
	}

	while (1)
	{
		len = read(fd, ptr, FIRMWARE_DNLD_PCKCNT);
		if (len <= 0) break;
		gspi_write_reg(SCRATCH_1_REG, FIRMWARE_DNLD_PCKCNT);

		if (wait_for_hostintstatus(RT_NULL)) {
			WlanDebug(WlanErr,"FW download Phase1 failed\n");
			rt_free(ptr);
			close(fd);
			return WLAN_STATUS_FAILURE;
		}

		gspi_write_data_direct(ptr, CMD_RDWRPORT_REG,
				(FIRMWARE_DNLD_PCKCNT / 2) + 1);
		gspi_write_reg(HOST_INT_STATUS_REG, 0x0000);
		gspi_write_reg(CARD_INT_CAUSE_REG, CIC_CmdDnLdOvr);
	}

	/* Writing 0 to Scr1 is to indicate the end of Firmware dwld */
	gspi_write_reg(SCRATCH_1_REG, FIRMWARE_DNLD_END);
	gspi_write_reg(HOST_INT_STATUS_REG, 0x0000);
	gspi_write_reg(CARD_INT_CAUSE_REG, CIC_CmdDnLdOvr);

	ret = WLAN_STATUS_SUCCESS;
	rt_free(ptr);
	close(fd);
	return ret;
}

/** 
 *  @brief enable the host interrupt on the GSPI device
 * 
 *  @param priv         A pointer to wlan_private structure
 *  @return 	        WLAN_STATUS_SUCCESS
 */
int sbi_enable_host_int(void)
{	
	gspi_irq_clear();
	enable_wlan_interrupt();
	return WLAN_STATUS_SUCCESS;
}

/** 
 *  @brief Disable the Host interrupt on the GSPI device
 * 
 *  @param priv         A pointer to wlan_private structure
 *  @return 	        WLAN_STATUS_SUCCESS
 */
int sbi_disable_host_int(void)
{
	disable_wlan_interrupt();
   	return WLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function is used to send the data/cmd to hardware
 * 
 *  @param priv         A pointer to WlanCard structure
 *  @param type 	1--Cmd, 0--Data
 *  @param payload	A point to the data or cmd buffer
 *  @param nb           len of data/cmd buffer 
 *  @return 	        WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int sbi_host_to_card(WlanCard *cardinfo, u8 type, u8 * payload, u16 nb)
{
	int ret = WLAN_STATUS_SUCCESS;
	u16 writeReg;
	u8 intType;
	int i;
	intType = type ? CIC_CmdDnLdOvr : CIC_TxDnLdOvr;
	writeReg = (type) ? CMD_RDWRPORT_REG : DATA_RDWRPORT_REG;
	cardinfo->SentStatus = (type) ? DNLD_CMD_SENT : DNLD_DATA_SENT;
	hexdump("send:", (unsigned char *) payload, nb);

	if (nb & 0x0001)
		nb += 1;

	if (!(nb % 4))
		ret = gspi_write_data_direct(payload, writeReg, (nb / 2) + 1);
	else
		ret = gspi_write_data_direct(payload, writeReg, (nb / 2) + 2);

	gspi_write_reg(CARD_INT_CAUSE_REG, intType);

	return ret;
}

/**
 *  @brief This function is used to read data/cmd from the card.
 * 
 *  @param priv         A pointer to wlan_private structure
 *  @param type 	1--Cmd, 0--Data
 *  @param nb    	A point to return how many bytes has been read back from hardware
 *  @param payload      A point to data buffer for receive data/cmd
 *  @param npayload     the size of payload buffer
 *  @return 	        WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int sbi_card_to_host(u32 type,u16* nb, u8 * payload, u16 npayload)
{
    int ret = WLAN_STATUS_SUCCESS;
    u16 len;
    u16 intType = 0, readReg;

    intType = type ? CIC_CmdUpLdOvr : CIC_RxUpLdOvr;
    readReg = type ? CMD_RDWRPORT_REG : DATA_RDWRPORT_REG;

    gspi_read_reg((type) ? SCRATCH_2_REG : SCRATCH_1_REG, &len);

    if (!len || len > npayload) {
        WlanDebug(WlanErr,"Error packet of len %d\n", len);
        len = MRVDRV_ETH_RX_PACKET_BUFFER_SIZE;
    }

    if (len & 0x0001)
        len += 1;

    if (!(len % 4))
        ret = gspi_read_data_direct(payload, readReg, (len / 2) + 1);
    else
        ret = gspi_read_data_direct(payload, readReg, (len / 2) + 2);

    gspi_write_reg(CARD_INT_CAUSE_REG, intType);

    *nb = len;
    return ret;
}

/** 
 *  @brief This function is used to read the event cause from card
 *  and re-enable event interrupt.
 * 
 *  @param priv         A pointer to wlan_private structure
 *  @return 	        WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int sbi_read_event_cause(WlanCard *card)
{
    gspi_read_event_scratch(card);

    /* re-enable the interrupt */
    gspi_write_host_int_status(GHIS_CardEvent);

    /* generate interrupt to firmware */
    gspi_write_reg(CARD_INT_CAUSE_REG, CIC_HostEvent);

    return WLAN_STATUS_SUCCESS;
}
int gspi_read_host_int_mask(void)
{
    	u16 his;
	gspi_read_reg(HOST_INT_STATUS_MASK_REG, &his);
 	return 0;
}
int gspi_read_test(void)
{
    u16 his;
 	  
	
	gspi_read_reg(HOST_INT_RESET_SELECT_REG, &his);

       rt_kprintf("read HOST_INT_RESET_SELECT_REG  %x\n",his);
	gspi_read_reg(HOST_INT_STATUS_MASK_REG, &his);

       rt_kprintf("read HOST_INT_STATUS_MASK_REG  %x\n",his);

	gspi_read_reg(HOST_INT_EVENT_MASK_REG, &his);

    	rt_kprintf("read HOST_INT_EVENT_MASK_REG  %x\n",his);

	gspi_read_reg(CARD_INT_CAUSE_REG, &his);
    	rt_kprintf("read CARD_INT_CAUSE_REG  %x\n",his);

	gspi_read_reg(CARD_INT_STATUS_REG, &his);
    	rt_kprintf("read CARD_INT_STATUS_REG  %x\n",his);
	gspi_read_reg(CARD_INT_EVENT_MASK_REG, &his);
    	rt_kprintf("read CARD_INT_EVENT_MASK_REG  %x\n",his);
	gspi_read_reg(CARD_INT_STATUS_MASK_REG, &his);
    	rt_kprintf("read CARD_INT_STATUS_MASK_REG  %x\n",his);
		gspi_read_reg(HOST_INT_STATUS_REG, &his);

        rt_kprintf("read HOST_INT_STATUS_REG  %x\n",his);

    return 0;
}
