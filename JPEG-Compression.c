#include<stdio.h>
#include<math.h>
#include<stdlib.h>

#define pi 3.14159265
#define PI 3.14159265

int i=0,j=0,k=0,l=0,m=0;
int u=0,v=0;
int r=0,c=0,s;

float sum_Y,sum_Cb,sum_Cr;

//亮度量化表
int Q_Y[64]={
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68,109,103, 77,
    24, 35, 55, 64, 81,104,113, 92,
    49, 64, 78, 87,103,121,120,101,
    72, 92, 95, 98,112,100,103, 99 };

//色度量化表
int Q_CbCr[64]={
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99 };

int z[64]={
     0,  1,  5,  6, 14, 15, 27, 28,
     2,  4,  7, 13, 16, 26, 29, 42,
     3,  8, 12, 17, 25, 30, 41, 43,
     9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63 };


/*construct a structure of BMP header*/
typedef struct Bmpheader {
	//Bitmap File Header
	char identifier[2]; //'BM'
	unsigned int filesize; //點陣圖檔大小(byte)
	unsigned short reserved; //保留欄位
	unsigned short reserved2;
	unsigned int bitmap_dataoffset; //資料開始前的偏移量
	//Bitmap Info Header
	unsigned int bitmap_headersize; //Bitmap Info Header的長度
	unsigned int width; //寬度(pixel)
	unsigned int height; //高度(pixel)
	unsigned short planes; //位元圖層數
	unsigned short bits_perpixel; //
	unsigned int compression; //壓縮方式
	unsigned int bitmap_datasize; //點陣圖資料的大小(byte)
	unsigned int hresolution; //水平解析度
	unsigned int vresolution; //垂直解析度
	unsigned int usedcolors; //使用的調色盤顏色數
	unsigned int importantcolors; //重要的顏色數
	//Pallette
	unsigned int palette; //調色盤資料
}Bitmap;

/*construct a structure of RGB*/
typedef struct RGB{
	int R;
	int G;
	int B;
}ImgRGB;

/*construct a structure of YCbCr*/
typedef struct YCbCr{
	float Y[8][8];
	float Cb[8][8];
	float Cr[8][8];
}ImgYCbCr;

/*construct a structure of DCT*/
typedef struct DCT{
    float Y[8][8];
    float Cb[8][8];
    float Cr[8][8];
}ImgDCT;

/*construct a structure of QUAN*/
typedef struct QUAN{
    int Y[8][8];
    int Cb[8][8];
    int Cr[8][8];
}ImgQUAN;

/*construct a structure of ZigZag*/
typedef struct ZigZag{
    int Y[64];
    int Cb[64];
    int Cr[64];
}ImgZigZag;

/*construct a structure of RLE*/
typedef struct RLE{
    int Y_value[64];
    int Cb_value[64];
    int Cr_value[64];
    int Y_zero_num[64];
    int Cb_zero_num[64];
    int Cr_zero_num[64];
}ImgRLE;

typedef struct HUFF{
    char code_length;
    short code_word;//huffman code(2 byte)
    char code_word_string[17];//16 + \0
}ImgHUFF;


