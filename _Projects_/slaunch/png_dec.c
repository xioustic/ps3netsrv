#include "include/png_dec.h"
#include "include/mem.h"
#include "include/vsh_exports.h"

static int32_t png_w = 0, png_h = 0;

static int32_t create_decoder(png_dec_info *dec_ctx);
static int32_t open_png(png_dec_info *dec_ctx, const char *file_path);
static int32_t set_dec_param(png_dec_info	*dec_ctx);
static int32_t decode_png_stream(png_dec_info	*dec_ctx, void *buf);
static void* cb_malloc(uint32_t size, void *cb_malloc_arg);
static int32_t cb_free(void *ptr, void *cb_free_arg);

/***********************************************************************
* create png decoder
***********************************************************************/
static int32_t create_decoder(png_dec_info *dec_ctx)
{
	CellPngDecThreadInParam   in;
	CellPngDecThreadOutParam  out;

	// set params
	dec_ctx->cb_arg.mallocCallCounts =
	dec_ctx->cb_arg.freeCallCounts	 = 0;

	in.spuThreadEnable		= CELL_PNGDEC_SPU_THREAD_DISABLE;   // ppu only
	in.ppuThreadPriority	= 512;
	in.spuThreadPriority	= 200;
	in.cbCtrlMallocFunc		= cb_malloc;
	in.cbCtrlMallocArg		= &dec_ctx->cb_arg;
	in.cbCtrlFreeFunc		= cb_free;
	in.cbCtrlFreeArg		= &dec_ctx->cb_arg;

	// create png decoder
	return PngDecCreate(&dec_ctx->main_h, &in, &out);
}

/***********************************************************************
* open png stream
***********************************************************************/
static int32_t open_png(png_dec_info *dec_ctx, const char *file_path)
{
	CellPngDecSrc      src;
	CellPngDecOpnInfo  info;

	// set stream source
	src.srcSelect  = CELL_PNGDEC_FILE;        // source is file
	src.fileName   = file_path;               // path to source file
	src.fileOffset =
	src.fileSize   =
	src.streamSize = 0;
	src.streamPtr  = NULL;

	// spu thread disable
	src.spuThreadEnable = CELL_PNGDEC_SPU_THREAD_DISABLE;

	// open stream
	return PngDecOpen(dec_ctx->main_h, &dec_ctx->sub_h, &src, &info);
}

/***********************************************************************
* set decode parameter
***********************************************************************/
static int32_t set_dec_param(png_dec_info	*dec_ctx)
{
	CellPngDecInfo      info;
	CellPngDecInParam   in;
	CellPngDecOutParam  out;

	// read png header
	if(PngDecReadHeader(dec_ctx->main_h, dec_ctx->sub_h, &info)!=CELL_OK) return -1;

	png_w = info.imageWidth;
	png_h = info.imageHeight;
	if(!png_w || !png_h || (png_w!=1920 && png_w*png_h*4>MAX_WH4)) return -1;

	// set decoder parameter
	in.commandPtr		= NULL;
	in.outputMode		= CELL_PNGDEC_TOP_TO_BOTTOM;
	in.outputColorSpace	= CELL_PNGDEC_ARGB;        // ps3 framebuffer is ARGB
	in.outputBitDepth	= 8;
	in.outputPackFlag	= CELL_PNGDEC_1BYTE_PER_1PIXEL;

	if((info.colorSpace == CELL_PNGDEC_GRAYSCALE_ALPHA) ||
	   (info.colorSpace == CELL_PNGDEC_RGBA) ||
	   (info.chunkInformation & 0x10))
	{
		in.outputAlphaSelect = CELL_PNGDEC_STREAM_ALPHA;
	}
	else
	{
		in.outputAlphaSelect = CELL_PNGDEC_FIX_ALPHA;
	}
	in.outputColorAlpha = 0xFF;

	return PngDecSetParameter(dec_ctx->main_h, dec_ctx->sub_h, &in, &out);
}

/***********************************************************************
* decode png stream
***********************************************************************/
static int32_t decode_png_stream(png_dec_info	*dec_ctx, void *buf)
{
	uint8_t *out;
	CellPngDecDataCtrlParam  param;
	CellPngDecDataOutInfo    info;

	param.outputBytesPerLine = png_w * 4;
	out = (void*)buf;

	// decode png...
	return PngDecDecodeData(dec_ctx->main_h, dec_ctx->sub_h, out, &param, &info);
}

/***********************************************************************
* malloc callback
***********************************************************************/
static void *cb_malloc(uint32_t size, void *cb_malloc_arg)
{
	Cb_Arg *arg;
	arg = (Cb_Arg *)cb_malloc_arg;
	arg->mallocCallCounts++;
	return malloc(size);
}

/***********************************************************************
* free callback
***********************************************************************/
static int32_t cb_free(void *ptr, void *cb_free_arg)
{
	Cb_Arg *arg;
	arg = (Cb_Arg *)cb_free_arg;
	arg->freeCallCounts++;
	free(ptr);
	return 0;
}


/***********************************************************************
* decode png file
* const char *file_path  =  path to png file e.g. "/dev_hdd0/test.png"
***********************************************************************/
Buffer load_png(const char *file_path, void* buf_addr)
{
	Buffer tmp;
	png_dec_info dec_ctx;          // decryption handles
	tmp.addr = (uint32_t*)buf_addr;

	tmp.b=1; // transparency
	tmp.w=tmp.h=
	tmp.x=tmp.y=
	png_w=png_h=0;

	// create png decoder
	if(create_decoder(&dec_ctx)==CELL_OK)
	{
		// open png stream
		if(open_png(&dec_ctx, file_path)==CELL_OK)
		{
			// set decode parameter
			if(set_dec_param(&dec_ctx)==CELL_OK)
			{
				// decode png stream, into target buffer
				decode_png_stream(&dec_ctx, buf_addr);
				tmp.w = png_w;
				tmp.h = png_h;
			}

			// close png stream
			PngDecClose(dec_ctx.main_h, dec_ctx.sub_h);
		}

		// destroy png decoder
		PngDecDestroy(dec_ctx.main_h);
	}

	return tmp;
}
