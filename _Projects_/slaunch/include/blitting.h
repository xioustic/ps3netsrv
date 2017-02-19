#ifndef __BLITT_H__
#define __BLITT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "vsh_exports.h"


// font constants
#define FONT_W         20.f            // default font width
#define FONT_H         20.f            // default font height
#define FONT_WEIGHT    1.f             // default font weight
#define FONT_CACHE_MAX 512             // max glyph cache count

// canvas constants
#define BASE          0xC0000000UL     // local memory base ea
#define CANVAS_W      1920              // canvas width in pixel
#define CANVAS_H      1080              // canvas height in pixel

#define ISHD(a)       (a == 1920)

#define IMG_MAX			 11             // bg + 10 image slots
#define MAX_W			336
#define MAX_H			336
#define MAX_WH4			(MAX_W * MAX_H * 4)

#define CENTER_TEXT  -1


// get pixel offset into framebuffer by x/y coordinates
/*#define OFFSET(x, y) (uint32_t)((((uint32_t)offset) + ((((int16_t)x) + \
                     (((int16_t)y) * (((uint32_t)pitch) / \
                     ((int32_t)4)))) * ((int32_t)4))) + (BASE))
*/
/*
******** 1920 0x2000 (8192) pitch
** ** ** 1280 0x1400 (5120)
*  *  *   720 0x0C00 (3072)
*/
#define OFFSET_1080p(x, y) ( (uint32_t) ( BASE_offset + (	( ((x)<<2) + (y) * 8192) ) ) )
#define OFFSET_720p(x, y)  ( (uint32_t) ( BASE_offset + (	(( ((((x)* 2)/3)<<2) + (((y)* 2)/ 3) * 5120) ) & 0xfffffff8) ) )
#define OFFSET_576p(x, y)  ( (uint32_t) ( BASE_offset + (	(( ((((x)* 3)/8)<<2) + (((y)* 8)/15) * 3072) ) & 0xfffffff8) ) )
#define OFFSET_480p(x, y)  ( (uint32_t) ( BASE_offset + (	(( ((((x)* 3)/8)<<2) + (((y)* 4)/ 9) * 3072) ) & 0xfffffff8) ) )

//#define OFFSET(x, y) OFFSET_1080p(x, y)
//#define OFFSET(x, y) OFFSET_720p(x, y)
//#define OFFSET(x, y) OFFSET_576p(x, y)
//#define OFFSET(x, y) OFFSET_480p(x, y)
#define OFFSET(x, y) ( (uint32_t) ( BASE_offset + (h==1080?((	( ((x)<<2) + (y) * 8192) )):(h==720?((	(( ((((x)* 2)/3)<<2) + (((y)* 2)/ 3) * 5120) ) & 0xfffffff8)):(h==576?((	(( ((((x)* 3)/8)<<2) + (((y)* 8)/15) * 3072) ) & 0xfffffff8)):((	(( ((((x)* 3)/8)<<2) + (((y)* 4)/ 9) * 3072) ) & 0xfffffff8))))) ) )

extern int32_t LINE_HEIGHT;

// graphic buffers
typedef struct _Buffer {
	uint32_t *addr;                // buffer address
	uint32_t  w;                   // buffer width
	uint32_t  h;                   // buffer height
	uint32_t  x;
	uint32_t  y;
	uint8_t	  b;
} Buffer;

// font cache
typedef struct _Glyph {
	uint32_t code;                           // char unicode
	CellFontGlyphMetrics metrics;            // glyph metrics
	uint16_t w;                              // image width
	uint16_t h;                              // image height
	uint8_t *image;                          // addr -> image data
} Glyph;

typedef struct _Bitmap {
	CellFontHorizontalLayout horizontal_layout;   // struct -> horizontal text layout info
	float font_w, font_h;                         // char w/h
	float_t weight, slant;                        // line weight and char slant
	int32_t distance;                             // distance between chars
	int32_t count;                                // count of current cached glyphs
	int32_t max;                                  // max glyph into this cache
	Glyph glyph[FONT_CACHE_MAX];                  // glyph struct
} Bitmap;

// drawing context
typedef struct _DrawCtx {
	uint32_t *canvas;             // addr of canvas
	uint32_t *menu;               // addr of bottom menu stip
	uint32_t *imgs;               // addr of images
	uint32_t *font_cache;         // addr of glyph bitmap cache buffer
	CellFont font;
	CellFontRenderer renderer;
	Buffer   img[IMG_MAX];        // bitmaps
	uint32_t bg_color;            // background color
	uint32_t fg_color;            // foreground color
} DrawCtx;

DrawCtx ctx;                      // drawing context

void font_finalize(void);
void init_graphic(void);
int32_t load_img_bitmap(int32_t idx, const char *path);
void flip_frame(uint64_t *canvas);
void dim_bg(float ds, float de);
void dim_img(float dim);
void dump_bg(void);
void set_background_color(uint32_t color);
void set_foreground_color(uint32_t color);
void set_font(float_t font_w, float_t font_h, float_t weight, int32_t distance);
void draw_background(void);
int32_t print_text(uint32_t *texture, int32_t x, int32_t y, const char *str);
int32_t draw_png(int32_t idx, int32_t c_x, int32_t c_y, int32_t p_x, int32_t p_y, int32_t w, int32_t h);
void set_texture_direct(uint32_t *texture, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t width2);
void set_texture(uint8_t idx, uint32_t x, uint32_t y);
void set_backdrop(uint8_t idx, uint8_t restore);
void set_frame(uint8_t idx, uint64_t color);
void set_textbox(uint64_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

//void draw_pixel(int32_t x, int32_t y);
//void draw_line(int32_t x, int32_t y, int32_t x2, int32_t y2);
//void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
//void draw_circle(int32_t x_c, int32_t y_c, int32_t r);


#endif // __BLITT_H__
