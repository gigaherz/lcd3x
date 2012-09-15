//---------------------------------------------------------------------------------------------------------------------------
// gigaherz's lcd3x filter
//---------------------------------------------------------------------------------------------------------------------------
#include "Common.h"

// TODO: Vblend support for 888/8888

#ifdef DOVBLEND
template<u16 cmask>
void DoVBlend16one(u16 *ssrc, u16 *sdst, u32 W3)
{
	// 75% src
	for(u32 i=0;i<W3;i++)
	{
		u16 c1 = *(ssrc++);
		u16 c2 = (c1>>2)&cmask;

		*(sdst++) = c1-c2;
	}
}

template<u16 cmask>
void DoVBlend16add(u16 *ssrc, u16 *sdst, u32 W3)
{
	// 75% existing + 25% src
	for(u32 i=0;i<W3;i++)
	{
		u16 c1 = *(ssrc++);
		u16 c2 = (c1>>2)&cmask;

		*(sdst++) += c2;
	}
}

template<u16 cmask>
void DoVBlend16two(u16 *ssrc1, u16 *ssrc2, u16 *sdst, u32 W3)
{
	// 75% src1 + 25% src2
	for(u32 i=0;i<W3;i++)
	{
		u16 c1 = *(ssrc1++);
		u16 c2 = *(ssrc2++);

		u16 c3 = (c1>>2)&cmask;
		u16 c4 = (c2>>2)&cmask;

		*(sdst++) = c1-c3+c4;
	}
}

template<u16 cmask>
void DoVBlend16a(u8* pdst, u32 D, u32 W3)
{
	// top row
	DoVBlend16one<cmask>((u16*)(pdst), (u16*)(pdst-D), W3);

	// bottom row
	DoVBlend16one<cmask>((u16*)(pdst), (u16*)(pdst+D), W3);
}

template<u16 cmask>
void DoVBlend16b(u8* pdst, u32 D, u32 W3)
{
	u32 D2 = D+D;
	u32 D3 = D+D2;

#ifndef HSCANLINES
	//// previous bottom row
	DoVBlend16add<cmask>((u16*)(pdst), (u16*)(pdst-D2), W3);
#endif

	// top row
	DoVBlend16two<cmask>((u16*)(pdst), (u16*)(pdst-D3), (u16*)(pdst-D), W3);
	
	// bottom row
	DoVBlend16one<cmask>((u16*)(pdst), (u16*)(pdst+D), W3);
}
#endif

void lcd3x_565(u8 *src, u32 S, u8 *dest, u32 D, int W, int H)
{
	u8 *psrc = src;
	u8 *pdst = dest;

#ifdef DOVBLEND
	// Start from the middle row
	pdst += D;

	u32 W3 = W*3;
#endif

	u32 D2 = D +D;
	u32 D3 = D2+D;

	for(int j=0;j<H;j++)
	{
		u16 *ssrc = (u16*)psrc;
		u16 *sdst = (u16*)pdst;
				
		u32 nr, ng, nb;
		u32 nc = *(ssrc++);

		FROM565(nr,ng,nb,nc);

		u32 pb = 0;
		for(int i=0;i<W;i++)
		{
			nc = *(ssrc++);

			u32 cr = nr, cg = ng, cb = nb;
			
			FROM565(nr,ng,nb,nc);

			*(sdst++) = MK565(cr, CLEAK(cg), CLEAK(pb));
			*(sdst++) = MK565(CLEAK(cr), cg, CLEAK(cb));
			*(sdst++) = MK565(CLEAK(nr), CLEAK(cg), cb);

			pb = cb;
		}
		
#ifdef DOVBLEND
		const u16 cmask = 0x39E7; // 00111 001111 00111

		if(j==0)
			DoVBlend16a<cmask>(pdst, D, W3);
		else
			DoVBlend16b<cmask>(pdst, D, W3);
#else
		// clone line twice
		u32 *pdst0 = (u32*)(pdst);
		u32 *pdst1 = (u32*)(pdst+D);
		lblit(pdst1, pdst0, D2);
#endif

		pdst+=D3;
		psrc+=S;
	}
}

