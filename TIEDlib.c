/*
 * Created by Leander on 07/05/2021.
 * TIED Library
 */

#include "TIEDlib.h"

char encryptImage(char *imageP, char *textP, char *outputP, unsigned char key)
{
	// Files
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
	char *encryptedTextData = NULL;
	// Data copying
	unsigned char mask = 0x80;
	unsigned char storageText = 0x00;
	unsigned char storageImage = 0x00;

	// Getting data
	// Getting data from image
	if (!(getImageData(imageP, &headerData, &imageData)))
	{
		printf("Failed to get image data from: %s.\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Failed to get image data from: %s.", _fullpath(NULL, imageP, _MAX_PATH));
		free(headerData);
		free(imageData);
		return 0;
	}
	// Getting header data
	imageFileSize = *(int *) &headerData[2];
	reserved = *(int *) &headerData[6];
	dataOffset = *(int *) &headerData[10];
	with = *(int *) &headerData[18];
	height = *(int *) &headerData[22];
	bitsPerPixel = *(short *) &headerData[28];

	// Checking file compatibility
	// Getting total character in text file
	fileCharacters = countFileCharacters(textP);
	// Calculating image bytes
	imageBytes = ((with * height * bitsPerPixel) / 8) / 8; // with * height = TOTAL PIXELS; * bitsPerPixel = TOTAL BITS; / 8 = TOTAL BYTES; / 8 = MAX CHARS IN IMAGE (8 bytes per char)
	// Checking sizes
	if (bitsPerPixel < 8)
	{
		printf("Image: %s\ndoes not have enough color depth: %i\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH), bitsPerPixel);
		liLog(3, __FILE__, __LINE__, 1, "Image does not have enough color depth: %i.", bitsPerPixel);
		free(headerData);
		free(imageData);
		return 0;
	}
	if (fileCharacters > imageBytes)
	{
		printf("Can not encrypt text in image, to small.\nText characters: %i :: Possible character in image: %i\nTerminating program.\n", fileCharacters, imageBytes);
		liLog(3, __FILE__, __LINE__, 1, "Image to small: %i :: %i.", fileCharacters, imageBytes);
		free(headerData);
		free(imageData);
		return 0;
	}
	if (reserved == ENCRYPTED)
	{
		printf("Image: %s\nalready encrypted or incompatible with program.\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Image: %s already encrypted or incompatible with program.", _fullpath(NULL, imageP, _MAX_PATH));
		free(headerData);
		free(imageData);
		return 0;
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
		return 0;
	}
	// Allocating memory for textData
	textData = malloc(sizeof(char) * (fileCharacters + 0x2)); // 0x2 Contains start of and end of text tags
	// Checking if memory allocation was successful
	if (textData == NULL)
	{
		printf("Failed to allocate memory.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		free(headerData);
		free(imageData);
		fclose(text);
		return 0;
	}
	// Filling memory
	memset(textData, '0', sizeof(char) * (fileCharacters + 0x2));
	// Reading text
	fread(textData + 0x1, sizeof(char), fileCharacters, text); // Not setting any data on the first byte, reserved for flag
	// Closing text file
	fclose(text);

	// Preparing for output
	// Allocating memory for encryptedTextData
	encryptedTextData = malloc(sizeof(char) * (fileCharacters + 0x2)); // 0x2 Contains start of and end of text tags
	// Checking if memory allocation was successful
	if (encryptedTextData == NULL)
	{
		printf("Failed to allocate memory.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		free(headerData);
		free(imageData);
		free(textData);
		fclose(text);
		return 0;
	}
	// Filling memory
	memset(encryptedTextData, '0', sizeof(char) * (fileCharacters + 0x2));
	// Encrypting text
	caesarCipherF(textData, (fileCharacters + 0x2), encryptedTextData, (fileCharacters + 0x2), 1, key);
	// Freeing memory
	free(textData);
	// Setting start and stop flags
	encryptedTextData[0] = SOD;
	encryptedTextData[fileCharacters + 1] = EOD;
	// Copying encrypted text data to image data
	printf("Copying data, this might take a moment.\n");
	for (unsigned int i = 0; i < (fileCharacters + 0x2); i++)
	{
		printf("\rWorking: %i %%", (int) ceil((i * 100.0) / (fileCharacters + 0x2))); // Progress tracking
		fflush(stdout);

		for (unsigned char j = 0; j < 8; j++)
		{
			storageText = ((encryptedTextData[i] & (mask >> j))) >> (7 - j); // You could remove the 2 variables, but it would become even more unreadable
			storageImage = (imageData[j + (8 * i)] & ~(mask >> 7));
			imageData[j + (8 * i)] = (char) (storageImage | storageText);
		}
	}
	printf("\n");
	// Freeing memory
	free(encryptedTextData);
	// Set encrypted flag in image
	headerData[6] = ENCRYPTED;

	// Generating output
	// Open output file
	output = fopen(_fullpath(NULL, outputP, _MAX_PATH), "wb");
	// Checking if output file is opened
	if (output == NULL)
	{
		printf("Failed to open output file: %s.\nTerminating program.\n", _fullpath(NULL, outputP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Failed to open output file: %s.", _fullpath(NULL, outputP, _MAX_PATH));
		free(headerData);
		free(imageData);
		return 0;
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
	printf("\nFinished text to image encryption.\nThe output file can be found here: %s\nKey used to encrypt = %i\n", _fullpath(NULL, outputP, _MAX_PATH), key);
	return 1;
}

char decryptImage(char *imageP, char *outputP, unsigned char key)
{
	// Files
	FILE *output = NULL;
	// Image data
	char *headerData = NULL;
	char *imageData = NULL;
	int reserved = 0;
	int with = 0;
	int height = 0;
	short bitsPerPixel = 0;
	unsigned int imageBytes = 0;
	// Text data
	char *textData = NULL;
	char *encryptedTextData = NULL;
	// Text retrieving
	unsigned int counter = 0;
	char indexer = 0;
	char storage = '\0';
	unsigned char mask = 0x01;

	// Getting data
	// Getting data from image
	if (!(getImageData(imageP, &headerData, &imageData)))
	{
		printf("Failed to get image data from: %s.\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Failed to get image data from: %s.", _fullpath(NULL, imageP, _MAX_PATH));
		free(headerData);
		free(imageData);
		return 0;
	}
	// Getting data from header
	reserved = *(int *) &headerData[6];
	with = *(int *) &headerData[18];
	height = *(int *) &headerData[22];
	bitsPerPixel = *(short *) &headerData[28];
	// Calculating total bytes
	imageBytes = (with * height * bitsPerPixel) / 8; // with * height = TOTAL PIXELS; * bitsPerPixel = TOTAL BITS; / 8 = TOTAL BYTES
	// Freeing memory
	free(headerData);

	// Checking image
	if (reserved != ENCRYPTED)
	{
		printf("Image: %s\nis not encrypted or incompatible with program.\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Image: %s is not encrypted or incompatible with program.", _fullpath(NULL, imageP, _MAX_PATH));
		free(imageData);
		return 0;
	}

	// Getting text data
	// Counting bytes to retrieve
	printf("Counting bytes, this might take a moment.\n");
	do
	{
		#ifdef EXTLOG
			printf("\rWorking: %i bytes", counter); // Progress tracking
			fflush(stdout);
		#endif
		for (indexer = 0; indexer < 8; indexer++)
		{
			storage <<= 1;
			storage = (char) (storage | (imageData[indexer + (counter * 8)] & mask)); // Bytes in file
		}
		counter++; // The amount of character in the file
	} while ((storage != EOD) && ((indexer + (counter * 8)) <= imageBytes)); // Check that it is not going over the total bytes in the file if EOD is not found
	#ifdef EXTLOG
		printf("\n");
	#endif
	// Checking that we found EOD
	if ((storage != EOD) || (counter > imageBytes))
	{
		printf("Could not find end tag.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to find end tag.");
		return 0;
	}
	// Allocating memory for encryptedTextData
	encryptedTextData = malloc(sizeof(char) * (counter));
	// Checking if memory allocation was successful
	if (encryptedTextData == NULL)
	{
		printf("Failed to allocate memory.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		free(imageData);
		return 0;
	}
	// Filling memory
	memset(encryptedTextData, '\0', sizeof(char) * counter);
	// Retrieving data
	printf("Retrieving text, this might take a moment.\n");
	for (unsigned int i = 0; i < counter; i++)
	{
		printf("\rWorking: %i %%", (int) ceil((i * 100.0) / counter)); // Progress tracking
		fflush(stdout);
		storage = '\0';
		for (char j = 0; j < 8; j++)
		{
			storage <<= 1;
			storage = (char) (storage | (imageData[j + (i * 8)] & mask));
		}
		encryptedTextData[i] = storage;
	}
	printf("\n");
	// Freeing memory
	free(imageData);

	// Checking text
	if (encryptedTextData[0] != SOD)
	{
		printf("Image: %s\ndoes not have a start tag, can be corrupted or incompatible with program.\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Image: %s does not have a start tag, corrupted or incompatible with program.", _fullpath(NULL, imageP, _MAX_PATH));
		return 0;
	}

	// Preparing output
	// Allocating memory for textData
	textData = malloc(sizeof(char) * counter);
	// Checking if memory allocation was successful
	if (textData == NULL)
	{
		printf("Failed to allocate memory.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		free(encryptedTextData);
		return 0;
	}
	// Filling memory
	memset(textData, '\0', sizeof(char) * counter);
	// Decrypting text
	caesarCipherF(encryptedTextData, counter, textData, counter, 0, key);
	// Freeing memory
	free(encryptedTextData);

	// Generating output
	// Open output file
	output = fopen(_fullpath(NULL, outputP, _MAX_PATH), "w");
	// Checking if output file is opened
	if (output == NULL)
	{
		printf("Failed to open output file: %s.\nTerminating program.\n", _fullpath(NULL, outputP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Failed to open output file: %s.", _fullpath(NULL, outputP, _MAX_PATH));
		return 0;
	}
	// Writing text
	fwrite(textData + 0x1, sizeof(char), (sizeof(char) * counter) - 0x2, output); // Ignoring the tags
	// Closing output file
	fclose(output);
	// Freeing memory
	free(textData);

	// Return
	printf("\nFinished image to text decryption.\nThe output file can be found here: %s\nKey used to decrypt = %i\n", _fullpath(NULL, outputP, _MAX_PATH), key);
	return 1;
}

char getImageData(char *imageP, char **headerReturn, char **dataReturn)
{
	// File
	FILE *image = NULL;
	// Image data
	char *headerData = NULL;
	int imageFileSize = 0;
	int dataOffset = 0;
	// Return data
	*headerReturn = NULL;
	*dataReturn = NULL;

	// Getting data from image
	// Opening image
	image = fopen(_fullpath(NULL, imageP, _MAX_PATH), "rb");
	// Checking if image is opened
	if (image == NULL)
	{
		printf("Failed to open image: %s.\nTerminating program.\n", _fullpath(NULL, imageP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Failed to open image: %s.", _fullpath(NULL, imageP, _MAX_PATH));
		return 0;
	}
	// Allocating memory for headerData
	headerData = malloc(54);
	// Checking if memory allocation was successful
	if (headerData == NULL)
	{
		printf("Failed to allocate memory.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		fclose(image);
		return 0;
	}
	// Filling memory
	memset(headerData, 0, 54);
	// Reading header data
	fread(headerData, sizeof(unsigned char), 54, image);
	// Getting header data
	imageFileSize = *(int *) &headerData[2];
	dataOffset = *(int *) &headerData[10];
	// Freeing memory
	free(headerData);

	// Getting full header & image data
	// Allocating memory for headerReturn & dataReturn
	*headerReturn = malloc(sizeof(char) * (dataOffset));
	*dataReturn = malloc(sizeof(char) * (imageFileSize - dataOffset));
	// Checking if memory allocation was successful
	if ((*dataReturn == NULL) || (*headerReturn == NULL))
	{
		printf("Failed to allocate memory.\nTerminating program.\n");
		liLog(3, __FILE__, __LINE__, 1, "Failed to allocate memory.");
		fclose(image);
		return 0;
	}
	// Filling memory
	memset(*headerReturn, 0, sizeof(char) * (dataOffset));
	memset(*dataReturn, 0, sizeof(char) * (imageFileSize - dataOffset));
	// Reading image data
	fseek(image, 0, SEEK_SET); // Start at the beginning of the file then leave the pointer there
	fread(*headerReturn, sizeof(unsigned char), dataOffset, image);
	fread(*dataReturn, sizeof(unsigned char), imageFileSize - dataOffset, image);
	// Closing image
	fclose(image);

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

	printf("Counting character, this might take a moment.\n");
	for (charStorage = (char) fgetc(file); charStorage != EOF; charStorage = (char) fgetc(file))
	{
		#ifdef EXTLOG // Printing this data slows the program down considerably with large files
			printf("\rWorking: %i characters", counter); // Progress tracking
			fflush(stdout);
		#endif
		counter++;
	}
	#ifdef EXTLOG
		printf("\n");
	#endif

	fclose(file);

	liLog(1, __FILE__, __LINE__, 1, "Characters counted in: %s: %i", fileName, counter);

	return counter;
}

void printHelp(void)
{
	printf("\nProject TIED, help overview.\n");
	printf("This program can encrypt or decrypt a BMP file with text and a given Caesar Cipher.\n");
	printf("Commands:\n");
	printf("- //help :: commands overview (this)\n");
	printf("- /e [IMAGE] :: encrypt :: REQUIRES: /t, /o\n");
	printf("- /d [IMAGE] :: decrypt :: REQUIRES: /o\n");
	printf("- /t [TEXT FILE] :: text file\n");
	printf("- /o [OUTPUT FILE] :: output file, either .txt OR .bmp\n");
	printf("- /k [KEY] :: encryption key, a key between 0 and 123 (incl) for the basic Caesar Cipher,\n\tan offset for the characters in ASCII format. Default value = 0.\n");
	printf("- /contact :: open website to contact the developers.\n");
	printf("Commands may be prefixed with '/' or '-'.\n");
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

char caesarCipherF(char *input, unsigned int inputLength, char *output, unsigned int outputLength, unsigned char encrypt, unsigned char key)
{
	if (inputLength != outputLength) // Checks if both arrays are of the same size
	{
		liLog(2, __FILE__, __LINE__, 1, "Size of input (%i) does not match size of output (%i).", inputLength, outputLength);
		return 0;
	}

	printf("%s data, this might take a moment.\n", (encrypt ? "Encrypting" : "Decrypting"));
	liLog(1, __FILE__, __LINE__, 1, "%s text.", (encrypt ? "Encrypting" : "Decrypting"));

	for (unsigned int i = 0; i < inputLength; i++) // Encrypts / decrypts the input to the output
	{
		if (encrypt && (input[i] <= 3)) // The first 4 characters of the ASCII table are not allowed to be in the input while encrypting deu to flags and NULL
		{
			printf("Unable to encrypt, input contains an illegal character (%c).\nTerminating program.\n", input[i]);
			liLog(3, __FILE__, __LINE__, 1, "Input contains an illegal character (%c).", input[i]);
			exit(EXIT_FAILURE);
		}

		printf("\rWorking: %i %%", (int) ceil((i * 100.0) / inputLength)); // Progress tracking
		fflush(stdout);

		if (encrypt)
		{
			output[i] = (char) (((input[i] + key) > 127) ? (((input[i] + key) % 128) + 4) : (input[i] + key));
		}
		else
		{
			output[i] = (char) (((input[i] - key) < 4) ? ((input[i] - key) + 124) : (input[i] - key));
		}

		// Log encryption / decryption
		#ifdef EXTLOG // Logging this data slows the program down considerably with large files
			liLog(1, __FILE__, __LINE__, 1, "%s '%c' to '%c': key: %i.", (encrypt ? "Encrypted" : "Decrypted"), input[i], output[i], key);
		#endif
	}

	printf("\n");

	return 1;
}
