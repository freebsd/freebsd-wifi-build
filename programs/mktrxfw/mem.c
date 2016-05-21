/*
 * mem.c
 *
 *  Created on: Mar 27, 2016
 *      Author: mizhka
 */

#include "trxloader.h"

uint8_t d_stop = 0;

void * tinfl_memcpy(void *o_dst, const void *o_src, size_t len){
	size_t size = len;
	uint32_t* dst = o_dst;
	uint32_t* src = (uint32_t*)o_src;

	int i = 0;

	if(((uint32_t)dst % 4) == 0 && ((uint32_t)src % 4) == 0)
		while(size >= 4){
			*dst = *src;
			size -= sizeof(uint32_t);
			src++;
			dst++;
			i++;
		}

	if(size > 0){
		uint8_t* bdst = (uint8_t*)dst;
		uint8_t* bsrc = (uint8_t*)src;
		while(size > 0){
			*bdst = *bsrc;
			size -= 1;
			bsrc++;
			bdst++;
		}
	}

	return o_dst;
}

void * tinfl_memset(void *b, int c, size_t len){
	size_t size = len;
	uint32_t* dst = b;
	uint8_t short_value = (uint8_t)c;
	uint32_t value = short_value;
	uint32_t word = (value << 24) | (value << 16) | (value << 8) | value;

	if(((uint32_t)b % 4) == 0)
		while(size >= sizeof(uint32_t)){
			*dst = word;
			size -= sizeof(uint32_t);
			dst++;
		}

	if(size > 0){
		uint8_t* bdst = (uint8_t*)dst;

		while(size > 0){
			*bdst = short_value;
			size -= 1;
			bdst++;
		}
	}
	return b;
}
