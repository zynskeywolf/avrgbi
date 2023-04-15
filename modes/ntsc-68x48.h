#define _FRAME_LINES 262
#define _VSYNC_LINES 3
#define _HSYNC_CYCLES ((4.7 * _CYCLES_PER_US) - 1)
#define _LINE_CYCLES ((63.55 * _CYCLES_PER_US) - 1)

#define _HBYTES 34
#define _HOFFSET 216
#define _VRES 48
#define _VOFFSET 35
#define _LINERPT 3

#define _RNDRLN \
"loop:\n"\
"LD r17,X+\n"\
"mov r16,r17\n"\
"andi r16,15\n"\
"out %[port],r16\n"\
"lsr r17\n"\
"lsr r17\n"\
"lsr r17\n"\
"lsr r17\n"\
"delay5\n"\
"out %[port],r17\n"\
"delay2\n"\
"dec %[bleft]\n"\
"brne loop\n"\
"clr r17\n"\
"delay4\n"\
"out %[port],r17\n"\