/*ImgRGB 的 2*2 array 的 malloc*/
ImgRGB** malloc_2D_ImgRGB(int x, int y){
    ImgRGB **array, *data;
    array = (ImgRGB**)malloc(x*sizeof(ImgRGB *));
    data = (ImgRGB*)malloc(x*y*sizeof(ImgRGB));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgYCbCr 的 2*2 array 的 malloc*/
ImgYCbCr** malloc_2D_ImgYCbCr(int x, int y){
    ImgYCbCr **array, *data;
    array = (ImgYCbCr**)malloc(x*sizeof(ImgYCbCr *));
    data = (ImgYCbCr*)malloc(x*y*sizeof(ImgYCbCr));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgDCT 的 2*2 array 的 malloc*/
ImgDCT** malloc_2D_ImgDCT(int x, int y){
    ImgDCT **array, *data;
    array = (ImgDCT**)malloc(x*sizeof(ImgDCT *));
    data = (ImgDCT*)malloc(x*y*sizeof(ImgDCT));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgQUAN 的 2*2 array 的 malloc*/
ImgQUAN** malloc_2D_ImgQUAN(int x, int y){
    ImgQUAN **array, *data;
    array = (ImgQUAN**)malloc(x*sizeof(ImgQUAN *));
    data = (ImgQUAN*)malloc(x*y*sizeof(ImgQUAN));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgZigZag 的 2*2 array 的 malloc*/
ImgZigZag** malloc_2D_ImgZigZag(int x, int y){
    ImgZigZag **array, *data;
    array = (ImgZigZag**)malloc(x*sizeof(ImgZigZag *));
    data = (ImgQUAN*)malloc(x*y*sizeof(ImgZigZag));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgRLE 的 2*2 array 的 malloc*/
ImgRLE** malloc_2D_ImgRLE(int x, int y){
    ImgRLE **array, *data;
    array = (ImgRLE**)malloc(x*sizeof(ImgRLE *));
    data = (ImgRLE*)malloc(x*y*sizeof(ImgRLE));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}




/*write header*/
void write_header(FILE *outfile, Bitmap *bmpheader){
	fwrite(&bmpheader->identifier, sizeof(short), 1, outfile);
	fwrite(&bmpheader->filesize, sizeof(int), 1, outfile);
	fwrite(&bmpheader->reserved, sizeof(short), 1, outfile);
	fwrite(&bmpheader->reserved2, sizeof(short), 1, outfile);
	fwrite(&bmpheader->bitmap_dataoffset, sizeof(int), 1, outfile);
	fwrite(&bmpheader->bitmap_headersize, sizeof(int), 1, outfile);
	fwrite(&bmpheader->width, sizeof(int), 1, outfile);
	fwrite(&bmpheader->height, sizeof(int), 1, outfile);
	fwrite(&bmpheader->planes, sizeof(short), 1, outfile);
	fwrite(&bmpheader->bits_perpixel, sizeof(short), 1, outfile);
	fwrite(&bmpheader->compression, sizeof(int), 1, outfile);
	fwrite(&bmpheader->bitmap_datasize, sizeof(int), 1, outfile);
	fwrite(&bmpheader->hresolution, sizeof(int), 1, outfile);
	fwrite(&bmpheader->vresolution, sizeof(int), 1, outfile);
	fwrite(&bmpheader->usedcolors, sizeof(int), 1, outfile);
	fwrite(&bmpheader->importantcolors, sizeof(int), 1, outfile);
}

/*read data*/
//寫進 RGB structure
void read_data(FILE *fp, ImgRGB **array, int H, int W, int skip){
    printf("\nread data...\n");
	int temp;
	char skip_buf[3];
	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			temp = fgetc(fp);
			array[i][j].B = temp;
			temp = fgetc(fp);
			array[i][j].G = temp;
			temp = fgetc(fp);
			array[i][j].R = temp;
		}
		if (skip != 0) fread(skip_buf, skip, 1, fp);
	}
	printf("finish\n");
}

/*write data*/
//寫出RGB的值
void write_data(FILE *outfile, ImgRGB **RGB, int H, int W, int skip){
    printf("\nwrite data...\n");
    char skip_buf[3] = { 0, 0, 0 };
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
        	fwrite(&RGB[i][j].B, sizeof(char),1,outfile);
        	fwrite(&RGB[i][j].G, sizeof(char),1,outfile);
        	fwrite(&RGB[i][j].R, sizeof(char),1,outfile);
        }
        if (skip != 0) { fwrite(skip_buf, skip, 1, outfile); }
    }
    printf("finish");
}

/*色度轉換YCbCr*/
//Y是亮度 Cb,Cr是色度
void YCbCr(ImgRGB **data_RGB, ImgYCbCr **data_YCbCr, int H, int W){
    printf("\nYCbCr...\n");
	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			for(k=0;k<8;k++){
				for(l=0;l<8;l++){
					data_YCbCr[i][j].Y[k][l]=0.299*data_RGB[i*8+k][j*8+l].R + 0.578*data_RGB[i*8+k][j*8+l].G + 0.114*data_RGB[i*8+k][j*8+l].B;
        			data_YCbCr[i][j].Cb[k][l]=0.564*(data_RGB[i*8+k][j*8+l].B - data_YCbCr[i][j].Y[k][l]);
        			data_YCbCr[i][j].Cr[k][l]=0.713*(data_RGB[i*8+k][j*8+l].R - data_YCbCr[i][j].Y[k][l]);
 				}
			}
		}
	}
	printf("finish\n");

	printf("Y:\n");
	for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3.3f ",data_YCbCr[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*反色度轉換YCbCr*/
