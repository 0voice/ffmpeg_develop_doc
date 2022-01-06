#ifndef AVUTIL_H
#define AVUTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "bswap.h"
#include "mathematics.h"
#include "rational.h"

#define AV_STRINGIFY(s)         AV_TOSTRING(s)
#define AV_TOSTRING(s)          #s

#define LIBAVUTIL_VERSION_INT   ((49<<16)+(0<<8)+0)
#define LIBAVUTIL_VERSION       49.0.0
#define LIBAVUTIL_BUILD         LIBAVUTIL_VERSION_INT

#define LIBAVUTIL_IDENT         "Lavu" AV_STRINGIFY(LIBAVUTIL_VERSION)

	/* ÏñËØ¸ñÊ½ */
enum PixelFormat
{
    PIX_FMT_NONE =  - 1,
	PIX_FMT_YUV420P,   // Planar YUV 4:2:0 (1 Cr & Cb sample per 2x2 Y samples)
    PIX_FMT_YUV422,    // Packed pixel, Y0 Cb Y1 Cr
    PIX_FMT_RGB24,     // Packed pixel, 3 bytes per pixel, RGBRGB...
    PIX_FMT_BGR24,     // Packed pixel, 3 bytes per pixel, BGRBGR...
    PIX_FMT_YUV422P,   // Planar YUV 4:2:2 (1 Cr & Cb sample per 2x1 Y samples)
    PIX_FMT_YUV444P,   // Planar YUV 4:4:4 (1 Cr & Cb sample per 1x1 Y samples)
    PIX_FMT_RGBA32,    // Packed pixel, 4 bytes per pixel, BGRABGRA..., stored in cpu endianness
    PIX_FMT_YUV410P,   // Planar YUV 4:1:0 (1 Cr & Cb sample per 4x4 Y samples)
    PIX_FMT_YUV411P,   // Planar YUV 4:1:1 (1 Cr & Cb sample per 4x1 Y samples)
    PIX_FMT_RGB565,    // always stored in cpu endianness
    PIX_FMT_RGB555,    // always stored in cpu endianness, most significant bit to 1
    PIX_FMT_GRAY8,
	PIX_FMT_MONOWHITE, // 0 is white
    PIX_FMT_MONOBLACK, // 0 is black
    PIX_FMT_PAL8,      // 8 bit with RGBA palette
    PIX_FMT_YUVJ420P,  // Planar YUV 4:2:0 full scale (jpeg)
    PIX_FMT_YUVJ422P,  // Planar YUV 4:2:2 full scale (jpeg)
    PIX_FMT_YUVJ444P,  // Planar YUV 4:4:4 full scale (jpeg)
    PIX_FMT_XVMC_MPEG2_MC,  // XVideo Motion Acceleration via common packet passing(xvmc_render.h)
    PIX_FMT_XVMC_MPEG2_IDCT,
	PIX_FMT_UYVY422,   // Packed pixel, Cb Y0 Cr Y1
    PIX_FMT_UYVY411,   // Packed pixel, Cb Y0 Y1 Cr Y2 Y3
    PIX_FMT_NB,
};

#ifdef __cplusplus
}

#endif

#endif
