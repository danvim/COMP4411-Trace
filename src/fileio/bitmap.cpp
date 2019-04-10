//
// bitmap.cpp
//
// handle MS bitmap I/O. For portability, we don't use the data structure defined in Windows.h
// However, there is some strange thing, the side of our structure is different from what it 
// should though we define it in the same way as MS did. So, there is a hack, we use the hardcoded
// constant, 14, instead of the sizeof to calculate the size of the structure.
// You are not supposed to worry about this part. However, I will appreciate if you find out the
// reason and let me know. Thanks.
//

#include "bitmap.h"
#include <cstdio>
#include <cstring>

BmpBitmapFileHeader fileHeader;
BmpBitmapInfoHeader infoHeader;

unsigned char* readBmp(const char* fName, int& width, int& height)
{
	FILE* file;
	fopen_s(&file, fName, "rb");

	if (file == nullptr)
		return nullptr;

	//	I am doing f read( &fileHeader, sizeof(BmpBitmapInfoHeader), 1, file ) in a safe way. :}
	fread(&fileHeader.bfType, 2, 1, file);
	fread(&fileHeader.bfSize, 4, 1, file);
	fread(&fileHeader.bfReserved1, 2, 1, file);
	fread(&fileHeader.bfReserved2, 2, 1, file);
	fread(&fileHeader.bfOffBits, 4, 1, file);

	const auto pos = fileHeader.bfOffBits;

	fread(&infoHeader, sizeof(BmpBitmapInfoHeader), 1, file);

	// error checking
	if (fileHeader.bfType != 0x4d42)
	{
		// "BM" actually
		return nullptr;
	}
	if (infoHeader.biBitCount != 24)
		return nullptr;
	/*
		 if ( infoHeader.biCompression != BMP_BI_RGB ) {
			return NULL;
		}
	*/
	fseek(file, pos, SEEK_SET);

	width = infoHeader.biWidth;
	height = infoHeader.biHeight;

	auto padWidth = width * 3;
	auto pad = 0;
	if (padWidth % 4 != 0)
	{
		pad = 4 - padWidth % 4;
		padWidth += pad;
	}
	const auto bytes = height * padWidth;

	auto* data = new unsigned char [bytes];

	const int foo = fread(data, bytes, 1, file);

	if (!foo)
	{
		delete [] data;
		return nullptr;
	}

	fclose(file);

	auto* in = data;
	auto* out = data;

	for (auto j = 0; j < height; ++j)
	{
		for (auto i = 0; i < width; ++i)
		{
			out[1] = in[1];
			const auto temp = in[2];
			out[2] = in[0];
			out[0] = temp;

			in += 3;
			out += 3;
		}
		in += pad;
	}

	return data;
}

void writeBmp(char* iName, const int width, const int height, unsigned char* data)
{
	auto bytes = width * 3;
	const auto pad = bytes % 4 ? 4 - bytes % 4 : 0;
	bytes += pad;
	bytes *= height;

	fileHeader.bfType = 0x4d42; // "BM"
	fileHeader.bfSize = sizeof(BmpBitmapFileHeader) + sizeof(BmpBitmapInfoHeader) + bytes;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = /*hack sizeof(BmpBitmapInfoHeader)=14, sizeof doesn't work?*/
		14 + sizeof(BmpBitmapInfoHeader);

	infoHeader.biSize = sizeof(BmpBitmapInfoHeader);
	infoHeader.biWidth = width;
	infoHeader.biHeight = height;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = BMP_BI_RGB;
	infoHeader.biSizeImage = 0;
	infoHeader.biXPixelsPerMeter = int(100 / 2.54 * 72);
	infoHeader.biYPixelsPerMeter = int(100 / 2.54 * 72);
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	FILE* foo;

	fopen_s(&foo, iName, "wb");

	//	f write(&fileHeader, sizeof(BmpBitmapInfoHeader), 1, foo);
	fwrite(&fileHeader.bfType, 2, 1, foo);
	fwrite(&fileHeader.bfSize, 4, 1, foo);
	fwrite(&fileHeader.bfReserved1, 2, 1, foo);
	fwrite(&fileHeader.bfReserved2, 2, 1, foo);
	fwrite(&fileHeader.bfOffBits, 4, 1, foo);

	fwrite(&infoHeader, sizeof(BmpBitmapInfoHeader), 1, foo);

	bytes /= height;
	const auto scanLine = new unsigned char [bytes];
	for (auto j = 0; j < height; ++j)
	{
		memcpy(scanLine, data + j * 3 * width, bytes);
		for (auto i = 0; i < width; ++i)
		{
			const auto temp = scanLine[i * 3];
			scanLine[i * 3] = scanLine[i * 3 + 2];
			scanLine[i * 3 + 2] = temp;
		}
		fwrite(scanLine, bytes, 1, foo);
	}

	delete [] scanLine;

	fclose(foo);
}
