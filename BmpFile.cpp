#include "BmpFile.h"

bool isBmpFile(ifstream& fin) {
	BmpSignature signature;

	fin.seekg(0, fin.beg); // move to the beginning of the file.
	fin.read((char*)&signature, 2);

	return (signature.first == 66 && signature.second == 77); // 66 is 'B', 77 is 'M'.
}

void readBmpHeader(ifstream& fin, BmpHeader& header) {
	fin.seekg(0, fin.beg); // move to the beginning of the file.

	fin.read((char*)&header.signature, sizeof(BmpSignature));
	fin.read((char*)&header.fileSize, sizeof(header.fileSize));
	fin.read((char*)&header.reserved1, sizeof(header.reserved1));
	fin.read((char*)&header.reserved2, sizeof(header.reserved2));
	fin.read((char*)&header.dataOffset, sizeof(header.dataOffset));
}

void readBmpDib(ifstream& fin, BmpDib& dib) {
	fin.seekg(sizeof(BmpHeader), fin.beg);

	fin.read((char*)&dib.dibSize, sizeof(dib.dibSize));
	fin.read((char*)&dib.imageWidth, sizeof(dib.imageWidth));
	fin.read((char*)&dib.imageHeight, sizeof(dib.imageHeight));
	fin.read((char*)&dib.colorPlaneCount, sizeof(dib.colorPlaneCount));
	fin.read((char*)&dib.pixelSize, sizeof(dib.pixelSize));
	fin.read((char*)&dib.compressionAlgorithm, sizeof(dib.compressionAlgorithm));
	fin.read((char*)&dib.pixelArraySize, sizeof(dib.pixelArraySize));
	fin.read((char*)&dib.horizontalResolution, sizeof(dib.horizontalResolution));
	fin.read((char*)&dib.verticalResolution, sizeof(dib.verticalResolution));
	fin.read((char*)&dib.colorCount, sizeof(dib.colorCount));
	fin.read((char*)&dib.importantColorCount, sizeof(dib.importantColorCount));
}

void readPixelArray(ifstream& fin, BmpFile& bmp) {
	PixelArray& pixelArray = bmp.pixelArray; // reference to memory of variable bmp.pixelArray.
	uint32_t widthSize;
	uint8_t padding;

	widthSize = bmp.dib.imageWidth * (bmp.dib.pixelSize / 8);
	padding = (4 - (widthSize % 4)) % 4;
	pixelArray.padding = padding;
	pixelArray.lineSize = padding + widthSize; // total number of bytes pixel in line and padding.
	pixelArray.rawByteSize = pixelArray.lineSize * bmp.dib.imageHeight;
	pixelArray.rawBytes = new uint8_t[pixelArray.rawByteSize];

	fin.seekg(bmp.header.dataOffset, fin.beg);
	fin.read((char*)pixelArray.rawBytes, pixelArray.rawByteSize); // read pixel array (include padding)

	initPixelArray(pixelArray, bmp.dib.imageHeight, bmp.dib.imageWidth);
}

void readBmpFile(const string& filePath, BmpFile& bmp) {
	ifstream fin;

	fin.open(filePath, ios::binary);

	if (!fin.is_open()) {
		cout << "Can not open file." << endl;
		return;
	}

	if (!isBmpFile(fin)) {
		cout << "This file is not bitmap file." << endl;
		fin.close();
		return;
	}

	readBmpHeader(fin, bmp.header);
	readBmpDib(fin, bmp.dib);
	readPixelArray(fin, bmp);

	fin.close();
}

void initPixelArray(PixelArray& pixelArray, const uint32_t& height, const uint32_t& width) {
	uint32_t lineSize, nRows;

	pixelArray.numberOfRows = height;
	pixelArray.numberOfColumns = width;
	lineSize = pixelArray.lineSize;
	nRows = height;

	pixelArray.pixels = new Color * [nRows];
	for (uint32_t i = 0; i < nRows; i++)
		pixelArray.pixels[nRows - 1 - i] = (Color*)&pixelArray.rawBytes[lineSize * i];
}

