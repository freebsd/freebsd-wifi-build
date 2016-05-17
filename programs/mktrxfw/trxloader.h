/*
 * trxloader.h
 *
 *  Created on: Mar 27, 2016
 *      Author: mizhka
 */

#ifndef TRXLOADER_H_
#define TRXLOADER_H_

#include <sys/types.h>

#define FLASHADDR 			0xbc000000
#define TARGETADDR			0x80800000 // Trampoline
#define FAIL				0x00000004 // CFE exception
#define FAIL2				0x00000008 // CFE exception
#define FAIL3				0x00000010 // CFE exception
#define MAGIC				0x30524448 // "HDR0"
#define NUM_OFFSETS			3

struct trx_header {
	u_int32_t magic;
	u_int32_t file_length;
	u_int32_t crc32;
	u_int16_t flags;
	u_int16_t version;
	u_int32_t offsets[NUM_OFFSETS];
};

void * memcpy(void *o_dst, const void *o_src, size_t len);
void * memset(void *b, int c, size_t len);

void * tinfl_memcpy(void *o_dst, const void *o_src, size_t len);
void * tinfl_memset(void *b, int c, size_t len);

extern uint8_t d_stop;


// Decompression flags used by tinfl_decompress().
// TINFL_FLAG_PARSE_ZLIB_HEADER: If set, the input has a valid zlib header and ends with an adler32 checksum (it's a valid zlib stream). Otherwise, the input is a raw deflate stream.
// TINFL_FLAG_HAS_MORE_INPUT: If set, there are more input bytes available beyond the end of the supplied input buffer. If clear, the input buffer contains all remaining input.
// TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF: If set, the output buffer is large enough to hold the entire decompressed stream. If clear, the output buffer is at least the size of the dictionary (typically 32KB).
// TINFL_FLAG_COMPUTE_ADLER32: Force adler-32 checksum computation of the decompressed bytes.
enum
{
  TINFL_FLAG_PARSE_ZLIB_HEADER = 1,
  TINFL_FLAG_HAS_MORE_INPUT = 2,
  TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4,
  TINFL_FLAG_COMPUTE_ADLER32 = 8,
  TINFL_FLAG_PARSE_GZIP_HEADER = 16,
};

// tinfl_decompress_mem_to_mem() decompresses a block in memory to another block in memory.
// Returns TINFL_DECOMPRESS_MEM_TO_MEM_FAILED on failure, or the number of bytes written on success.
#define TINFL_DECOMPRESS_MEM_TO_MEM_FAILED ((size_t)(-1))
size_t tinfl_decompress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pSrc_buf, size_t src_buf_len, int flags);


#endif /* TRXLOADER_H_ */
