//---------------------------------------------------------------------------------------------------------------------------
// gigaherz's lcd3x filter
//---------------------------------------------------------------------------------------------------------------------------
#include "Common.h"

void lcd3x_16_565(u8 *src, u32 S, u8 *dest, u32 D, int W, int H)
{
	u8 *psrc = src;
	u8 *pdst = (dest+D);

	u32 D2 = 2*D;
	u32 D3 = 3*D;
	int W3 = 3*W;

	for(int j=0;j<H;j++)
	{
		u16 *ssrc1 = ((u16*)psrc);
		u16 *ssrc2;
		u16 *sdst = ((u16*)pdst);

		// do current row (middle)
		u16 c,nc=*ssrc1;
		u16 r,g,b,nr,pb=0;
		u16 or,og,ob;

		nr= (*(ssrc1++))&0x1f;
		b=0;
		for(int i=0;i<W;i++)
		{
			c = nc;
			nc = *(ssrc1++);

			pb=b;
			r = nr;
			g = (c>> 5)&0x3f;
			b = (c    )&0x1f;
			nr= (nc>>11)&0x1f;

			// o0 =  25% pb,  25% cg, 100% cr
			or = r;
			og = CLEAK(g) + CBLOOM(r);
			ob = CLEAK(pb) + CBLOOM(r);
			*(sdst++) = (or<<11) | (og<<5) | (ob);

			// o1 =  25% cb, 100% cg,  25% cr
			or = CLEAK(r) + CBLOOM(g);
			og = g;
			ob = CLEAK(b) + CBLOOM(g);
			*(sdst++) = (or<<11) | (og<<5) | (ob);

			// o2 = 100% cb,  25% cg,  25% nr
			or = CLEAK(nr) + CBLOOM(b);
			og = CLEAK(g) + CBLOOM(b);
			ob = b;
			*(sdst++) = (or<<11) | (og<<5) | (ob);
		}

		// do VBlend
		if(j>0)
		{
			{
				ssrc1 = ((u16*)(pdst-D2));
				ssrc2 = ((u16*)(pdst   ));
				sdst  = ((u16*)(pdst-D ));

				const u16 cmask = 0x39E7; // 0 00111 001111 00111

				//blend 25% prev + 75% cur
				for(int i=0;i<W3;i++)
				{
					u16 c1 = (*(ssrc1++) >> 2)&cmask;
					u16 c2 = *(ssrc2++);
					u16 c3 = c2 - ((c2>>2)&cmask);

					*(sdst++) = c1+c3;
				}
			}
			//lblit(pdst1,pdst0,D+D);
			{
				ssrc1 = ((u16*)(pdst   ));
				ssrc2 = ((u16*)(pdst-D3));
				sdst  = ((u16*)(pdst-D2));

				const u16 cmask = 0x39E7; // 0 00111 001111 00111

				//blend 25% prev + 75% cur
				for(int i=0;i<W3;i++)
				{
					u16 c1 = (*(ssrc1++) >> 2)&cmask;
					u16 c2 = *(ssrc2++);
					u16 c3 = c2 - ((c2>>2)&cmask);

					*(sdst++) = c1+c3;
				}
			}
		}
		pdst+=D;
		pdst+=D;
		pdst+=D;
		psrc+=S;
	}
}