void IYCbCr(ImgRGB **data_RGB, ImgYCbCr **data_YCbCr, int H, int W){
    printf("\nIYCbCr...\n");
	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			for(k=0;k<8;k++){
				for(l=0;l<8;l++){
					data_RGB[i*8+k][j*8+l].R=round(data_YCbCr[i][j].Y[k][l] + 1.402*data_YCbCr[i][j].Cr[k][l]);
        			data_RGB[i*8+k][j*8+l].G=round(data_YCbCr[i][j].Y[k][l] - 0.344*data_YCbCr[i][j].Cb[k][l] - 0.714*data_YCbCr[i][j].Cr[k][l]);
        			data_RGB[i*8+k][j*8+l].B=round(data_YCbCr[i][j].Y[k][l] + 1.772*data_YCbCr[i][j].Cb[k][l]);
        			//限制RGB的值在0~255
        			if(data_RGB[8*i+k][8*j+l].R > 255) data_RGB[8*i+k][8*j+l].R = 255;
        			if(data_RGB[8*i+k][8*j+l].G > 255) data_RGB[8*i+k][8*j+l].G = 255;
        			if(data_RGB[8*i+k][8*j+l].B > 255) data_RGB[8*i+k][8*j+l].B = 255;
        			if(data_RGB[8*i+k][8*j+l].R < 0) data_RGB[8*i+k][8*j+l].R = 0;
        			if(data_RGB[8*i+k][8*j+l].G < 0) data_RGB[8*i+k][8*j+l].G = 0;
        			if(data_RGB[8*i+k][8*j+l].B < 0) data_RGB[8*i+k][8*j+l].B = 0;
 				}
			}
		}
	}
	printf("finish\n");
}

/*DCT*/
//值的範圍要在-128~127,所以要先-128
void DCT(ImgYCbCr **data_YCbCr, ImgDCT **data_DCT, int H, int W){
    printf("\nDCT...\n");
    float cos1=0, cos2=0;
    float cosA[8][8];
    float cosB[8][8];

    for(r=0;r<8;r++){
        for(u=0;u<8;u++){
            cosA[r][u]=cos((2*r+1)*u*PI/16);
        }
    }

    for(c=0;c<8;c++){
        for(v=0;v<8;v++){
            cosB[c][v]=cos((2*c+1)*v*PI/16);
        }
    }

 	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			for(u=0;u<8;u++){
				for(v=0;v<8;v++){
                    sum_Y = 0;
                    sum_Cb = 0;
                    sum_Cr = 0;
                    for(r=0;r<8;r++){
                        for(c=0;c<8;c++){
                            if(u==0 && v==0){
                                sum_Y += (data_YCbCr[i][j].Y[r][c]-128)*cosA[r][u]*cosB[c][v]/2;
                                sum_Cb += (data_YCbCr[i][j].Cb[r][c]-128)*cosA[r][u]*cosB[c][v]/2;
                                sum_Cr += (data_YCbCr[i][j].Cr[r][c]-128)*cosA[r][u]*cosB[c][v]/2;
                            }

                            else if(u==0 || v==0){
                                sum_Y += (data_YCbCr[i][j].Y[r][c]-128)*cosA[r][u]*cosB[c][v]/sqrt(2);
                                sum_Cb += (data_YCbCr[i][j].Cb[r][c]-128)*cosA[r][u]*cosB[c][v]/sqrt(2);
                                sum_Cr += (data_YCbCr[i][j].Cr[r][c]-128)*cosA[r][u]*cosB[c][v]/sqrt(2);
                            }

                            else{
                                sum_Y += (data_YCbCr[i][j].Y[r][c]-128)*cosA[r][u]*cosB[c][v];
                                sum_Cb += (data_YCbCr[i][j].Cb[r][c]-128)*cosA[r][u]*cosB[c][v];
                                sum_Cr += (data_YCbCr[i][j].Cr[r][c]-128)*cosA[r][u]*cosB[c][v];
                            }
                        }
                    }
                    data_DCT[i][j].Y[u][v]=sum_Y/4;
                    data_DCT[i][j].Cb[u][v]=sum_Cb/4;
                    data_DCT[i][j].Cr[u][v]=sum_Cr/4;
 				}
			}
		}
	}
	printf("finish\n");

	printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3.3f ",data_DCT[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*IDCT*/
