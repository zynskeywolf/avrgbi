#define _FRAME_LINES 262
#define _VSYNC_LINES 3
#define _HSYNC_CYCLES ((4.7 * _CYCLES_PER_US) - 1)
#define _LINE_CYCLES ((63.5 * _CYCLES_PER_US) - 1)

#define _HBYTES 60
#define _HOFFSET 280
#define _VRES 96
#define _VOFFSET 40
#define _LINERPT 1

#define _RNDRLN \
"loop:\n"\
"LD r16,X+\n"\
"delay2\n"\
"out 0x0d,r16\n"\
"delay4\n"\
"dec %[bleft]\n"\
"swap r16\n"\
"out 0x0d,r16\n"\
"brne loop\n"\
"clr r16\n"\
"delay4\n"\
"out 0x0d,r16\n"\

