/*
 * Created by Leander on 07/05/2021.
 * TIED Library
 */

#include "TIEDlib.h"

unsigned int fileCharacters(char *fileName)
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
	printf("- /e [IMAGE] :: encrypt :: REQUIRES: /t, /o, /k\n");
	printf("- /d [IMAGE] :: decrypt :: REQUIRES: /o, /k\n");
	printf("- /t [TEXT FILE] :: text file\n");
	printf("- /o [OUTPUT FILE] :: output file, either .txt OR .bmp\n");
	printf("- /k [KEY] :: encryption key, a key between 0 and 95 (incl) for the basic Caesar Cipher,\n\ta offset for the characters in ASCII format (32 - 127)\n");
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

char caesarCipherF(char *input, unsigned int inputLength, char *output, unsigned int outputLength, unsigned char encrypt, unsigned char key) // Encrypts character to NULL, ending the return array early
{
	if (inputLength != outputLength) // Checks if both arrays are of the same size
	{
		liLog(2, __FILE__, __LINE__, 1, "Size of input (%i) does not match size of output (%i).", inputLength, outputLength);
		return 0;
	}

	unsigned char *storage; // Storage for calculations, they go over 127 but are below 255
	storage = malloc(sizeof(unsigned char) * inputLength); // Reserve memory
	memset(storage, '\0', sizeof(unsigned char) * inputLength); // Fill storage

	for (unsigned int i = 0; i < inputLength; i++) // Encrypts / decrypts the input to the output
	{
		if (encrypt && (input[i] <= 4)) // The first 4 character of the ASCII table are not allowed to be in text file, these are used as limiters by the program
		{
			printf("The text contains forbidden characters.\nTerminating program.");
			liLog(3, __FILE__, __LINE__, 1, "Forbidden character in text (%c).", input[i]);
			exit(EXIT_FAILURE);
		}
		storage[i] = (input[i] + (encrypt ? 0 : (-1))); // Copy input to storage
		storage[i] += (encrypt ? 0 : ((storage[i] <= key) ? 126 : 0)); // Adds 127 if decrypting and the value is <= to key
		storage[i] += (encrypt ? key : (-key)); // Applies the key
		storage[i] %= (encrypt ? 127 : 256); // Applies rest division
		output[i] = (char) (storage[i] + (encrypt ? 1 : 0)); // Converts storage to char and copies it to output
		liLog(1, __FILE__, __LINE__, 1, "%s '%c' to '%c': key: %i.", (encrypt ? "Encrypted" : "Decrypted"), input[i], output[i], key); // Log encryption / decryption
	}

	free(storage); // Free memory

	return 1;
}