void IDCT(ImgYCbCr **data_YCbCr, ImgDCT **data_DCT, int H, int W){
    printf("\nIDCT...\n");
    float cos1=0, cos2=0;
    float cosA[8][8];
    float cosB[8][8];

    for(r=0;r<8;r++){
        for(u=0;u<8;u++){
            cosA[r][u]=cos((2*r+1)*u*PI/16);
        }
    }

    for(c=0;c<8;c++){
        for(v=0;v<8;v++){
            cosB[c][v]=cos((2*c+1)*v*PI/16);
        }
    }

 	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			for(r=0;r<8;r++){
				for(c=0;c<8;c++){
                    sum_Y = 0;
                    sum_Cb = 0;
                    sum_Cr = 0;
                    for(u=0;u<8;u++){
                        for(v=0;v<8;v++){
                            if(u==0 && v==0){
                                sum_Y += (data_DCT[i][j].Y[u][v])*cosA[r][u]*cosB[c][v]/2;
                                sum_Cb += (data_DCT[i][j].Cb[u][v])*cosA[r][u]*cosB[c][v]/2;
                                sum_Cr += (data_DCT[i][j].Cr[u][v])*cosA[r][u]*cosB[c][v]/2;
                            }

                            else if(u==0 || v==0){
                                sum_Y += (data_DCT[i][j].Y[u][v])*cosA[r][u]*cosB[c][v]/sqrt(2);
                                sum_Cb += (data_DCT[i][j].Cb[u][v])*cosA[r][u]*cosB[c][v]/sqrt(2);
                                sum_Cr += (data_DCT[i][j].Cr[u][v])*cosA[r][u]*cosB[c][v]/sqrt(2);
                            }

                            else{
                                sum_Y += (data_DCT[i][j].Y[u][v])*cosA[r][u]*cosB[c][v];
                                sum_Cb += (data_DCT[i][j].Cb[u][v])*cosA[r][u]*cosB[c][v];
                                sum_Cr += (data_DCT[i][j].Cr[u][v])*cosA[r][u]*cosB[c][v];
                            }
                        }
                    }
                    //要把128加回來
                    data_YCbCr[i][j].Y[r][c]=sum_Y/4+128;
                    data_YCbCr[i][j].Cb[r][c]=sum_Cb/4+128;
                    data_YCbCr[i][j].Cr[r][c]=sum_Cr/4+128;
 				}
			}
		}
	}
	printf("finish\n");

	printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3.3f ",data_YCbCr[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*量化*/
