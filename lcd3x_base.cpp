//---------------------------------------------------------------------------------------------------------------------------
// gigaherz's lcd3x filter
//---------------------------------------------------------------------------------------------------------------------------
#include "Common.h"

typedef void (* processFn)(u8*, u32, u8*,  u8*, u32, int, int);

//---------------------------------------------------------------------------------------------------------------------------

const u32 RpiVersion = 0x02;

enum RpiMmxSupport : u32
{
	RpiNoMmx = 0,
	RpiSupportMmx = 1,
	RpiRequireMmx = 2,
};

enum RpiPixelFormatSupport : u32
{
	RpiPixelFormat555 = 1,
	RpiPixelFormat565 = 2,
	RpiPixelFormat888 = 4,
};

u32 __forceinline RpiMakeFlags(int mmx, int formats, int scaleLevel)
{
	return RpiVersion | (mmx << 8) | (formats << 10) | (scaleLevel << 16);
}

//---------------------------------------------------------------------------------------------------------------------------

typedef struct
{
	u32		Size;
	u32		Flags;
	void	*SrcPtr;
	u32		SrcPitch;
	u32		SrcW;
	u32		SrcH;
	void	*DstPtr;
	u32		DstPitch;
	u32		DstW;
	u32		DstH;
	u32		OutW;
	u32		OutH;
} RENDER_PLUGIN_OUTP;

extern	"C"	__declspec(dllexport) void RenderPluginOutput(RENDER_PLUGIN_OUTP *rpo);

//---------------------------------------------------------------------------------------------------------------------------

struct RENDER_PLUGIN_INFO
{
	char	Name[60];
	u32		Flags;
	void*	Handle;
	void*	OutputFunction; // Presumably to override the exported one
} static PluginInfo = {
	"LCD3x by gigaherz"
#ifdef _DEBUG
	" [DEBUG]"
#endif
#ifdef DOVBLEND
	" [VBlend"
#ifdef HSCANLINES
	",Scanlines"
#endif
	"]"
#endif
	, RpiMakeFlags(
#ifndef NOMMX
		RpiSupportMmx | RpiRequireMmx,
#else
		RpiNoMmx,
#endif
		RpiPixelFormat555 | RpiPixelFormat565 | RpiPixelFormat888, 3),
	nullptr,
	nullptr
};

//---------------------------------------------------------------------------------------------------------------------------
extern	"C"	__declspec(dllexport) RENDER_PLUGIN_INFO * RenderPluginGetInfo(void) { return &PluginInfo; }

//---------------------------------------------------------------------------------------------------------------------------
extern	"C"	__declspec(dllexport) void RenderPluginOutput(RENDER_PLUGIN_OUTP *rpo)
{
	RpiPixelFormatSupport format = (RpiPixelFormatSupport)((rpo->Flags >> 10) & 0xF);

#ifdef _DEBUG
	RpiMmxSupport mmxMode = (RpiMmxSupport)((rpo->Flags >> 8) & 3);
#endif

	int bytesperpixel = rpo->SrcPitch / rpo->SrcW;

	if(	((rpo->SrcW*3)>rpo->DstW) ||
		((rpo->SrcH*3)>rpo->DstH) ||
		bytesperpixel > 4)
	{
		rpo->OutW=0;
		rpo->OutH=0;
		return;
	}

	if (format & RpiPixelFormat888)
	{
		if (bytesperpixel >= 4) lcd3x_8888((u8*)rpo->SrcPtr, rpo->SrcPitch, (u8*)rpo->DstPtr, rpo->DstPitch, rpo->SrcW, rpo->SrcH);
		else if (bytesperpixel == 3) lcd3x_888((u8*)rpo->SrcPtr, rpo->SrcPitch, (u8*)rpo->DstPtr, rpo->DstPitch, rpo->SrcW, rpo->SrcH);
	}
	else if (format&RpiPixelFormat565)  lcd3x_565((u8*)rpo->SrcPtr, rpo->SrcPitch, (u8*)rpo->DstPtr, rpo->DstPitch, rpo->SrcW, rpo->SrcH);
	else if (format&RpiPixelFormat555)  lcd3x_555((u8*)rpo->SrcPtr, rpo->SrcPitch, (u8*)rpo->DstPtr, rpo->DstPitch, rpo->SrcW, rpo->SrcH);

	// Set the output size incase anybody cares.
	rpo->OutW=rpo->SrcW*3;
	rpo->OutH=rpo->SrcH*3;
}

//---------------------------------------------------------------------------------------------------------------------------
