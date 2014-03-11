#ifndef __CODEC_WM8978_I2C_H__
#define __CODEC_WM8978_I2C_H__

#include <stdint.h>

/* Register Definitions */
#define REG_SOFTWARE_RESET		((uint16_t)0)
#define REG_POWER_MANAGEMENT1	((uint16_t)(1 << 9))
#define REG_POWER_MANAGEMENT2	((uint16_t)(2 << 9))
#define REG_POWER_MANAGEMENT3	((uint16_t)(3 << 9))
#define REG_AUDIO_INTERFACE		((uint16_t)(4 << 9))
#define REG_COMPANDING			((uint16_t)(5 << 9))
#define REG_CLOCK_GEN			((uint16_t)(6 << 9))
#define REG_ADDITIONAL			((uint16_t)(7 << 9))
#define REG_GPIO				((uint16_t)(8 << 9))
#define REG_JACK_DETECT1		((uint16_t)(9 << 9))
#define REG_DAC					((uint16_t)(10 << 9))
#define REG_LEFT_DAC_VOL		((uint16_t)(11 << 9))
#define REG_RIGHT_DAC_VOL		((uint16_t)(12 << 9))
#define REG_JACK_DETECT2		((uint16_t)(13 << 9))
#define REG_ADC					((uint16_t)(14 << 9))
#define REG_LEFT_ADC_VOL		((uint16_t)(15 << 9))
#define REG_RIGHT_ADC_VOL		((uint16_t)(16 << 9))
#define REG_EQ1					((uint16_t)(18 << 9))
#define REG_EQ2					((uint16_t)(19 << 9))
#define REG_EQ3					((uint16_t)(20 << 9))
#define REG_EQ4					((uint16_t)(21 << 9))
#define REG_EQ5					((uint16_t)(22 << 9))
#define REG_DAC_LIMITER1		((uint16_t)(24 << 9))
#define REG_DAC_LIMITER2		((uint16_t)(25 << 9))
#define REG_NOTCH_FILTER1		((uint16_t)(27 << 9))
#define REG_NOTCH_FILTER2		((uint16_t)(28 << 9))
#define REG_NOTCH_FILTER3		((uint16_t)(29 << 9))
#define REG_NOTCH_FILTER4		((uint16_t)(30 << 9))
#define REG_ALC1				((uint16_t)(32 << 9))
#define REG_ALC2				((uint16_t)(33 << 9))
#define REG_ALC3				((uint16_t)(34 << 9))
#define REG_NOISE_GATE			((uint16_t)(35 << 9))
#define REG_PLL_N				((uint16_t)(36 << 9))
#define REG_PLL_K1				((uint16_t)(37 << 9))
#define REG_PLL_K2				((uint16_t)(38 << 9))
#define REG_PLL_K3				((uint16_t)(39 << 9))
#define REG_3D					((uint16_t)(41 << 9))
#define REG_BEEP				((uint16_t)(43 << 9))
#define REG_INPUT				((uint16_t)(44 << 9))
#define REG_LEFT_PGA_GAIN		((uint16_t)(45 << 9))
#define REG_RIGHT_PGA_GAIN		((uint16_t)(46 << 9))
#define REG_LEFT_ADC_BOOST		((uint16_t)(47 << 9))
#define REG_RIGHT_ADC_BOOST		((uint16_t)(48 << 9))
#define REG_OUTPUT				((uint16_t)(49 << 9))
#define REG_LEFT_MIXER			((uint16_t)(50 << 9))
#define REG_RIGHT_MIXER			((uint16_t)(51 << 9))
#define REG_LOUT1_VOL			((uint16_t)(52 << 9))
#define REG_ROUT1_VOL			((uint16_t)(53 << 9))
#define REG_LOUT2_VOL			((uint16_t)(54 << 9))
#define REG_ROUT2_VOL			((uint16_t)(55 << 9))
#define REG_OUT3_MIXER			((uint16_t)(56 << 9))
#define REG_OUT4_MIXER			((uint16_t)(57 << 9))

