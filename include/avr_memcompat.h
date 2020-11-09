#ifndef AVR_MEMCOMPAT_H
#define AVR_MEMCOMPAT_H

#ifdef __AVR__

#include <avr/pgmspace.h>

#else

#include <stdint.h>

#define PROGMEM
#define pgm_read_dword(ptr) (*((uint64_t *) (ptr)))
#define pgm_read_byte(ptr) (*((uint8_t *) (ptr)))

#endif
#endif
