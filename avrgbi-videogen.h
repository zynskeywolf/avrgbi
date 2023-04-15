// See available modes in ./modes, include the one you need
#include "modes/ntsc-68x48.h"

#define _VSYNC_CYCLES ((_VSYNC_LINES * _LINE_CYCLES) - 1)
#define _HRES (_HBYTES*2)
#define _VBUFSIZE (_HBYTES*_VRES)
#define _LASTLINE (_VOFFSET+_VRES*(_LINERPT+1))

unsigned char* render_setup();
void vsync_line();
void backporch_line();
void active_line();
void frontporch_line();
static void inline wait_until(unsigned char);
