/*
 * ST7735.h
 *
 * Created: 2025-10-12 19:09:18
 *  Author: Saulius
 */ 


#ifndef ST7735_H_
#define ST7735_H_

#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

#define MAX_TEXT_LENGTH_HORIZONTAL 21 //128x160 screen 21 symbols (5x7 font +1 for gap in bottom and side) in one line (horizontal) and 20 vertical
#define MAX_TEXT_LENGTH_VERTICAL 20

#define TEXT_BUFFER_START_LINE 3   // Nuo kurios eilutës prasideda bufferis

typedef enum {
	ALIGN_LEFT,   /**< Left alignment. */
	ALIGN_CENTER, /**< Center alignment. */
	ALIGN_RIGHT  /**< Right alignment. */
} alignment_t;

#define DC_LOW()   PORTD.OUTCLR = PIN2_bm
#define DC_HIGH()  PORTD.OUTSET = PIN2_bm
#define RES_LOW()  PORTD.OUTCLR = PIN3_bm
#define RES_HIGH() PORTD.OUTSET = PIN3_bm
#define CS_LOW()   PORTA.OUTCLR = PIN7_bm
#define CS_HIGH()  PORTA.OUTSET = PIN7_bm

#define ALICE_BLUE  0xefbf
#define AZURE		0xefff
#define BURLYWOOD	0xddb0
#define CORAL		0xfbea
#define CRIMSON		0xd8a7
#define MISTYROSE	0xff1b
#define LIGHT_BLUE	0xaebc
#define BLACK       0x0000
#define WHITE       0xFFFF
#define LIGHT_GRAY  0x7BEF
#define DARK_GRAY   0x39E7
#define DARK_GREEN	0x01a0
#define RED         0xF800
#define GREEN       0x07E0
#define BLUE        0x001F
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define ORANGE      0xFC00
#define BROWN       0xA145
#define PURPLE      0x780F
#define NAVY        0x000F
#define TEAL        0x0410
#define DARK_RED	0x5000

// Nauja struktûra vienam simboliui su spalva
typedef struct {
	char character;
	uint16_t fg_color;
	// Bendrasis fono spalvos (bg) parametras perduodamas funkcijoms, nebuferizuojamas
} ColouredChar;

#endif /* ST7735_H_ */