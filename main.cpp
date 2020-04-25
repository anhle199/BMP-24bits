#include "BmpFile.h"

int main() {
	string filePath;
	BmpFile bmp;

	cout << "Enter the path of the bitmap file: ";
	getline(cin, filePath);

	readBmpFile(filePath, bmp);
	showMenu(bmp);

	return 0;
}