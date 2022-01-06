/************************************************************************/
/* 定义并实现图像颜色空间转换使用的函数和宏                                                                     */
/************************************************************************/
#include "avcodec.h"
#include "dsputil.h"

#define xglue(x, y) x ## y
#define glue(x, y) xglue(x, y)

#define FF_COLOR_RGB      0 // RGB color space
#define FF_COLOR_GRAY     1 // gray color space
#define FF_COLOR_YUV      2 // YUV color space. 16 <= Y <= 235, 16 <= U, V <= 240
#define FF_COLOR_YUV_JPEG 3 // YUV color space. 0 <= Y <= 255, 0 <= U, V <= 255

#define FF_PIXEL_PLANAR   0 // each channel has one component in AVPicture
#define FF_PIXEL_PACKED   1 // only one components containing all the channels
#define FF_PIXEL_PALETTE  2 // one components containing indexes for a palette

typedef struct PixFmtInfo
{
    const char *name;
    uint8_t nb_channels; // number of channels (including alpha)
    uint8_t color_type; // color type (see FF_COLOR_xxx constants)
    uint8_t pixel_type; // pixel storage type (see FF_PIXEL_xxx constants)
    uint8_t is_alpha; // true if alpha can be specified
    uint8_t x_chroma_shift; // X chroma subsampling factor is 2 ^ shift
    uint8_t y_chroma_shift; // Y chroma subsampling factor is 2 ^ shift
    uint8_t depth; // bit depth of the color components
} PixFmtInfo;

// this table gives more information about formats
static PixFmtInfo pix_fmt_info[PIX_FMT_NB] =
{
    { "yuv420p",  3, FF_COLOR_YUV, FF_PIXEL_PLANAR, 0, 1, 1, 8},
    { "yuv422",   1, FF_COLOR_YUV, FF_PIXEL_PACKED, 0, 1, 0, 8},
    { "rgb24",    3, FF_COLOR_RGB, FF_PIXEL_PACKED, 0, 0, 0, 8},
    { "bgr24",    3, FF_COLOR_RGB, FF_PIXEL_PACKED, 0, 0, 0, 8},
    { "yuv422p",  3, FF_COLOR_YUV, FF_PIXEL_PLANAR, 0, 1, 0, 8},
    { "yuv444p",  3, FF_COLOR_YUV, FF_PIXEL_PLANAR, 0, 0, 0, 8},
    { "rgba32",   4, FF_COLOR_RGB, FF_PIXEL_PACKED, 1, 0, 0, 8},
    { "yuv410p",  3, FF_COLOR_YUV, FF_PIXEL_PLANAR, 0, 2, 2, 8},
    { "yuv411p",  3, FF_COLOR_YUV, FF_PIXEL_PLANAR, 0, 2, 0, 8},
    { "rgb565",   3, FF_COLOR_RGB, FF_PIXEL_PACKED, 0, 0, 0, 5},
    { "rgb555",   4, FF_COLOR_RGB, FF_PIXEL_PACKED, 1, 0, 0, 5},
    { "gray",     1, FF_COLOR_GRAY, FF_PIXEL_PLANAR, 0, 0, 0, 8},
    { "monow",    1, FF_COLOR_GRAY, FF_PIXEL_PLANAR, 0, 0, 0, 1},
    { "monob",    1, FF_COLOR_GRAY, FF_PIXEL_PLANAR, 0, 0, 0, 1},
    { "pal8",     4, FF_COLOR_RGB, FF_PIXEL_PALETTE, 1, 0, 0, 8},
    { "yuvj420p", 3, FF_COLOR_YUV_JPEG, FF_PIXEL_PLANAR, 0, 1, 1, 8},
    { "yuvj422p", 3, FF_COLOR_YUV_JPEG, FF_PIXEL_PLANAR, 0, 1, 0, 8},
    { "yuvj444p", 3, FF_COLOR_YUV_JPEG, FF_PIXEL_PLANAR, 0, 0, 0, 8},
    { "xvmcmc",  },
    { "xvmcidct",},
    { "uyvy422",  1, FF_COLOR_YUV, FF_PIXEL_PACKED, 0, 1, 0, 8},
    { "uyvy411",  1, FF_COLOR_YUV, FF_PIXEL_PACKED, 0, 2, 0, 8},
};

void avcodec_get_chroma_sub_sample(int pix_fmt, int *h_shift, int *v_shift)
{
    *h_shift = pix_fmt_info[pix_fmt].x_chroma_shift;
    *v_shift = pix_fmt_info[pix_fmt].y_chroma_shift;
}

// Picture field are filled with 'ptr' addresses. Also return size
int avpicture_fill(AVPicture *picture, uint8_t *ptr, int pix_fmt, int width, int height)
{
    int size, w2, h2, size2;
    PixFmtInfo *pinfo;
	
    if (avcodec_check_dimensions(NULL, width, height))
        goto fail;
	
    pinfo = &pix_fmt_info[pix_fmt];
    size = width * height;
    switch (pix_fmt)
    {
    case PIX_FMT_YUV420P:
	case PIX_FMT_YUV422P:
	case PIX_FMT_YUV444P:
	case PIX_FMT_YUV410P:
	case PIX_FMT_YUV411P:
	case PIX_FMT_YUVJ420P:
	case PIX_FMT_YUVJ422P:
	case PIX_FMT_YUVJ444P:		
		w2 = (width + (1 << pinfo->x_chroma_shift) - 1) >> pinfo->x_chroma_shift;
		h2 = (height + (1 << pinfo->y_chroma_shift) - 1) >> pinfo->y_chroma_shift;
		size2 = w2 * h2;
		picture->data[0] = ptr;
		picture->data[1] = picture->data[0] + size;
		picture->data[2] = picture->data[1] + size2;
		picture->linesize[0] = width;
		picture->linesize[1] = w2;
		picture->linesize[2] = w2;
		return size + 2 * size2;
	case PIX_FMT_RGB24:
	case PIX_FMT_BGR24:
		picture->data[0] = ptr;
		picture->data[1] = NULL;
		picture->data[2] = NULL;
		picture->linesize[0] = width * 3;
		return size *3;
	case PIX_FMT_RGBA32:
		picture->data[0] = ptr;
		picture->data[1] = NULL;
		picture->data[2] = NULL;
		picture->linesize[0] = width * 4;
		return size *4;
	case PIX_FMT_RGB555:
	case PIX_FMT_RGB565:
	case PIX_FMT_YUV422:
		picture->data[0] = ptr;
		picture->data[1] = NULL;
		picture->data[2] = NULL;
		picture->linesize[0] = width * 2;
		return size *2;
	case PIX_FMT_UYVY422:
		picture->data[0] = ptr;
		picture->data[1] = NULL;
		picture->data[2] = NULL;
		picture->linesize[0] = width * 2;
		return size *2;
	case PIX_FMT_UYVY411:
		picture->data[0] = ptr;
		picture->data[1] = NULL;
		picture->data[2] = NULL;
		picture->linesize[0] = width + width / 2;
		return size + size / 2;
	case PIX_FMT_GRAY8:
		picture->data[0] = ptr;
		picture->data[1] = NULL;
		picture->data[2] = NULL;
		picture->linesize[0] = width;
		return size;
	case PIX_FMT_MONOWHITE:
	case PIX_FMT_MONOBLACK:
		picture->data[0] = ptr;
		picture->data[1] = NULL;
		picture->data[2] = NULL;
		picture->linesize[0] = (width + 7) >> 3;
		return picture->linesize[0] *height;
	case PIX_FMT_PAL8:
		size2 = (size + 3) &~3;
		picture->data[0] = ptr;
		picture->data[1] = ptr + size2; // palette is stored here as 256 32 bit words
		picture->data[2] = NULL;
		picture->linesize[0] = width;
		picture->linesize[1] = 4;
		return size2 + 256 * 4;
	default:
fail: 
		picture->data[0] = NULL;
		picture->data[1] = NULL;
		picture->data[2] = NULL;
		picture->data[3] = NULL;
		return  - 1;
    }
}

