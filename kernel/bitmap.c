#include "bitmap.h"

/**
 * check bits contains n free bits
 * @return start bit
 */
int check(bitmap bits, int n){
	int i;
	int count = 0;
	int start = 0;
	for(i = 0; i < bits.bitmap_bit_length / 8; i ++){
		char byte = *(bits.bits + i);	
		int j;
		for(j = 0; j < 8; j ++){
			if(((0x80 >> j) & byte) >> (8 - j - 1) == 0){
				count ++;
			}else{
				start ++;
			}
			if(count >= n){
				return start;
			}
		}
	}
	return -1;
}
/**
 * set n bits
 */
void set(bitmap bits,int start, int n){
	char* start_address = bits.bits + (start / 8);
	int bit_index = start % 8;
	int count = 0;
	while(count < n){
		if(bit_index >= 8){
			start_address ++;
			bit_index = 0;
		}
		*start_address = *start_address | (0x80 >> bit_index);
		count ++;
		bit_index ++;
	}
}

/**
 * @return start bit
 */
int check_and_set(bitmap bits, int n){
	int check_result = check(bits,n);
	if(check_result == -1){
		return -1;
	}else{
		set(bits,check_result,n);
		return check_result;
	}
}
