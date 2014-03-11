#ifndef SSD1963_H_INCLUDED
#define SSD1963_H_INCLUDED

/* LCD color */
#define White            0xFFFF
#define Black            0x0000
#define Grey             0xF7DE
#define Blue             0x001F
#define Blue2            0x051F
#define Red              0xF800
#define Magenta          0xF81F
#define Green            0x07E0
#define Cyan             0x7FFF
#define Yellow           0xFFE0

/*---------------------- Graphic LCD size definitions ------------------------*/
#define LCD_WIDTH       800                 /* Screen Width (in pixels)           */
#define LCD_HEIGHT      480                 /* Screen Hight (in pixels)           */

#define BPP             16                  /* Bits per pixel                     */
#define BYPP            ((BPP+7)/8)         /* Bytes per pixel                    */

#endif // SSD1963_H_INCLUDED
