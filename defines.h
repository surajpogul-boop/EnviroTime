//defines.h

#define SETBIT(WORD,BITPOS) \
    ((WORD) |= ((u32)1 << (BITPOS)))

#define CLRBIT(WORD,BITPOS) \
    ((WORD) &= ~((u32)1 << (BITPOS)))

#define CPLBIT(WORD,BITPOS) \
    ((WORD) ^= ((u32)1 << (BITPOS)))

#define READBIT(WORD,BITPOS) \
    (((WORD) >> (BITPOS)) & 1)

#define WRITE2BIT(WORD,BITPOS,BITLEVEL) \
    ((WORD) = ((WORD & ~((u32)1 << (BITPOS))) | ((u32)(BITLEVEL) << (BITPOS))))

#define READWRITEBIT(WORD,DBIT,SBIT) \
    ((WORD) = ((WORD & ~((u32)1 << (DBIT))) | (((WORD >> (SBIT)) & 1) << (DBIT))))

#define READNIBBLE(WORD,STARTBITPOS) \
    (((WORD) >> (STARTBITPOS)) & 15)

#define WRITENIBBLE(WORD,STARTBITPOS,NIBBLEVALUE) \
    ((WORD) = ((WORD & ~(15 << (STARTBITPOS))) | ((u32)(NIBBLEVALUE) << (STARTBITPOS))))

#define READBYTE(WORD,STARTBITPOS) \
    (((WORD) >> (STARTBITPOS)) & 255)

#define WRITEBYTE(WORD,STARTBYTE,BYTEVAL) \
    ((WORD) = ((WORD & ~(255<< (STARTBYTE))) | ((u32)(BYTEVAL) << (STARTBYTE))))
			
#define READWRITEBIT2(DWORD,DBIT,SWORD,SBIT) \
		DWORD=((DWORD & ~((u32)1<<DBIT)|((SWORD>>SBIT)&1)<<DBIT))

