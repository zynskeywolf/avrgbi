#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "macros.h"
#include "avrgbi-videogen.h"

volatile unsigned short scanLine;
unsigned char linerepeat,renderLine,*ptr;
void (*line_handler)();

unsigned char * render_setup() {
	ptr=(unsigned char*)malloc(_VBUFSIZE);
	DDRD |= 0b11110000;
	PORTD = 0;

	// inverted fast pwm mode on timer 1
	TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM11);
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);

	ICR1 = _LINE_CYCLES; // reset at line end
	OCR1A = _HSYNC_CYCLES; // sync pulse off

	scanLine = _FRAME_LINES;
	line_handler = &vsync_line;
	TIMSK1 = _BV(TOIE1);
	sei();
	return ptr;
}

// start of line after sync pulse
ISR(TIMER1_OVF_vect) {
	line_handler();
}

void vsync_line() {
	if (scanLine == _FRAME_LINES) {
		OCR1A = _VSYNC_CYCLES; // keep sync pulse on until back porch start
		scanLine = 0;
	}
	else if (scanLine == _VSYNC_LINES) {
		OCR1A = _HSYNC_CYCLES;
		line_handler = &backporch_line;
	}
	scanLine++;
}

void backporch_line() {
	if (scanLine == _VOFFSET) // first line of picture
	{
		renderLine = 0;
		linerepeat = _LINERPT;
		line_handler = &active_line;
	}
	scanLine++;
}

void active_line() {
	wait_until(_HOFFSET);
	__asm__ __volatile__ (
    _RNDRLN
    :
    : [port] "i" (_SFR_IO_ADDR(PORTD)),
    "x" (ptr+renderLine*_HBYTES),
    [bleft] "d" (_HBYTES) //bytes left
    : "r16", "r17", "r18"
  );
	if (linerepeat)
		linerepeat--;
	else
	{
		linerepeat = _LINERPT;
		renderLine ++;
	}
	if (scanLine == _LASTLINE)
		line_handler = &frontporch_line;
	scanLine++;
}

void frontporch_line() {
  if (scanLine == _FRAME_LINES-1)
    line_handler = &vsync_line;
  scanLine++;
}

static void inline wait_until(unsigned char time) {
	__asm__ __volatile__ (
		"sub %[time], %[tcnt1l]\n"
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
		[tcnt1l] "a" (TCNT1L)
	);
}
