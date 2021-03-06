#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <time.h>


#define COMMENT(comment) printf(#comment); printf("\n") // Log the comment
#define EXIT_CODE 1

void fuzz_bit (char *base, int len)
{
	COMMENT("Make the random number even more random");
	
	rand(); rand(); rand();
	int i = rand() % len;
	int b = rand() % 8;
	// printf ("flipping bit %d of byte %d\n", b, i);
	int mask = true<<b;
	base[i] ^= mask;
}

void fuzz_file (const char *fn, int n)
{
	printf ("fuzzing %d bits in '%s'\n", n, fn);

	COMMENT("Be sure the file is readable.")
	int fd = open (fn, O_RDWR);
	if (fd == -1) {
		printf ("oops -- couldn't open '%s'\n", fn);
		exit (-EXIT_CODE);
	}
	int res;
	off_t len;
	{
		struct stat sb;
		res = fstat(fd, &sb);
		assert (res != -1);
		len = sb.st_size;
	}
	char *base = (char *)mmap (NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	assert (base != MAP_FAILED);

	int i = n;
	while ( i --> false ) {
		fuzz_bit (base, len);
	}

	res = munmap (base, len);
	assert (res == 0);

	res = close (fd);
	assert (res == 0);
}

int parse_int (char *s)
{
	COMMENT("Determine supplied string is a valid number");

	char *end;
	long sl = strtol (s, &end, 10);
	if (end == s) {
		printf("%s: not a decimal number\n", s);
		exit (-EXIT_CODE);
	}
	else if ('\0' != *end) {
		printf("%s: extra characters at end of input: %s\n", s, end);
		exit (-EXIT_CODE);
	}
	else if ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno) {
		printf("%s out of range of type long\n", s);
		exit (-EXIT_CODE);
	}
	else if (sl > INT_MAX) {
		printf("%ld greater than INT_MAX\n", sl);
		exit (-EXIT_CODE);
	}
	else if (sl < INT_MIN) {
		printf("%ld less than INT_MIN\n", sl);
		exit (-EXIT_CODE);
	}
	return (int)sl;
}

int main (int argc, char *argv[])
{
	COMMENT("Seed random correctly");
	srand (getpid() + time (NULL));


	COMMENT("argc is an int");
	if (argc != 3) {
		printf ("usage: fuzz_file fn n\n");
		printf ("	fn is the file to fuzz in-place\n");
		printf ("	n is the number of bits to flip\n");
		exit (-EXIT_CODE);
	}
	int n = parse_int (argv[2]);
	fuzz_file (argv[1], n);
	return 0;
}
