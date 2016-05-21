/*
 * viewer.c
 *
 *  Created on: Feb 6, 2016
 *      Author: mizhka
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "extern.h"
#include <stdlib.h>


///https://wiki.openwrt.org/doc/techref/header
#define MAGIC				0x30524448 // "HDR0"
#define TRX_VERSION			1
#define TRX_HEADER_SIZE 	28
#define NUM_OFFSETS			3
#define READ_HEADER(a) 		if((size = read(fd, &(a), sizeof(a))) < 0){ \
		perror("can't read file"); \
		return -1; \
	}

#define WRITE_HEADER(a) 		if(write(fd, &(a), sizeof(a)) < 0){ \
		perror("can't write file"); \
		return -1; \
	}

struct trx_header {
	u_int32_t magic;
	u_int32_t file_length;
	u_int32_t crc32;
	u_int16_t flags;
	u_int16_t version;
	u_int32_t offsets[NUM_OFFSETS];
};

void usage();
int print_trx(const char* filename);
int create_trx_header(char** filenames, char* output);
int write_trx_header(char* output, struct trx_header* header, int is_full);
int write_zero_padding(char* output, int nzeros);
int fappend(char* src, char* dst);

int main(int argc, char** argv){
	if(argc > 1){
		if(strcmp("-v",argv[1]) == 0)
			if (argc == 3)
				return print_trx(argv[2]);
		if(strcmp("-c",argv[1]) == 0)
			if (argc == 6){
				char* filenames[3];
				filenames[0] = argv[2];
				filenames[1] = argv[3];
				filenames[2] = argv[4];
				return create_trx_header(filenames, argv[5]);
			}
	}
	usage(argv[0]);
	return 0;
}

void usage(char* progname){
	printf("usage: %s [-v] filename\n",progname);
	printf("       %s [-c] lzmaloader lzmakernel fsimage output\n",progname);
	return;
}

int print_trx(const char* filename){
	u_int8_t buf[TRX_HEADER_SIZE];
	int fd = open(filename,O_RDONLY);
	if(fd < 0){
		perror("can't open file:");
		return -1;
	}
	int size;

	struct trx_header header;

	READ_HEADER(header.magic);
	READ_HEADER(header.file_length);
	READ_HEADER(header.crc32);
	READ_HEADER(header.flags);
	READ_HEADER(header.version);
	for (int i = 0; i < NUM_OFFSETS; i++){
		READ_HEADER(header.offsets[i]);
	}

	printf("magic = 0x%04x\n",header.magic);
	printf("length = %d\n", header.file_length);
	printf("crc32 = %u\n", header.crc32);
	printf("~crc32 = %u\n", ~header.crc32);
	printf("flags = 0x%04x\n", header.flags);
	printf("version = %d\n", header.version);
	for (int i = 0; i < NUM_OFFSETS; i++){
		printf("offset[%d] = %d\n", i, header.offsets[i]);
	}
	close(fd);
	return 0;
}

int create_trx_header(char** filenames, char* output){
	int sizes[NUM_OFFSETS];
	int nzeros;
	struct stat filestat;
	struct trx_header header;
	int offset = TRX_HEADER_SIZE;
	header.magic = MAGIC;
	header.flags = 0;
	header.version = 1;
	for(int i = 0; i< NUM_OFFSETS; i++){
		if(stat(filenames[i],&filestat) != 0){
			perror(filenames[i]);
			return -1;
		}
		//align last offset to sector size (512)
		if (i == NUM_OFFSETS - 1){
			int nsectors = offset / 0x200;
			int nrest = offset % 0x200;
			if(nrest > 0){
				nrest = offset;
				offset = (nsectors + 1) * 0x200;
				nzeros = offset - nrest;
			}
		}
		header.offsets[i] = offset;
		offset += filestat.st_size;
	}
	header.file_length = offset;

	//Temporary
	if(write_trx_header(output, &header, 0) < 0){
		return -1;
	}

	char* padding = malloc(sizeof(char) * (strlen(output)+strlen(".zeros") + 1));
	strcpy(padding,output);
	strcat(padding,".zeros");

	if(write_zero_padding(padding,nzeros) < 0){
		return -1;
	}

	crc32_total = 0;

	uint32_t crc;
	off_t size;
	int fd;

	if((fd  = open(output, O_RDONLY)) < 0){
		perror("can't open temp TRX file");
		return -1;
	}
	printf("crc32 for %s\n", output);
	crc32(fd, &crc, &size);
	close(fd);

	for(int i = 0; i < NUM_OFFSETS; i++){
		if(i == NUM_OFFSETS -1){
			if((fd  = open(padding, O_RDONLY)) < 0){
				perror("can't open file");
				return -1;
			}
			crc32(fd, &crc, &size);
			close(fd);
		}

		if((fd  = open(filenames[i], O_RDONLY)) < 0){
			perror("can't open file");
			return -1;
		}

		printf("crc32 for %s\n", filenames[i]);
		crc32(fd, &crc, &size);
		close(fd);
	}

	header.crc32 = ~crc32_total;

	if(write_trx_header(output, &header, 1) < 0){
		return -1;
	}

	for(int i = 0; i < NUM_OFFSETS; i++){
		if(i == NUM_OFFSETS -1){
			if(fappend(padding, output) < 0){
				return -1;
			}
		}
		if(fappend(filenames[i], output) < 0){
			return -1;
		}
	}

	print_trx(output);
	return 0;
}

int write_zero_padding(char* output, int nzeros){
	int fd = open(output, O_WRONLY | O_CREAT);
	if(fd < 0){
		perror("can't create padding file");
		return -1;
	}

	if(ftruncate(fd,0) < 0){
		perror("can't erase padding file");
		return -1;
	}

	if(chmod(output, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0){
		perror("can't set permissions 0644 of TRX file");
	}

	uint8_t zero = 0;

	for(int i = 0; i < nzeros; i++){
		WRITE_HEADER(zero);
	}

	close(fd);
	return 0;
}

int write_trx_header(char* output, struct trx_header* header, int is_full)
{
	int fd = open(output, O_WRONLY | O_CREAT);
	if(fd < 0){
		perror("can't create file");
		return -1;
	}

	if(ftruncate(fd,0) < 0){
		perror("can't erase TRX file");
		return -1;
	}

	if(chmod(output, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0){
		perror("can't set permissions 0644 of TRX file");
	}

	if(is_full){
		WRITE_HEADER(header->magic);
		WRITE_HEADER(header->file_length);
		WRITE_HEADER(header->crc32);
	}
	WRITE_HEADER(header->flags);
	WRITE_HEADER(header->version);
	for (int i = 0; i < NUM_OFFSETS; i++){
		WRITE_HEADER(header->offsets[i]);
	}

	close(fd);
	return 0;
}

#define BUFSIZE 4096
int fappend(char* src, char* dst){
	int fs, fd;
	uint8_t buf[BUFSIZE];
	if((fs = open(src, O_RDONLY)) < 0){
		perror("can't open file");
		return -1;
	}

	if((fd = open(dst, O_WRONLY | O_APPEND)) < 0){
		perror("can't open file");
		return -1;
	}

	do{
		int n = read(fs, buf, BUFSIZE);
		uint8_t* wbuf = buf;
		if(n > 0){
			while(n > 0){
				int w;

				if((w = write(fd, wbuf, n)) < 0){
					perror("can't append to file");
					return -1;
				}
				n -= w;
				wbuf += w;
			}
		}else if(n == 0){
			break;
		}else{
			perror("can't read from file");
			return -1;
		}
	}while(1);

	close(fs);
	close(fd);
	return 0;
}

