/*
 * ST7735.h
 *
 * Created: 2025-10-12 19:09:18
 *  Author: Saulius
 */ 

#ifndef ST7735_H_
#define ST7735_H_

// Display dimensions
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

// Maximum number of characters on screen (5x7 font + 1 pixel gap)
#define MAX_TEXT_LENGTH_HORIZONTAL 21  // Up to 21 characters per horizontal line
#define MAX_TEXT_LENGTH_VERTICAL   20  // Up to 20 lines vertically

// Starting line for the text buffer
#define TEXT_BUFFER_START_LINE 3

// Text alignment options
typedef enum {
	ALIGN_LEFT,   /**< Left alignment. */
	ALIGN_CENTER, /**< Center alignment. */
	ALIGN_RIGHT   /**< Right alignment. */
} alignment_t;

// LCD control pin macros
#define DC_LOW()   PORTD.OUTCLR = PIN2_bm
#define DC_HIGH()  PORTD.OUTSET = PIN2_bm
#define RES_LOW()  PORTD.OUTCLR = PIN3_bm
#define RES_HIGH() PORTD.OUTSET = PIN3_bm
#define CS_LOW()   PORTA.OUTCLR = PIN7_bm
#define CS_HIGH()  PORTA.OUTSET = PIN7_bm

// Common color definitions (RGB565 format)
#define ALICE_BLUE  0xefbf
#define AZURE       0xefff
#define BURLYWOOD   0xddb0
#define CORAL       0xfbea
#define CRIMSON     0xd8a7
#define MISTYROSE   0xff1b
#define LIGHT_BLUE  0xaebc
#define BLACK       0x0000
#define WHITE       0xFFFF
#define LIGHT_GRAY  0x7BEF
#define DARK_GRAY   0x39E7
#define DARK_GREEN  0x01a0
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
#define DARK_RED    0x5000

// Structure representing a single character with its foreground color
typedef struct {
	char character;      // Character itself
	uint16_t fg_color;   // Foreground color of the character
	// Background color is passed separately to drawing functions, not buffered
} ColouredChar;

#endif /* ST7735_H_ */
