/************************************************************************/
/* 字节顺序交换                                                                     */
/************************************************************************/

#ifndef __BSWAP_H__
#define __BSWAP_H__

/* 16bit的字节顺序交换 */
static inline uint16_t bswap_16(uint16_t x)
{
    return (x >> 8) | (x << 8);
}

/* 32bit的字节顺序交换 */
static inline uint32_t bswap_32(uint32_t x)
{
    x = ((x << 8) &0xFF00FF00) | ((x >> 8) &0x00FF00FF);
    return (x >> 16) | (x << 16);
}

// be2me ... BigEndian to MachineEndian
// le2me ... LittleEndian to MachineEndian

#define be2me_16(x) bswap_16(x)
#define be2me_32(x) bswap_32(x)
#define le2me_16(x) (x)
#define le2me_32(x) (x)

#endif
