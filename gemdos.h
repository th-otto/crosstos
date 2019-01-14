#ifndef _GEMDOS_H_
#define _GEMDOS_H_

#include <stdint.h>

#define GEMDOS_E_OK 	(0)

extern void 	Mfree(uint32_t block);
extern uint32_t Malloc(int32_t bytes);
extern uint32_t gemdos_dispatch(uint16_t opcode, uint32_t prm);
extern void 	gemdos_init(uint8_t* ram, uint32_t ramsize);

#endif /* _GEMDOS_H_ */
