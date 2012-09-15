//---------------------------------------------------------------------------------------------------------------------------
// gigaherz's lcd3x filter
//---------------------------------------------------------------------------------------------------------------------------

#define N mm0
#define C mm1
#define P mm2
#define T mm3
#define R mm4
#define cmr mm5
#define cmg mm6
#define cmb mm7

void lcd3x_16(unsigned char *src, unsigned int S, unsigned char *dest, unsigned int D, int W, int H)
{
	int Q=S-W*2;
	int O=3*D-W*6; //2 lines + what is left in current
	int eaxsave,ebxsave;

	__int16 mask[12]={0x00FF,0x0000,0x0000,0x0000,
					  0x0000,0x00FF,0x0000,0x0000,
					  0x0000,0x0000,0x00FF,0x0000};
	int M=(int)mask;

	__asm {
		//init
		MOV eax,M
		MOVQ cmr,[eax]
		MOVQ cmg,[eax+8]
		MOVQ cmb,[eax+16]

		MOV ebx,H
		MOV ecx,src
		MOV edx,dest
		MOV esi,edx

		//before line
Y_loop:	PXOR P,P
		PXOR C,C
		PUNPCKLBW C,[ecx]
		PSRLW C,8

		MOV eax,W

		//line
X_loop:	ADD ecx,2
		
		MOV eaxsave,eax
		MOV ebxsave,ebx

		MOV di,[ecx]
		CWDE
		MOV ebx,edi
		SHR ebx,7
		AND ebx,248
		SHL ebx,19
		MOV eax,ebx

		MOV ebx,edi
		AND ebx,31
		SHL ebx,3
		MOV al,bl
		MOV ebx,edi
		SHR ebx,2
		AND ebx,248
		MOV ah,bl

		MOVD N,eax

		PUNPCKLBW N,N
		PSRLW N,8

		//R part + 25% current G + 25% previous B
		MOVQ R,C
		PAND R,cmr

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		MOVQ T,P
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		PACKUSWB R,R

		MOVD ebx,R
		XOR eax,eax

		SHR ebx,3
		AND ebx,0x001F1F1F

		MOV al,bl

		SHR ebx,3
		AND ebx,0x03E0
		OR eax,ebx
		SHR ebx,3
		AND ebx,0x7C00
		OR eax,ebx

		MOV [edx],ax

		//G part + 25% current B + 25% current R
		MOVQ R,C
		PAND R,cmg

		MOVQ T,C
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		PACKUSWB R,R

		MOVD ebx,R
		XOR eax,eax

		SHR ebx,3
		AND ebx,0x001F1F1F

		MOV al,bl

		SHR ebx,3
		AND ebx,0x03E0
		OR eax,ebx
		SHR ebx,3
		AND ebx,0x7C00
		OR eax,ebx

		MOV [edx+2],ax

		//B part + 25% next R + 25% current G
		MOVQ R,C
		PAND R,cmb

		MOVQ T,N
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		PACKUSWB R,R

		MOVD ebx,R
		XOR eax,eax

		SHR ebx,3
		AND ebx,0x001F1F1F

		MOV al,bl

		SHR ebx,3
		AND ebx,0x03E0
		OR eax,ebx
		SHR ebx,3
		AND ebx,0x7C00
		OR eax,ebx

		MOV [edx+4],ax

		MOV ebx,ebxsave
		MOV eax,eaxsave

		ADD edx,6

		MOVQ P,C
		MOVQ C,N

		//loop x
		DEC eax
		JNZ X_loop

		//after line
		MOV eax,ecx

		MOV ecx,D
		MOV edi,esi
		ADD edi,ecx

		SHR ecx,1	//duplicate twice

		REP MOVSD

		MOV ecx,eax

		ADD ecx,Q
		ADD edx,O
		MOV esi,edx

		//loop Y
		DEC ebx
		JNZ Y_loop

		EMMS
	}
}

