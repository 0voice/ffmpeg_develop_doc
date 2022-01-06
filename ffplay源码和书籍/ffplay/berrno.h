#ifndef BERRNO_H
#define BERRNO_H

/* ���õĴ����� ��Ҳ��ʾ�жϴ����źŵȣ�*/
#ifdef ENOENT
#undef ENOENT
#endif
#define ENOENT 2

#ifdef EINTR
#undef EINTR
#endif
#define EINTR  4

#ifdef EIO
#undef EIO
#endif
#define EIO    5

#ifdef EAGAIN
#undef EAGAIN
#endif
#define EAGAIN 11

#ifdef ENOMEM
#undef ENOMEM
#endif
#define ENOMEM 12

#ifdef EINVAL
#undef EINVAL
#endif
#define EINVAL 22

#ifdef EPIPE
#undef EPIPE
#endif
#define EPIPE  32

#endif
