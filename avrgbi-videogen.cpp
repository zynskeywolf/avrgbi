#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "macros.h"
#include "avrgbi-videogen.h"

volatile unsigned short scanLine;
unsigned char linerepeat, renderLine, *ptr, nextLine, field=0;

unsigned char * render_setup() {
	TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
	TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.PER = _LINE_CYCLES;
	TCA0.SINGLE.CMP0 = _HSYNC_CYCLES;

	scanLine = _FRAME_LINES;
	nextLine = 0;
	ptr = (unsigned char*)malloc(_VBUFSIZE);

	sei();
	return ptr;
}

// start of line after sync pulse
ISR(TCA0_OVF_vect) {

	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
	switch(nextLine)
	{
	case 0: // vsync
		if (scanLine == _VSYNC_LINES) {
			PORTB.PIN0CTRL = 128;
			nextLine = 1;
		}
		else if (scanLine == _FRAME_LINES) {
			PORTB.PIN0CTRL = 0;
			scanLine = 0;
		}
		break;

	case 1: // back porch
		if (scanLine == _VOFFSET)
		{
			renderLine = 0;
			linerepeat = _LINERPT;
			nextLine = 2;
		}
		break;

	case 2: // active line
		wait_until(_HOFFSET);
		__asm__ __volatile__ (
		    _RNDRLN
		    :
		    : [port] "i" (&VPORTD.OUT),
		    "x" (ptr+renderLine*_HBYTES+field*_VRES*_HBYTES),
		    [bleft] "d" (_HBYTES) //bytes left
		    : "r16", "r17"
		);
		if (linerepeat)
			linerepeat--;
		else
		{
			linerepeat = _LINERPT;
			renderLine ++;
		}
		if (scanLine == _LASTLINE)
			nextLine = 3;
		break;

	case 3: // front porch
		if (scanLine == _FRAME_LINES - 1)
		{
			nextLine = 0;
			#ifdef STEREO
			field^=1;
			VPORTE.OUT^=6;
			#endif
		}
		break;
	}
	scanLine++;
}

void inline wait_until(unsigned short time) {
	__asm__ __volatile__ (
	    "sub %[time], %[tcnt]\n"
	    "100:\n"
	    "subi %[time], 3\n"
	    "brcc 100b\n"
	    "subi %[time], 0-3\n"
	    "breq 101f\n"
	    "dec %[time]\n"
	    "breq 102f\n"
	    "rjmp 102f\n"
	    "101:\n"
	    "nop\n"
	    "102:\n"
	    :
	    : [time] "a" (time),
	    [tcnt] "a" (TCA0.SINGLE.CNT)
	);
}
