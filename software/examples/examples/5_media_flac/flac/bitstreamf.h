/**
 * @file bitstream.h
 * bitstream api header.
 */

#ifndef BITSTREAMF_H
#define BITSTREAMF_H


#define IBSS_ATTR
#define ICONST_ATTR
#define ICODE_ATTR

#ifndef ICODE_ATTR_FLAC
#define ICODE_ATTR_FLAC ICODE_ATTR
#endif

#ifndef IBSS_ATTR_FLAC_DECODED0
#define IBSS_ATTR_FLAC_DECODED0 IBSS_ATTR
#endif

/* Endian conversion routines for standalone compilation */
#define letoh32(x) (x)
#define betoh32(x) swap32(x)

/* Taken from rockbox/firmware/export/system.h */

static __inline unsigned short swap16(unsigned short value)
/*
	result[15..8] = value[ 7..0];
	result[ 7..0] = value[15..8];
*/
{
	return (value >> 8) | (value << 8);
}

static __inline unsigned long swap32(unsigned long value)
/*
	result[31..24] = value[ 7.. 0];
	result[23..16] = value[15.. 8];
	result[15.. 8] = value[23..16];
	result[ 7.. 0] = value[31..24];
*/
{
	unsigned long hi = swap16(value >> 16);
	unsigned long lo = swap16(value & 0xffff);
	
	return (lo << 16) | hi;

//	register int temp;
//	__asm{
//		EOR temp, value, value, ROR#16
//		MOV temp, temp, LSR#8
//		BIC temp, temp, #0xFF00
//		EOR value, temp, value, ROR#8
//	}
//	return value;
}


/* FLAC files are big-endian */
#define ALT_BITSTREAM_READER_BE
 
#define NEG_SSR32(a,s) (((int32_t)(a))>>(32-(s)))
#define NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))

/* bit input */
/* buffer, buffer_end and size_in_bits must be present and used by every reader */
typedef struct GetBitContext {
	const uint8_t *buffer, *buffer_end;
	int index;
	int size_in_bits;
} GetBitContext;

#define VLC_TYPE int16_t

typedef struct VLC {
	int bits;
	VLC_TYPE (*table)[2]; ///< code, bits
	int table_size, table_allocated;
} VLC;

typedef struct RL_VLC_ELEM {
	int16_t level;
	int8_t len;
	uint8_t run;
} RL_VLC_ELEM;


static __inline uint32_t unaligned32(const void *v) {
/*
	__packed struct Unaligned {
		uint32_t i;
	};
	
	return ((struct Unaligned *)v)->i;
*/
	return *(__packed unsigned long *)v;
}


/* Bitstream reader API docs:
name
    abritary name which is used as prefix for the internal variables

gb
    getbitcontext

OPEN_READER(name, gb)
    loads gb into local variables

CLOSE_READER(name, gb)
    stores local vars in gb

UPDATE_CACHE(name, gb)
    refills the internal cache from the bitstream
    after this call at least MIN_CACHE_BITS will be available,

GET_CACHE(name, gb)
    will output the contents of the internal cache, next bit is MSB of 32 or 64 bit (FIXME 64bit)

SHOW_UBITS(name, gb, num)
    will return the next num bits

SHOW_SBITS(name, gb, num)
    will return the next num bits and do sign extension

SKIP_BITS(name, gb, num)
    will skip over the next num bits
    note, this is equivalent to SKIP_CACHE; SKIP_COUNTER

SKIP_CACHE(name, gb, num)
    will remove the next num bits from the cache (note SKIP_COUNTER MUST be called before UPDATE_CACHE / CLOSE_READER)

SKIP_COUNTER(name, gb, num)
    will increment the internal bit counter (see SKIP_CACHE & SKIP_BITS)

LAST_SKIP_CACHE(name, gb, num)
    will remove the next num bits from the cache if it is needed for UPDATE_CACHE otherwise it will do nothing

LAST_SKIP_BITS(name, gb, num)
    is equivalent to SKIP_LAST_CACHE; SKIP_COUNTER

for examples see get_bits, show_bits, skip_bits, get_vlc
*/

static __inline int unaligned32_be(const void *v){
	return betoh32(unaligned32(v));  // original
}

static __inline int unaligned32_le(const void *v){
	return letoh32(unaligned32(v));  // original
}

