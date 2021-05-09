/*
 * Created by Leander on 07/05/2021.
 * TIED Library
 */

#include "TIEDlib.h"

char encryptImage(char *imageP, char *textP, char *outputP, unsigned char key)
{
	// Files
	FILE *image = NULL;
	FILE *text = NULL;
	FILE *output = NULL;
	// Image data
	char *headerData = NULL;
	char *imageData = NULL;
	int imageFileSize = 0;
	int reserved = 0;
	int dataOffset = 0;
	int with = 0;
	int height = 0;
	short bitsPerPixel = 0;
	unsigned int imageBytes = 0;
	// Text data
	unsigned int fileCharacters = 0;
	char *textData = NULL;

	// Opening image
	image = fopen(_fullpath(NULL, imageP, _MAX_PATH), "rb");
	// Checking if image is opened
	if (image == NULL)
	{
		printf("Failed to open image: %s.\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Failed to open image: %s.", _fullpath(NULL, imageP, _MAX_PATH));
		exit(EXIT_FAILURE);
	}

	// Getting data from image
	// Allocating memory for headerData
	headerData = malloc(54);
	// Checking if memory allocation was successful
	if (!headerData)
	{
		printf("Failed to allocate memory.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		fclose(image);
		exit(EXIT_FAILURE);
	}
	// Filling memory
	memset(headerData, 0, 54);
	// Reading header data
	fread(headerData, sizeof(unsigned char), 54, image);
	// Getting header data
	imageFileSize = *(int *) &headerData[2];
	reserved = *(int *) &headerData[6];
	dataOffset = *(int *) &headerData[10];
	with = *(int *) &headerData[18];
	height = *(int *) &headerData[22];
	bitsPerPixel = *(short *) &headerData[28];
	// Freeing memory
	free(headerData);

	// Getting full header & image data
	// Allocating memory for headerData & imageData
	headerData = malloc(sizeof(char) * (dataOffset));
	imageData = malloc(sizeof(char) * (imageFileSize - dataOffset));
	// Checking if memory allocation was successful
	if ((imageData == NULL) || (headerData == NULL))
	{
		printf("Failed to allocate memory.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		fclose(image);
		exit(EXIT_FAILURE);
	}
	// Filling memory
	memset(headerData, 0, sizeof(char) * (dataOffset));
	memset(imageData, 0, sizeof(char) * (imageFileSize - dataOffset));
	// Reading image data
	fseek(image, 0, SEEK_SET); // Start at the beginning of the file then leave the pointer there
	fread(headerData, sizeof(unsigned char), dataOffset, image);
	fread(imageData, sizeof(unsigned char), imageFileSize - dataOffset, image);
	// Closing image
	fclose(image);

	// Checking file compatibility
	// Getting total character in text file
	fileCharacters = countFileCharacters(textP);
	// Calculating image bytes
	imageBytes = ((with * height * bitsPerPixel) / 8) / 8;
	// Checking sizes
	if (bitsPerPixel < 8)
	{
		printf("Image: %s\ndoes not have enough color depth: %i\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH), bitsPerPixel);
		liLog(3, __FILE__, __LINE__, 1, "Image does not have enough color depth: %i.", bitsPerPixel);
		free(headerData);
		free(imageData);
		exit(EXIT_FAILURE);
	}
	if (fileCharacters > imageBytes)
	{
		printf("Can not encrypt text in image, to small.\nText characters: %i :: Image bytes: %i\nTerminating program.\n", fileCharacters, imageBytes);
		liLog(3, __FILE__, __LINE__, 1, "Image to small: %i :: %i.", fileCharacters, imageBytes);
		free(headerData);
		free(imageData);
		exit(EXIT_FAILURE);
	}
	if (reserved == ENCRYPTED)
	{
		printf("Image: %s\nalready encrypted or incompatible with program.\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Image: %s already encrypted or incompatible with program.", _fullpath(NULL, imageP, _MAX_PATH));
		free(headerData);
		free(imageData);
		exit(EXIT_FAILURE);
	}

	// Getting text data
	// Open text file
	text = fopen(_fullpath(NULL, textP, _MAX_PATH), "r");
	// Checking if text file is opened
	if (text == NULL)
	{
		printf("Failed to open text file: %s.\nTerminating program.\n", _fullpath(NULL, textP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Failed to open text file: %s.", _fullpath(NULL, textP, _MAX_PATH));
		free(headerData);
		free(imageData);
		exit(EXIT_FAILURE);
	}
	// Allocating memory for textData
	textData = malloc(sizeof(char) * (fileCharacters + 0x2)); // 0x2 Contains start of and end of text
	// Checking if memory allocation was successful
	if (textData == NULL)
	{
		printf("Failed to allocate memory.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		free(headerData);
		free(imageData);
		fclose(text);
		exit(EXIT_FAILURE);
	}
	// Filling memory
	memset(textData, '\0', sizeof(char) * (fileCharacters + 0x2));
	// Reading text
	fread(textData + 0x1, sizeof(char), fileCharacters, text); // Not setting any data on the first byte, flag
	// Closing text file
	fclose(text);

	// Preparing for output
	// Setting start and stop flags
	textData[0] = SOD;
	textData[fileCharacters + 1] = EOD;
	// Encrypting text
	caesarCipherS(textData, textData, 1, key);
	// Copying encrypted text data to image data
	unsigned char mask = 0xFE;
	unsigned char storageText = 0x00;
	unsigned char storageImage = 0x00;
	for (unsigned int i = 0; i < (fileCharacters + 0x2); i++)
	{
		for (unsigned char j = 0; j < 8; j++)
		{
			storageText = (textData[i] | mask); // 1111 1110
			storageImage = (imageData[j + (8 * i)] | !mask); // 0000 0001
			imageData[j + (8 * i)] = (char) (storageText & storageImage);
		}
	}
	printf("\n");
	// Freeing memory
	free(textData);
	// Set encrypted flag in image
	headerData[6] = ENCRYPTED;

	// Generating output
	// Open output file
	output = fopen(_fullpath(NULL, outputP, _MAX_PATH), "wb");
	// Checking if text file is opened
	if (output == NULL)
	{
		printf("Failed to open output file: %s.\nTerminating program.\n", _fullpath(NULL, outputP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Failed to open output file: %s.", _fullpath(NULL, outputP, _MAX_PATH));
		free(headerData);
		free(imageData);
		exit(EXIT_FAILURE);
	}
	// Writing header
	fwrite(headerData, sizeof(char), (sizeof(char) * (dataOffset)), output);
	// Writing image
	fwrite(imageData, sizeof(char), (sizeof(char) * (imageFileSize - dataOffset)), output);
	// Closing output file
	fclose(output);
	// Freeing memory
	free(headerData);
	free(imageData);

	// Return
	printf("Finished text to image encryption.\nThe output file can be found here: %s\nKey = %i\n", _fullpath(NULL, outputP, _MAX_PATH), key);
	return 1;
}

char decryptImage(char *image, char *output, unsigned char key)
{
	return 1;
}

unsigned int countFileCharacters(char *fileName)
{
	FILE *file = NULL;
	unsigned int counter = 0;
	char charStorage = '\0';

	file = fopen(fileName, "r");

	if (file == NULL)
	{
		liLog(2, __FILE__, __LINE__, 1, "Could not open: %s", fileName);
		return 0;
	}

	for (charStorage = (char) fgetc(file); charStorage != EOF; charStorage = (char) fgetc(file))
	{
		counter++;
	}

	fclose(file);

	liLog(1, __FILE__, __LINE__, 1, "Characters counted in: %s: %i", fileName, counter);

	return counter;
}

void printHelp(void)
{
	printf("\nProject TIED, help overview.\n");
	printf("This program can encrypt or decrypt a BMP file with text and a given Caesar Cipher.\n");
	printf("Commands:\n");
	printf("- /help :: commands overview (this)\n");
	printf("- /e [IMAGE] :: encrypt :: REQUIRES: /t, /o\n");
	printf("- /d [IMAGE] :: decrypt :: REQUIRES: /o\n");
	printf("- /t [TEXT FILE] :: text file\n");
	printf("- /o [OUTPUT FILE] :: output file, either .txt OR .bmp\n");
	printf("- /k [KEY] :: encryption key, a key between 0 and 124 (incl) for the basic Caesar Cipher,\n\tan offset for the characters in ASCII format. Default value = 0.\n");
}

char checkFile(char *fileName, char *fileExtension, char terminate, char existing)
{
	liLog(1, __FILE__, __LINE__, 1, "Checking file: %s.", _fullpath(NULL, fileName, _MAX_PATH));

	int index = indexOf(fileName, '.');
	if (index < 0)
	{
		printf("Unable to check file: %s.\nTerminating program.\n", _fullpath(NULL, fileName, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Unable to check file: %s.", _fullpath(NULL, fileName, _MAX_PATH));
		exit(EXIT_FAILURE);
	}
	char extension[(strlen(fileName) - index)];

	subString(fileName, extension, index + 1, (int) (strlen(fileName) - index));

	if ((strcmp(extension, fileExtension) != 0) || existing ? (access(fileName, F_OK) != 0) : 0)
	{
		liLog(2, __FILE__, __LINE__, 1, "Wrong file extension or non existing: %s :: %s.", _fullpath(NULL, fileName, _MAX_PATH), fileExtension);
		if (terminate)
		{
			printf("File: %s,\ndoes not have %s as extension or does not exist.\nTerminating program.\n", _fullpath(NULL, fileName, _MAX_PATH), fileExtension);
			liLog(3, __FILE__, __LINE__, 1, "Termination of program requested.");
			exit(EXIT_FAILURE);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 1;
	}
}

// Encrypts character to NULL, ending the return array early <- should be fixed
char caesarCipherF(char *input, unsigned int inputLength, char *output, unsigned int outputLength, unsigned char encrypt, unsigned char key)
{
	if (inputLength != outputLength) // Checks if both arrays are of the same size
	{
		liLog(2, __FILE__, __LINE__, 1, "Size of input (%i) does not match size of output (%i).", inputLength, outputLength);
		return 0;
	}

	printf("%s data, this might take a moment.\n", (encrypt ? "Encrypting" : "Decrypting"));

	unsigned char *storage; // Storage for calculations, they go over 127 but are below 255
	storage = malloc(sizeof(unsigned char) * inputLength); // Reserve memory
	memset(storage, '\0', sizeof(unsigned char) * inputLength); // Fill storage

	for (unsigned int i = 0; i < inputLength; i++) // Encrypts / decrypts the input to the output
	{
		if (encrypt && (input[i] == 0)) // The first character of the ASCII table (NULL) is not allowed to be in the input while encrypting, though the input should never have this anyway
		{
			printf("Unable to encrypt, input contains an illegal character (%c).\nTerminating program.\n", input[i]);
			liLog(3, __FILE__, __LINE__, 1, "Input contains an illegal character (%c).", input[i]);
			exit(EXIT_FAILURE);
		}

		printf("\rWorking: %i %%", (int) ceil((i * 100.0) / inputLength)); // Progress tracking
		fflush(stdout);

		storage[i] = (encrypt ? input[i] : (((input[i] + 125) > 127) ? ((input[i] + 125) % 127) : (input[i] + 125))); // Decompress to 0 - 127 range & copy input to storage
		storage[i] += (encrypt ? 0 : ((storage[i] <= key) ? 127 : 0)); // Adds 127 if decrypting and the value is <= to key
		storage[i] += (encrypt ? key : (-key)); // Applies the key
		storage[i] %= (encrypt ? 127 : 256); // Applies rest division
		storage[i] = (encrypt ? (((storage[i] + 1) % 127) + 1) : storage[i]); // Compress to 1 - 127 range <- NULL is not allowed as input
		output[i] = (char) storage[i]; // Converts storage to char and copies it to output
		liLog(1, __FILE__, __LINE__, 1, "%s '%c' to '%c': key: %i.", (encrypt ? "Encrypted" : "Decrypted"), input[i], output[i], key); // Log encryption / decryption
	}

	printf("\n");
	free(storage); // Free memory

	return 1;
}
