#ifndef HUFF_H
#define HUFF_H

const int ERROR_EXIT = 255;
const int SUCCESS_EXIT = 0;

const char HUFF_MAGIC[] = "HUFF";
const char HUFF_EXT[] = ".huff";

const int HUFF_MAGICLEN = sizeof(HUFF_MAGIC) - 1;
const int HUFF_EXTLEN = sizeof(HUFF_EXT) - 1;

const int EFILETOOLONG = -1;

typedef unsigned long long uint64_t;
typedef unsigned char uint8_t;

/* A Huffman table specifies an encoding for all 256 bytes as a null terminated
   string: char * HuffmanTable[256] */

/* Compression
----------------------------------------------------------------------------- */

/* Returns a new translation table generated from an array of 256
   unsigned long longs representing the number of times the byte index has
   appeared in a file. */
int huff_tablefromfreq(uint64_t freq[256], char *out_table[256]);

/* Decompression
----------------------------------------------------------------------------- */

struct DecompressionContext;

/* Frees a DecompressionContext struct. Returns 1 on success, 0 otherwise. */
int huff_freecontext(struct DecompressionContext *context);

/* Returns a pointer to a new DecompressionContext struct generated from a
   translation table, which is an array of 256 char *s which are the ASCII
   representations of that byte index's translation. */
struct DecompressionContext *huff_makecontext(char *table[]);

/* Takes the next bit to decode. Returns an unsigned char converted to an int if
   a character is decoded, returns -1 otherwise. If an error occurs, -2 or lower
   is returned */
int huff_decode(int bit, struct DecompressionContext *context);

#endif
