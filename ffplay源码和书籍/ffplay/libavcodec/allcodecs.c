#include "avcodec.h"

/************************************************************************/
/* �򵥵�ע��/��ʼ���������ѱ����������Ӧ�������������ڲ���ʶ�� */
/************************************************************************/

/* ����ȫ�ֵı������ */
extern AVCodec truespeech_decoder;
extern AVCodec msrle_decoder;

/* ע�����еı������ */
void avcodec_register_all(void)
{
    static int inited = 0;

    if (inited != 0)
        return ;

    inited = 1;

	// ����򻯰��ffmpegֻ֧�����ֱ���뷽ʽ��MSRLE��truespeech
    register_avcodec(&msrle_decoder);

    register_avcodec(&truespeech_decoder);
}
