#if defined(TOS_BINARY)

    .globl _binary

_binary:

    .incbin TOS_BINARY
    
#endif
    .end
