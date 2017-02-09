#ifndef _JPG_DEC_H_
#define _JPG_DEC_H_

#include <stdlib.h>
#include <string.h>
#include <cell/codec/jpgdec.h>

#include "blitting.h"


// memory callback's
typedef struct{
	uint32_t mallocCallCounts;
	uint32_t freeCallCounts;
} Cb_Arg_J;

// decoder context
typedef struct{
	CellJpgDecMainHandle main_h;             // decoder
	CellJpgDecSubHandle sub_h;               // stream
	Cb_Arg_J cb_arg_j;                       // callback arg
} jpg_dec_info;

Buffer load_jpg(const char *file_path, void* buf_addr);

#endif // _JPG_DEC_H_
