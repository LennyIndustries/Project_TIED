/*
 * Created by Leander on 30/04/2021.
 * Main file for TIED program.
 *
 * MFR = Mark For Removal (AKA DEBUG), must be removed out of release version.
 *
 * gcc -Wall -pedantic -c myLog.c -o myLog.o
 * gcc -Wall -pedantic TIED.c liLog.o -o TIED.exe
 *
 * http://www.libsdl.org/index.php
 */

// Libraries
// Own
#include "TIEDlib.h"
/*
 * This header contains the following headers:
 * - stdio.h
 * - stdlib.h
 * - time.h
 * - stdarg.h
 * - string.h
 * - ctype.h
 * - unistd.h
 */

// Standard
//#include <limits.h>
//#include <unistd.h>
#include <stdint.h>

// Definitions
#define START 0x2 // STX :: 0000 0010 :: 2
#define STOP 0x3 // ETX :: 0000 0011 :: 3

// Functions
/*
 * Reads hex from the header of a BMP file and converts it to decimal. Can handle up to 4 bytes and does not rely on type.
 * @param: const char * (header) the header array, unsigned char (startPos) the starting point in the array, unsigned char (bytesToRead) the amount of bytes to read out of the header
 * @return: int (decimal) the decimal number read out of the BMP
 */
int decimalFromHeader(const char *header, unsigned int startPos, unsigned char bytesToRead); // MFR

// Main
/*
 * Commands: (minimum of 7 arguments is required for either function of the program)
 * - /help :: commands overview
 * - /e [IMAGE] :: encrypt :: REQUIRES: /t, /o, /k
 * - /d [IMAGE] :: decrypt :: REQUIRES: /o, /k
 * - /t [TEST FILE] :: text file
 * - /o [OUTPUT FILE} :: output file, either .txt OR .bmp
 * - /k [KEY] :: encryption key, a key between 0 and 127 for the basic Caesar Cipher, a offset for the characters in ASCII format (0 - 127)
 */
