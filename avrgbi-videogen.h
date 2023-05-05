// See available modes in ./modes, include the one you need
#include "modes/ntsc-120x96.h"

#define _VSYNC_CYCLES ((_VSYNC_LINES * _LINE_CYCLES) - 1)
#define _HRES (_HBYTES*2)
#define _VBUFSIZE (_HBYTES*_VRES)
#define _LASTLINE (_VOFFSET+_VRES*(_LINERPT+1))

unsigned char* render_setup();
void inline wait_until(unsigned short);