// R01 REG_POWER_MANAGEMENT1
#define BUFDCOPEN				(1 << 8)
#define OUT4MIXEN				(1 << 7)
#define OUT3MIXEN				(1 << 6)
#define PLLEN					(1 << 5)
#define MICBEN					(1 << 4)
#define BIASEN					(1 << 3)
#define BUFIOEN					(1 << 2)
#define VMIDSEL_OFF				(0)
#define VMIDSEL_75K				(1)
#define VMIDSEL_300K			(2)
#define VMIDSEL_5K				(3)

// R02 REG_POWER_MANAGEMENT2
#define ROUT1EN					(1 << 8)
#define LOUT1EN					(1 << 7)
#define SLEEP					(1 << 6)
#define BOOSTENR				(1 << 5)
#define BOOSTENL				(1 << 4)
#define INPPGAENR				(1 << 3)
#define INPPGAENL				(1 << 2)
#define ADCENR					(1 << 1)
#define ADCENL					(1)

// R03 REG_POWER_MANAGEMENT3
#define OUT4EN					(1 << 8)
#define OUT3EN					(1 << 7)
#define LOUT2EN					(1 << 6)
#define ROUT2EN					(1 << 5)
#define RMIXEN					(1 << 3)
#define LMIXEN					(1 << 2)
#define DACENR					(1 << 1)
#define DACENL					(1)

// R04 REG_AUDIO_INTERFACE
#define BCP_NORMAL				(0)
#define BCP_INVERTED			(1 << 8)
#define LRP_NORMAL				(0)
#define LRP_INVERTED			(1 << 7)
#define WL_16BITS				(0)
#define WL_20BITS				(1 << 5)
#define WL_24BITS				(2 << 5)	// Default value
#define WL_32BITS				(3 << 5)
#define FMT_RIGHT_JUSTIFIED		(0)
#define FMT_LEFT_JUSTIFIED		(1 << 3)
#define FMT_I2S					(2 << 3)	// Default value
#define FMT_PCM					(3 << 3)
#define DACLRSWAP				(1 << 2)
#define ADCLRSWAP				(1 << 1)
#define MONO					(1)

// R05 REG_COMPANDING
#define WL8						(1 << 5)
#define DAC_COMP_OFF			(0)			// Default value
#define DAC_COMP_ULAW			(2 << 3)
#define DAC_COMP_ALAW			(3 << 3)
#define ADC_COMP_OFF			(0)			// Default value
#define ADC_COMP_ULAW			(2 << 1)
#define ADC_COMP_ALAW			(3 << 1)
#define LOOPBACK				(1)

// R06 REG_CLOCK_GEN
#define CLKSEL_MCLK				(0)
#define CLKSEL_PLL				(1 << 8)	// Default value
#define MCLK_DIV1				(0)
#define MCLK_DIV1_5				(1 << 5)
#define MCLK_DIV2				(2 << 5)	// Default value
#define MCLK_DIV3				(3 << 5)
#define MCLK_DIV4				(4 << 5)
#define MCLK_DIV6				(5 << 5)
#define MCLK_DIV8				(6 << 5)
#define MCLK_DIV12				(7 << 5)
#define BCLK_DIV1				(0)			// Default value
#define BCLK_DIV2				(1 << 2)
#define BCLK_DIV4				(2 << 2)
#define BCLK_DIV8				(3 << 2)
#define BCLK_DIV16				(4 << 2)
#define BCLK_DIV32				(5 << 2)
#define MS						(1)

// R07 REG_ADDITIONAL
#define SR_48KHZ				(0)			// Default value
#define SR_32KHZ				(1 << 1)
#define SR_24KHZ				(2 << 1)
#define SR_16KHZ				(3 << 1)
#define SR_12KHZ				(4 << 1)
#define SR_8KHZ					(5 << 1)
#define SLOWCLKEN				(1)