void lcd3x_16_555(u8 *src, u32 S, u8 *dest, u32 D, int W, int H)
{
	u8 *psrc = src;
	u8 *pdst = (dest+D);

	u32 D2 = 2*D;
	u32 D3 = 3*D;
	int W3 = 3*W;

	for(int j=0;j<H;j++)
	{
		u16 *ssrc1 = ((u16*)psrc);
		u16 *ssrc2;
		u16 *sdst = ((u16*)pdst);

		// do current row (middle)
		u16 c,nc=*ssrc1;
		u16 r,g,b,nr,pb=0;
		u16 or,og,ob;

		nr= (*(ssrc1++))&0x1f;
		b=0;
		for(int i=0;i<W;i++)
		{
			c = nc;
			nc = *(ssrc1++);

			pb=b;
			r = nr;
			g = (c>> 5)&0x3f;
			b = (c    )&0x1f;
			nr= (nc>>10)&0x1f;

			// o0 =  25% pb,  25% cg, 100% cr
			or = r;
			og = CLEAK(g) + CBLOOM(r);
			ob = CLEAK(pb) + CBLOOM(r);
			*(sdst++) = (or<<10) | (og<<5) | (ob);

			// o1 =  25% cb, 100% cg,  25% cr
			or = CLEAK(r) + CBLOOM(g);
			og = g;
			ob = CLEAK(b) + CBLOOM(g);
			*(sdst++) = (or<<10) | (og<<5) | (ob);

			// o2 = 100% cb,  25% cg,  25% nr
			or = CLEAK(nr) + CBLOOM(b);
			og = CLEAK(g) + CBLOOM(b);
			ob = b;
			*(sdst++) = (or<<10) | (og<<5) | (ob);
		}

		// do VBlend
		if(j>0)
		{
			{
				ssrc1 = ((u16*)(pdst-D2));
				ssrc2 = ((u16*)(pdst   ));
				sdst  = ((u16*)(pdst-D ));

				const u16 cmask = 0x1CE7; // 0 00111 001111 00111

				//blend 25% prev + 75% cur
				for(int i=0;i<W3;i++)
				{
					u16 c1 = (*(ssrc1++) >> 2)&cmask;
					u16 c2 = *(ssrc2++);
					u16 c3 = c2 - ((c2>>2)&cmask);

					*(sdst++) = c1+c3;
				}
			}
			//lblit(pdst1,pdst0,D+D);
			{
				ssrc1 = ((u16*)(pdst   ));
				ssrc2 = ((u16*)(pdst-D3));
				sdst  = ((u16*)(pdst-D2));

				const u16 cmask = 0x1CE7; // 0 00111 001111 00111

				//blend 25% prev + 75% cur
				for(int i=0;i<W3;i++)
				{
					u16 c1 = (*(ssrc1++) >> 2)&cmask;
					u16 c2 = *(ssrc2++);
					u16 c3 = c2 - ((c2>>2)&cmask);

					*(sdst++) = c1+c3;
				}
			}
		}
		pdst+=D;
		pdst+=D;
		pdst+=D;
		psrc+=S;
	}
}

void lcd3x_24    (u8 *src, u32 S, u8 *dest, u32 D, int W, int H)
{
	u8 *psrc = src;
	u8 *pdst = dest;

	for(int j=0;j<H;j++)
	{
		u8 *ssrc = psrc;
		u8 *sdst = pdst;
		
		// do row
		u8 r,g,b,pr,nb=0;

		nb= *(ssrc++);
		r=0;
		for(int i=0;i<W;i++)
		{
			pr=r;
			b = nb;
			g = *(ssrc++);
			r = *(ssrc++);
			nb= *(ssrc++);

			// o2 = 100% cb,  25% cg,  25% nr
			*(sdst++) = b;
			*(sdst++) = CLEAK(g) + CBLOOM(b);
			*(sdst++) = CLEAK(pr) + CBLOOM(b);

			// o1 =  25% cb, 100% cg,  25% cr
			*(sdst++) = CLEAK(b) + CBLOOM(g);
			*(sdst++) = g;
			*(sdst++) = CLEAK(r) + CBLOOM(g);

			// o0 =  25% pb,  25% cg, 100% cr
			*(sdst++) = CLEAK(nb) + CBLOOM(r);
			*(sdst++) = CLEAK(g) + CBLOOM(r);
			*(sdst++) = r;
		}

		// clone line twice
		u32 *pdst0 = (u32*)(pdst);
		u32 *pdst1 = (u32*)(pdst+D);
		lblit(pdst1,pdst0,D+D);
		pdst+=D;
		pdst+=D;
		pdst+=D;
		psrc+=S;
	}
}
