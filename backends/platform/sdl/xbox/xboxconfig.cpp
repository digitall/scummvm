#ifdef _XBOX

#include <xtl.h>
#include "xboxconfig.h"
#include "stdlib.h"

#include <SDL.h>

#include "SDL_config_lib.h"
#include "SDL_config_auto_tests.h"

int xpos = 0;
int ypos = 0;
int xstretch = 0;
int ystretch = 0;

#ifndef _XBOX360
int flickerfilter = 1;
int m720p = 1;
LPDIRECT3DDEVICE8 D3D_Device;
#endif

/*  ----------------------------------------- */

CFG_File config;
int result;

/*  ----------------------------------------- */

void XboxSetScreen (void)
{
#ifndef _XBOX360
if (flickerfilter>5) flickerfilter=5;
 else if (flickerfilter<0) flickerfilter=0;
IDirect3DDevice8_SetFlickerFilter(D3D_Device, flickerfilter);
#endif
SDL_XBOX_SetScreenPosition(xpos, ypos);
SDL_XBOX_SetScreenStretch(xstretch, ystretch);
SDL_ShowCursor(SDL_DISABLE);
}

void XboxLoadConfig (void)
 {
  int result = CFG_OpenFile("D:\\xbox.cfg", &config );
  Uint32 flags = SDL_INIT_VIDEO;

  /* CFG_OpenFile returns value, that allows you to check if everything went right, or do we have problems */
  if ( CFG_ERROR == result )
   {
    result = CFG_OpenFile((0), &config );
   }

  xpos = CFG_ReadInt("xpos", 0);
  ypos = CFG_ReadInt("ypos", 0);
  xstretch = CFG_ReadInt("xstretch", 0);
  ystretch = CFG_ReadInt("ystretch", 0);

#ifndef _XBOX360
  flickerfilter = CFG_ReadInt("flickerfilter", 1);
  if (flickerfilter>5) flickerfilter=5;
	else if (flickerfilter<0) flickerfilter=0;

  m720p = CFG_ReadInt("720p", 1);
  if (m720p) flags |= SDL_INIT_VIDEO720;
#endif
  SDL_Init(flags);
 }

 void XboxSaveConfig (void)
 {
	CFG_WriteInt("xpos", xpos);
	CFG_WriteInt("ypos", ypos);
	CFG_WriteInt("xstretch", xstretch);
	CFG_WriteInt("ystretch", ystretch);
#ifndef _XBOX360
	CFG_WriteInt("flickerfilter", flickerfilter);
	CFG_WriteInt("720p", m720p);
#endif

	CFG_SaveFile("D:\\xbox.cfg", CFG_SORT_ORIGINAL, CFG_COMPRESS_OUTPUT);

  /* Close selected file (we would pass to it 'config' variable instead of 0, if it wasn't selected) */
	CFG_CloseFile(0);

 }
#endif