// R08 REG_GPIO
#define OPCLK_DIV1				(0)			// Default value
#define OPCLK_DIV2				(1 << 4)
#define OPCLK_DIV3				(2 << 4)
#define OPCLK_DIV4				(3 << 4)
#define GPIO1POL_NONINVERTED	(0)			// Default value
#define GPIO1POL_INVERTED		(1 << 3)
#define GPIO1SEL_INPUT			(0)			// Default value
#define GPIO1SEL_TEMP_OK		(2)
#define GPIO1SEL_AMUTE_ACTIVE	(3)
#define GPIO1SEL_PLL_CLK_OP		(4)
#define GPIO1SEL_PLL_LOCK		(5)
#define GPIO1SEL_LOGIC1			(6)
#define GPIO1SEL_LOGIC0			(7)

// R09 REG_JACK_DETECT1
#define JD_VMID_EN1				(1 << 8)
#define JD_VMID_EN0				(1 << 7)
#define JD_EN					(1 << 6)
#define JD_SEL_GPIO1			(0 << 4)	// Default value
#define JD_SEL_GPIO2			(1 << 4)
#define JD_SEL_GPIO3			(2 << 4)

// R10 REG_DAC
#define SOFTMUTE				(1 << 6)
#define DACOSR128				(1 << 3)
#define AMUTE					(1 << 2)
#define DACPOLR					(1 << 1)
#define DACPOLL					(1)

// R11 & R12 REG_LEFT_DAC_VOL & REG_RIGHT_DAC_VOL
#define DACVU					(1 << 8)
#define DACVOL_POS				(0)
#define DACVOL_MASK				(0xFF)

// R13 REG_JACK_DETECT2
#define JD_OUT4_EN1				(1 << 7)
#define JD_OUT3_EN1				(1 << 6)
#define JD_OUT2_EN1				(1 << 5)
#define JD_OUT1_EN1				(1 << 4)
#define JD_OUT4_EN0				(1 << 3)
#define JD_OUT3_EN0				(1 << 2)
#define JD_OUT2_EN0				(1 << 1)
#define JD_OUT1_EN0				(1)

// R14 REG_ADC
#define HPFEN					(1 << 8)
#define HPFAPP					(1 << 7)
#define HPFCUT_POS				(4)
#define HPFCUT_MASK				(7)
#define HPFCUT_0				(0)
#define HPFCUT_1				(1 << 4)
#define HPFCUT_2				(2 << 4)
#define HPFCUT_3				(3 << 4)
#define HPFCUT_4				(4 << 4)
#define HPFCUT_5				(5 << 4)
#define HPFCUT_6				(6 << 4)
#define HPFCUT_7				(7 << 4)
#define ADCOSR128				(1 << 3)
#define ADCRPOL					(1 << 1)
#define ADCLPOL					(1)

// R15 & R16 REG_LEFT_ADC_VOL & REG_RIGHT_ADC_VOL
#define ADCVU					(1 << 8)
#define ADCVOL_POS				(0)
#define ADCVOL_MASK				(0xFF)

// R18 REG_EQ1
#define EQ3DMODE_ADC			(0)
#define EQ3DMODE_DAC			(1 << 8)	// Default value
#define EQ1C_80HZ				(0)
#define EQ1C_105HZ				(1 << 5)	// Default value
#define EQ1C_135HZ				(2 << 5)
#define EQ1C_175HZ				(3 << 5)

// R19 REG_EQ2
#define EQ2BW_NARROW			(0)			// Default value
#define EQ2BW_WIDE				(1 << 8)
#define EQ2C_230HZ				(0)
#define EQ2C_300HZ				(1 << 5)	// Default value
#define EQ2C_385HZ				(2 << 5)
#define EQ2C_500HZ				(3 << 5)

