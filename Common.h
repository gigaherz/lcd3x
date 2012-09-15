#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

//---------------------------------------------------------------------------------------------------------------------------

void __forceinline lblit(void *dest, void *src, u32 len)
{
	int c = len>>2;

	u32 *d = (u32*)dest, *s = (u32*)src;

	while(c--) *(d++) = *(s++);
}

//---------------------------------------------------------------------------------------------------------------------------
extern void lcd3x_555(u8 *src, u32 S, u8 *dest, u32 D, int W, int H);
extern void lcd3x_565(u8 *src, u32 S, u8 *dest, u32 D, int W, int H);
extern void lcd3x_888(u8 *src, u32 S, u8 *dest, u32 D, int W, int H);

//---------------------------------------------------------------------------------------------------------------------------

#define CLEAK(c) (((c)*6)>>3)

//---------------------------------------------------------------------------------------------------------------------------
#define MK888(r,g,b) (((r)<<16)|((g)<<8)|(b))
#define MK565(r,g,b) (u16)(((r)<<11)|((g)<<5)|(b))
#define MK555(r,g,b) (u16)(((r)<<10)|((g)<<5)|(b))

#define FROM888(r,g,b,c) r=((c)>>16)&0xff;g=((c)>>8)&0xff;b=(c)&0xff
#define FROM565(r,g,b,c) r=((c)>>11)&0x1f;g=((c)>>5)&0x3f;b=(c)&0x1f
#define FROM555(r,g,b,c) r=((c)>>10)&0x1f;g=((c)>>5)&0x1f;b=(c)&0x1f
