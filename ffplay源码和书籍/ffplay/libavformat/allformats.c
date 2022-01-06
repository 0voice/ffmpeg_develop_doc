/*
** �򵥵�ע��/��ʼ������������Ӧ��Э�飬�ļ���ʽ��������������Ӧ�������������ڲ���
*/
#include "avformat.h"

extern URLProtocol file_protocol;

/* ע������֧�ֵĸ�ʽ */
void av_register_all(void)
{
    static int inited = 0;

	// �Ƿ��Ѿ���ʼ�����
    if (inited != 0)
        return ;
    inited = 1;

	// ffplay �� CPU ����һ������� DSP����Щ��������� CPU �Դ��ļ���ָ�����Ż���ffplay �����ຯ��
	// ���������ŵ� dsputil.h �� dsputil.c �ļ��У� �ú���ָ��ķ���ӳ�䵽���� CPU ����ļ����Ż�ʵ�ֺ�����
	//	�˴���ʼ����Щ����ָ��
    avcodec_init();

	// ע�����еı������
    avcodec_register_all();

	// ע��֧�ֵĸ�ʽ
    avidec_init();

	// ע��Э�飨�ļ�Э�顢����Э��ȣ�
    register_protocol(&file_protocol);
}
