#ifndef RA8875_H_INCLUDED
#define RA8875_H_INCLUDED

/* RA8875 register list */
#define PWRR            0x01    /* Power and Display Control Register */
#define MRWC            0x02    /* Memory Read/Write Command */
#define PCSR            0x04    /* Pixel Clock Setting Register */
#define SROC            0x05    /* Serial Flash/ROM Configuration Register */
#define SFCLR           0x06    /* Serial Flash/ROM CLK Setting Register */
#define SYSR            0x10    /* System Configuration Register */

#define MRCD            0x45    /* Memory Read Cursor Direction. */

#define CURH0           0x46    /* Memory Write Cursor Horizontal Position Register 0 */
#define CURH1           0x47    /* Memory Write Cursor Horizontal Position Register 1 */
#define CURV0           0x48    /* Memory Write Cursor Vertical Position Register 0 */
#define CURV1           0x49    /* Memory Write Cursor Vertical Position Register 1 */

#define RCURH0          0x4A    /* Memory read Cursor Horizontal Position Register 0 */
#define RCURH1          0x4B    /* Memory read Cursor Horizontal Position Register 1 */
#define RCURV0          0x4C    /* Memory read Cursor Vertical Position Register 0 */
#define RCURV1          0x4D    /* Memory read Cursor Vertical Position Register 1 */

#define TEST            0x00    /*  */

#endif // RA8875_H_INCLUDED