// R20 REG_EQ3
#define EQ3BW_NARROW			(0)			// Default value
#define EQ3BW_WIDE				(1 << 8)
#define EQ3C_650HZ				(0)
#define EQ3C_850HZ				(1 << 5)	// Default value
#define EQ3C_1_1KHZ				(2 << 5)
#define EQ3C_1_4KHZ				(3 << 5)

// R21 REG_EQ4
#define EQ4BW_NARROW			(0)			// Default value
#define EQ4BW_WIDE				(1 << 8)
#define EQ4C_1_8KHZ				(0)
#define EQ4C_2_4KHZ				(1 << 5)	// Default value
#define EQ4C_3_2KHZ				(2 << 5)
#define EQ4C_4_1KHZ				(3 << 5)

// R22 REG_EQ5
#define EQ5C_5_3KHZ				(0)
#define EQ5C_6_9KHZ				(1 << 5)	// Default value
#define EQ5C_9KHZ				(2 << 5)
#define EQ5C_11_7KHZ			(3 << 5)

// R18 - R22
#define EQC_POS					(5)
#define EQC_MASK				(3)
#define EQG_POS					(0)
#define EQG_MASK				(31)

// R24 REG_DAC_LIMITER1
#define LIMEN					(1 << 8)
#define LIMDCY_POS				(4)
#define LIMDCY_MASK				(15)
#define LIMDCY_750US			(0)
#define LIMDCY_1_5MS			(1 << 4)
#define LIMDCY_3MS				(2 << 4)
#define LIMDCY_6MS				(3 << 4)	// Default value
#define LIMDCY_12MS				(4 << 4)
#define LIMDCY_24MS				(5 << 4)
#define LIMDCY_48MS				(6 << 4)
#define LIMDCY_96MS				(7 << 4)
#define LIMDCY_192MS			(8 << 4)
#define LIMDCY_384MS			(9 << 4)
#define LIMDCY_768MS			(10 << 4)
#define LIMATK_POS				(0)
#define LIMATK_MASK				(15)
#define LIMATK_94US				(0)
#define LIMATK_188US			(1)
#define LIMATK_375US			(2)			// Default value
#define LIMATK_750US			(3)
#define LIMATK_1_5MS			(4)
#define LIMATK_3MS				(5)
#define LIMATK_6MS				(6)
#define LIMATK_12MS				(7)
#define LIMATK_24MS				(8)
#define LIMATK_48MS				(9)
#define LIMATK_96MS				(10)
#define LIMATK_192MS			(11)

// R25 REG_DAC_LIMITER2
#define LIMLVL_POS				(4)
#define LIMLVL_MASK				(7)
#define LIMLVL_N1DB				(0)			// Default value
#define LIMLVL_N2DB				(1 << 4)
#define LIMLVL_N3DB				(2 << 4)
#define LIMLVL_N4DB				(3 << 4)
#define LIMLVL_N5DB				(4 << 4)
#define LIMLVL_N6DB				(5 << 4)
#define LIMBOOST_POS			(0)
#define LIMBOOST_MASK			(15)
#define LIMBOOST_0DB			(0)
#define LIMBOOST_1DB			(1)
#define LIMBOOST_2DB			(2)
#define LIMBOOST_3DB			(3)
#define LIMBOOST_4DB			(4)
#define LIMBOOST_5DB			(5)
#define LIMBOOST_6DB			(6)
#define LIMBOOST_7DB			(7)
#define LIMBOOST_8DB			(8)
#define LIMBOOST_9DB			(9)
#define LIMBOOST_10DB			(10)
#define LIMBOOST_11DB			(11)
#define LIMBOOST_12DB			(12)

// R27 - R30 REG_NOTCH_FILTER1 - REG_NOTCH_FILTER4
#define NFU						(1 << 8)
#define NFEN					(1 << 7)
#define NFA_POS					(0)
#define NFA_MASK				(127)

