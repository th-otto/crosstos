#ifndef _BINARY_H_
#define _BINARY_H_

#if defined(WIN32)

/*
 * Some WIN32/MingW kludges
 */

extern uint8_t _binary[];
#define binary _binary

#else
extern uint8_t binary[];
#endif

#endif
