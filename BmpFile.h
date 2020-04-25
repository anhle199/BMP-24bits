#ifndef BITMAP_FILE_H_
#define BITMAP_FILE_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <Windows.h>

#pragma pack(1)

using namespace std;

struct BmpSignature {
	uint8_t first;
	uint8_t second;
};

struct BmpHeader {
	BmpSignature signature;
	uint32_t fileSize;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t dataOffset;
};

struct BmpDib {
	uint32_t dibSize;
	uint32_t imageWidth;
	uint32_t imageHeight;
	uint16_t colorPlaneCount;
	uint16_t pixelSize;
	uint32_t compressionAlgorithm;
	uint32_t pixelArraySize;
	uint32_t horizontalResolution;
	uint32_t verticalResolution;
	uint32_t colorCount;
	uint32_t importantColorCount;
};

struct Color {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
};

struct PixelArray {
	uint8_t* rawBytes;
	uint32_t rawByteSize;
	uint32_t lineSize;
	uint8_t padding;

	Color** pixels;
	uint32_t numberOfRows;
	uint32_t numberOfColumns;
};

struct BmpFile {
	BmpHeader header;
	BmpDib dib;
	PixelArray pixelArray;
};

// Returns true if it is a bitmap file; otherwise returns false.
bool isBmpFile(ifstream& fin);

// Read the header data in the bitmap file.
void readBmpHeader(ifstream& fin, BmpHeader& header);

// Read the DIB data in the bitmap file.
void readBmpDib(ifstream& fin, BmpDib& dib);

// Read the pixel array data in the bitmap file.
void readPixelArray(ifstream& fin, BmpFile& bmp);

// Read all data in the bitmap file.
void readBmpFile(const string& filePath, BmpFile& bmp);

// Initialize the pixel matrix and calculate the relevant values.
void initPixelArray(PixelArray& pixelArray, const uint32_t& height, const uint32_t& width);

// Draw the bitmap image to console.
void drawImage(const BmpFile& bmp);

// Display data of Bmp Header to console.
void showInfoBmpHeader(const BmpHeader& header);

// Display data of Bmp DIB to console.
void showInfoBmpDib(const BmpDib& dib);

// Display information of a pixel on the screen.
void showPixelAtPosition(const BmpFile& bmp);

// Display functions to console.
void showMenu(const BmpFile& bmp);

// Release dynamically allocated memory.
void releaseDynamicArray(BmpFile& bmp);

#endif