// R32 REG_ALC1
#define ALCSEL_OFF				(0)			// Default value
#define ALCSEL_RIGHT_ONLY		(1 << 7)
#define ALCSEL_LEFT_ONLY		(2 << 7)
#define ALCSEL_BOTH_ON			(3 << 7)
#define ALCMAXGAIN_POS			(3)
#define ALCMAXGAIN_MASK			(7)
#define ALCMAXGAIN_N6_75DB		(0)
#define ALCMAXGAIN_N0_75DB		(1 << 3)
#define ALCMAXGAIN_5_25DB		(2 << 3)
#define ALCMAXGAIN_11_25DB		(3 << 3)
#define ALCMAXGAIN_17_25DB		(4 << 3)
#define ALCMAXGAIN_23_25DB		(5 << 3)
#define ALCMAXGAIN_29_25DB		(6 << 3)
#define ALCMAXGAIN_35_25DB		(7 << 3)	// Default value
#define ALCMINGAIN_POS			(0)
#define ALCMINGAIN_MASK			(7)
#define ALCMINGAIN_N12DB		(0)			// Default value
#define ALCMINGAIN_N6DB			(1)
#define ALCMINGAIN_0DB			(2)
#define ALCMINGAIN_6DB			(3)
#define ALCMINGAIN_12DB			(4)
#define ALCMINGAIN_18DB			(5)
#define ALCMINGAIN_24DB			(6)
#define ALCMINGAIN_30DB			(7)

// R33 REG_ALC2
#define ALCHLD_POS				(4)
#define ALCHLD_MASK				(15)
#define ALCHLD_0MS				(0)			// Default value
#define ALCHLD_2_67MS			(1 << 4)
#define ALCHLD_5_33MS			(2 << 4)
#define ALCHLD_10_67MS			(3 << 4)
#define ALCHLD_21_33MS			(4 << 4)
#define ALCHLD_42_67MS			(5 << 4)
#define ALCHLD_85_33MS			(6 << 4)
#define ALCHLD_170_67MS			(7 << 4)
#define ALCHLD_341_33MS			(8 << 4)
#define ALCHLD_682_67MS			(9 << 4)
#define ALCHLD_1_36S			(10 << 4)
#define ALCLVL_POS				(0)
#define ALCLVL_MASK				(15)
#define ALCLVL_N22_5DBFS		(0)
#define ALCLVL_N21DBFS			(1)
#define ALCLVL_N19_5DBFS		(2)
#define ALCLVL_N18DBFS			(3)
#define ALCLVL_N16_5DBFS		(4)
#define ALCLVL_N15DBFS			(5)
#define ALCLVL_N13_5DBFS		(6)
#define ALCLVL_N12DBFS			(7)
#define ALCLVL_N10_5DBFS		(8)
#define ALCLVL_N9DBFS			(9)
#define ALCLVL_N7_5DBFS			(10)
#define ALCLVL_N6DBFS			(11)		// Default value
#define ALCLVL_N4_5DBFS			(12)
#define ALCLVL_N3DBFS			(13)
#define ALCLVL_N1_5DBFS			(14)

// R34 REG_ALC3
#define ALCMODE_ALC				(0)			// Default value
#define ALCMODE_LIMITER			(1 << 8)
#define ALCDCY_POS				(4)
#define ALCDCY_MASK				(15)
#define ALCDCY_0				(0)
#define ALCDCY_1				(1 << 4)
#define ALCDCY_2				(2 << 4)
#define ALCDCY_3				(3 << 4)	// Default value
#define ALCDCY_4				(4 << 4)
#define ALCDCY_5				(5 << 4)
#define ALCDCY_6				(6 << 4)
#define ALCDCY_7				(7 << 4)
#define ALCDCY_8				(8 << 4)
#define ALCDCY_9				(9 << 4)
#define ALCDCY_10				(10 << 4)
#define ALCATK_POS				(0)
#define ALCATK_MASK				(15)
#define ALCATK_0				(0)
#define ALCATK_1				(1)
#define ALCATK_2				(2)			// Default value
#define ALCATK_3				(3)
#define ALCATK_4				(4)
#define ALCATK_5				(5)
#define ALCATK_6				(6)
#define ALCATK_7				(7)
#define ALCATK_8				(8)
#define ALCATK_9				(9)
#define ALCATK_10				(10)

