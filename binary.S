#if defined(BINARY)

	.data

.globl _binary

_binary:

    .incbin BINARY
    
#endif
    .end
