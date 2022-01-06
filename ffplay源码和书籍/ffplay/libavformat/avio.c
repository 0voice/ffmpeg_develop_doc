/************************************************************************
** ���ļ�ʵ���� URLProtocol ���������ļ�����������
** ���� URLProtocol �ǵײ����������ļ�(file,pipe ��)�ļ򵥷�װ��
** ��һ��ֻ��һ����תվ���󲿷ֺ������Ǽ���ת���ײ�ľ���ʵ�ֺ���                                                                     
/************************************************************************/

#include "../berrno.h"
#include "avformat.h"

/* ffmpeg����֧�ֶ���Э�飬�����������֧�ֵ�Э�鴮���� */
URLProtocol *first_protocol = NULL;

/*  ע��Э�飨��νЭ������������ݵ����壺�ļ����׽��ֵ���ʽ�� */
int register_protocol(URLProtocol *protocol)
{
    URLProtocol **p;
    p = &first_protocol;
    while (*p != NULL)
        p = &(*p)->next;
    *p = protocol;
    protocol->next = NULL;
    return 0;
}

/* ��URL */
int url_open(URLContext **puc, const char *filename, int flags)
{
	// ������
    URLContext *uc;
	// Э�鼰����
    URLProtocol *up;
    const char *p;
	// Э���ַ���
    char proto_str[128],  *q;
    int err;

    p = filename;
    q = proto_str;

	// ȷ��Э�������
    while (*p != '\0' &&  *p != ':')
    {
        if (!isalpha(*p))  // protocols can only contain alphabetic chars
            goto file_proto;
        if ((q - proto_str) < sizeof(proto_str) - 1)
            *q++ =  *p;
        p++;
    }
    // if the protocol has length 1, we consider it is a dos drive
    if (*p == '\0' || (q - proto_str) <= 1)
    {
file_proto: 
		strcpy(proto_str, "file");
    }
    else
    {
        *q = '\0';
    }

    up = first_protocol;

	// Ѱ�Һ���Э��
    while (up != NULL)
    {
        if (!strcmp(proto_str, up->name))
            goto found;
        up = up->next;
    }
    err =  - ENOENT;
    goto fail;
found: 

	// URL������
	uc = av_malloc(sizeof(URLContext) + strlen(filename));
    if (!uc)
    {
        err =  - ENOMEM;
        goto fail;
    }
    strcpy(uc->filename, filename);
    uc->prot = up;
    uc->flags = flags;
    uc->max_packet_size = 0; // default: stream file

	// ��URL
    err = up->url_open(uc, filename, flags);
    if (err < 0)
    {
        av_free(uc);
        *puc = NULL;
        return err;
    }
    *puc = uc;
    return 0;
fail:
	*puc = NULL;
    return err;
}

/* ��ȡ���� */
int url_read(URLContext *h, unsigned char *buf, int size)
{
    int ret;
    if (h->flags &URL_WRONLY)
        return AVERROR_IO;
    ret = h->prot->url_read(h, buf, size);
    return ret;
}

/* �ڹ����ļ�����ת */
offset_t url_seek(URLContext *h, offset_t pos, int whence)
{
    offset_t ret;

    if (!h->prot->url_seek)
        return  - EPIPE;
    ret = h->prot->url_seek(h, pos, whence);
    return ret;
}

/* �ر�URL */
int url_close(URLContext *h)
{
    int ret;

    ret = h->prot->url_close(h);
    av_free(h);
    return ret;
}

int url_get_max_packet_size(URLContext *h)
{
    return h->max_packet_size;
}