// R35 REG_NOISE_GATE
#define NGEN					(1 << 3)
#define NGTH_POS				(0)
#define NGTH_MASK				(7)
#define NGTH_N39DB				(0)			// Default value
#define NGTH_N45DB				(1)
#define NGTH_N51DB				(2)
#define NGTH_N57DB				(3)
#define NGTH_N63DB				(4)
#define NGTH_N69DB				(5)
#define NGTH_N75DB				(6)
#define NGTH_N81DB				(7)

// R36 REG_PLL_N
#define PLLPRESCALE				(1 << 4)
#define PLLN_POS				(0)
#define PLLN_MASK				(15)

// R37 - R39 REG_PLL_K1 - REG_PLL_K3
#define PLLK1_POS				(0)
#define PLLK1_MASK				(63)
#define PLLK2_POS				(0)
#define PLLK2_MASK				(511)
#define PLLK3_POS				(0)
#define PLLK3_MASK				(511)

// R41 REG_3D
#define DEPTH3D_POS				(0)
#define DEPTH3D_MASK			(15)
#define DEPTH3D_0				(0)			// Default value
#define DEPTH3D_6_67			(1)
#define DEPTH3D_13_33			(2)
#define DEPTH3D_20				(3)
#define DEPTH3D_26_67			(4)
#define DEPTH3D_33_33			(5)
#define DEPTH3D_40				(6)
#define DEPTH3D_46_67			(7)
#define DEPTH3D_53_33			(8)
#define DEPTH3D_60				(9)
#define DEPTH3D_66_67			(10)
#define DEPTH3D_73_33			(11)
#define DEPTH3D_80				(12)
#define DEPTH3D_86_67			(13)
#define DEPTH3D_93_33			(14)
#define DEPTH3D_100				(15)

// R43 REG_BEEP
#define MUTERPGA2INV			(1 << 5)
#define INVROUT2				(1 << 4)
#define BEEPVOL_POS				(1)
#define BEEPVOL_MASK			(7)
#define BEEPVOL_N15DB			(0)
#define BEEPVOL_N12DB			(1 << 1)
#define BEEPVOL_N9DB			(2 << 1)
#define BEEPVOL_N6DB			(3 << 1)
#define BEEPVOL_N3DB			(4 << 1)
#define BEEPVOL_0DB				(5 << 1)
#define BEEPVOL_3DB				(6 << 1)
#define BEEPVOL_6DB				(7 << 1)
#define BEEPEN					(1)

// R44 REG_INPUT
#define MBVSEL_0_9AVDD			(0)			// Default value
#define MBVSEL_0_65AVDD			(1 << 8)
#define R2_2INPVGA				(1 << 6)
#define RIN2INPVGA				(1 << 5)	// Default value
#define RIP2INPVGA				(1 << 4)	// Default value
#define L2_2INPVGA				(1 << 2)
#define LIN2INPVGA				(1 << 1)	// Default value
#define LIP2INPVGA				(1)			// Default value

// R45 REG_LEFT_PGA_GAIN
#define INPPGAUPDATE			(1 << 8)
#define INPPGAZCL				(1 << 7)
#define INPPGAMUTEL				(1 << 6)

// R46 REG_RIGHT_PGA_GAIN
#define INPPGAZCR				(1 << 7)
#define INPPGAMUTER				(1 << 6)

// R45 - R46
#define INPPGAVOL_POS			(0)
#define INPPGAVOL_MASK			(63)