//DCT的值除以量化表
void QUAN(ImgQUAN **data_QUAN, ImgDCT ** data_DCT, int H, int W){
    printf("\nQUAN...\n");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=0;k<8;k++){
                for(l=0;l<8;l++){
                    data_QUAN[i][j].Y[k][l]=round(data_DCT[i][j].Y[k][l]/Q_Y[k*8+l]);
                    data_QUAN[i][j].Cb[k][l]=round(data_DCT[i][j].Cb[k][l]/Q_CbCr[k*8+l]);
                    data_QUAN[i][j].Cr[k][l]=round(data_DCT[i][j].Cr[k][l]/Q_CbCr[k*8+l]);
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3d ",data_QUAN[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*反量化*/
void IQUAN(ImgQUAN **data_QUAN, ImgDCT ** data_DCT, int H, int W){
    printf("\nIQUAN...\n");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=0;k<8;k++){
                for(l=0;l<8;l++){
                    data_DCT[i][j].Y[k][l]=data_QUAN[i][j].Y[k][l]*Q_Y[k*8+l];
                    data_DCT[i][j].Cb[k][l]=data_QUAN[i][j].Cb[k][l]*Q_CbCr[k*8+l];
                    data_DCT[i][j].Cr[k][l]=data_QUAN[i][j].Cr[k][l]*Q_CbCr[k*8+l];
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3.3f ",data_DCT[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*ZigZag*/
void ZigZag(ImgZigZag **data_ZigZag, ImgQUAN ** data_QUAN, int H, int W){
    printf("\nZigZag...\n");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=0;k<8;k++){
                for(l=0;l<8;l++){
                    data_ZigZag[i][j].Y[z[k*8+l]]=data_QUAN[i][j].Y[k][l];
                    data_ZigZag[i][j].Cb[z[k*8+l]]=data_QUAN[i][j].Cb[k][l];
                    data_ZigZag[i][j].Cr[z[k*8+l]]=data_QUAN[i][j].Cr[k][l];
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3d ",data_ZigZag[0][0].Y[k*8+l]);
 		}
 		printf("\n");
    }
}

/*IZigZag*/
void IZigZag(ImgZigZag **data_ZigZag, ImgQUAN ** data_QUAN, int H, int W){
    printf("\nIZigZag...\n");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=0;k<8;k++){
                for(l=0;l<8;l++){
                    data_QUAN[i][j].Y[k][l]=data_ZigZag[i][j].Y[z[k*8+l]];
                    data_QUAN[i][j].Cb[k][l]=data_ZigZag[i][j].Cb[z[k*8+l]];
                    data_QUAN[i][j].Cr[k][l]=data_ZigZag[i][j].Cr[z[k*8+l]];
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3d ",data_QUAN[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*DPCM*/
void DPCM(ImgQUAN **data_QUAN, int H, int W){
    printf("\nDPCM...\n");

    printf("Y:(original)\n");
    for(k=0;k<1;k++){
        for(l=0;l<64;l++){
            printf("%3d ",data_QUAN[k][l].Y[0][0]);
            if(l%8==7) printf("\n");
        }
        printf("\n");
    }

    //先做第一行以外的
    for(i=0;i<H;i++){
        for(j=W;j>0;j--){
            data_QUAN[i][j].Y[0][0]=data_QUAN[i][j].Y[0][0]-data_QUAN[i][j-1].Y[0][0];
            data_QUAN[i][j].Cb[0][0]=data_QUAN[i][j].Cb[0][0]-data_QUAN[i][j-1].Cb[0][0];
            data_QUAN[i][j].Cr[0][0]=data_QUAN[i][j].Cr[0][0]-data_QUAN[i][j-1].Cr[0][0];
        }
    }
    //第一行的另外做 要扣掉上一行的
    for(i=H-1;i>0;i--){
        data_QUAN[i][0].Y[0][0]=data_QUAN[i][0].Y[0][0]-data_QUAN[i-1][W-1].Y[0][0];
        data_QUAN[i][0].Cb[0][0]=data_QUAN[i][0].Cb[0][0]-data_QUAN[i-1][W-1].Cb[0][0];
        data_QUAN[i][0].Cr[0][0]=data_QUAN[i][0].Cr[0][0]-data_QUAN[i-1][W-1].Cr[0][0];
    }

    printf("finish\n");

    printf("Y:(Diff)\n");
    for(k=0;k<1;k++){
        for(l=0;l<64;l++){
			printf("%3d ",data_QUAN[k][l].Y[0][0]);
			if(l%8==7) printf("\n");
 		}
 		printf("\n");
    }
}

/*IDPCM*/
void IDPCM(ImgQUAN **data_QUAN, int H, int W){
    printf("\nIDPCM...\n");

    for(i=1;i<H;i++){
        data_QUAN[i][0].Y[0][0]=data_QUAN[i][0].Y[0][0]+data_QUAN[i-1][W-1].Y[0][0];
        data_QUAN[i][0].Cb[0][0]=data_QUAN[i][0].Cb[0][0]+data_QUAN[i-1][W-1].Cb[0][0];
        data_QUAN[i][0].Cr[0][0]=data_QUAN[i][0].Cr[0][0]+data_QUAN[i-1][W-1].Cr[0][0];
    }

    for(i=0;i<H;i++){
        for(j=1;j<W;j++){
            data_QUAN[i][j].Y[0][0]=data_QUAN[i][j].Y[0][0]+data_QUAN[i][j-1].Y[0][0];
            data_QUAN[i][j].Cb[0][0]=data_QUAN[i][j].Cb[0][0]+data_QUAN[i][j-1].Cb[0][0];
            data_QUAN[i][j].Cr[0][0]=data_QUAN[i][j].Cr[0][0]+data_QUAN[i][j-1].Cr[0][0];
        }
    }

    printf("finish\n");

    printf("Y:(original)\n");
    for(k=0;k<1;k++){
        for(l=0;l<64;l++){
			printf("%3d ",data_QUAN[k][l].Y[0][0]);
			if(l%8==7) printf("\n");
 		}
 		printf("\n");
    }
}

/*RLE*/
void RLE(ImgRLE **data_RLE, ImgZigZag ** data_ZigZag, int H, int W){
    printf("\nRLE...\n");
    int Y_zero, Cb_zero, Cr_zero;
    int n1,n2,n3;
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            Y_zero = 0;//計算連續幾個0
            Cb_zero = 0;
            Cr_zero = 0;
            n1=1;
            n2=1;
            n3=1;
            for(k=1;k<64;k++){
                //Y
                //值不為0時
                if((data_ZigZag[i][j].Y[k])!=0){
                    data_RLE[i][j].Y_value[n1]=data_ZigZag[i][j].Y[k];
                    data_RLE[i][j].Y_zero_num[n1]=Y_zero;
                    Y_zero=0;
                    n1++;
                }
                //值為0時Y_zero+1，計算連續多少個0
                else{ //data_ZigZag[i][j].Y[k]==0
                    Y_zero++;
                }

                //Cb
                if((data_ZigZag[i][j].Cb[k])!=0){
                    data_RLE[i][j].Cb_value[n2]=data_ZigZag[i][j].Cb[k];
                    data_RLE[i][j].Cb_zero_num[n2]=Cb_zero;
                    Cb_zero=0;
                    n2++;
                }
                else{
                    Cb_zero++;
                }

                //Cr
                if((data_ZigZag[i][j].Cr[k])!=0){
                    data_RLE[i][j].Cr_value[n3]=data_ZigZag[i][j].Cr[k];
                    data_RLE[i][j].Cr_zero_num[n3]=Cr_zero;
                    Cr_zero=0;
                    n3++;
                }
                else{
                    Cr_zero++;
                }
            }
            //最後都是0寫(0,0)
            data_RLE[i][j].Y_value[n1]=0;
            data_RLE[i][j].Y_zero_num[n1]=0;
            data_RLE[i][j].Cb_value[n2]=0;
            data_RLE[i][j].Cb_zero_num[n2]=0;
            data_RLE[i][j].Cr_value[n3]=0;
            data_RLE[i][j].Cr_zero_num[n3]=0;
        }
    }
    printf("finish\n");

    for(i=1;i<15;i++){
        printf("%3d ", data_RLE[0][0].Y_zero_num[i]);
        printf("%3d ", data_RLE[0][0].Y_value[i]);
    }
}

//middle product
void middle_product(ImgRLE **data_RLE, ImgQUAN ** data_QUAN, int H, int W){
    //寫出壓縮後的大小
    FILE *fp_RLE_txt = fopen("RLE.txt","wb");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            fprintf(fp_RLE_txt,"%d",data_QUAN[i][j].Y[0][0]);//先寫DPCM的DC
            for(k=1;k<64;k++){
                fprintf(fp_RLE_txt,"(%d,%d)",data_RLE[i][j].Y_zero_num[k], data_RLE[i][j].Y_value[k]);//再寫RLE的AC
                if(data_RLE[i][j].Y_value[k]==0) break;//會寫(0,0)後跳到下一個block
            }
        }
    }

    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            fprintf(fp_RLE_txt,"%d",data_QUAN[i][j].Cb[0][0]);
            for(k=1;k<64;k++){
                fprintf(fp_RLE_txt,"(%d,%d)",data_RLE[i][j].Cb_zero_num[k], data_RLE[i][j].Cb_value[k]);
                if(data_RLE[i][j].Cb_value[k]==0) break;
            }
        }
    }

    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            fprintf(fp_RLE_txt,"%d",data_QUAN[i][j].Cr[0][0]);
            for(k=1;k<64;k++){
                fprintf(fp_RLE_txt,"(%d,%d)",data_RLE[i][j].Cr_zero_num[k], data_RLE[i][j].Cr_value[k]);
                if(data_RLE[i][j].Cr_value[k]==0) break;
            }
        }
    }
    fclose(fp_RLE_txt);
}