void drawImage(const BmpFile& bmp) {
	HWND console = GetConsoleWindow();
	HDC hdc = GetDC(console);
	Color pixel;

	for (int i = 0; i < bmp.pixelArray.numberOfRows; i++) {
		for (int j = 0; j < bmp.pixelArray.numberOfColumns; j++) {
			pixel = bmp.pixelArray.pixels[i][j];
			SetPixel(hdc, j, i, RGB(pixel.red, pixel.green, pixel.blue));
		}
	}

	ReleaseDC(console, hdc);
}

void showInfoBmpHeader(const BmpHeader& header) {
	cout << "----- BMP HEADER -----" << endl;
	cout << setw(35) << left << "- File size: " << header.fileSize << endl;
	cout << setw(35) << left << "- Reserved 1: " << header.reserved1 << endl;
	cout << setw(35) << left << "- Reserved 2: " << header.reserved2 << endl;
	cout << setw(35) << left << "- Data offset: " << header.dataOffset << endl;
}

void showInfoBmpDib(const BmpDib& dib) {
	cout << "----- DIB -----" << endl;
	cout << setw(35) << left << "- DIB size: " << dib.dibSize << endl;
	cout << setw(35) << left << "- Image width: " << dib.imageWidth << endl;
	cout << setw(35) << left << "- Image Height: " << dib.imageHeight << endl;
	cout << setw(35) << left << "- Number of color planes: " << dib.colorPlaneCount << endl;
	cout << setw(35) << left << "- Pixel size: " << dib.pixelSize << endl;
	cout << setw(35) << left << "- Compression algorithm: " << dib.compressionAlgorithm << endl;
	cout << setw(35) << left << "- Pixel array size: " << dib.pixelArraySize << endl;
	cout << setw(35) << left << "- Horizontal resolution: " << dib.horizontalResolution << endl;
	cout << setw(35) << left << "- Vertical resolution: " << dib.verticalResolution << endl;
	cout << setw(35) << left << "- Number of colors: " << dib.colorCount << endl;
	cout << setw(35) << left << "- Number of important colors: " << dib.importantColorCount << endl;
}

void showPixelAtPosition(const BmpFile& bmp) {
	Color pixel;
	int row, col;

	cout << "Enter row and column: ";
	cin >> row >> col;

	pixel = bmp.pixelArray.pixels[row - 1][col - 1];

	cout << "Pixel at position (" << row << ", " << col << "):" << endl;
	cout << "- Red: " << (int)pixel.red << endl;
	cout << "- Green: " << (int)pixel.green << endl;
	cout << "- Blue: " << (int)pixel.blue << endl;
}

void showMenu(const BmpFile& bmp) {
	int choice;
	
	do {
		system("cls");

		cout << "\t----- Menu -----" << endl;
		cout << "1 - Display the all data of bitmap file." << endl;
		cout << "2 - Display RGB information of pixel at row i and column j (i, j >= 1)." << endl;
		cout << "3 - Display the bitmap image." << endl;
		cout << "4 - Exit." << endl;

		cout << "Enter your choice: ";
		cin >> choice;

		switch (choice) {
		case 1:
			showInfoBmpHeader(bmp.header);
			cout << endl;
			showInfoBmpDib(bmp.dib);
			break;
		case 2:
			showPixelAtPosition(bmp);
			break;
		case 3:
			system("cls");
			drawImage(bmp);
			break;
		case 4:
			cout << "You have exited the program." << endl;
			break;
		default:
			cout << "Invalid choice." << endl;
			break;
		}

		system("pause");
	} while (choice != 4);
}

void releaseDynamicArray(BmpFile& bmp) {
	delete[] bmp.pixelArray.pixels;
	delete[] bmp.pixelArray.rawBytes;
}