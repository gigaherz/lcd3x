//---------------------------------------------------------------------------------------------------------------------------
// gigaherz's lcd3x filter
//---------------------------------------------------------------------------------------------------------------------------
#include "Common.h"

typedef void (* processFn)(u8*, u32, u8*,  u8*, u32, int, int);

//---------------------------------------------------------------------------------------------------------------------------

const u32 RpiVersion = 0x02;

enum RpiMmxSupport : int
{
	RpiNoMmx = 0,
	RpiSupportMmx = 1,
	RpiRequireMmx = 2,
};

enum RpiPixelFormatSupport : int
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
	"LCD3x"
#ifndef NOMMX
	" w/MMX"
#endif
#ifdef DOVBLEND
	" w/VBlend"
#endif	
	" (gigaherz)",
	
	RpiMakeFlags(
#ifndef NOMMX
		RpiSupportMmx | RpiRequireMmx,
#else
		RpiNoMmx,
#endif
		RpiPixelFormat555 | RpiPixelFormat565 | RpiPixelFormat888, 3)
};

//---------------------------------------------------------------------------------------------------------------------------
extern	"C"	__declspec(dllexport) RENDER_PLUGIN_INFO * RenderPluginGetInfo(void) { return &PluginInfo; }

//---------------------------------------------------------------------------------------------------------------------------
extern	"C"	__declspec(dllexport) void RenderPluginOutput(RENDER_PLUGIN_OUTP *rpo)
{
	int bytesperpixel = rpo->SrcPitch / rpo->SrcW;

	if(	((rpo->SrcW*3)>rpo->DstW) ||
		((rpo->SrcH*3)>rpo->DstH) )
	{
		rpo->OutW=0;
		rpo->OutH=0;
		return;
	}

	if(bytesperpixel >= 3 &&
		    (rpo->Flags&RpiPixelFormat888)) lcd3x_24    ((u8*)rpo->SrcPtr, rpo->SrcPitch, (u8*)rpo->DstPtr, rpo->DstPitch, rpo->SrcW, rpo->SrcH);
	else if (rpo->Flags&RpiPixelFormat565)  lcd3x_16_565((u8*)rpo->SrcPtr, rpo->SrcPitch, (u8*)rpo->DstPtr, rpo->DstPitch, rpo->SrcW, rpo->SrcH);
	else if (rpo->Flags&RpiPixelFormat888)  lcd3x_16_555((u8*)rpo->SrcPtr, rpo->SrcPitch, (u8*)rpo->DstPtr, rpo->DstPitch, rpo->SrcW, rpo->SrcH);

	// Set the output size incase anybody cares.
	rpo->OutW=rpo->SrcW*3;
	rpo->OutH=rpo->SrcH*3;
}

//---------------------------------------------------------------------------------------------------------------------------
