#ifndef VARHEXUTILS
#define VARHEXUTILS

#include "endian.h"
#include "multicodec/varint.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
HEADER TEST: int header_from_string(char *dest, size_t destsize, char *src, size_t
srcsize) { char result[destsize + 2]; bzero(result, destsize + 2); strcpy(result,
Int_To_Hex(srcsize - 1)); printf("SZ: %s\n", result); char srchex[((srcsize - 1) * 2)
+ 1]; str2hex(srchex, src); strcat(result, srchex); strcpy(dest, result); return 1;
    char A[] = "/lol/meow";
    char B[((sizeof(A) - 1) * 2) + 3];
    bzero(B, sizeof(B));
    header(B, sizeof(B), A, sizeof(A));
    printf("Result: %s\n", B);
}

*/
/*
 *  //getcontent
 *  char getc_dest[1000];
 *  bzero(getc_dest,1000);
 *  get_content(getc_dest, add_hdr_dest);
 *  printf("get_content - Result: %s\n", getc_dest);
 */
int get_content(char *dest, unsigned char *src);
/* 	//getHeader
 *  char geth_dest[1000];
 *  bzero(geth_dest,1000);
 *  get_header(geth_dest, add_hdr_dest);
 *  printf("get_header - Result: %s\n", geth_dest);
 */
int get_header(char *dest, unsigned char *src);
/*//Remove Header - Usage Example:
 *  char remove_hdr_dst[1000]; //Much bigger than needed tbh..
 *  bzero(remove_hdr_dst,1000);
 *  remove_header(remove_hdr_dst, add_hdr_dest);
 *  printf("remove_header - Result: %s\n",remove_hdr_dst);
 */
int remove_header(unsigned char *dest, unsigned char *lel);
/*
 * 	//Add Header - Usage Example:
 * 	char continut[] = "1234";
 * 	char continuthex[((sizeof(continut)-1)*2)+1];
 * 	bzero(continuthex, sizeof(continuthex));
 * 	str2hex(continuthex, continut);
 * 	char cap[] = "/lol/";
 * 	char caphex[((sizeof(cap)-1)*2)+1];
 * 	bzero(caphex,sizeof(caphex));
 * 	str2hex(caphex,cap);
 * 	//printf("caphex = %s\n", caphex);
 * 	//printf("conhex = %s\n", continuthex);
 * 	char add_hdr_dest[sizeof(continuthex)-1+sizeof(caphex)-1+3];
 * 	bzero(add_hdr_dest, sizeof(add_hdr_dest));
 * 	add_header(add_hdr_dest, caphex,sizeof(caphex), continuthex,
 * sizeof(continuthex)); printf("add_header - Result: %s\n",add_hdr_dest);
 */
int add_header(unsigned char *dest, unsigned char *header, size_t headersize,
               unsigned char *content, size_t contentsize);
/*///Header - Usage Example:
 * unsigned char str[] = "/lol/meow/";
 * unsigned char strhex[((strlen(str)-1)*2)+1];
 * str2hex(strhex,str);
 * unsigned char header_result[sizeof(strhex+2)]; // Extra 2 for size
 * bzero(header_result, sizeof(header_result));
 * header(header_result, strhex);
 * printf("header - Result: %s\n", header_result);
 * unsigned char header_path_result[strlen(header_result)-2];
 */
int header(unsigned char *dest, unsigned char *src);
/*
 * 	//Header_Path - Usage Example
 * 	header_path(header_path_result,header_result);
 * 	printf("header_path - Result: %s\n",header_path_result);
 */
int header_path(unsigned char *dest, unsigned char *src);

void hex2str(const char *h, char *s, int sizs);
void str2hex(unsigned char *hexstring, unsigned char *string);
int8_t Var_Bytes_Count(uint8_t *countbytesofthis);
uint8_t *Num_To_Varint_64(uint64_t TOV64INPUT);
uint8_t *Num_To_Varint_32(uint32_t TOV32INPUT);
uint64_t *Varint_To_Num_64(uint8_t TON64INPUT[60]);
uint32_t *Varint_To_Num_32(uint8_t TON32INPUT[60]);
char *Int_To_Hex(uint64_t int2hex);
uint64_t Hex_To_Int(char *hax);
void vthconvert(int size, char *crrz01, uint8_t *xbuf);
char *Var_To_Hex(int realsize, uint8_t *TOHEXINPUT);
uint8_t *Hex_To_Var(char *Hexstr);
void convert(char *convert_result, uint8_t *buf);
char *Num_To_HexVar_64(uint64_t TOHVINPUT);
void convert2(char *convert_result2, uint8_t *bufhx);
char *Num_To_HexVar_32(uint32_t TOHVINPUT);
uint64_t HexVar_To_Num_64(char *theHEXstring);
uint32_t HexVar_To_Num_32(char theHEXstring[]);
#endif