/*IRLE*/
void IRLE(ImgRLE **data_RLE, ImgZigZag ** data_ZigZag, int H, int W){
    printf("\nIRLE...\n");
    int n1,n2,n3;

    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=1;k<64;k++){
                data_ZigZag[i][j].Y[k]=0;
                data_ZigZag[i][j].Cb[k]=0;
                data_ZigZag[i][j].Cr[k]=0;
                n1=1;
                n2=1;
                n3=1;
                for(l=1;l<64;l++){
                    //Y
                    if(n1<64){
                        if(data_RLE[i][j].Y_zero_num[l]!=0){ //補上被壓縮的0
                        for(m=0;m<data_RLE[i][j].Y_zero_num[l];m++){
                            data_ZigZag[i][j].Y[n1]=0;
                            n1++;
                        }
                    }
                    data_ZigZag[i][j].Y[n1]=data_RLE[i][j].Y_value[l];
                    n1++;
                    }
                }

                for(l=1;l<64;l++){
                    //Cb
                    if(n2<64){
                        if(data_RLE[i][j].Cb_zero_num[l]!=0){
                        for(m=0;m<data_RLE[i][j].Cb_zero_num[l];m++){
                            data_ZigZag[i][j].Cb[n2]=0;
                            n2++;
                        }
                    }
                    data_ZigZag[i][j].Cb[n2]=data_RLE[i][j].Cb_value[l];
                    n2++;
                    }
                }

                 for(l=1;l<64;l++){
                    //Cr
                    if(n3<64){
                        if(data_RLE[i][j].Cr_zero_num[l]!=0){
                        for(m=0;m<data_RLE[i][j].Cr_zero_num[l];m++){
                            data_ZigZag[i][j].Cr[n3]=0;
                            n3++;
                        }
                    }
                    data_ZigZag[i][j].Cr[n3]=data_RLE[i][j].Cr_value[l];
                    n3++;
                    }
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3d ",data_ZigZag[0][0].Y[k*8+l]);
 		}
 		printf("\n");
    }
}