void lcd3x_555(u8 *src, u32 S, u8 *dest, u32 D, int W, int H)
{
	u8 *psrc = src;
	u8 *pdst = dest;
	
#ifdef DOVBLEND
	// Start from the middle row
	pdst += D;

	u32 W3 = W*3;
#endif

	u32 D2 = D +D;
	u32 D3 = D2+D;

	for(int j=0;j<H;j++)
	{
		u16 *ssrc = (u16*)psrc;
		u16 *sdst = (u16*)pdst;
				
		u32 nr, ng, nb;
		u32 nc = *(ssrc++);

		FROM555(nr,ng,nb,nc);

		u32 pb = 0;
		for(int i=0;i<W;i++)
		{
			nc = *(ssrc++);

			u32 cr = nr, cg = ng, cb = nb;
			
			FROM555(nr,ng,nb,nc);

			*(sdst++) = MK555(cr, CLEAK(cg), CLEAK(pb));
			*(sdst++) = MK555(CLEAK(cr), cg, CLEAK(cb));
			*(sdst++) = MK555(CLEAK(nr), CLEAK(cg), cb);

			pb = cb;
		}
		
#ifdef DOVBLEND
		const u16 cmask = 0x1CE7; // 0 00111 00111 00111

		if(j==0)
			DoVBlend16a<cmask>(pdst, D, W3);
		else
			DoVBlend16b<cmask>(pdst, D, W3);
#else
		// clone line twice
		u32 *pdst0 = (u32*)(pdst);
		u32 *pdst1 = (u32*)(pdst+D);
		lblit(pdst1, pdst0, D2);
#endif

		pdst+=D3;
		psrc+=S;
	}
}

void lcd3x_888(u8 *src, u32 S, u8 *dest, u32 D, int W, int H)
{
	u8 *psrc = src;
	u8 *pdst = dest;
	
	u32 D2 = D +D;
	u32 D3 = D2+D;
	
	for(int j=0;j<H;j++)
	{
		u8 *ssrc = (u8*)psrc;
		u8 *sdst = (u8*)pdst;
				
		u32 nr, ng, nb;
		u32 nc = *(u32*)ssrc;
		ssrc+=3;

		FROM888(nr,ng,nb,nc);

		u32 pb = 0;
		for(int i=0;i<W;i++)
		{
			nc = *(u32*)ssrc;
			ssrc+=3;

			u32 cr = nr, cg = ng, cb = nb;
			
			FROM888(nr,ng,nb,nc);

			*(u32*)(sdst) = MK888(cr, CLEAK(cg), CLEAK(pb)); sdst += 3;
			*(u32*)(sdst) = MK888(CLEAK(cr), cg, CLEAK(cb)); sdst += 3;
			*(u32*)(sdst) = MK888(CLEAK(nr), CLEAK(cg), cb); sdst += 3;

			pb = cb;
		}
		
		// clone line twice
		u32 *pdst0 = (u32*)(pdst);
		u32 *pdst1 = (u32*)(pdst+D);
		lblit(pdst1, pdst0, D2);

		pdst+=D3;
		psrc+=S;
	}
}

// Not used (yet)
void lcd3x_8888(u8 *src, u32 S, u8 *dest, u32 D, int W, int H)
{
	u8 *psrc = src;
	u8 *pdst = dest;

	u32 D2 = D +D;
	u32 D3 = D2+D;

	for(int j=0;j<H;j++)
	{
		u32 *ssrc = (u32*)psrc;
		u32 *sdst = (u32*)pdst;
				
		u32 nr, ng, nb;
		u32 nc = *(ssrc++);

		FROM888(nr,ng,nb,nc);

		u32 pb = 0;
		for(int i=0;i<W;i++)
		{
			nc = *(u32*)ssrc;
			ssrc+=3;

			u32 cr = nr, cg = ng, cb = nb;
			
			FROM888(nr,ng,nb,nc);

			*(sdst++) = MK888(cr, CLEAK(cg), CLEAK(pb));
			*(sdst++) = MK888(CLEAK(cr), cg, CLEAK(cb));
			*(sdst++) = MK888(CLEAK(nr), CLEAK(cg), cb);

			pb = cb;
		}

		// clone line twice
		u32 *pdst0 = (u32*)(pdst);
		u32 *pdst1 = (u32*)(pdst+D);
		lblit(pdst1,pdst0,D2);

		pdst+=D3;
		psrc+=S;
	}
}
