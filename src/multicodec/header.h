#ifndef MULTICODEC_HEADER
#define MULTICODEC_HEADER
#include "vhutils.h"
int header_from_string(char * dest,size_t destsize, char * src,size_t srcsize)
{
	char result[destsize+2];
	bzero(result,destsize+2);
	strcpy(result, Int_To_Hex(srcsize-1));
	printf("SZ: %s\n",result);
	char srchex[((srcsize-1)*2)+1];
	str2hex(srchex,src);
	strcat(result,srchex);
	strcpy(dest,result);
	return 1;
		char A[] = "/lol/meow";
	char B[((sizeof(A)-1)*2)+3];
	bzero(B,sizeof(B));
	header(B,sizeof(B),A,sizeof(A));
	printf("Result: %s\n", B);
}
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
int header(unsigned char * dest, unsigned char * src)
{
	if(!src)
	{
		return 0;
	}
	char size[3];
	bzero(size,3);
	strcpy(size, Int_To_Hex((strlen(src)/2)-1));
	strcat(dest, size);
	strcat(dest, src);
	return 1;
}
/*
 * 	//Header_Path - Usage Example
 * 	header_path(header_path_result,header_result);
 * 	printf("header_path - Result: %s\n",header_path_result);
 */
int header_path(unsigned char * dest, unsigned char * src)
{
	if(!src)
	{
		return 0;
	}
	sprintf(dest,"%s",src+2);
	return 1;
}
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
 * 	add_header(add_hdr_dest, caphex,sizeof(caphex), continuthex, sizeof(continuthex));
 * 	printf("add_header - Result: %s\n",add_hdr_dest);
 */
int add_header(unsigned char * dest,unsigned char * header,size_t headersize, unsigned char * content, size_t contentsize)
{
	if(!header||!content)
	{
		return 0;
	}
	int realsize = (headersize-1+contentsize-1)/2;
	char szhex[3];
	bzero(szhex,3);
	strcat(szhex, Int_To_Hex(realsize));
	strcat(dest, szhex);
	strcat(dest, header);
	strcat(dest, content);
	return 1;
}
/*//Remove Header - Usage Example:
 *  char remove_hdr_dst[1000]; //Much bigger than needed tbh..
 *  bzero(remove_hdr_dst,1000);
 *  remove_header(remove_hdr_dst, add_hdr_dest);
 *  printf("remove_header - Result: %s\n",remove_hdr_dst);
 */
int remove_header(unsigned char * dest, unsigned char * lel)
{
	if(!lel)
	{
		return 0;
	}
	int init = 0;
	char s[(strlen(lel)/2)+1];
	int sizs = strlen(lel)/2+1;
	hex2str(lel, s, sizs);
	sprintf(s,"%s", s+1);
	char * pch;
	char * end;
	pch = strtok_r(s,"/",&end);
	int pos = 0;
	while(pch)
	{
		//printf("PCH: %s\n", pch);
		if(pos==1)
		{
			str2hex(dest, pch);
		}
		//sprintf(dest,"%s",pch);
		pch = strtok_r(NULL,"/",&end);
		pos=1;
	}
	return 1;
}
/* 	//getHeader
 *  char geth_dest[1000];
 *  bzero(geth_dest,1000);
 *  get_header(geth_dest, add_hdr_dest);
 *  printf("get_header - Result: %s\n", geth_dest);
 */
int get_header(char * dest, unsigned char * src)
{
	if(!src)
	{
		return 0;
	}
	char ascii[strlen(src)/2];
	hex2str(src, ascii,strlen(src)/2);
	sprintf(ascii, "%s",ascii+1);
	char * pch;
	char * end;
	pch = strtok_r(ascii,"/",&end);
	while(pch)
	{
		strcat(dest,pch);
		break;
	}
	return 1;
}
/*
 *  //getcontent
 *  char getc_dest[1000];
 *  bzero(getc_dest,1000);
 *  get_content(getc_dest, add_hdr_dest);
 *  printf("get_content - Result: %s\n", getc_dest);
 */
int get_content(char * dest, unsigned char * src)
{
	if(!src)
	{
		return 0;
	}
	char ascii[strlen(src)/2];
	hex2str(src, ascii,strlen(src)/2);
	sprintf(ascii, "%s",ascii+1);
	char * pch;
	char * end;
	int ax = 0;
	pch = strtok_r(ascii,"/",&end);
	while(pch)
	{
		//printf("PCH: %s", pch);
		if(ax == 1)
		{
			strcat(dest,pch);
			break;
		}
		ax=1;
		pch = strtok_r(NULL,"/",&end);
	}
	return 1;
}
#endif
