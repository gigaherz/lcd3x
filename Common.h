#pragma once

#define CLEAK(c) (((c)*6)>>3)
#define CBLOOM(c) 0

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

//---------------------------------------------------------------------------------------------------------------------------

void __forceinline lblit(u32 *dest, u32 *src, u32 len)
{
	int iD = len>>2;
	//if(len&3) iD++;

	while(iD--)
	{
		*(dest++) = *(src++);
	}
}

//---------------------------------------------------------------------------------------------------------------------------
extern void lcd3x_16_555(u8 *src, u32 S, u8 *dest, u32 D, int W, int H);
extern void lcd3x_16_565(u8 *src, u32 S, u8 *dest, u32 D, int W, int H);
extern void lcd3x_24    (u8 *src, u32 S, u8 *dest, u32 D, int W, int H);