void lcd3x_24(unsigned char *src, unsigned int S, unsigned char *dest, unsigned int D, int W, int H)
{
	int Q=S-W*3;
	int O=3*D-W*9; //2 lines + what is left in current

	__int16 mask[12]={0x00FF,0x0000,0x0000,0x0000,
					  0x0000,0x00FF,0x0000,0x0000,
					  0x0000,0x0000,0x00FF,0x0000};
	int M=(int)mask;

	__asm {
		//init
		MOV eax,M
		MOVQ cmr,[eax]
		MOVQ cmg,[eax+8]
		MOVQ cmb,[eax+16]

		MOV ebx,H
		MOV ecx,src
		MOV edx,dest
		MOV esi,edx

		//before line
Y_loop:	PXOR P,P
		PXOR C,C
		PUNPCKLBW C,[ecx]
		PSRLW C,8

		MOV eax,W

		//line
X_loop:	ADD ecx,3

		PUNPCKLBW N,[ecx]
		PSRLW N,8

		//R part + 25% current G + 25% previous B
		MOVQ R,C
		PAND R,cmr

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		MOVQ T,P
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		PACKUSWB R,R
		MOVD [edx],R
		MOV edi,edx
		ADD edi,D
		MOVD [edi],R
		ADD edi,D
		MOVD [edi],R

		//G part + 25% current B + 25% current R
		MOVQ R,C
		PAND R,cmg

		MOVQ T,C
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		PACKUSWB R,R
		MOVD [edx+3],R
		MOV edi,edx
		ADD edi,D
		MOVD [edi+3],R
		ADD edi,D
		MOVD [edi+3],R

		//B part + 25% next R + 25% current G
		MOVQ R,C
		PAND R,cmb

		MOVQ T,N
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		PACKUSWB R,R
		MOVD [edx+6],R
		MOV edi,edx
		ADD edi,D
		MOVD [edi+6],R
		ADD edi,D
		MOVD [edi+6],R
		ADD edx,9

		MOVQ P,C
		MOVQ C,N

		//loop x
		DEC eax
		JNZ X_loop

		/*
		//after line
		MOV eax,ecx

		MOV ecx,D
		MOV edi,esi
		ADD edi,ecx

		SHR ecx,1	//duplicate twice

		REP MOVSD

		MOV ecx,eax
		*/

		ADD ecx,Q
		ADD edx,O
		MOV esi,edx

		//loop Y
		DEC ebx
		JNZ Y_loop

		EMMS
	}
}

void lcd3x_32a(unsigned char *src, unsigned int S, unsigned char *dest, unsigned int D, int W, int H)
{
	int Q=S-W*4;
	int O=3*D-W*12; //2 lines + what is left in current

	__int16 mask[12]={0x00FF,0x0000,0x0000,0x0000,
					  0x0000,0x00FF,0x0000,0x0000,
					  0x0000,0x0000,0x00FF,0x0000};
	int M=(int)mask;

	__asm {
		//init
		MOV eax,M
		MOVQ cmr,[eax]
		MOVQ cmg,[eax+8]
		MOVQ cmb,[eax+16]

		MOV ebx,H
		MOV ecx,src
		MOV edx,dest
		MOV esi,edx

		//before line
Y_loop:	PXOR P,P
		PXOR C,C
		PUNPCKLBW C,[ecx]
		PSRLW C,8

		MOV eax,W

		//line
X_loop:	ADD ecx,4

		PXOR N,N
		PUNPCKLBW N,[ecx]
		PSRLW N,8

		//R part + 25% current G + 25% previous B
		MOVQ R,C
		PAND R,cmr

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		MOVQ T,P
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		PUSH eax
		PACKUSWB R,R
		MOVD [edx],R
		MOV eax,D
		MOV edi,edx
		ADD edi,eax
		MOVD [edi],R
		ADD edi,eax
		MOVD [edi],R
		POP eax

		//G part + 25% current B + 25% current R
		MOVQ R,C
		PAND R,cmg

		MOVQ T,C
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		PUSH eax
		PACKUSWB R,R
		MOVD [edx+4],R
		MOV eax,D
		MOV edi,edx
		ADD edi,eax
		MOVD [edi+4],R
		ADD edi,eax
		MOVD [edi+4],R
		POP eax

		//B part + 25% next R + 25% current G
		MOVQ R,C
		PAND R,cmb

		MOVQ T,N
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		PUSH eax
		PACKUSWB R,R
		MOVD [edx+8],R
		MOV eax,D
		MOV edi,edx
		ADD edi,eax
		MOVD [edi+8],R
		ADD edi,eax
		MOVD [edi+8],R
		POP eax
		ADD edx,12

		MOVQ P,C
		MOVQ C,N

		//loop x
		DEC eax
		JNZ X_loop

		//after line
		/*
		MOV eax,ecx
		MOV ecx,D
		MOV edi,esi
		ADD edi,ecx

		SHR ecx,1	//duplicate twice

		REP MOVSD

		MOV ecx,eax
		*/

		ADD ecx,Q
		ADD edx,O
		MOV esi,edx

		//loop Y
		DEC ebx
		JNZ Y_loop

		EMMS
	}
}

