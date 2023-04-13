#define _CYCLES_PER_US (F_CPU / 1000000)

__asm__ __volatile__ (
  ".macro delay1\n"
   "nop\n"
  ".endm\n"

  ".macro delay2\n"
    "nop\n"
    "nop\n"
  ".endm\n"
  
  ".macro delay3\n"
    "delay2\n"
    "nop\n"
  ".endm\n"
  
  ".macro delay4\n"
    "delay2\n"
    "delay2\n"
  ".endm\n"

  ".macro delay5\n"
    "delay3\n"
    "delay2\n"
  ".endm\n"
  
  ".macro delay6\n"
    "delay3\n"
    "delay3\n"
  ".endm\n"
  
  ".macro delay7\n"
    "delay4\n"
    "delay3\n"
  ".endm\n"
  
  ".macro delay8\n"
    "delay4\n"
    "delay4\n"
  ".endm\n"
);