#   define MIN_CACHE_BITS 25

#   define OPEN_READER(name, gb)\
        int name##_index= (gb)->index;\
        int name##_cache= 0;\

#   define CLOSE_READER(name, gb)\
        (gb)->index= name##_index;\

#   define UPDATE_CACHE(name, gb)\
        name##_cache= unaligned32_be( ((const uint8_t *)(gb)->buffer)+(name##_index>>3) ) << (name##_index&0x07);\

#   define SKIP_CACHE(name, gb, num)\
        name##_cache <<= (num);


// FIXME name?
#   define SKIP_COUNTER(name, gb, num)\
        name##_index += (num);\

#   define SKIP_BITS(name, gb, num)\
        {\
            SKIP_CACHE(name, gb, num)\
            SKIP_COUNTER(name, gb, num)\
        }\

#   define LAST_SKIP_BITS(name, gb, num) SKIP_COUNTER(name, gb, num)
#   define LAST_SKIP_CACHE(name, gb, num) ;

#   define SHOW_UBITS(name, gb, num)\
        NEG_USR32(name##_cache, num)

#   define SHOW_SBITS(name, gb, num)\
        NEG_SSR32(name##_cache, num)

#   define GET_CACHE(name, gb)\
        ((uint32_t)name##_cache)


static __inline int get_bits_count(GetBitContext *s){
	return s->index;
}

static __inline int get_sbits(GetBitContext *s, int n){
	register int tmp;
	OPEN_READER(re, s)
	UPDATE_CACHE(re, s)
	tmp = SHOW_SBITS(re, s, n);
	LAST_SKIP_BITS(re, s, n)
	CLOSE_READER(re, s)
	return tmp;
}

static __inline unsigned int get_bits(GetBitContext *s, int n){
	register int tmp;
	OPEN_READER(re, s)
	UPDATE_CACHE(re, s)
	tmp = SHOW_UBITS(re, s, n);
	LAST_SKIP_BITS(re, s, n)
	CLOSE_READER(re, s)
    return tmp;
}

static __inline unsigned int show_bits(GetBitContext *s, int n){
	register int tmp;
	OPEN_READER(re, s)
	UPDATE_CACHE(re, s)
	tmp = SHOW_UBITS(re, s, n);
//	CLOSE_READER(re, s)
	return tmp;
}

static __inline void skip_bits(GetBitContext *s, int n){
/*
	OPEN_READER(re, s)
	UPDATE_CACHE(re, s)
	LAST_SKIP_BITS(re, s, n)
	CLOSE_READER(re, s)

	int re_index = s->index;
	unaligned32_be(((const uint8_t *)s->buffer) + (re_index >> 3)) << (re_index & 0x07);
	re_index += n;
	s->index = re_index;
*/
	s->index += n;
}

static __inline unsigned int get_bits1(GetBitContext *s){
	int index = s->index;
	uint8_t result = s->buffer[index >> 3];
	result <<= (index & 0x07);
	result >>= 8 - 1;
	index ++;
	s->index = index;
	return result;
}

static __inline unsigned int show_bits1(GetBitContext *s){
    return show_bits(s, 1);
}

static __inline void skip_bits1(GetBitContext *s){
    skip_bits(s, 1);
}

static __inline void init_get_bits(GetBitContext *s, const uint8_t *buffer, int bit_size){
    int buffer_size= (bit_size+7)>>3;
    if(buffer_size < 0 || bit_size < 0) {
        buffer_size = bit_size = 0;
        buffer = 0;
    }

    s->buffer= buffer;
    s->size_in_bits= bit_size;
    s->buffer_end= buffer + buffer_size;
    s->index=0;
/*
    {
        OPEN_READER(re, s)
        UPDATE_CACHE(re, s)
        UPDATE_CACHE(re, s)
        CLOSE_READER(re, s)
    }
	
	{
		int re_index = s->index;
		unaligned32_be(((const uint8_t *)s->buffer) + (re_index >> 3)) << (re_index & 0x07);
		unaligned32_be(((const uint8_t *)s->buffer) + (re_index >> 3)) << (re_index & 0x07);
		s->index = re_index;
	}
*/
}

void align_get_bits(GetBitContext *s);

#endif /* BITSTREAM_H */