// R47 REG_LEFT_ADC_BOOST
#define PGABOOSTL				(1 << 8)	// Default value
#define L2_2BOOSTVOL_POS		(4)
#define L2_2BOOSTVOL_MASK		(7)
#define L2_2BOOSTVOL_DISABLED	(0)			// Default value
#define L2_2BOOSTVOL_N12DB		(1 << 4)
#define L2_2BOOSTVOL_N9DB		(2 << 4)
#define L2_2BOOSTVOL_N6DB		(3 << 4)
#define L2_2BOOSTVOL_N3DB		(4 << 4)
#define L2_2BOOSTVOL_0DB		(5 << 4)
#define L2_2BOOSTVOL_3DB		(6 << 4)
#define L2_2BOOSTVOL_6DB		(7 << 4)
#define AUXL2BOOSTVOL_POS		(0)
#define AUXL2BOOSTVOL_MASK		(7)
#define AUXL2BOOSTVOL_DISABLED	(0)			// Default value
#define AUXL2BOOSTVOL_N12DB		(1)
#define AUXL2BOOSTVOL_N9DB		(2)
#define AUXL2BOOSTVOL_N6DB		(3)
#define AUXL2BOOSTVOL_N3DB		(4)
#define AUXL2BOOSTVOL_0DB		(5)
#define AUXL2BOOSTVOL_3DB		(6)
#define AUXL2BOOSTVOL_6DB		(7)

// R48 REG_RIGHT_ADC_BOOST
#define PGABOOSTR				(1 << 8)	// Default value
#define R2_2BOOSTVOL_POS		(4)
#define R2_2BOOSTVOL_MASK		(7)
#define R2_2BOOSTVOL_DISABLED	(0)			// Default value
#define R2_2BOOSTVOL_N12DB		(1 << 4)
#define R2_2BOOSTVOL_N9DB		(2 << 4)
#define R2_2BOOSTVOL_N6DB		(3 << 4)
#define R2_2BOOSTVOL_N3DB		(4 << 4)
#define R2_2BOOSTVOL_0DB		(5 << 4)
#define R2_2BOOSTVOL_3DB		(6 << 4)
#define R2_2BOOSTVOL_6DB		(7 << 4)
#define AUXR2BOOSTVOL_POS		(0)
#define AUXR2BOOSTVOL_MASK		(7)
#define AUXR2BOOSTVOL_DISABLED	(0)			// Default value
#define AUXR2BOOSTVOL_N12DB		(1)
#define AUXR2BOOSTVOL_N9DB		(2)
#define AUXR2BOOSTVOL_N6DB		(3)
#define AUXR2BOOSTVOL_N3DB		(4)
#define AUXR2BOOSTVOL_0DB		(5)
#define AUXR2BOOSTVOL_3DB		(6)
#define AUXR2BOOSTVOL_6DB		(7)

// R49 REG_OUTPUT
#define DACL2RMIX				(1 << 6)
#define DACR2LMIX				(1 << 5)
#define OUT4BOOST				(1 << 4)
#define OUT3BOOST				(1 << 3)
#define SPKBOOST				(1 << 2)
#define TSDEN					(1 << 1)
#define VROI					(1)

// R50 REG_LEFT_MIXER
#define AUXLMIXVOL_POS			(6)
#define AUXLMIXVOL_MASK			(7)
#define AUXLMIXVOL_N15DB		(0)			// Default value
#define AUXLMIXVOL_N12DB		(1 << 6)
#define AUXLMIXVOL_N9DB			(2 << 6)
#define AUXLMIXVOL_N6DB			(3 << 6)
#define AUXLMIXVOL_N3DB			(4 << 6)
#define AUXLMIXVOL_0DB			(5 << 6)
#define AUXLMIXVOL_3DB			(6 << 6)
#define AUXLMIXVOL_6DB			(7 << 6)
#define AUXL2LMIX				(1 << 5)
#define BYPLMIXVOL_POS			(2)
#define BYPLMIXVOL_MASK			(7)
#define BYPLMIXVOL_N15DB		(0)			// Default value
#define BYPLMIXVOL_N12DB		(1 << 2)
#define BYPLMIXVOL_N9DB			(2 << 2)
#define BYPLMIXVOL_N6DB			(3 << 2)
#define BYPLMIXVOL_N3DB			(4 << 2)
#define BYPLMIXVOL_0DB			(5 << 2)
#define BYPLMIXVOL_3DB			(6 << 2)
#define BYPLMIXVOL_6DB			(7 << 2)
#define BYPL2LMIX				(1 << 1)
#define DACL2LMIX				(1)

