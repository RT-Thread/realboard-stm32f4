#ifndef RA8875_H_INCLUDED
#define RA8875_H_INCLUDED

/* LCD config */
// #define USE_REGISTER_TEST
// #define USE_GRAM_TEST
#define USE_DRAW_FUNCTION

/* RA8875 register list */
#define PWRR            0x01    /* Power and Display Control Register */
#define MRWC            0x02    /* Memory Read/Write Command */
#define PCSR            0x04    /* Pixel Clock Setting Register */
#define SROC            0x05    /* Serial Flash/ROM Configuration Register */
#define SFCLR           0x06    /* Serial Flash/ROM CLK Setting Register */
#define SYSR            0x10    /* System Configuration Register */

#define MWCR0           0x40    /* Memory Write Control Register0 */
#define MWCR1           0x41    /* Memory Write Control Register1 */

#define MRCD            0x45    /* Memory Read Cursor Direction. */

#define CURH0           0x46    /* Memory Write Cursor Horizontal Position Register 0 */
#define CURH1           0x47    /* Memory Write Cursor Horizontal Position Register 1 */
#define CURV0           0x48    /* Memory Write Cursor Vertical Position Register 0 */
#define CURV1           0x49    /* Memory Write Cursor Vertical Position Register 1 */

#define RCURH0          0x4A    /* Memory read Cursor Horizontal Position Register 0 */
#define RCURH1          0x4B    /* Memory read Cursor Horizontal Position Register 1 */
#define RCURV0          0x4C    /* Memory read Cursor Vertical Position Register 0 */
#define RCURV1          0x4D    /* Memory read Cursor Vertical Position Register 1 */

#define FGCR0           0x63    /* Foreground Color Register 0 : red */
#define FGCR1           0x64    /* Foreground Color Register 1 : green */
#define FGCR2           0x65    /* Foreground Color Register 2 : blue */

#define GCHP0           0x80    /* Graphic Cursor Horizontal Position Register 0 */
#define GCHP1           0x81    /* Graphic Cursor Horizontal Position Register 1 */
#define GCVP0           0x82    /* Graphic Cursor Vertical Position Register 0 */
#define GCVP1           0x83    /* Graphic Cursor Vertical Position Register 1 */

#define GCC0            0x84    /* Graphic Cursor Color 0 */
#define GCC1            0x85    /* Graphic Cursor Color 1 */

#define LCD_DCR         0x90    /* Draw Line/Circle/Square Control Register */

#define DLHSR0          0x91    /* Draw Line/Square Horizontal Start Address Register0 */
#define DLHSR1          0x92    /* Draw Line/Square Horizontal Start Address Register1 */
#define DLVSR0          0x93    /* Draw Line/Square Vertical Start Address Register0 */
#define DLVSR1          0x94    /* Draw Line/Square Vertical Start Address Register1 */

#define DLHER0          0x95    /* Draw Line/Square Horizontal End Address Register0 */
#define DLHER1          0x96    /* Draw Line/Square Horizontal End Address Register1 */
#define DLVER0          0x97    /* Draw Line/Square Vertical End Address Register0 */
#define DLVER1          0x98    /* Draw Line/Square Vertical End Address Register1 */

#define DCHR0           0x99    /* Draw Circle Center Horizontal Address Register0 */
#define DCHR1           0x9A    /* Draw Circle Center Horizontal Address Register1 */
#define DCVR0           0x9B    /* Draw Circle Center Vertical Address Register0 */
#define DCVR1           0x9C    /* Draw Circle Center Vertical Address Register1 */
#define DCRR            0x9D    /* Draw Circle Radius Register */

#define DECR            0xA0    /* Draw Ellipse/Ellipse Curve/Circle Square Control Register */

#define ELL_A0          0xA1    /* Draw Ellipse/Circle Square Long axis Setting Register0 */
#define ELL_A1          0xA2    /* Draw Ellipse/Circle Square Long axis Setting Register1 */
#define ELL_B0          0xA3    /* Draw Ellipse/Circle Square Short axis Setting Register0 */
#define ELL_B1          0xA4    /* Draw Ellipse/Circle Square Short axis Setting Register1 */

#define DEHR0           0xA5    /* Draw Ellipse/Circle Square Center Horizontal Address Register0 */
#define DEHR1           0xA6    /* Draw Ellipse/Circle Square Center Horizontal Address Register1 */
#define DEVR0           0xA7    /* Draw Ellipse/Circle Square Center Vertical Address Register0 */
#define DEVR1           0xA8    /* Draw Ellipse/Circle Square Center Vertical Address Register1 */

#define DTPH0           0xA9    /* Draw Triangle Point 2 Horizontal Address Register0 */
#define DTPH1           0xAA    /* Draw Triangle Point 2 Horizontal Address Register1 */
#define DTPV0           0xAB    /* Draw Triangle Point 2 Vertical Address Register0 */
#define DTPV1           0xAC    /* Draw Triangle Point 2 Vertical Address Register1 */

#define DCR_DRAW0_LINE_SQUARE                   (0<<0)
#define DCR_DRAW0_TRIANGLE                      (1<<0)
#define DCR_DRAW1_LINE                          (0<<4)
#define DCR_DRAW1_SQUARE                        (1<<4)
#define DCR_DRAW2_NO_FILL                       (0<<5)
#define DCR_DRAW2_FILL                          (1<<5)
#define DCR_DRAW3_CIRCLE                        (1<<6)
#define DCR_DRAW3_LINE_SQUARE_TRIANGLE          (1<<7)

#define DECR_DRAW0_ALL                          (0<<0)
#define DECR_DRAW0_DECP_LEFT_DOWN               (0<<0)
#define DECR_DRAW0_DECP_LEFT_UP                 (1<<0)
#define DECR_DRAW0_DECP_RIGHT_UP                (2<<0)
#define DECR_DRAW0_DECP_RIGHT_DOWN              (3<<0)
#define DECR_DRAW1_ELLIPSE                      (0<<4)
#define DECR_DRAW1_ELLIPSE_CURVE                (1<<4)
#define DECR_DRAW2_ELLIPSE                      (0<<5)
#define DECR_DRAW2_CIRCLE_SQUARE                (1<<5)
#define DECR_DRAW3_NO_FILL                      (0<<6)
#define DECR_DRAW3_FILL                         (1<<6)
#define DECR_DRAW4_ELLIPSE_CIRCLE_SQUARE        (1<<7)

#define TEST            0x00    /*  */

#endif // RA8875_H_INCLUDED