void lcd3x_32rgb(unsigned char *src, unsigned int S, unsigned char *dest, unsigned int D, int W, int H)
{
	int Q=S-W*4;
	int O=3*D-W*12; //2 lines + what is left in current

	__int16 mask[12]={0x00FF,0x0000,0x0000,0x0000,
					  0x0000,0x00FF,0x0000,0x0000,
					  0x0000,0x0000,0x00FF,0x0000};
	int M=(int)mask;

	__asm {
		//init
		MOV eax,M
		MOVQ cmr,[eax]
		MOVQ cmg,[eax+8]
		MOVQ cmb,[eax+16]

		MOV ebx,H
		MOV ecx,src
		MOV edx,dest
		MOV esi,edx

		//before line
Y_loop:	PXOR P,P
		PXOR C,C
		PUNPCKLBW C,[ecx]
		PSRLW C,8

		MOV eax,W

		//line
X_loop:	ADD ecx,4

		PXOR N,N
		PUNPCKLBW N,[ecx]
		PSRLW N,8

		//R part + 25% current G + 25% previous B
		MOVQ R,C
		PAND R,cmr

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		MOVQ T,N
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		PACKUSWB R,R
		MOVD [edx+8],R

		//G part + 25% current B + 25% current R
		MOVQ R,C
		PAND R,cmg

		MOVQ T,C
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		PACKUSWB R,R
		MOVD [edx+4],R

		//B part + 25% next R + 25% current G
		MOVQ R,C
		PAND R,cmb

		MOVQ T,P
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		PACKUSWB R,R
		MOVD [edx],R
		ADD edx,12

		MOVQ P,C
		MOVQ C,N

		//loop x
		DEC eax
		JNZ X_loop

		//after line
		MOV eax,ecx
		MOV ecx,D
		MOV edi,esi
		ADD edi,ecx

		SHR ecx,1	//duplicate twice

		REP MOVSD

		MOV ecx,eax

		ADD ecx,Q
		ADD edx,O
		MOV esi,edx

		//loop Y
		DEC ebx
		JNZ Y_loop

		EMMS
	}
}

void lcd3x_32(unsigned char *src, unsigned int S, unsigned char *dest, unsigned int D, int W, int H)
{
	int Q=S-W*4;
	int O=3*D-W*12; //2 lines + what is left in current

	__int16 mask[12]={0x00FF,0x0000,0x0000,0x0000,
					  0x0000,0x00FF,0x0000,0x0000,
					  0x0000,0x0000,0x00FF,0x0000};

	__int16 mulm[4]={3,3,3,3};

	int M=(int)mask;

	__asm {
		//init
		MOV eax,M
		MOVQ cmr,[eax]
		MOVQ cmg,[eax+8]
		MOVQ cmb,[eax+16]

		MOV ebx,H
		MOV ecx,src
		MOV edx,dest
		MOV esi,edx

		//before line
Y_loop:	PXOR P,P
		PXOR C,C
		PUNPCKLBW C,[ecx]
		PSRLW C,8

		MOV eax,W

		//line
X_loop:	ADD ecx,4

		PXOR N,N
		PUNPCKLBW N,[ecx]
		PSRLW N,8

		//R part + 25% current G + 25% previous B
		MOVQ R,C
		PAND R,cmr

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		MOVQ T,P
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		PMULLW R,mulm
		PSRLW R,1

		PUSH eax
		PACKUSWB R,R
		MOVD [edx],R
		MOV eax,D
		MOV edi,edx
		ADD edi,eax
		MOVD [edi],R
		ADD edi,eax
		MOVD [edi],R
		POP eax

		//G part + 25% current B + 25% current R
		MOVQ R,C
		PAND R,cmg

		MOVQ T,C
		PAND T,cmb
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		PMULLW R,mulm
		PSRLW R,1

		PUSH eax
		PACKUSWB R,R
		MOVD [edx+4],R
		MOV eax,D
		MOV edi,edx
		ADD edi,eax
		MOVD [edi+4],R
		ADD edi,eax
		MOVD [edi+4],R
		POP eax

		//B part + 25% next R + 25% current G
		MOVQ R,C
		PAND R,cmb

		MOVQ T,N
		PAND T,cmr
		PSRLW T,1
		POR  R,T

		MOVQ T,C
		PAND T,cmg
		PSRLW T,1
		POR  R,T

		PMULLW R,mulm
		PSRLW R,1

		PUSH eax
		PACKUSWB R,R
		MOVD [edx+8],R
		MOV eax,D
		MOV edi,edx
		ADD edi,eax
		MOVD [edi+8],R
		ADD edi,eax
		MOVD [edi+8],R
		POP eax
		ADD edx,12

		MOVQ P,C
		MOVQ C,N

		//loop x
		DEC eax
		JNZ X_loop

		//after line
		/*
		MOV eax,ecx
		MOV ecx,D
		MOV edi,esi
		ADD edi,ecx

		SHR ecx,1	//duplicate twice

		REP MOVSD

		MOV ecx,eax
		*/

		ADD ecx,Q
		ADD edx,O
		MOV esi,edx

		//loop Y
		DEC ebx
		JNZ Y_loop

		EMMS
	}
}