// R51 REG_RIGHT_MIXER
#define AUXRMIXVOL_POS			(6)
#define AUXRMIXVOL_MASK			(7)
#define AUXRMIXVOL_N15DB		(0)			// Default value
#define AUXRMIXVOL_N12DB		(1 << 6)
#define AUXRMIXVOL_N9DB			(2 << 6)
#define AUXRMIXVOL_N6DB			(3 << 6)
#define AUXRMIXVOL_N3DB			(4 << 6)
#define AUXRMIXVOL_0DB			(5 << 6)
#define AUXRMIXVOL_3DB			(6 << 6)
#define AUXRMIXVOL_6DB			(7 << 6)
#define AUXR2RMIX				(1 << 5)
#define BYPRMIXVOL_POS			(2)
#define BYPRMIXVOL_MASK			(7)
#define BYPRMIXVOL_N15DB		(0)			// Default value
#define BYPRMIXVOL_N12DB		(1 << 2)
#define BYPRMIXVOL_N9DB			(2 << 2)
#define BYPRMIXVOL_N6DB			(3 << 2)
#define BYPRMIXVOL_N3DB			(4 << 2)
#define BYPRMIXVOL_0DB			(5 << 2)
#define BYPRMIXVOL_3DB			(6 << 2)
#define BYPRMIXVOL_6DB			(7 << 2)
#define BYPR2RMIX				(1 << 1)
#define DACR2RMIX				(1)

// R52 - R55 REG_LOUT1_VOL - REG_ROUT2_VOL
#define HPVU					(1 << 8)
#define SPKVU					(1 << 8)
#define LOUT1ZC					(1 << 7)
#define LOUT1MUTE				(1 << 6)
#define ROUT1ZC					(1 << 7)
#define ROUT1MUTE				(1 << 6)
#define LOUT2ZC					(1 << 7)
#define LOUT2MUTE				(1 << 6)
#define ROUT2ZC					(1 << 7)
#define ROUT2MUTE				(1 << 6)
#define VOL_POS					(0)
#define VOL_MASK				(63)

// R56 REG_OUT3_MIXER
#define OUT3MUTE				(1 << 6)
#define OUT4_2OUT3				(1 << 3)
#define BYPL2OUT3				(1 << 2)
#define LMIX2OUT3				(1 << 1)
#define LDAC2OUT3				(1)

// R57 REG_OUT4_MIXER
#define OUT4MUTE				(1 << 6)
#define HALFSIG					(1 << 5)
#define LMIX2OUT4				(1 << 4)
#define LDAC2OUT4				(1 << 3)
#define BYPR2OUT4				(1 << 2)
#define RMIX2OUT4				(1 << 1)
#define RDAC2OUT4				(1)


/* Device Control Commands */
#define CODEC_CMD_RESET			0
#define CODEC_CMD_VOLUME		1
#define CODEC_CMD_SAMPLERATE	2
#define CODEC_CMD_EQ			3
#define CODEC_CMD_3D			4

#define CODEC_VOLUME_MAX		(63)

struct codec_eq_args
{
    uint8_t channel;
    uint8_t frequency;
    uint8_t gain;
    uint8_t mode_bandwidth;
};
typedef struct codec_eq_args* codec_eq_args_t;

extern rt_err_t codec_hw_init(const char * i2c_bus_device_name);

#endif	// #ifndef __CODEC_H__
