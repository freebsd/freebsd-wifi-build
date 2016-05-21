#include "trxloader.h"

void _startC(register_t a0, register_t a1, register_t a2, register_t a3){
	__asm__ __volatile__(
			"lui	$t0, 0x80ff\n"
			"move 	$sp, $t0\n"
			"ehb");

	uint32_t* ptr = (unsigned int*)FLASHADDR;
	void (*entry_point)(register_t, register_t, register_t, register_t) = (void*)FAIL;

	for(int i = 0; i < 0x200; i++) {//32MB
		if(*ptr != MAGIC){
			ptr += 0x1000; // 0xbc800000 0x1000 / sizeof(uint32_t); //+4 - 0x40000
			continue;
		}
		//found TRX - copy as-is to 0x80001000
		struct trx_header* h = (struct trx_header*)ptr;
		uint32_t* kstart = (uint32_t*)(((char*)ptr) + h->offsets[1]);
		int size = h->offsets[2] - h->offsets[1];
		uint32_t* dst = (uint32_t*)TARGETADDR;

		entry_point = (void*)TARGETADDR;
		size_t res = tinfl_decompress_mem_to_mem((uint32_t*)TARGETADDR, 0x700000, kstart, size, TINFL_FLAG_PARSE_GZIP_HEADER);

		if(res > 0x800000){
			entry_point = (void*)FAIL3;
			entry_point(res,a1,a2,a3);
		}

		entry_point(a0,a1,a2,a3);
	}
	entry_point(a0,a1,a2,a3);
}

void * memcpy(void *o_dst, const void *o_src, size_t len){
	size_t size = len;
	uint32_t* dst = o_dst;
	uint32_t* src = (uint32_t*)o_src;

	while(size >= sizeof(uint32_t)){
		*dst = *src;
		size -= sizeof(uint32_t);
		src++;
		dst++;
	}

	uint8_t* bdst = (uint8_t*)dst;
	uint8_t* bsrc = (uint8_t*)src;
	while(size > 0){
		*bdst = *bsrc;
		size -= 1;
		bsrc++;
		bdst++;
	}

	return o_dst;
}

void * memset(void *b, int c, size_t len){
	size_t size = len;
	uint32_t* dst = b;
	uint8_t short_value = (uint8_t)c;
	uint32_t value = short_value;
	uint32_t word = (value << 24) | (value << 16) | (value << 8) | value;

	while(size >= sizeof(uint32_t)){
		*dst = word;
		size -= sizeof(uint32_t);
		dst++;
	}

	uint8_t* bdst = (uint8_t*)dst;

	while(size > 0){
		*bdst = short_value;
		size -= 1;
		bdst++;
	}

	return b;
}
