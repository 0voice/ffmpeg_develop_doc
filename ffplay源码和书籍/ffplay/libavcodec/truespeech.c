/************************************************************************/
/* ���ļ�ʵ�� truespeed ��Ƶ������                                                                     */
/************************************************************************/
#include "avcodec.h"

#include "truespeech_data.h"

// TrueSpeech decoder context

typedef struct TSContext
{
    // input data
    int16_t vector[8];   // input vector: 5/5/4/4/4/3/3/3
    int offset1[2];      // 8-bit value, used in one copying offset
    int offset2[4];      // 7-bit value, encodes offsets for copying and for two-point filter
    int pulseoff[4];     // 4-bit offset of pulse values block
    int pulsepos[4];     // 27-bit variable, encodes 7 pulse positions
    int pulseval[4];     // 7x2-bit pulse values
    int flag;            // 1-bit flag, shows how to choose filters
    // temporary data
    int filtbuf[146];    // some big vector used for storing filters
    int prevfilt[8];     // filter from previous frame
    int16_t tmp1[8];     // coefficients for adding to out
    int16_t tmp2[8];     // coefficients for adding to out
    int16_t tmp3[8];     // coefficients for adding to out
    int16_t cvector[8];  // correlated input vector
    int filtval;         // gain value for one function
    int16_t newvec[60];  // tmp vector
    int16_t filters[32]; // filters for every subframe
} TSContext;

#if !defined(LE_32)
#define LE_32(x)  ((((uint8_t*)(x))[3] << 24)| (((uint8_t*)(x))[2] << 16) | \
                   (((uint8_t*)(x))[1] << 8) | ((uint8_t*)(x))[0])
#endif

static int truespeech_decode_init(AVCodecContext *avctx)
{
    return 0; //  TSContext *c = avctx->priv_data;
}

static void truespeech_read_frame(TSContext *dec, uint8_t *input)
{
    uint32_t t;

    t = LE_32(input); // first dword
    input += 4;

    dec->flag = t &1;

    dec->vector[0] = ts_codebook[0][(t >> 1) &0x1F];
    dec->vector[1] = ts_codebook[1][(t >> 6) &0x1F];
    dec->vector[2] = ts_codebook[2][(t >> 11) &0xF];
    dec->vector[3] = ts_codebook[3][(t >> 15) &0xF];
    dec->vector[4] = ts_codebook[4][(t >> 19) &0xF];
    dec->vector[5] = ts_codebook[5][(t >> 23) &0x7];
    dec->vector[6] = ts_codebook[6][(t >> 26) &0x7];
    dec->vector[7] = ts_codebook[7][(t >> 29) &0x7];


    t = LE_32(input); // second dword
    input += 4;

    dec->offset2[0] = (t >> 0) &0x7F;
    dec->offset2[1] = (t >> 7) &0x7F;
    dec->offset2[2] = (t >> 14) &0x7F;
    dec->offset2[3] = (t >> 21) &0x7F;

    dec->offset1[0] = ((t >> 28) &0xF) << 4;


    t = LE_32(input); // third dword
    input += 4;

    dec->pulseval[0] = (t >> 0) &0x3FFF;
    dec->pulseval[1] = (t >> 14) &0x3FFF;

    dec->offset1[1] = (t >> 28) &0x0F;


    t = LE_32(input); // fourth dword
    input += 4;

    dec->pulseval[2] = (t >> 0) &0x3FFF;
    dec->pulseval[3] = (t >> 14) &0x3FFF;

    dec->offset1[1] |= ((t >> 28) &0x0F) << 4;


    t = LE_32(input); // fifth dword
    input += 4;

    dec->pulsepos[0] = (t >> 4) &0x7FFFFFF;

    dec->pulseoff[0] = (t >> 0) &0xF;

    dec->offset1[0] |= (t >> 31) &1;


    t = LE_32(input); // sixth dword
    input += 4;

    dec->pulsepos[1] = (t >> 4) &0x7FFFFFF;

    dec->pulseoff[1] = (t >> 0) &0xF;

    dec->offset1[0] |= ((t >> 31) &1) << 1;


    t = LE_32(input); // seventh dword
    input += 4;

    dec->pulsepos[2] = (t >> 4) &0x7FFFFFF;

    dec->pulseoff[2] = (t >> 0) &0xF;

    dec->offset1[0] |= ((t >> 31) &1) << 2;


    t = LE_32(input); // eighth dword
    input += 4;

    dec->pulsepos[3] = (t >> 4) &0x7FFFFFF;

    dec->pulseoff[3] = (t >> 0) &0xF;

    dec->offset1[0] |= ((t >> 31) &1) << 3;
}