int avpicture_get_size(int pix_fmt, int width, int height)
{
    AVPicture dummy_pict;
    return avpicture_fill(&dummy_pict, NULL, pix_fmt, width, height);
}

int avpicture_alloc(AVPicture *picture, int pix_fmt, int width, int height)
{
    unsigned int size;
    void *ptr;
	
    size = avpicture_get_size(pix_fmt, width, height);
    if (size < 0)
        goto fail;
    ptr = av_malloc(size);
    if (!ptr)
        goto fail;
    avpicture_fill(picture, ptr, pix_fmt, width, height);
    return 0;
fail: 
	memset(picture, 0, sizeof(AVPicture));
	return  - 1;
}

void avpicture_free(AVPicture *picture)
{
    av_free(picture->data[0]);
}

static int avg_bits_per_pixel(int pix_fmt)
{
    int bits;
    const PixFmtInfo *pf;
	
    pf = &pix_fmt_info[pix_fmt];
    switch (pf->pixel_type)
    {
	case FF_PIXEL_PACKED:
		switch (pix_fmt)
		{
		case PIX_FMT_YUV422:
		case PIX_FMT_UYVY422:
		case PIX_FMT_RGB565:
		case PIX_FMT_RGB555:
			bits = 16;
			break;
		case PIX_FMT_UYVY411:
			bits = 12;
			break;
		default:
			bits = pf->depth *pf->nb_channels;
			break;
		}
		break;
    case FF_PIXEL_PLANAR:
        if (pf->x_chroma_shift == 0 && pf->y_chroma_shift == 0)
        {
            bits = pf->depth *pf->nb_channels;
        }
        else
        {
            bits = pf->depth + ((2 *pf->depth) >> (pf->x_chroma_shift + pf->y_chroma_shift));
        }
        break;
    case FF_PIXEL_PALETTE:
        bits = 8;
        break;
    default:
        bits =  - 1;
        break;
    }
    return bits;
}

////////////////////////////

void ff_img_copy_plane(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    if ((!dst) || (!src))
        return ;

    for (; height > 0; height--)
    {
        memcpy(dst, src, width);
        dst += dst_wrap;
        src += src_wrap;
    }
}

void img_copy(AVPicture *dst, const AVPicture *src, int pix_fmt, int width, int height)
{
    int bwidth, bits, i;
    PixFmtInfo *pf = &pix_fmt_info[pix_fmt];
	
    pf = &pix_fmt_info[pix_fmt];
    switch (pf->pixel_type)
    {
    case FF_PIXEL_PACKED:
		switch (pix_fmt)
		{
		case PIX_FMT_YUV422:
		case PIX_FMT_UYVY422:
		case PIX_FMT_RGB565:
		case PIX_FMT_RGB555:
			bits = 16;
			break;
		case PIX_FMT_UYVY411:
			bits = 12;
			break;
		default:
			bits = pf->depth *pf->nb_channels;
			break;
		}
		bwidth = (width *bits + 7) >> 3;
		ff_img_copy_plane(dst->data[0], dst->linesize[0], src->data[0], src->linesize[0], bwidth, height);
		break;
    case FF_PIXEL_PLANAR:
        for (i = 0; i < pf->nb_channels; i++)
        {
            int w, h;
            w = width;
            h = height;
            if (i == 1 || i == 2)
            {
                w >>= pf->x_chroma_shift;
                h >>= pf->y_chroma_shift;
            }
            bwidth = (w *pf->depth + 7) >> 3;
            ff_img_copy_plane(dst->data[i], dst->linesize[i], src->data[i], src->linesize[i], bwidth, h);
        }
        break;
    case FF_PIXEL_PALETTE:
        ff_img_copy_plane(dst->data[0], dst->linesize[0], src->data[0], src->linesize[0], width, height);
        // copy the palette
        ff_img_copy_plane(dst->data[1], dst->linesize[1], src->data[1], src->linesize[1], 4, 256);
        break;
    }
}

static void yuv422_to_yuv420p(AVPicture *dst, const AVPicture *src, int width, int height)
{
    const uint8_t *p,  *p1;
    uint8_t *lum,  *cr,  *cb,  *lum1,  *cr1,  *cb1;
    int w;
	
    p1 = src->data[0];
    lum1 = dst->data[0];
    cb1 = dst->data[1];
    cr1 = dst->data[2];
	
    for (; height >= 1; height -= 2)
    {
        p = p1;
        lum = lum1;
        cb = cb1;
        cr = cr1;
        for (w = width; w >= 2; w -= 2)
        {
            lum[0] = p[0];
            cb[0] = p[1];
            lum[1] = p[2];
            cr[0] = p[3];
            p += 4;
            lum += 2;
            cb++;
            cr++;
        }
        if (w)
        {
            lum[0] = p[0];
            cb[0] = p[1];
            cr[0] = p[3];
            cb++;
            cr++;
        }
        p1 += src->linesize[0];
        lum1 += dst->linesize[0];
        if (height > 1)
        {
            p = p1;
            lum = lum1;
            for (w = width; w >= 2; w -= 2)
            {
                lum[0] = p[0];
                lum[1] = p[2];
                p += 4;
                lum += 2;
            }
            if (w)
            {
                lum[0] = p[0];
            }
            p1 += src->linesize[0];
            lum1 += dst->linesize[0];
        }
        cb1 += dst->linesize[1];
        cr1 += dst->linesize[2];
    }
}

static void uyvy422_to_yuv420p(AVPicture *dst, const AVPicture *src, int width, int height)
{
    const uint8_t *p,  *p1;
    uint8_t *lum,  *cr,  *cb,  *lum1,  *cr1,  *cb1;
    int w;
	
    p1 = src->data[0];
	
    lum1 = dst->data[0];
    cb1 = dst->data[1];
    cr1 = dst->data[2];
	
    for (; height >= 1; height -= 2)
    {
        p = p1;
        lum = lum1;
        cb = cb1;
        cr = cr1;
        for (w = width; w >= 2; w -= 2)
        {
            lum[0] = p[1];
            cb[0] = p[0];
            lum[1] = p[3];
            cr[0] = p[2];
            p += 4;
            lum += 2;
            cb++;
            cr++;
        }
        if (w)
        {
            lum[0] = p[1];
            cb[0] = p[0];
            cr[0] = p[2];
            cb++;
            cr++;
        }
        p1 += src->linesize[0];
        lum1 += dst->linesize[0];
        if (height > 1)
        {
            p = p1;
            lum = lum1;
            for (w = width; w >= 2; w -= 2)
            {
                lum[0] = p[1];
                lum[1] = p[3];
                p += 4;
                lum += 2;
            }
            if (w)
            {
                lum[0] = p[1];
            }
            p1 += src->linesize[0];
            lum1 += dst->linesize[0];
        }
        cb1 += dst->linesize[1];
        cr1 += dst->linesize[2];
    }
}