/*huffman*/

// Category          DC/AC value
// -----------------------------------------
// 0 					   0
// 1 		  		   –1 , 1
// 2 				 3,–2 , 2,3
// 3 			  –7..–4 , 4..7
// 4		 	 –15..–8 , 8..15
// 5 			–31..–16 , 16..31
// 6 			–63..–32 , 32..63
// 7 		   –127..–64 , 64..127
// 8 		  –255..–128 , 128..255
// 9 		  –511..–256 , 256..511
// 10 		 –1023..–512 , 512..1023
// 11 		–2047..–1024 , 1024..2047


//category
int get_category(int value){
    //先把值轉正
    int category;
    int a[20];
    if(value<0){
        value = -value;
    }
    int num;
	for(num=0;value>0;num++){//轉成二進位
		a[num]=value%2;
		value=value/2;
	}
    category=num;//i即是category的數值
    return category;
}

void huffman(ImgRLE **data_RLE, ImgQUAN ** data_QUAN, int H, int W){
    int DC_string[256];
    int category =0;
    int value;
    int run;
    int runsize;

    FILE *fp_temp_txt = fopen("temp.txt","wb");

/*------------------Y------------------*/
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            //DC_Y
            category = get_category(data_QUAN[i][j].Y[0][0]);
            //print Huffman code
            fprintf(fp_temp_txt,"%s",huffman_Y_DC[category].code_word_string);

            //DC值轉二進位後print------
            value = data_QUAN[i][j].Y[0][0];

            for(l=0;l<256;l++){
                DC_string[256] = 0;
            }

            if(value < 0){ //如果值是負的，先變成正的，再把0和1對調(1010->0101)
                value = -value;
                for(k=0;value>0;k++){
                    if(value%2==1){
                        DC_string[k]=0;
                    }
                    if(value%2==0){
                        DC_string[k]=1;
                    }
                    value=value/2;
                }
                for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }

            else{
                for(k=0;value>0;k++){
                    DC_string[k]=value%2;
                    value=value/2;
                }
                 for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }
            //AC_Y
            for(m=1;m<64;m++){
                category = get_category(data_RLE[i][j].Y_value[m]);
                run = data_RLE[i][j].Y_zero_num[m];
                runsize = run*10 + category;
                fprintf(fp_temp_txt,"%s",huffman_Y_AC[runsize].code_word_string);
                if(data_RLE[i][j].Y_value[m]==0) break;
            }
        }
    }
/*------------------Cb------------------*/
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            //DC_Cb
            category = get_category(data_QUAN[i][j].Cb[0][0]);
            //print Huffman code
            fprintf(fp_temp_txt,"%s",huffman_C_DC[category].code_word_string);

            //DC值轉二進位後print------
            value = data_QUAN[i][j].Cb[0][0];

            for(l=0;l<256;l++){
                DC_string[256] = 0;
            }

            if(value < 0){ //如果值是負的，先變成正的，再把0和1對調(1010->0101)
                value = -value;
                for(k=0;value>0;k++){
                    if(value%2==1){
                        DC_string[k]=0;
                    }
                    if(value%2==0){
                        DC_string[k]=1;
                    }
                    value=value/2;
                }

                for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }

            else{
                for(k=0;value>0;k++){
                    DC_string[k]=value%2;
                    value=value/2;
                }
                 for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }
            //AC_Cb
            for(m=1;m<64;m++){
                category = get_category(data_RLE[i][j].Cb_value[m]);
                run = data_RLE[i][j].Cb_zero_num[m];
                runsize = run*10 + category;
                fprintf(fp_temp_txt,"%s",huffman_C_AC[runsize].code_word_string);
                if(data_RLE[i][j].Cb_value[m]==0) break;
            }
        }
    }
/*------------------Cr------------------*/
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            //DC_Cr
            category = get_category(data_QUAN[i][j].Cr[0][0]);
            //print Huffman code
            fprintf(fp_temp_txt,"%s",huffman_C_DC[category].code_word_string);

            //DC值轉二進位後print------
            value = data_QUAN[i][j].Cr[0][0];

            for(l=0;l<256;l++){
                DC_string[256] = 0;
            }

            if(value < 0){ //如果值是負的，先變成正的，再把0和1對調(1010->0101)
                value = -value;
                for(k=0;value>0;k++){
                    if(value%2==1){
                        DC_string[k]=0;
                    }
                    if(value%2==0){
                        DC_string[k]=1;
                    }
                    value=value/2;
                }

                for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }

            else{
                for(k=0;value>0;k++){
                    DC_string[k]=value%2;
                    value=value/2;
                }
                 for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }
            //AC_Cr
            for(m=1;m<64;m++){
                category = get_category(data_RLE[i][j].Cr_value[m]);
                run = data_RLE[i][j].Cr_zero_num[m];
                runsize = run*10 + category;
                fprintf(fp_temp_txt,"%s",huffman_C_AC[runsize].code_word_string);
                if(data_RLE[i][j].Cr_value[m]==0) break;
            }
        }
    }