static void truespeech_correlate_filter(TSContext *dec)
{
    int16_t tmp[8];
    int i, j;

    for (i = 0; i < 8; i++)
    {
        if (i > 0)
        {
            memcpy(tmp, dec->cvector, i *2);
            for (j = 0; j < i; j++)
                dec->cvector[j] = ((tmp[i - j - 1] *dec->vector[i]) + (dec->cvector[j] << 15) + 0x4000) >> 15;
        }
        dec->cvector[i] = (8-dec->vector[i]) >> 3;
    }

    for (i = 0; i < 8; i++)
        dec->cvector[i] = (dec->cvector[i] *ts_230[i]) >> 15;

    dec->filtval = dec->vector[0];
}

static void truespeech_filters_merge(TSContext *dec)
{
    int i;

    if (!dec->flag)
    {
        for (i = 0; i < 8; i++)
        {
            dec->filters[i + 0] = dec->prevfilt[i];
            dec->filters[i + 8] = dec->prevfilt[i];
        }
    }
    else
    {
        for (i = 0; i < 8; i++)
        {
            dec->filters[i + 0] = (dec->cvector[i] *21846+dec->prevfilt[i] *10923+16384) >> 15;
            dec->filters[i + 8] = (dec->cvector[i] *10923+dec->prevfilt[i] *21846+16384) >> 15;
        }
    }
    for (i = 0; i < 8; i++)
    {
        dec->filters[i + 16] = dec->cvector[i];
        dec->filters[i + 24] = dec->cvector[i];
    }
}

static void truespeech_apply_twopoint_filter(TSContext *dec, int quart)
{
    int16_t tmp[146+60],  *ptr0,  *ptr1,  *filter;
    int i, t, off;

    t = dec->offset2[quart];
    if (t == 127)
    {
        memset(dec->newvec, 0, 60 *2);
        return ;
    }

    for (i = 0; i < 146; i++)
        tmp[i] = dec->filtbuf[i];

    off = (t / 25) + dec->offset1[quart >> 1] + 18;
    ptr0 = tmp + 145-off;
    ptr1 = tmp + 146;
    filter = (int16_t*)ts_240 + (t % 25) *2;
    for (i = 0; i < 60; i++)
    {
        t = (ptr0[0] *filter[0] + ptr0[1] *filter[1] + 0x2000) >> 14;
        ptr0++;
        dec->newvec[i] = t;
        ptr1[i] = t;
    }
}

static void truespeech_place_pulses(TSContext *dec, int16_t *out, int quart)
{
    int16_t tmp[7];
    int i, j, t;
    int16_t *ptr1,  *ptr2;
    int coef;

    memset(out, 0, 60 *2);
    for (i = 0; i < 7; i++)
    {
        t = dec->pulseval[quart] &3;
        dec->pulseval[quart] >>= 2;
        tmp[6-i] = ts_562[dec->pulseoff[quart] *4+t];
    }

    coef = dec->pulsepos[quart] >> 15;
    ptr1 = (int16_t*)ts_140 + 30;
    ptr2 = tmp;
    for (i = 0, j = 3; (i < 30) && (j > 0); i++)
    {
        t =  *ptr1++;
        if (coef >= t)
            coef -= t;
        else
        {
            out[i] =  *ptr2++;
            ptr1 += 30;
            j--;
        }
    }
    coef = dec->pulsepos[quart] &0x7FFF;
    ptr1 = (int16_t*)ts_140;
    for (i = 30, j = 4; (i < 60) && (j > 0); i++)
    {
        t =  *ptr1++;
        if (coef >= t)
            coef -= t;
        else
        {
            out[i] =  *ptr2++;
            ptr1 += 30;
            j--;
        }
    }
}

