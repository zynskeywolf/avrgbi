#define _FRAME_LINES 262
#define _VSYNC_LINES 3
#define _HSYNC_CYCLES ((4.7 * _CYCLES_PER_US) - 1)
#define _LINE_CYCLES ((63.5 * _CYCLES_PER_US) - 1)

#define _HBYTES 45
#define _HOFFSET (14 * _CYCLES_PER_US)
#define _VRES 64
#define _VOFFSET 40
#define _LINERPT 2

#define STEREO

#define _RNDRLN \
"loop:\n"\
"LD r16,X+\n"\
"delay4\n"\
"out 0x0d,r16\n"\
"delay6\n"\
"dec %[bleft]\n"\
"swap r16\n"\
"out 0x0d,r16\n"\
"brne loop\n"\
"clr r16\n"\
"delay6\n"\
"out 0x0d,r16\n"\