static void uyvy422_to_yuv422p(AVPicture *dst, const AVPicture *src, int width, int height)
{
    const uint8_t *p,  *p1;
    uint8_t *lum,  *cr,  *cb,  *lum1,  *cr1,  *cb1;
    int w;
	
    p1 = src->data[0];
    lum1 = dst->data[0];
    cb1 = dst->data[1];
    cr1 = dst->data[2];
    for (; height > 0; height--)
    {
        p = p1;
        lum = lum1;
        cb = cb1;
        cr = cr1;
        for (w = width; w >= 2; w -= 2)
        {
            lum[0] = p[1];
            cb[0] = p[0];
            lum[1] = p[3];
            cr[0] = p[2];
            p += 4;
            lum += 2;
            cb++;
            cr++;
        }
        p1 += src->linesize[0];
        lum1 += dst->linesize[0];
        cb1 += dst->linesize[1];
        cr1 += dst->linesize[2];
    }
}

static void yuv422_to_yuv422p(AVPicture *dst, const AVPicture *src, int width, int height)
{
    const uint8_t *p,  *p1;
    uint8_t *lum,  *cr,  *cb,  *lum1,  *cr1,  *cb1;
    int w;
	
    p1 = src->data[0];
    lum1 = dst->data[0];
    cb1 = dst->data[1];
    cr1 = dst->data[2];
    for (; height > 0; height--)
    {
        p = p1;
        lum = lum1;
        cb = cb1;
        cr = cr1;
        for (w = width; w >= 2; w -= 2)
        {
            lum[0] = p[0];
            cb[0] = p[1];
            lum[1] = p[2];
            cr[0] = p[3];
            p += 4;
            lum += 2;
            cb++;
            cr++;
        }
        p1 += src->linesize[0];
        lum1 += dst->linesize[0];
        cb1 += dst->linesize[1];
        cr1 += dst->linesize[2];
    }
}

static void yuv422p_to_yuv422(AVPicture *dst, const AVPicture *src, int width, int height)
{
    uint8_t *p,  *p1;
    const uint8_t *lum,  *cr,  *cb,  *lum1,  *cr1,  *cb1;
    int w;
	
    p1 = dst->data[0];
    lum1 = src->data[0];
    cb1 = src->data[1];
    cr1 = src->data[2];
    for (; height > 0; height--)
    {
        p = p1;
        lum = lum1;
        cb = cb1;
        cr = cr1;
        for (w = width; w >= 2; w -= 2)
        {
            p[0] = lum[0];
            p[1] = cb[0];
            p[2] = lum[1];
            p[3] = cr[0];
            p += 4;
            lum += 2;
            cb++;
            cr++;
        }
        p1 += dst->linesize[0];
        lum1 += src->linesize[0];
        cb1 += src->linesize[1];
        cr1 += src->linesize[2];
    }
}

static void yuv422p_to_uyvy422(AVPicture *dst, const AVPicture *src, int width, int height)
{
    uint8_t *p,  *p1;
    const uint8_t *lum,  *cr,  *cb,  *lum1,  *cr1,  *cb1;
    int w;
	
    p1 = dst->data[0];
    lum1 = src->data[0];
    cb1 = src->data[1];
    cr1 = src->data[2];
    for (; height > 0; height--)
    {
        p = p1;
        lum = lum1;
        cb = cb1;
        cr = cr1;
        for (w = width; w >= 2; w -= 2)
        {
            p[1] = lum[0];
            p[0] = cb[0];
            p[3] = lum[1];
            p[2] = cr[0];
            p += 4;
            lum += 2;
            cb++;
            cr++;
        }
        p1 += dst->linesize[0];
        lum1 += src->linesize[0];
        cb1 += src->linesize[1];
        cr1 += src->linesize[2];
    }
}

static void uyvy411_to_yuv411p(AVPicture *dst, const AVPicture *src, int width, int height)
{
    const uint8_t *p,  *p1;
    uint8_t *lum,  *cr,  *cb,  *lum1,  *cr1,  *cb1;
    int w;
	
    p1 = src->data[0];
    lum1 = dst->data[0];
    cb1 = dst->data[1];
    cr1 = dst->data[2];
    for (; height > 0; height--)
    {
        p = p1;
        lum = lum1;
        cb = cb1;
        cr = cr1;
        for (w = width; w >= 4; w -= 4)
        {
            cb[0] = p[0];
            lum[0] = p[1];
            lum[1] = p[2];
            cr[0] = p[3];
            lum[2] = p[4];
            lum[3] = p[5];
            p += 6;
            lum += 4;
            cb++;
            cr++;
        }
        p1 += src->linesize[0];
        lum1 += dst->linesize[0];
        cb1 += dst->linesize[1];
        cr1 += dst->linesize[2];
    }
}

static void yuv420p_to_yuv422(AVPicture *dst, const AVPicture *src, int width, int height)
{
    int w, h;
    uint8_t *line1,  *line2,  *linesrc = dst->data[0];
    uint8_t *lum1,  *lum2,  *lumsrc = src->data[0];
    uint8_t *cb1,  *cb2 = src->data[1];
    uint8_t *cr1,  *cr2 = src->data[2];
	
    for (h = height / 2; h--;)
    {
        line1 = linesrc;
        line2 = linesrc + dst->linesize[0];
		
        lum1 = lumsrc;
        lum2 = lumsrc + src->linesize[0];
		
        cb1 = cb2;
        cr1 = cr2;
		
        for (w = width / 2; w--;)
        {
            *line1++ =  *lum1++;
            *line2++ =  *lum2++;
            *line1++ =  *line2++ =  *cb1++;
            *line1++ =  *lum1++;
            *line2++ =  *lum2++;
            *line1++ =  *line2++ =  *cr1++;
        }
		
        linesrc += dst->linesize[0] *2;
        lumsrc += src->linesize[0] *2;
        cb2 += src->linesize[1];
        cr2 += src->linesize[2];
    }
}

static void yuv420p_to_uyvy422(AVPicture *dst, const AVPicture *src, int width, int height)
{
    int w, h;
    uint8_t *line1,  *line2,  *linesrc = dst->data[0];
    uint8_t *lum1,  *lum2,  *lumsrc = src->data[0];
    uint8_t *cb1,  *cb2 = src->data[1];
    uint8_t *cr1,  *cr2 = src->data[2];
	
    for (h = height / 2; h--;)
    {
        line1 = linesrc;
        line2 = linesrc + dst->linesize[0];
		
        lum1 = lumsrc;
        lum2 = lumsrc + src->linesize[0];
		
        cb1 = cb2;
        cr1 = cr2;
		
        for (w = width / 2; w--;)
        {
            *line1++ =  *line2++ =  *cb1++;
            *line1++ =  *lum1++;
            *line2++ =  *lum2++;
            *line1++ =  *line2++ =  *cr1++;
            *line1++ =  *lum1++;
            *line2++ =  *lum2++;
        }
		
        linesrc += dst->linesize[0] *2;
        lumsrc += src->linesize[0] *2;
        cb2 += src->linesize[1];
        cr2 += src->linesize[2];
    }
}

#define SCALEBITS 10
#define ONE_HALF  (1 << (SCALEBITS - 1))
#define FIX(x)    ((int) ((x) * (1<<SCALEBITS) + 0.5))

