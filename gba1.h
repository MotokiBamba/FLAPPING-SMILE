#ifndef _GBA1_H_
#define _GBA1_H_

typedef unsigned short hword;		// Half word (2 bytes)
typedef unsigned char  byte;		// Byte (1 byte)

typedef struct {
	hword x;		/* x coordinate for point */
	hword y;		/* y coordinate for point */
} point;


#define VRAM		0x06000000	// Video RAM frame buffer
#define IOBASE		0x04000000	// I/O register base address

#define LCD_WIDTH		240		// 240 dots / horizontal line
#define LCD_HEIGHT		160		// 160 dots / vertical line
#define LCD_CHAR_WIDTH	30		// width for character unit
#define LCD_CHAR_HEIGHT	20		// height for character unit
#define CHAR_WIDTH		8		// character width
#define CHAR_HEIGHT		8		// character height

#define BGR(r, g, b)	(((b) << 10) + ((g) << 5) + (r))

#define KEY_STATUS	(IOBASE + 304)	// Key status
#define KEY_L		0x0200		// L
#define KEY_R		0x0100		// R
#define KEY_DOWN	0x0080		// Down
#define KEY_UP		0x0040		// Up
#define KEY_LEFT	0x0020		// Left
#define KEY_RIGHT	0x0010		// Right
#define KEY_START	0x0008		// Start
#define KEY_SELECT	0x0004		// Select
#define KEY_B		0x0002		// B
#define KEY_A		0x0001		// A
#define KEY_ALL		0x03FF		// All key bits


#endif
