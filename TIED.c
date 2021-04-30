/*
 * Created by Leander on 30/04/2021.
 * Main file for TIED program.
 */

// Libraries
#include "myLog.h"

#include <stdio.h>
#include <stdlib.h>

// Definitions
#define B(x) S_to_binary_(#x) // https://stackoverflow.com/a/15114188/4620857

static inline unsigned long long S_to_binary_(const char *s)
{
	unsigned long long i = 0;
	while (*s)
	{
		i <<= 1;
		i += *s++ - '0';
	}
	return i;
}

//#define START 0x2 // STX :: 0000 0010 :: 2
//#define STOP 0x3 // ETX :: 0000 0011 :: 3

// Functions
/*
 * Counts the character in a file.
 * @param: char * (fileName) the name of the file.
 * @return: int (counter) the total counted characters
 */
int fileCharacters(char *fileName);
//test
/*
 * Reads hex from the header of a BMP file and converts it to decimal. Can handle up to 4 bytes and does not rely on type.
 * @param: const char * (header) the header array, unsigned char (startPos) the starting point in the array, unsigned char (bytesToRead) the amount of bytes to read out of the header
 * @return: int (decimal) the decimal number read out of the BMP
 */
int decimalFromHeader(const char *header, unsigned char startPos, unsigned char bytesToRead);

int main(void) // int argc, char const *argv[]
{
	// TESTING
	myLog(1, __FILE__, __LINE__, 0, "%s", "This is a test.");
	myLog(2, __FILE__, __LINE__, 1, "%s", "This is a test.");
	myLog(3, __FILE__, __LINE__, 1, "%s", "This is a test.");
	myLog(-1, __FILE__, __LINE__, 1, "%s", "This is a test.");

	printf("Characters counted: %i\n", fileCharacters("TestFile.txt"));

	FILE *image = NULL;
	char header[54];
	int width = 0;
	int height = 0;
	int size = 0;

	image = fopen("TestImage.bmp", "rb");

	if (image == NULL)
	{
		printf("Failed to open image");
		exit(EXIT_FAILURE);
	}

	fread(header, sizeof(unsigned char), 54, image);

	size = (int) decimalFromHeader(header, 2, 4);
	width = (int) decimalFromHeader(header, 18, 4);
	height = (int) decimalFromHeader(header, 22, 4);


	printf("Width: %i\nHeight: %i\nSize: %i\n", width, height, size);
	int test = decimalFromHeader(header, 0, 4);
	printf("%i\tSizeOf: %i\n", test, sizeof(unsigned long long));
	// END OF TESTING
}

int fileCharacters(char *fileName)
{
	FILE *file = NULL;
	int counter = 0;
	char charStorage = '\0';

	file = fopen(fileName, "r");

	if (file == NULL)
	{
		return -1;
	}

	for (charStorage = (char) fgetc(file); charStorage != EOF; charStorage = (char) fgetc(file))
	{
		counter++;
	}

	fclose(file);

	myLog(1, __FILE__, __LINE__, 1, "Characters counted in: %s: %i", fileName, counter);

	return counter;
}

int decimalFromHeader(const char *header, unsigned char startPos, unsigned char bytesToRead)
{
	int decimal = 0;
	unsigned long long base = 1;
	unsigned char nybbleStorage = '\0';
	unsigned char nybbleMask = B(00001111);

	for (int i = (int) startPos; i <= (startPos + bytesToRead); i++)
	{
		for (char j = 0; j < 2; j++)
		{
			nybbleStorage = (unsigned char) (header[i] & (nybbleMask << (j * 4))) >> (j * 4);
			decimal += (int) ((unsigned long long) nybbleStorage * base);
			myLog(1, __FILE__, __LINE__, 1, "nybbleStorage: %hu\tShift 1: %i\tShift 2: %i\tDecimal: %i\tBase: %llu", nybbleStorage, (j * 4), (4 - (j * 4)), decimal, base);
			base *= 16;
		}
	}

	return decimal;
}