#define YUV_TO_RGB1_CCIR(cb1, cr1)\
{\
	cb = (cb1) - 128;\
	cr = (cr1) - 128;\
	r_add = FIX(1.40200*255.0/224.0) * cr + ONE_HALF;\
	g_add = - FIX(0.34414*255.0/224.0) * cb - FIX(0.71414*255.0/224.0) * cr + \
	ONE_HALF;\
	b_add = FIX(1.77200*255.0/224.0) * cb + ONE_HALF;\
}

#define YUV_TO_RGB2_CCIR(r, g, b, y1)\
{\
	y = ((y1) - 16) * FIX(255.0/219.0);\
	r = cm[(y + r_add) >> SCALEBITS];\
	g = cm[(y + g_add) >> SCALEBITS];\
	b = cm[(y + b_add) >> SCALEBITS];\
}

#define YUV_TO_RGB1(cb1, cr1)\
{\
	cb = (cb1) - 128;\
	cr = (cr1) - 128;\
	r_add = FIX(1.40200) * cr + ONE_HALF;\
	g_add = - FIX(0.34414) * cb - FIX(0.71414) * cr + ONE_HALF;\
	b_add = FIX(1.77200) * cb + ONE_HALF;\
}

#define YUV_TO_RGB2(r, g, b, y1)\
{\
	y = (y1) << SCALEBITS;\
	r = cm[(y + r_add) >> SCALEBITS];\
	g = cm[(y + g_add) >> SCALEBITS];\
	b = cm[(y + b_add) >> SCALEBITS];\
}

#define Y_CCIR_TO_JPEG(y)\
  cm[((y) * FIX(255.0/219.0) + (ONE_HALF - 16 * FIX(255.0/219.0))) >> SCALEBITS]

#define Y_JPEG_TO_CCIR(y)\
  (((y) * FIX(219.0/255.0) + (ONE_HALF + (16 << SCALEBITS))) >> SCALEBITS)

#define C_CCIR_TO_JPEG(y)\
  cm[(((y) - 128) * FIX(127.0/112.0) + (ONE_HALF + (128 << SCALEBITS))) >> SCALEBITS]

/* NOTE: the clamp is really necessary! */
static inline int C_JPEG_TO_CCIR(int y)
{
    y = (((y - 128) *FIX(112.0 / 127.0) + (ONE_HALF + (128 << SCALEBITS))) >> SCALEBITS);
    if (y < 16)
        y = 16;
    return y;
}

#define RGB_TO_Y(r, g, b) \
  ((FIX(0.29900) * (r) + FIX(0.58700) * (g) + \
  FIX(0.11400) * (b) + ONE_HALF) >> SCALEBITS)