/*-------------------------------------------*/

    fclose(fp_temp_txt);
}

int main(int argc, char **argv){

	FILE *fp_in;
	FILE *fp_out;

/*open*/
    printf("open...\n");
	fp_in = fopen(argv[1],"rb");
	fp_out = fopen(argv[2],"wb");
    //fp_in = fopen("input.bmp","rb");
	//fp_out = fopen("output.bmp","wb");

/*read*/
	//read header
	printf("read header...\n");
	Bitmap bmpheader;
	read_header(fp_in, &bmpheader);

	//取出width和height
	int H = bmpheader.height;
	int W = bmpheader.width;

	ImgRGB **data_RGB = malloc_2D_ImgRGB(H,W);
        ImgYCbCr **data_YCbCr = malloc_2D_ImgYCbCr(H/8,W/8);
        ImgDCT **data_DCT = malloc_2D_ImgDCT(H/8,W/8);
        ImgQUAN **data_QUAN = malloc_2D_ImgQUAN(H/8,W/8);
        ImgZigZag **data_ZigZag = malloc_2D_ImgZigZag(H/8,W/8);
        ImgRLE **data_RLE = malloc_2D_ImgRLE(H/8,W/8);

	//skip paddings at the end of each image line
	int skip = (4-W*3%4);
	if (skip == 4) skip = 0;
	char skip_buf[3] = {0,0,0};

	// 8x8, if not multiples of 8, then bitmap padded, i.e. one more block
	int block_H = H / 8;
	if (H % 8 != 0) block_H++;
	int block_W = W / 8;
	if (W % 8 != 0) block_W++;

	printf("Image size: Width: %d x Height=%d pixels, total %d bytes\n", W, H, W*H * 3);
	printf("Image line skip=%d bytes\n", skip);
	printf("# of 8x8 blocks: W=%d x H=%d blocks, total %d blocks\n", block_W, block_H, block_W*block_H);
	printf("\nLoading BMP data: RRGGBB RRGGBB ... (BGR format in BMP file)\n");

	//read data
	read_data(fp_in, data_RGB, H, W, skip);
	fclose(fp_in);

	//寫出原始大小
	FILE *fp_RGB_txt = fopen("RGB.txt","wb");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            fprintf(fp_RGB_txt,"%d,",data_RGB[i][j].B);
            fprintf(fp_RGB_txt,"%d,",data_RGB[i][j].G);
            fprintf(fp_RGB_txt,"%d,",data_RGB[i][j].R);
        }
    }
    fclose(fp_RGB_txt);

/*色度轉換*/
    YCbCr(data_RGB, data_YCbCr, H/8, W/8);

/*DCT*/
    DCT(data_YCbCr, data_DCT, H/8, W/8);

/*量化*/
    QUAN(data_QUAN, data_DCT, H/8, W/8);

/*ZigZag*/
    ZigZag(data_ZigZag, data_QUAN, H/8, W/8);

/*DPCM*/
    DPCM(data_QUAN, H/8, W/8);

/*RLE*/
    RLE(data_RLE, data_ZigZag, H/8, W/8);

    //middle product
    middle_product(data_RLE, data_QUAN, H/8, W/8);

/*Huffman*/
    huffman(data_RLE, data_QUAN, H/8, W/8);

/*----decoder----*/

/*IRLE*/
    IRLE(data_RLE, data_ZigZag, H/8, W/8);

/*IDPCM*/
    IDPCM(data_QUAN, H/8, W/8);

/*IZigZag*/
    IZigZag(data_ZigZag, data_QUAN, H/8, W/8);

/*反量化*/
    IQUAN(data_QUAN, data_DCT, H/8, W/8);

/*IDCT*/
    IDCT(data_YCbCr, data_DCT, H/8, W/8);

/*反色度轉換*/
    IYCbCr(data_RGB, data_YCbCr, H/8, W/8);

/*write*/
    write_header(fp_out, &bmpheader);
    write_data(fp_out, data_RGB, H, W, skip);

/*free pointer*/
	free(data_RGB);
	free(data_YCbCr);
	free(data_DCT);
	free(data_QUAN);
	free(data_ZigZag);
	free(data_RLE);
}


