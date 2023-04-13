#define _FRAME_LINES 525
#define _VSYNC_LINES 2
#define _HSYNC_CYCLES ((3.81 * _CYCLES_PER_US) - 1)
#define _LINE_CYCLES ((31.78 * _CYCLES_PER_US) - 1)

#define _HBYTES 22
#define _HOFFSET 90
#define _VRES 77
#define _VOFFSET 40
#define _LINERPT 5

#define _RNDRLN \
"loop:\n"\
"LD r17,X+\n"\
"mov r16,r17\n"\
"andi r16,240\n"\
"out %[port],r16\n"\
"lsl r17\n"\
"lsl r17\n"\
"lsl r17\n"\
"lsl r17\n"\
"delay3\n"\
"out %[port],r17\n"\
"dec %[bleft]\n"\
"brne loop\n"\
"clr r17\n"\
"delay4\n"\
"out %[port],r17\n"\

