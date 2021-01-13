#ifndef H_BITMAP
#define H_BITMAP
typedef struct {
	/*byte count*/
	long long int bitmap_bit_length;
	/*start address*/
	char* bits;
}bitmap;
#endif