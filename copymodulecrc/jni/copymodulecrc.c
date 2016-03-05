/* copymodulecrc
*  Copyright (C) 2016 aleksj
*  alexgptr@gmail.com
*
* When a module is loaded, the crc values contained in the kernel are compared with similar values in
* the module. if they are not equal, the kernel refuses to load the module. Therefore, if you want to
* use a kernel module in different Android versions the module_layout symbol crc should be equal.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>

enum open_mode {MODE_READ, MODE_WRITE};

/* checks if current offset is a signature */
bool compare(const unsigned char* data, const char* sigdata, unsigned int siglen)
{
        size_t i=0;
        /* search the whole length */
	for (i=0; i<siglen; i++)
	{
		if(data[i] != sigdata[i])
			return false; /* they are not equal, return false */
	}
	return true; /* they are equal, return true */
}

/**
  find_addr - search a region of memory for a specified signature.
  @address: specifies the base memory address, start address.
  @size: the search size range that will be analyzed.
  @sigdata: pointer to signature.
  @len: the length of signature in bytes.
  @return: the address of signature, or 0 if not found.
*/
unsigned long find_addr(unsigned long address, unsigned long size, char *sigdata, unsigned int len)
{
	size_t i=0;

	for(i=0; i<(size-len); i++)

		if( compare((unsigned char*)(address+i), sigdata, len ))

			return (unsigned long)(address+i); /* address found! return it */

	return 0;

}
/**
 * Get the size of a file.
 * @filename The name of the file to check size for.
 * @return the filesize in bytes, or -1 on error.
 */
off_t get_fsize(const char* filename)
{
	struct stat st;

	if (stat(filename, &st)==0)
		return st.st_size;
	return -1;
}

/**
 * read_module - open() and map a file into memory.
 * @filename: pointer to input file.
 * @_size: pointer to output file size.
 * @_fd: pointer to file descriptors.
 * @mode: open, mapped mode (MODE_READ - reading only/pages may be read;
 * MODE_WRITE - reading and writing/pages can be written.
 * @return: pointer to the mapped area, or NULL on error.
 */
static void* read_module(const char* filename, ssize_t* _size, int* _fd, int mode)
{
	void* buffer = NULL;
	ssize_t lsize;
	int fd;

	/* open the file in target mode */
	if(mode == MODE_WRITE)
		fd = open(filename, O_RDWR);
	else
		fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "can't open '%s', (%s)\n", filename, strerror (errno));
		return NULL;
	}

	lsize = get_fsize(filename);
	if(lsize < 0)
	{
		fprintf(stderr, "can't determine size of '%s', (%s)\n", filename, strerror(errno));
		close(fd);
		return NULL;
	}

	if(mode == MODE_WRITE)
		buffer = mmap(NULL, lsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	else
		buffer = mmap(NULL, lsize, PROT_READ, MAP_SHARED, fd, 0);
	if(buffer == MAP_FAILED)
	{
		fprintf(stderr, "can't mmap '%s', (%s)\n", filename, strerror(errno));
		close(fd);
		return NULL;
	}

	/* let the caller to know the file size and descriptor */
	*_size = lsize;
	*_fd = fd;

	return buffer;
}
int main(int argc, char* argv[])
{

	void* data = NULL;
	unsigned long crc0 = 0;
	unsigned long crc1 = 0;
	ssize_t size = 0;
	int fd;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <srcmodule> <dstmodule>\n", argv[0]);
		return -EINVAL;
	}

	data = read_module(argv[1], &size, &fd, MODE_READ);
	if (!data)
		return -1;

	/*
	__versions:000000C0 31 DC 67 42                       DCD 0x4267DC31
	__versions:000000C4 6D 6F 64 75 6C 65+aModule_layout  DCB "module_layout"
	*/
	crc0 = find_addr((ptrdiff_t)data, size, "\x6d\x6f\x64\x75\x6c\x65\x5f\x6c\x61\x79\x6f\x75\x74", 13);
	if(!crc0)
	{
		fprintf(stderr, "crc not found in source module\n");
		munmap(data, size);
		close(fd);
		return -1;
	}

	crc0 = *(unsigned long*)(crc0-4);
	printf("module1 crc: %08lx\n", crc0);

	munmap(data, size);
	close(fd);

	data = read_module(argv[2], &size, &fd, MODE_WRITE);
	if (data==NULL)
		return -1;

	crc1 = find_addr((ptrdiff_t)data, size, "\x6d\x6f\x64\x75\x6c\x65\x5f\x6c\x61\x79\x6f\x75\x74", 13);
	if(!crc1)
	{
		fprintf(stderr, "crc not found in destination module\n");
		munmap(data, size);
		return -1;
	}

	crc1-=4;
	printf("module2 crc: %08x\n", *(uint32_t*)crc1);

	memcpy((void*)crc1, (void*)&crc0, 4); /* copy the crc value from source to destination module */
	printf("module crc copied\n");

	munmap(data, size);
	close(fd);

	return 0;
}
