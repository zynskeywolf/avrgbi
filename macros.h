#define _CYCLES_PER_US (F_CPU / 1000000)

#ifdef ARDUINO_AVR_UNO
#define VDDR DDRC
#define VPORT PORTC
#endif

__asm__ __volatile__ (

	".macro delay2\n"
		"nop\n"
		"nop\n"
	".endm\n"

	".macro delay4\n"
		"delay2\n"
		"delay2\n"
	".endm\n"

);
