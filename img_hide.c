#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void change_lsb( uint8_t *save , uint8_t digit , uint8_t bit ){

	uint8_t tmp[8];
	uint8_t count=1 , c=0;
	uint8_t tmp_digit = digit;
	uint8_t d=0;

	while( digit!=0 ){
		for( int i=0 ; i<bit ; i++ ){

			if (digit==0){
				continue;
			}

			if( tmp_digit-bit*count<0 ){
				d = 0;
			}
			else{
				d = tmp_digit-bit*count;
			}
			tmp[c] = save[ d+i ];
			c ++;
			digit --;
			//printf("%d ",tmp[c-1]);
		}
		count ++;
	}
	for( int i=0 ; i<tmp_digit ; i++ ){

		save[i] = tmp[i];
	}

}


void change_msb( uint8_t *save , uint8_t bit ){

	uint8_t tmp[8];
	uint8_t count=0 , c=0;
	uint8_t digit = 8;
	uint8_t tmp_count[8];


	while( digit>0 ){

		if( digit%bit==0 ){
			count = bit;
		}
		else{
			count = digit%bit;
		}

		digit -= count;
		//printf("digit: %d\n",digit);
		if( digit<0 ){
			digit = 0;
		}

		for( int i=0 ; i<count ; i++ ){

			tmp[c] = save[ digit+i ];
			c ++;
		}
		
	}

	for( int i=0 ; i<8 ; i++ ){

		save[i] = tmp[i];
	}
	
}

void print_num( uint8_t pPixels ){

	uint8_t tmp =0;
	uint8_t a= 1;
	//printf("num: %d\n",pPixels);
	a = a<<7;
	for( int i=0 ; i<8 ; i++ ){

		if( a & pPixels ){
			printf("1 ");
		}
		else{
			printf("0 ");
		}
		a = a>>1;
	}
	printf("\n");

}

void print_array( uint8_t *save ){

	uint8_t tmp =0;
	
	for( int i=0 ; i<8 ; i++ ){

		printf("%d ",save[i]);
	}
	printf("\n");

}

void change_binary( uint8_t pPixels , uint8_t *take ){

	uint8_t tmp =0;
	uint8_t a= 1;
	//printf("num: %d\n",pPixels);
	a = a<<7;
	for( int i=0 ; i<8 ; i++ ){

		if( a & pPixels )
			take[i] = 1;
		else
			take[i] = 0;
		a = a>>1;
	}
}


void change( uint8_t *save ){

	uint8_t tmp[8];

	for( int i=0 ; i<8 ; i++ ){

		tmp[i] = save[7-i];
	}

	for( int i=0 ; i<8 ; i++ ){

		save[i] = tmp[i];
		
	}
/*
	printf("change: ");
	print_array( save );
	*/
}

struct _sBmpHeader
{
    char		bm[2];
    uint32_t	size;
    uint32_t	reserve;
    uint32_t	offset;
    uint32_t	header_size;
    uint32_t	width;
    uint32_t	height;
    uint16_t	planes;
    uint16_t	bpp;
    uint32_t	compression;
    uint32_t	bitmap_size;
    uint32_t	hres;
    uint32_t	vres;
    uint32_t	used;
    uint32_t	important;
}__attribute__ ((__packed__));

typedef struct _sBmpHeader sBmpHeader;