#define RGB_TO_U(r1, g1, b1, shift)\
  (((- FIX(0.16874) * r1 - FIX(0.33126) * g1 +         \
  FIX(0.50000) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)

#define RGB_TO_V(r1, g1, b1, shift)\
  (((FIX(0.50000) * r1 - FIX(0.41869) * g1 -           \
  FIX(0.08131) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)

#define RGB_TO_Y_CCIR(r, g, b) \
  ((FIX(0.29900*219.0/255.0) * (r) + FIX(0.58700*219.0/255.0) * (g) + \
  FIX(0.11400*219.0/255.0) * (b) + (ONE_HALF + (16 << SCALEBITS))) >> SCALEBITS)

#define RGB_TO_U_CCIR(r1, g1, b1, shift)\
  (((- FIX(0.16874*224.0/255.0) * r1 - FIX(0.33126*224.0/255.0) * g1 +         \
  FIX(0.50000*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)

#define RGB_TO_V_CCIR(r1, g1, b1, shift)\
  (((FIX(0.50000*224.0/255.0) * r1 - FIX(0.41869*224.0/255.0) * g1 -           \
  FIX(0.08131*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)

static uint8_t y_ccir_to_jpeg[256];
static uint8_t y_jpeg_to_ccir[256];
static uint8_t c_ccir_to_jpeg[256];
static uint8_t c_jpeg_to_ccir[256];

/* apply to each pixel the given table */
static void img_apply_table(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap,
							   int width, int height, const uint8_t *table1)
{
    int n;
    const uint8_t *s;
    uint8_t *d;
    const uint8_t *table;
	
    table = table1;
    for (; height > 0; height--)
    {
        s = src;
        d = dst;
        n = width;
        while (n >= 4)
        {
            d[0] = table[s[0]];
            d[1] = table[s[1]];
            d[2] = table[s[2]];
            d[3] = table[s[3]];
            d += 4;
            s += 4;
            n -= 4;
        }
        while (n > 0)
        {
            d[0] = table[s[0]];
            d++;
            s++;
            n--;
        }
        dst += dst_wrap;
        src += src_wrap;
    }
}

/* XXX: use generic filter ? */
/* XXX: in most cases, the sampling position is incorrect */

/* 4x1 -> 1x1 */
static void shrink41(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    int w;
    const uint8_t *s;
    uint8_t *d;
	
    for (; height > 0; height--)
    {
        s = src;
        d = dst;
        for (w = width; w > 0; w--)
        {
            d[0] = (s[0] + s[1] + s[2] + s[3] + 2) >> 2;
            s += 4;
            d++;
        }
        src += src_wrap;
        dst += dst_wrap;
    }
}

/* 2x1 -> 1x1 */
static void shrink21(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    int w;
    const uint8_t *s;
    uint8_t *d;
	
    for (; height > 0; height--)
    {
        s = src;
        d = dst;
        for (w = width; w > 0; w--)
        {
            d[0] = (s[0] + s[1]) >> 1;
            s += 2;
            d++;
        }
        src += src_wrap;
        dst += dst_wrap;
    }
}

/* 1x2 -> 1x1 */
static void shrink12(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    int w;
    uint8_t *d;
    const uint8_t *s1,  *s2;
	
    for (; height > 0; height--)
    {
        s1 = src;
        s2 = s1 + src_wrap;
        d = dst;
        for (w = width; w >= 4; w -= 4)
        {
            d[0] = (s1[0] + s2[0]) >> 1;
            d[1] = (s1[1] + s2[1]) >> 1;
            d[2] = (s1[2] + s2[2]) >> 1;
            d[3] = (s1[3] + s2[3]) >> 1;
            s1 += 4;
            s2 += 4;
            d += 4;
        }
        for (; w > 0; w--)
        {
            d[0] = (s1[0] + s2[0]) >> 1;
            s1++;
            s2++;
            d++;
        }
        src += 2 * src_wrap;
        dst += dst_wrap;
    }
}

/* 2x2 -> 1x1 */
void ff_shrink22(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    int w;
    const uint8_t *s1,  *s2;
    uint8_t *d;
	
    for (; height > 0; height--)
    {
        s1 = src;
        s2 = s1 + src_wrap;
        d = dst;
        for (w = width; w >= 4; w -= 4)
        {
            d[0] = (s1[0] + s1[1] + s2[0] + s2[1] + 2) >> 2;
            d[1] = (s1[2] + s1[3] + s2[2] + s2[3] + 2) >> 2;
            d[2] = (s1[4] + s1[5] + s2[4] + s2[5] + 2) >> 2;
            d[3] = (s1[6] + s1[7] + s2[6] + s2[7] + 2) >> 2;
            s1 += 8;
            s2 += 8;
            d += 4;
        }
        for (; w > 0; w--)
        {
            d[0] = (s1[0] + s1[1] + s2[0] + s2[1] + 2) >> 2;
            s1 += 2;
            s2 += 2;
            d++;
        }
        src += 2 * src_wrap;
        dst += dst_wrap;
    }
}

/* 4x4 -> 1x1 */
void ff_shrink44(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    int w;
    const uint8_t *s1,  *s2,  *s3,  *s4;
    uint8_t *d;
	
    for (; height > 0; height--)
    {
        s1 = src;
        s2 = s1 + src_wrap;
        s3 = s2 + src_wrap;
        s4 = s3 + src_wrap;
        d = dst;
        for (w = width; w > 0; w--)
        {
            d[0] = (s1[0] + s1[1] + s1[2] + s1[3] + s2[0] + s2[1] + s2[2] + s2[3] + 
				    s3[0] + s3[1] + s3[2] + s3[3] + s4[0] + s4[1] + s4[2] + s4[3] + 8) >> 4;
            s1 += 4;
            s2 += 4;
            s3 += 4;
            s4 += 4;
            d++;
        }
        src += 4 * src_wrap;
        dst += dst_wrap;
    }
}

static void grow21_line(uint8_t *dst, const uint8_t *src, int width)
{
    int w;
    const uint8_t *s1;
    uint8_t *d;
	
    s1 = src;
    d = dst;
    for (w = width; w >= 4; w -= 4)
    {
        d[1] = d[0] = s1[0];
        d[3] = d[2] = s1[1];
        s1 += 2;
        d += 4;
    }
    for (; w >= 2; w -= 2)
    {
        d[1] = d[0] = s1[0];
        s1++;
        d += 2;
    }
    /* only needed if width is not a multiple of two */
    /* XXX: veryfy that */
    if (w)
    {
        d[0] = s1[0];
    }
}

static void grow41_line(uint8_t *dst, const uint8_t *src, int width)
{
    int w, v;
    const uint8_t *s1;
    uint8_t *d;
	
    s1 = src;
    d = dst;
    for (w = width; w >= 4; w -= 4)
    {
        v = s1[0];
        d[0] = v;
        d[1] = v;
        d[2] = v;
        d[3] = v;
        s1++;
        d += 4;
    }
}

/* 1x1 -> 2x1 */
static void grow21(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    for (; height > 0; height--)
    {
        grow21_line(dst, src, width);
        src += src_wrap;
        dst += dst_wrap;
    }
}

/* 1x1 -> 2x2 */
static void grow22(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    for (; height > 0; height--)
    {
        grow21_line(dst, src, width);
        if (height % 2)
            src += src_wrap;
        dst += dst_wrap;
    }
}

/* 1x1 -> 4x1 */
static void grow41(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    for (; height > 0; height--)
    {
        grow41_line(dst, src, width);
        src += src_wrap;
        dst += dst_wrap;
    }
}

/* 1x1 -> 4x4 */
static void grow44(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    for (; height > 0; height--)
    {
        grow41_line(dst, src, width);
        if ((height &3) == 1)
            src += src_wrap;
        dst += dst_wrap;
    }
}

/* 1x2 -> 2x1 */
static void conv411(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap, int width, int height)
{
    int w, c;
    const uint8_t *s1,  *s2;
    uint8_t *d;
	
    width >>= 1;
	
    for (; height > 0; height--)
    {
        s1 = src;
        s2 = src + src_wrap;
        d = dst;
        for (w = width; w > 0; w--)
        {
            c = (s1[0] + s2[0]) >> 1;
            d[0] = c;
            d[1] = c;
            s1++;
            s2++;
            d += 2;
        }
        src += src_wrap * 2;
        dst += dst_wrap;
    }
}

/* XXX: add jpeg quantize code */

#define TRANSP_INDEX (6*6*6)

/* this is maybe slow, but allows for extensions */
static inline unsigned char gif_clut_index(uint8_t r, uint8_t g, uint8_t b)
{
    return ((((r) / 47) % 6) *6 * 6+(((g) / 47) % 6) *6+(((b) / 47) % 6));
}

static void build_rgb_palette(uint8_t *palette, int has_alpha)
{
    uint32_t *pal;
    static const uint8_t pal_value[6] = {0x00, 0x33, 0x66, 0x99, 0xcc, 0xff };
    int i, r, g, b;
	
    pal = (uint32_t*)palette;
    i = 0;
    for (r = 0; r < 6; r++)
    {
        for (g = 0; g < 6; g++)
        {
            for (b = 0; b < 6; b++)
            {
                pal[i++] = (0xff << 24) | (pal_value[r] << 16) | (pal_value[g] << 8) | pal_value[b];
            }
        }
    }
    if (has_alpha)
        pal[i++] = 0;
    while (i < 256)
        pal[i++] = 0xff000000;
}

/* copy bit n to bits 0 ... n - 1 */
static inline unsigned int bitcopy_n(unsigned int a, int n)
{
    int mask;
    mask = (1 << n) - 1;
    return (a &(0xff &~mask)) | (( - ((a >> n) &1)) &mask);
}

/* rgb555 handling */

#define RGB_NAME rgb555

#define RGB_IN(r, g, b, s)\
{\
	unsigned int v = ((const uint16_t *)(s))[0];\
	r = bitcopy_n(v >> (10 - 3), 3);\
	g = bitcopy_n(v >> (5 - 3), 3);\
	b = bitcopy_n(v << 3, 3);\
}

#define RGBA_IN(r, g, b, a, s)\
{\
	unsigned int v = ((const uint16_t *)(s))[0];\
	r = bitcopy_n(v >> (10 - 3), 3);\
	g = bitcopy_n(v >> (5 - 3), 3);\
	b = bitcopy_n(v << 3, 3);\
	a = (-(v >> 15)) & 0xff;\
}

#define RGBA_OUT(d, r, g, b, a)\
{\
	((uint16_t *)(d))[0] = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3) | \
	((a << 8) & 0x8000);\
}

#define BPP 2

#include "imgconvert_template.h"

/* rgb565 handling */

#define RGB_NAME rgb565

#define RGB_IN(r, g, b, s)\
{\
	unsigned int v = ((const uint16_t *)(s))[0];\
	r = bitcopy_n(v >> (11 - 3), 3);\
	g = bitcopy_n(v >> (5 - 2), 2);\
	b = bitcopy_n(v << 3, 3);\
}

#define RGB_OUT(d, r, g, b)\
{\
	((uint16_t *)(d))[0] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);\
}

#define BPP 2

#include "imgconvert_template.h"

/* bgr24 handling */

#define RGB_NAME bgr24

#define RGB_IN(r, g, b, s)\
{\
	b = (s)[0];\
	g = (s)[1];\
	r = (s)[2];\
}

#define RGB_OUT(d, r, g, b)\
{\
	(d)[0] = b;\
	(d)[1] = g;\
	(d)[2] = r;\
}

#define BPP 3

#include "imgconvert_template.h"

#undef RGB_IN
#undef RGB_OUT
#undef BPP

/* rgb24 handling */

#define RGB_NAME rgb24
#define FMT_RGB24

#define RGB_IN(r, g, b, s)\
{\
	r = (s)[0];\
	g = (s)[1];\
	b = (s)[2];\
}

#define RGB_OUT(d, r, g, b)\
{\
	(d)[0] = r;\
	(d)[1] = g;\
	(d)[2] = b;\
}

#define BPP 3

#include "imgconvert_template.h"

/* rgba32 handling */

#define RGB_NAME rgba32
#define FMT_RGBA32

#define RGB_IN(r, g, b, s)\
{\
	unsigned int v = ((const uint32_t *)(s))[0];\
	r = (v >> 16) & 0xff;\
	g = (v >> 8) & 0xff;\
	b = v & 0xff;\
}

#define RGBA_IN(r, g, b, a, s)\
{\
	unsigned int v = ((const uint32_t *)(s))[0];\
	a = (v >> 24) & 0xff;\
	r = (v >> 16) & 0xff;\
	g = (v >> 8) & 0xff;\
	b = v & 0xff;\
}

#define RGBA_OUT(d, r, g, b, a)\
{\
	((uint32_t *)(d))[0] = (a << 24) | (r << 16) | (g << 8) | b;\
}

#define BPP 4

#include "imgconvert_template.h"

static void mono_to_gray(AVPicture *dst, const AVPicture *src, int width, int height, int xor_mask)
{
    const unsigned char *p;
    unsigned char *q;
    int v, dst_wrap, src_wrap;
    int y, w;
	
    p = src->data[0];
    src_wrap = src->linesize[0] - ((width + 7) >> 3);
	
    q = dst->data[0];
    dst_wrap = dst->linesize[0] - width;
    for (y = 0; y < height; y++)
    {
        w = width;
        while (w >= 8)
        {
            v =  *p++ ^ xor_mask;
            q[0] =  - (v >> 7);
            q[1] =  - ((v >> 6) &1);
            q[2] =  - ((v >> 5) &1);
            q[3] =  - ((v >> 4) &1);
            q[4] =  - ((v >> 3) &1);
            q[5] =  - ((v >> 2) &1);
            q[6] =  - ((v >> 1) &1);
            q[7] =  - ((v >> 0) &1);
            w -= 8;
            q += 8;
        }
        if (w > 0)
        {
            v =  *p++ ^ xor_mask;
            do
            {
                q[0] =  - ((v >> 7) &1);
                q++;
                v <<= 1;
            }
            while (--w);
        }
        p += src_wrap;
        q += dst_wrap;
    }
}

static void monowhite_to_gray(AVPicture *dst, const AVPicture *src, int width, int height)
{
    mono_to_gray(dst, src, width, height, 0xff);
}

static void monoblack_to_gray(AVPicture *dst, const AVPicture *src, int width, int height)
{
    mono_to_gray(dst, src, width, height, 0x00);
}

static void gray_to_mono(AVPicture *dst, const AVPicture *src, int width, int height, int xor_mask)
{
    int n;
    const uint8_t *s;
    uint8_t *d;
    int j, b, v, n1, src_wrap, dst_wrap, y;
	
    s = src->data[0];
    src_wrap = src->linesize[0] - width;
	
    d = dst->data[0];
    dst_wrap = dst->linesize[0] - ((width + 7) >> 3);
	
    for (y = 0; y < height; y++)
    {
        n = width;
        while (n >= 8)
        {
            v = 0;
            for (j = 0; j < 8; j++)
            {
                b = s[0];
                s++;
                v = (v << 1) | (b >> 7);
            }
            d[0] = v ^ xor_mask;
            d++;
            n -= 8;
        }
        if (n > 0)
        {
            n1 = n;
            v = 0;
            while (n > 0)
            {
                b = s[0];
                s++;
                v = (v << 1) | (b >> 7);
                n--;
            }
            d[0] = (v << (8-(n1 &7))) ^ xor_mask;
            d++;
        }
        s += src_wrap;
        d += dst_wrap;
    }
}

static void gray_to_monowhite(AVPicture *dst, const AVPicture *src, int width, int height)
{
    gray_to_mono(dst, src, width, height, 0xff);
}

static void gray_to_monoblack(AVPicture *dst, const AVPicture *src, int width, int height)
{
    gray_to_mono(dst, src, width, height, 0x00);
}

typedef struct ConvertEntry
{
    void(*convert)(AVPicture *dst, const AVPicture *src, int width, int height);
} ConvertEntry;

/* Add each new convertion function in this table. In order to be able
to convert from any format to any format, the following constraints must be satisfied:

  - all FF_COLOR_RGB formats must convert to and from PIX_FMT_RGB24
  
  - all FF_COLOR_GRAY formats must convert to and from PIX_FMT_GRAY8
	
  - all FF_COLOR_RGB formats with alpha must convert to and from PIX_FMT_RGBA32
	  
  - PIX_FMT_YUV444P and PIX_FMT_YUVJ444P must convert to and from PIX_FMT_RGB24.
		
  - PIX_FMT_422 must convert to and from PIX_FMT_422P.
		  
  The other conversion functions are just optimisations for common cases.
*/

static ConvertEntry convert_table[PIX_FMT_NB][PIX_FMT_NB];

static void img_convert_init(void)
{
    int i;
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
	
    for (i = 0; i < 256; i++)
    {
        y_ccir_to_jpeg[i] = Y_CCIR_TO_JPEG(i);
        y_jpeg_to_ccir[i] = Y_JPEG_TO_CCIR(i);
        c_ccir_to_jpeg[i] = C_CCIR_TO_JPEG(i);
        c_jpeg_to_ccir[i] = C_JPEG_TO_CCIR(i);
    }
	
    convert_table[PIX_FMT_YUV420P][PIX_FMT_YUV422].convert = yuv420p_to_yuv422;
    convert_table[PIX_FMT_YUV420P][PIX_FMT_YUV422].convert = yuv420p_to_yuv422;
    convert_table[PIX_FMT_YUV420P][PIX_FMT_RGB555].convert = yuv420p_to_rgb555;
    convert_table[PIX_FMT_YUV420P][PIX_FMT_RGB565].convert = yuv420p_to_rgb565;
    convert_table[PIX_FMT_YUV420P][PIX_FMT_BGR24].convert = yuv420p_to_bgr24;
    convert_table[PIX_FMT_YUV420P][PIX_FMT_RGB24].convert = yuv420p_to_rgb24;
    convert_table[PIX_FMT_YUV420P][PIX_FMT_RGBA32].convert = yuv420p_to_rgba32;
    convert_table[PIX_FMT_YUV420P][PIX_FMT_UYVY422].convert = yuv420p_to_uyvy422;
	
    convert_table[PIX_FMT_YUV422P][PIX_FMT_YUV422].convert = yuv422p_to_yuv422;
    convert_table[PIX_FMT_YUV422P][PIX_FMT_UYVY422].convert = yuv422p_to_uyvy422;
	
    convert_table[PIX_FMT_YUV444P][PIX_FMT_RGB24].convert = yuv444p_to_rgb24;
	
    convert_table[PIX_FMT_YUVJ420P][PIX_FMT_RGB555].convert = yuvj420p_to_rgb555;
    convert_table[PIX_FMT_YUVJ420P][PIX_FMT_RGB565].convert = yuvj420p_to_rgb565;
    convert_table[PIX_FMT_YUVJ420P][PIX_FMT_BGR24].convert = yuvj420p_to_bgr24;
    convert_table[PIX_FMT_YUVJ420P][PIX_FMT_RGB24].convert = yuvj420p_to_rgb24;
    convert_table[PIX_FMT_YUVJ420P][PIX_FMT_RGBA32].convert = yuvj420p_to_rgba32;
	
    convert_table[PIX_FMT_YUVJ444P][PIX_FMT_RGB24].convert = yuvj444p_to_rgb24;
	
    convert_table[PIX_FMT_YUV422][PIX_FMT_YUV420P].convert = yuv422_to_yuv420p;
    convert_table[PIX_FMT_YUV422][PIX_FMT_YUV422P].convert = yuv422_to_yuv422p;
	
    convert_table[PIX_FMT_UYVY422][PIX_FMT_YUV420P].convert = uyvy422_to_yuv420p;
    convert_table[PIX_FMT_UYVY422][PIX_FMT_YUV422P].convert = uyvy422_to_yuv422p;
	
    convert_table[PIX_FMT_RGB24][PIX_FMT_YUV420P].convert = rgb24_to_yuv420p;
    convert_table[PIX_FMT_RGB24][PIX_FMT_RGB565].convert = rgb24_to_rgb565;
    convert_table[PIX_FMT_RGB24][PIX_FMT_RGB555].convert = rgb24_to_rgb555;
    convert_table[PIX_FMT_RGB24][PIX_FMT_RGBA32].convert = rgb24_to_rgba32;
    convert_table[PIX_FMT_RGB24][PIX_FMT_BGR24].convert = rgb24_to_bgr24;
    convert_table[PIX_FMT_RGB24][PIX_FMT_GRAY8].convert = rgb24_to_gray;
    convert_table[PIX_FMT_RGB24][PIX_FMT_PAL8].convert = rgb24_to_pal8;
    convert_table[PIX_FMT_RGB24][PIX_FMT_YUV444P].convert = rgb24_to_yuv444p;
    convert_table[PIX_FMT_RGB24][PIX_FMT_YUVJ420P].convert = rgb24_to_yuvj420p;
    convert_table[PIX_FMT_RGB24][PIX_FMT_YUVJ444P].convert = rgb24_to_yuvj444p;
	
    convert_table[PIX_FMT_RGBA32][PIX_FMT_RGB24].convert = rgba32_to_rgb24;
    convert_table[PIX_FMT_RGBA32][PIX_FMT_RGB555].convert = rgba32_to_rgb555;
    convert_table[PIX_FMT_RGBA32][PIX_FMT_PAL8].convert = rgba32_to_pal8;
    convert_table[PIX_FMT_RGBA32][PIX_FMT_YUV420P].convert = rgba32_to_yuv420p;
    convert_table[PIX_FMT_RGBA32][PIX_FMT_GRAY8].convert = rgba32_to_gray;
	
    convert_table[PIX_FMT_BGR24][PIX_FMT_RGB24].convert = bgr24_to_rgb24;
    convert_table[PIX_FMT_BGR24][PIX_FMT_YUV420P].convert = bgr24_to_yuv420p;
    convert_table[PIX_FMT_BGR24][PIX_FMT_GRAY8].convert = bgr24_to_gray;
	
    convert_table[PIX_FMT_RGB555][PIX_FMT_RGB24].convert = rgb555_to_rgb24;
    convert_table[PIX_FMT_RGB555][PIX_FMT_RGBA32].convert = rgb555_to_rgba32;
    convert_table[PIX_FMT_RGB555][PIX_FMT_YUV420P].convert = rgb555_to_yuv420p;
    convert_table[PIX_FMT_RGB555][PIX_FMT_GRAY8].convert = rgb555_to_gray;
	
    convert_table[PIX_FMT_RGB565][PIX_FMT_RGB24].convert = rgb565_to_rgb24;
    convert_table[PIX_FMT_RGB565][PIX_FMT_YUV420P].convert = rgb565_to_yuv420p;
    convert_table[PIX_FMT_RGB565][PIX_FMT_GRAY8].convert = rgb565_to_gray;
	
    convert_table[PIX_FMT_GRAY8][PIX_FMT_RGB555].convert = gray_to_rgb555;
    convert_table[PIX_FMT_GRAY8][PIX_FMT_RGB565].convert = gray_to_rgb565;
    convert_table[PIX_FMT_GRAY8][PIX_FMT_RGB24].convert = gray_to_rgb24;
    convert_table[PIX_FMT_GRAY8][PIX_FMT_BGR24].convert = gray_to_bgr24;
    convert_table[PIX_FMT_GRAY8][PIX_FMT_RGBA32].convert = gray_to_rgba32;
    convert_table[PIX_FMT_GRAY8][PIX_FMT_MONOWHITE].convert = gray_to_monowhite;
    convert_table[PIX_FMT_GRAY8][PIX_FMT_MONOBLACK].convert = gray_to_monoblack;
	
    convert_table[PIX_FMT_MONOWHITE][PIX_FMT_GRAY8].convert = monowhite_to_gray;
	
    convert_table[PIX_FMT_MONOBLACK][PIX_FMT_GRAY8].convert = monoblack_to_gray;
	
    convert_table[PIX_FMT_PAL8][PIX_FMT_RGB555].convert = pal8_to_rgb555;
    convert_table[PIX_FMT_PAL8][PIX_FMT_RGB565].convert = pal8_to_rgb565;
    convert_table[PIX_FMT_PAL8][PIX_FMT_BGR24].convert = pal8_to_bgr24;
    convert_table[PIX_FMT_PAL8][PIX_FMT_RGB24].convert = pal8_to_rgb24;
    convert_table[PIX_FMT_PAL8][PIX_FMT_RGBA32].convert = pal8_to_rgba32;
	
    convert_table[PIX_FMT_UYVY411][PIX_FMT_YUV411P].convert = uyvy411_to_yuv411p;
}

static inline int is_yuv_planar(PixFmtInfo *ps)
{
    return (ps->color_type == FF_COLOR_YUV || ps->color_type == FF_COLOR_YUV_JPEG)
		 && ps->pixel_type == FF_PIXEL_PLANAR;
}

int img_convert(AVPicture *dst, int dst_pix_fmt, const AVPicture *src, int src_pix_fmt, 
				 int src_width, int src_height)
{
    static int inited;
    int i, ret, dst_width, dst_height, int_pix_fmt;
    PixFmtInfo *src_pix,  *dst_pix;
    ConvertEntry *ce;
    AVPicture tmp1,  *tmp = &tmp1;
	
    if (src_pix_fmt < 0 || src_pix_fmt >= PIX_FMT_NB || dst_pix_fmt < 0 || dst_pix_fmt >= PIX_FMT_NB)
        return  - 1;

    if (src_width <= 0 || src_height <= 0)
        return 0;
	
    if (!inited)
    {
        inited = 1;
        img_convert_init();
    }
	
    dst_width = src_width;
    dst_height = src_height;
	
    dst_pix = &pix_fmt_info[dst_pix_fmt];
    src_pix = &pix_fmt_info[src_pix_fmt];

    if (src_pix_fmt == dst_pix_fmt)  // no conversion needed: just copy
    {
        img_copy(dst, src, dst_pix_fmt, dst_width, dst_height);
        return 0;
    }
	
    ce = &convert_table[src_pix_fmt][dst_pix_fmt];
    if (ce->convert)
    {
        ce->convert(dst, src, dst_width, dst_height); // specific conversion routine
        return 0;
    }	

    if (is_yuv_planar(dst_pix) && src_pix_fmt == PIX_FMT_GRAY8) // gray to YUV
    {
        int w, h, y;
        uint8_t *d;
		
        if (dst_pix->color_type == FF_COLOR_YUV_JPEG)
        {
            ff_img_copy_plane(dst->data[0], dst->linesize[0], src->data[0], src->linesize[0], 
				              dst_width, dst_height);
        }
        else
        {
            img_apply_table(dst->data[0], dst->linesize[0], src->data[0], src->linesize[0],
				            dst_width, dst_height, y_jpeg_to_ccir);
        }

        w = dst_width;     // fill U and V with 128
        h = dst_height;
        w >>= dst_pix->x_chroma_shift;
        h >>= dst_pix->y_chroma_shift;
        for (i = 1; i <= 2; i++)
        {
            d = dst->data[i];
            for (y = 0; y < h; y++)
            {
                memset(d, 128, w);
                d += dst->linesize[i];
            }
        }
        return 0;
    }
	
    if (is_yuv_planar(src_pix) && dst_pix_fmt == PIX_FMT_GRAY8)  // YUV to gray
    {
        if (src_pix->color_type == FF_COLOR_YUV_JPEG)
        {
            ff_img_copy_plane(dst->data[0], dst->linesize[0], src->data[0], src->linesize[0], 
				              dst_width, dst_height);
        }
        else
        {
            img_apply_table(dst->data[0], dst->linesize[0], src->data[0], src->linesize[0],
				            dst_width, dst_height, y_ccir_to_jpeg);
        }
        return 0;
    }
	
    if (is_yuv_planar(dst_pix) && is_yuv_planar(src_pix))   // YUV to YUV planar
    {
        int x_shift, y_shift, w, h, xy_shift;
        void(*resize_func)(uint8_t *dst, int dst_wrap, const uint8_t *src, int src_wrap,
			                                int width, int height);
		
        // compute chroma size of the smallest dimensions
        w = dst_width;
        h = dst_height;
        if (dst_pix->x_chroma_shift >= src_pix->x_chroma_shift)
            w >>= dst_pix->x_chroma_shift;
        else
            w >>= src_pix->x_chroma_shift;
        if (dst_pix->y_chroma_shift >= src_pix->y_chroma_shift)
            h >>= dst_pix->y_chroma_shift;
        else
            h >>= src_pix->y_chroma_shift;
		
        x_shift = (dst_pix->x_chroma_shift - src_pix->x_chroma_shift);
        y_shift = (dst_pix->y_chroma_shift - src_pix->y_chroma_shift);
        xy_shift = ((x_shift &0xf) << 4) | (y_shift &0xf);
        
		// there must be filters for conversion at least from and to YUV444 format
        switch (xy_shift)
        {
		case 0x00:
			resize_func = ff_img_copy_plane;
			break;
		case 0x10:
			resize_func = shrink21;
			break;
		case 0x20:
			resize_func = shrink41;
			break;
		case 0x01:
			resize_func = shrink12;
			break;
		case 0x11:
			resize_func = ff_shrink22;
			break;
		case 0x22:
			resize_func = ff_shrink44;
			break;
		case 0xf0:
			resize_func = grow21;
			break;
		case 0xe0:
			resize_func = grow41;
			break;
		case 0xff:
			resize_func = grow22;
			break;
		case 0xee:
			resize_func = grow44;
			break;
		case 0xf1:
			resize_func = conv411;
			break;
		default:	
			goto no_chroma_filter; // currently not handled
        }
		
        ff_img_copy_plane(dst->data[0], dst->linesize[0], src->data[0], src->linesize[0],
			                 dst_width, dst_height);
		
        for (i = 1; i <= 2; i++)
            resize_func(dst->data[i], dst->linesize[i], src->data[i], src->linesize[i], 
			   dst_width >> dst_pix->x_chroma_shift, dst_height >> dst_pix->y_chroma_shift);
		
		// if yuv color space conversion is needed, we do it here on the destination image
        if (dst_pix->color_type != src_pix->color_type)
        {
            const uint8_t *y_table,  *c_table;
            if (dst_pix->color_type == FF_COLOR_YUV)
            {
                y_table = y_jpeg_to_ccir;
                c_table = c_jpeg_to_ccir;
            }
            else
            {
                y_table = y_ccir_to_jpeg;
                c_table = c_ccir_to_jpeg;
            }

            img_apply_table(dst->data[0], dst->linesize[0], dst->data[0], dst->linesize[0], 
				               dst_width, dst_height, y_table);
			
            for (i = 1; i <= 2; i++)
                img_apply_table(dst->data[i], dst->linesize[i], dst->data[i], dst->linesize[i],
				      dst_width >> dst_pix->x_chroma_shift, dst_height >> dst_pix->y_chroma_shift, c_table);
        }
        return 0;
    }

no_chroma_filter:  // try to use an intermediate format

    if (src_pix_fmt == PIX_FMT_YUV422 || dst_pix_fmt == PIX_FMT_YUV422)
    {        
        int_pix_fmt = PIX_FMT_YUV422P; // specific case: convert to YUV422P first
    }
    else if (src_pix_fmt == PIX_FMT_UYVY422 || dst_pix_fmt == PIX_FMT_UYVY422)
    {
        
        int_pix_fmt = PIX_FMT_YUV422P; // specific case: convert to YUV422P first
    }
    else if (src_pix_fmt == PIX_FMT_UYVY411 || dst_pix_fmt == PIX_FMT_UYVY411)
    {
       
        int_pix_fmt = PIX_FMT_YUV411P; // specific case: convert to YUV411P first
    }
    else if ((src_pix->color_type == FF_COLOR_GRAY && src_pix_fmt != PIX_FMT_GRAY8)
		  || (dst_pix->color_type == FF_COLOR_GRAY && dst_pix_fmt != PIX_FMT_GRAY8))
    {
        
        int_pix_fmt = PIX_FMT_GRAY8;   // gray8 is the normalized format
    }
    else if ((is_yuv_planar(src_pix) && src_pix_fmt != PIX_FMT_YUV444P 
		                             && src_pix_fmt != PIX_FMT_YUVJ444P))
    {
        if (src_pix->color_type == FF_COLOR_YUV_JPEG)	// yuv444 is the normalized format
            int_pix_fmt = PIX_FMT_YUVJ444P;
        else
            int_pix_fmt = PIX_FMT_YUV444P;
    }
    else if ((is_yuv_planar(dst_pix) && dst_pix_fmt != PIX_FMT_YUV444P 
		                             && dst_pix_fmt != PIX_FMT_YUVJ444P))
    {
        if (dst_pix->color_type == FF_COLOR_YUV_JPEG)	// yuv444 is the normalized format
            int_pix_fmt = PIX_FMT_YUVJ444P;
        else
            int_pix_fmt = PIX_FMT_YUV444P;
    }
    else                                 // the two formats are rgb or gray8 or yuv[j]444p
    {
        if (src_pix->is_alpha && dst_pix->is_alpha) 
            int_pix_fmt = PIX_FMT_RGBA32;
        else
            int_pix_fmt = PIX_FMT_RGB24;
    }

    if (avpicture_alloc(tmp, int_pix_fmt, dst_width, dst_height) < 0)
        return  - 1;

    ret =  - 1;

    if (img_convert(tmp, int_pix_fmt, src, src_pix_fmt, src_width, src_height) < 0)
        goto fail1;

    if (img_convert(dst, dst_pix_fmt, tmp, int_pix_fmt, dst_width, dst_height) < 0)
        goto fail1;
    ret = 0;

fail1: 
	avpicture_free(tmp);
	return ret;
}

#undef FIX
