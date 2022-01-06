/*
** 字符串的操作
*/

#include "avformat.h"

/*
** 在 str 字符串中搜索 val 字符串指示的头，并且去掉头后用*ptr 返回
*/
int strstart(const char *str, const char *val, const char **ptr)
{
    const char *p,  *q;
    p = str;
    q = val;
    while (*q != '\0')
    {
        if (*p !=  *q)
            return 0;
        p++;
        q++;
    }
    if (ptr)
        *ptr = p;
    return 1;
}

/*
** 字符串拷贝函数
*/
void pstrcpy(char *buf, int buf_size, const char *str)
{
    int c;
    char *q = buf;

    if (buf_size <= 0)
        return ;

    for (;;)
    {
        c =  *str++;
        if (c == 0 || q >= buf + buf_size - 1)
            break;
        *q++ = c;
    }
    *q = '\0';
}