int main( int argc, char *argv[] ){

	int32_t c=0;
	int32_t option=0,oc=0;
	int32_t bit=1;
	uint32_t byte=0;

	while( ( c = getopt( argc,argv,"web" ) )!=-1 ){

		oc ++;
		switch(c){

			case 'w':
				option = 1;
				break;
			case 'e':
				option = 2;
				break;
			case 'b':
				//printf("%d\n",oc);
				sscanf(argv[oc+1],"%d",&bit);
				//printf("bit: %d\n",bit);
		
				if( bit<1 || bit>8 ){

					printf("Bit: %d\n",bit);
					printf("Bit is from 1 to 8.The bit input is wrong.\n");
					return -1;
				}
				break;
			default:
				printf("Input is wrong.\n");
				return -1;
		}
	}
 
	//bit = 8;
	FILE *pf_bmp;

	if( (pf_bmp = fopen( argv[argc-2],"rb+" ))==NULL ){
		
		printf( "File could not be opened!\n" );
	    perror( "Error" );
	    return -1;
	}

	sBmpHeader header;

	fread( &header,54,1,pf_bmp );


// 資料是否太大 
	if( option==1 ){

		FILE *pf_secret;

		if( (pf_secret = fopen( argv[argc-1],"rb+" ))==NULL ){
			
			printf( "File could not be opened!\n" );
		    perror( "Error" );
		    return -1;
		}



		uint64_t sec_tol_byte = 0;
		uint64_t bmp_tol_bit = 0;

		fseek( pf_secret,0,SEEK_END );
		sec_tol_byte = ftell(pf_secret);
		bmp_tol_bit = bit*header.width*header.height;

		//change_binary_32( sec_tol_byte,byte_array );

		//printf("%ld \n",sec_tol_byte);
		printf("Done !\n");
		//printf("%ld %ld\n",sec_tol_bit,bmp_tol_bit);

		if( (sec_tol_byte*8)>bmp_tol_bit ){
			printf("The secret data cannot fit in the bmp file.\n");
			return -1;
		}

		uint8_t *pPixels = NULL;
		pPixels = malloc( header.bitmap_size );
		fread( pPixels, 1, header.bitmap_size, pf_bmp );

	 	int32_t size=ftell(pf_secret);

	 	rewind( pf_bmp );
	 	header.reserve = sec_tol_byte;
	 	//printf("head %u\n",header.reserve);
	 	
	 	fwrite( &header,54,1,pf_bmp );

	 	rewind( pf_secret );
	 	uint32_t b = 0 , use=0;

//回圈
	 	for( int i=0 ; i<size ; i++ ){

	 		uint8_t a;
	 		uint8_t save[8];
	 		uint8_t count=0;

	// 取b.txt 
	 		fread( &a,1,1,pf_secret );
	 		 		
	 		//printf("\n%d %c\n",i,a);

	 		for( int j=0 ; j<8 ; j++ ){
	 			save[j] = a%2;
	 			//digit ++;
	 			a /= 2;
	 		}

		 		change_lsb( save,8,bit );
			
				uint8_t tmp;
				
				if( use==0 && count==0 ){
					
					pPixels[use] = (pPixels[use]>>bit);
				}
				
			while( count<8 ){
					
				
				if( b==bit ){

					use ++;
					b = 0;
						
					if( i==(size-1) ){
   
						if( 8-count<bit ){
							pPixels[use] = (pPixels[use]>>(8-count));
						}
						else{
							pPixels[use] = (pPixels[use]>>bit);
						}

					}
					else{
						pPixels[use] = (pPixels[use]>>bit);

					}
						
				}
					
				if( bit==8 && use!=0 ){
					pPixels[use] += save[count];
					pPixels[use] = pPixels[use]<<1;
				}
				else{
					pPixels[use] = pPixels[use]<<1;
					pPixels[use] += save[count];
				}
					
					count ++;
					b ++;
				if( b==bit && count==bit && bit==8 && use==0){
					count --;
				}
			}
			
	 	}
	 	//printf("new: \n");
		//print_num( pPixels[use] );

	 	fwrite( pPixels,1 , header.bitmap_size, pf_bmp);
	 	fclose( pf_secret );
	 	fclose( pf_bmp );
	 }
	 else{

	 	FILE *pf_secret;

		if( (pf_secret = fopen( argv[argc-1],"wb+" ))==NULL ){
			
			printf( "File could not be opened!\n" );
		    perror( "Error" );
		    return -1;
		}

	 	uint8_t *pPixels = NULL;
		pPixels = malloc( header.bitmap_size );
		fread( pPixels, 1, header.bitmap_size, pf_bmp );
		uint8_t save[8];
		printf("Done !\n");

		//printf("head %u\n",header.reserve);
		uint8_t ans[32];
		uint8_t ans_num=0;
		byte = header.reserve;
		//printf("head %u\n",header.reserve);

//
//rgb
 		uint8_t tmp =0;
 		uint8_t b=0; 
 		uint64_t use = 0 ;
 		uint8_t take[8] , save_count=0 ,save_num=0;
 		uint32_t digit_bit=0;
 		//uint8_t d = 0;

 		uint8_t tmp_array[8];
 		uint32_t start=0;

 		while( byte!=0 ){

 		//pPixel 轉二進位儲存
 			//printf("up b: %d %d\n",b,bit); 
 			if( b==bit|| use==0  ){
 				//take
 				
 				change_binary( pPixels[use] , take );

 				if( byte==1 ){
 					//printf("bb:%d",b);
 					if( (8-save_count)>=bit )
 						start = 8-bit;
 					else
 						start = 8-b;
 				}
 				else{
 					start = 8-bit;
 				}

	 			b=0;
 				use ++;

 			}
// 取pPixel bit 使用 
 			//printf("start: %d\n",start);
 			for( int j=start ; j<=7 ; j++ ){

 			//save 有沒有 8
 				if( save_count<8 ){
	 				save[save_count] = take[j];

	 				//b是用了 take的幾 bit 
	 				b ++;
		 			save_count ++;
		 			//printf("b: %d , j %d\n",b,j);
		 			
	 				
		 		}
		 		
		 		if( save_count==8 ){
		 			

					//頭尾交換
					change_msb( save,bit );
					 
					//反過來讀 
					change(save);
				
		 			for( int k=0 ; k<8 ; k++ ){

		 				save_num += save[k];
			 			if( k<7 ){
			 				save_num = save_num<<1;
						}

		 			}
		 			//printf("save_num: %d\n",save_num);
		 			fwrite( &save_num,1,1,pf_secret );
		 			save_num = 0;
		 			save_count = 0;
		 			
		 			byte --;
		 		}
		 	
 			}
 		
 		}
 		fclose( pf_secret );
	 	fclose( pf_bmp );

	 }
	 //printf("Done!\n");


	return 0;
}