static void truespeech_update_filters(TSContext *dec, int16_t *out, int quart)
{
    int i;

    for (i = 0; i < 86; i++)
        dec->filtbuf[i] = dec->filtbuf[i + 60];

    for (i = 0; i < 60; i++)
    {
        dec->filtbuf[i + 86] = out[i] + dec->newvec[i] - (dec->newvec[i] >> 3);
        out[i] += dec->newvec[i];
    }
}

static void truespeech_synth(TSContext *dec, int16_t *out, int quart)
{
    int i, k;
    int t[8];
    int16_t *ptr0,  *ptr1;

    ptr0 = dec->tmp1;
    ptr1 = dec->filters + quart * 8;
    for (i = 0; i < 60; i++)
    {
        int sum = 0;
        for (k = 0; k < 8; k++)
            sum += ptr0[k] *ptr1[k];
        sum = (sum + (out[i] << 12) + 0x800) >> 12;
        out[i] = clip(sum,  - 0x7FFE, 0x7FFE);
        for (k = 7; k > 0; k--)
            ptr0[k] = ptr0[k - 1];
        ptr0[0] = out[i];
    }

    for (i = 0; i < 8; i++)
        t[i] = (ts_5E2[i] *ptr1[i]) >> 15;

    ptr0 = dec->tmp2;
    for (i = 0; i < 60; i++)
    {
        int sum = 0;
        for (k = 0; k < 8; k++)
            sum += ptr0[k] *t[k];
        for (k = 7; k > 0; k--)
            ptr0[k] = ptr0[k - 1];
        ptr0[0] = out[i];
        out[i] = ((out[i] << 12) - sum) >> 12;
    }

    for (i = 0; i < 8; i++)
        t[i] = (ts_5F2[i] *ptr1[i]) >> 15;

    ptr0 = dec->tmp3;
    for (i = 0; i < 60; i++)
    {
        int sum = out[i] << 12;
        for (k = 0; k < 8; k++)
            sum += ptr0[k] *t[k];
        for (k = 7; k > 0; k--)
            ptr0[k] = ptr0[k - 1];
        ptr0[0] = clip((sum + 0x800) >> 12,  - 0x7FFE, 0x7FFE);

        sum = ((ptr0[1]*(dec->filtval - (dec->filtval >> 2))) >> 4) + sum;
        sum = sum - (sum >> 3);
        out[i] = clip((sum + 0x800) >> 12,  - 0x7FFE, 0x7FFE);
    }
}

static void truespeech_save_prevvec(TSContext *c)
{
    int i;

    for (i = 0; i < 8; i++)
        c->prevfilt[i] = c->cvector[i];
}

static int truespeech_decode_frame(AVCodecContext *avctx, void *data, int *data_size, uint8_t *buf, int buf_size)
{
    TSContext *c = avctx->priv_data;

    int i;
    short *samples = data;
    int consumed = 0;
    int16_t out_buf[240];

    if (!buf_size)
        return 0;

    while (consumed < buf_size)
    {
        truespeech_read_frame(c, buf + consumed);
        consumed += 32;

        truespeech_correlate_filter(c);
        truespeech_filters_merge(c);

        memset(out_buf, 0, 240 *2);
        for (i = 0; i < 4; i++)
        {
            truespeech_apply_twopoint_filter(c, i);
            truespeech_place_pulses(c, out_buf + i * 60, i);
            truespeech_update_filters(c, out_buf + i * 60, i);
            truespeech_synth(c, out_buf + i * 60, i);
        }

        truespeech_save_prevvec(c);

        for (i = 0; i < 240; i++)      // finally output decoded frame
            *samples++ = out_buf[i];

    }

    *data_size = consumed * 15;

    return buf_size;
}

AVCodec truespeech_decoder =
{
    "truespeech", 
	CODEC_TYPE_AUDIO, 
	CODEC_ID_TRUESPEECH,
	sizeof(TSContext), 
	truespeech_decode_init,
	NULL,
	NULL, 
	truespeech_decode_frame,
};
