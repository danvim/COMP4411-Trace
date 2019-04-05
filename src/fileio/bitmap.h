//
// bitmap.h
//
// header file for MS bitmap format
//
//

#ifndef BITMAP_H
#define BITMAP_H

constexpr auto BMP_BI_RGB = 0L;

typedef unsigned short BmpWord;
typedef unsigned int BmpDword;
typedef int BmpLong;

typedef struct
{
	BmpWord bfType;
	BmpDword bfSize;
	BmpWord bfReserved1;
	BmpWord bfReserved2;
	BmpDword bfOffBits;
} BmpBitmapFileHeader;

typedef struct
{
	BmpDword biSize;
	BmpLong biWidth;
	BmpLong biHeight;
	BmpWord biPlanes;
	BmpWord biBitCount;
	BmpDword biCompression;
	BmpDword biSizeImage;
	BmpLong biXPixelsPerMeter;
	BmpLong biYPixelsPerMeter;
	BmpDword biClrUsed;
	BmpDword biClrImportant;
} BmpBitmapInfoHeader;

// global I/O routines
extern unsigned char* readBmp(char* fName, int& width, int& height);
extern void writeBmp(char* iName, int width, int height, unsigned char* data);

#endif