int main(const int argc, char *argv[])
{
	FILE *image = NULL;
	char input = '\0';

	// Argument pointers
	char encrypt = -1; // Whether to encrypt or decrypt
	char *imageP = NULL; // BMP image pointer input
	char *outputP = NULL; // Pointer output
	char *textP = NULL; // TXT file pointer input
	short key = 0; // Caesar Cipher key, must be between 0 and 127
	// Header data
	int dataOffset = 0;
//	short bitsPerPixel = 0;
	int imageSize = 0;
	// Data storage arrays
	char header[54] = {'\0'};
	char *imageData = NULL;

	liLog(1, __FILE__, __LINE__, 0, "Program running: clearing log."); // This clears the log, do NOT clear it anymore.
	printf("Hello %s\nWelcome to TIED.\n\n", getenv("USERNAME")); // Welcomes to user

	// Resolving arguments
	if (argc <= 1) // Making sure there are arguments
	{
		printf("To few arguments: %i.\nRun /help for more info.\nTerminating program.\n", argc);
		printHelp();
		liLog(3, __FILE__, __LINE__, 1, "User did not enter enough arguments: %i", argc);
		exit(EXIT_FAILURE);
	}
	else
	{
		for (int i = 1; i < argc; i++) // Setting any command to lowercase
		{
			if (argv[i][0] == '/') // Checking the first position so it is a command and not a path that contains '/'
			{
				lowerStr(argv[i]);
			}
		}

		printf("Commands.\n");

		for (int i = 1; i < argc; i++) // Loading arguments into local variables
		{
			if (strcmp(argv[i], "/help") == 0) // strcmp returns 0 if strings match // User want's help, does not matter where it is requested, this exits to program
			{
				liLog(1, __FILE__, __LINE__, 1, "User requested help. Printing & exiting.");
				printHelp();
				exit(EXIT_SUCCESS); // Since only help was requested this was successful
			}
			else if ((strcmp(argv[i], "/e") == 0) && (encrypt == -1)) // Encrypting, if encrypt is not -1 it is already decrypting
			{
				i++;
				imageP = argv[i];
				encrypt = 1;
				printf("Encrypting: %s.\n", imageP);
				liLog(1, __FILE__, __LINE__, 1, "Encrypting: %s.", imageP);
			}
			else if ((strcmp(argv[i], "/d") == 0) && (encrypt == -1)) // Decrypting, if encrypt is not -1 it is already encrypting
			{
				i++;
				imageP = argv[i];
				encrypt = 0;
				printf("Decrypting: %s.\n", imageP);
				liLog(1, __FILE__, __LINE__, 1, "Decrypting: %s.", imageP);
			}
			else if (strcmp(argv[i], "/t") == 0) // Text file as in or output, depends on encryption setting
			{
				i++;
				textP = argv[i];
				printf("Text file: %s.\n", textP);
				liLog(1, __FILE__, __LINE__, 1, "Text file: %s.", textP);
			}
			else if (strcmp(argv[i], "/o") == 0) // Output file, either bmp or txt, depends on encryption setting
			{
				i++;
				outputP = argv[i];
				printf("Output file: %s.\n", outputP);
				liLog(1, __FILE__, __LINE__, 1, "Output file: %s.", outputP);
			}
			else if (strcmp(argv[i], "/k") == 0) // Key for Caesar Cipher (shifting)
			{
				i++;
				key = (short) strtol(argv[i], NULL, 10);
				// Obviously storing the key as plain text is not a good idea, but it is not a safe/secure encryption anyway, this way you can find the key back if you forget it.
				liLog(1, __FILE__, __LINE__, 1, "Key: %d.", key);
			}
			else // Argument is not known, exiting
			{
				printf("Unknown argument: %s.\nRun /help for more info.\nTerminating program.\n", argv[i]);
				printHelp();
				liLog(3, __FILE__, __LINE__, 1, "Unknown argument: %s.", argv[i]);
				exit(EXIT_FAILURE);
			}
		}
	}

	// Checking arguments
	// File extension & existence
	checkFile(imageP, "bmp", 1, 1); // If wrong program is terminated, must exist
	encrypt ? checkFile(textP, "txt", 1, 1) : 0;
	if (!(encrypt ? checkFile(outputP, "bmp", 0, 0) : checkFile(outputP, "txt", 0, 0))) // Does not need to exist
	{
		printf("File: %s,\ndoes not have the correct extension.\nTerminating program.\n", _fullpath(NULL, outputP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Output file is not correct.");
		exit(EXIT_FAILURE);
	}
	// Key range
	if (key < 0 || key > 127)
	{
		printf("Key (%d) out of bounds: 0 - 127 (incl).\nTerminating program.\n", key);
		liLog(3, __FILE__, __LINE__, 1, "Key (%d) out of bounds.", key);
		exit(EXIT_FAILURE);
	}

	// Confirming settings
	printf("Do you want to continue with these settings? (y/n)\n");
	do
	{
		scanf(" %c", &input);
		fflush(stdin);
		lowerStr(&input);
		if (input == 'n')
		{
			printf("Terminating program");
			liLog(1, __FILE__, __LINE__, 1, "User requested termination of program.");
			exit(EXIT_SUCCESS); // Since the user asked for it
		}
	} while (input != 'y');

	// Opening image
	image = fopen(_fullpath(NULL, imageP, _MAX_PATH), "rb"); // Need a way to escape '\' from path. Or convert '\' to '/' (<- more portable). // NM, use this: _fullpath(NULL, fileName, _MAX_PATH)
	// Check if image is opened
	if (image == NULL)
	{
		printf("Failed to open image\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to open: %s", _fullpath(NULL, imageP, _MAX_PATH)); // Change "TestImage.bmp" to string with custom image name.
		exit(EXIT_FAILURE);
	}
	// Read header
	fread(header, sizeof(unsigned char), 54, image);
	// Get data from header
	dataOffset = *(int *) &header[10];
	imageSize = *(int *) &header[34];
//	dataOffset = (int) decimalFromHeader(header, 10, 4); // ref
//	imageSize = (int) decimalFromHeader(header, 34, 4);
	// Allocate memory
	imageData = malloc(sizeof(char) * (imageSize));
	// Checking if memory allocation was successful
	if (!imageData)
	{
		printf("Failed to allocate memory\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		exit(EXIT_FAILURE);
	}
	// Set memory values
	memset(imageData, 0, sizeof(char) * (imageSize));
	// Read image data
//	fread(offsetData, sizeof(unsigned char), dataOffset - 54, image); // Useless to keep but needed to set the next fread start position correctly // ref
	fseek(image, dataOffset, SEEK_SET); // Start at the beginning of the file then go dataOffset bytes further end leave the pointer there
	fread(imageData, sizeof(unsigned char), imageSize, image);

	// Old test code, here for reference
//	bitsPerPixel = (short) decimalFromHeader(header, 28, 2); // 24 BPP has an offset to get rows of 32 bytes
//	short signature = (short) decimalFromHeader(header, 0, 2);
//	int fileSize = (int) decimalFromHeader(header, 2, 4);
//	int reserved = (int) decimalFromHeader(header, 6, 4);
//	int size = (int) decimalFromHeader(header, 14, 4); // Size of info header, 40 bytes contain all info however this seems to be equal to data offset - 14 (header size)
//	int width = (int) decimalFromHeader(header, 18, 4);
//	int height = (int) decimalFromHeader(header, 22, 4);
//	short planes = (short) decimalFromHeader(header, 26, 2);
//	int compression = (int) decimalFromHeader(header, 30, 4);
//	int xPixelsPerM = (int) decimalFromHeader(header, 38, 4);
//	int yPixelsPerM = (int) decimalFromHeader(header, 42, 4);
//	int colorsUsed = (int) decimalFromHeader(header, 46, 4);
//	int importantColors = (int) decimalFromHeader(header, 50, 4);

//	int firstPixel = decimalFromHeader(imageData, 0, bitsPerPixel / 8); // Do a check to make sure the image has at least 8 bits per pixel
//	int lastPixel = decimalFromHeader(imageData, imageSize - 1, 1);
//	printf("Data Offset: %i\n", dataOffset);
//	printf("bitsPerPixel: %i\n", bitsPerPixel);
//	printf("%i ::: %i\n", firstPixel, lastPixel);

	// Free memory
	free(imageData);
	// Exit
	return 0;
}


int decimalFromHeader(const char *header, unsigned int startPos, unsigned char bytesToRead) // This works but is overcomplicated and uses large data types // MFR
{
	int decimal = 0;
	// https://docs.oracle.com/cd/E19455-01/806-0477/chapter3-10/index.html // uintptr_t
	unsigned long long base = 1; // Needs long long because base can go to 68719476736 if 4 bytes are being read, long gives only 4 bytes on 32 bit OS
	unsigned char nybbleStorage = '\0';
	unsigned char nybbleMask = B(00001111);

	for (unsigned int i = startPos; i < (startPos + bytesToRead); i++)
	{
		for (char j = 0; j < 2; j++)
		{
			nybbleStorage = (unsigned char) (header[i] & (nybbleMask << (j * 4))) >> (j * 4);
			decimal += (int) ((unsigned long long) nybbleStorage * base);
			liLog(1, __FILE__, __LINE__, 1, "nybbleStorage: %hu\tShift 1: %i\tShift 2: %i\tDecimal: %i\tBase: %llu", nybbleStorage, (j * 4), (4 - (j * 4)), decimal, base);
			base *= 16;
		}
	}

	liLog(1, __FILE__, __LINE__, 1, "Returning from decimalFromHeader: %i", decimal);
	return decimal;
}
