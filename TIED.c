/*
 * Created by Leander on 30/04/2021.
 * Main file for TIED program.
 *
 * MFR = Mark For Removal (AKA DEBUG), must be removed out of release version.
 *
 * gcc -Wall -pedantic -c myLog.c -o myLog.o
 * gcc -Wall -pedantic TIED.c myLog.o -o TIED.exe
 */

// Libraries
#include "myLog.h"

#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>

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

#define START 0x2 // STX :: 0000 0010 :: 2
#define STOP 0x3 // ETX :: 0000 0011 :: 3

// Functions
/*
 * Counts the character in a file.
 * @param: char * (fileName) the name of the file.
 * @return: int (counter) the total counted characters
 */
unsigned int fileCharacters(char *fileName);

/*
 * Reads hex from the header of a BMP file and converts it to decimal. Can handle up to 4 bytes and does not rely on type.
 * @param: const char * (header) the header array, unsigned char (startPos) the starting point in the array, unsigned char (bytesToRead) the amount of bytes to read out of the header
 * @return: int (decimal) the decimal number read out of the BMP
 */
int decimalFromHeader(const char *header, unsigned char startPos, unsigned char bytesToRead); // MFR

/*
 * Converts a string to all lowercase, string is edited directly if you would like to keep a copy of the original string you need to make it yourself.
 * @param: char * (str) a pointer to the string
 * @return: void
 */
void lowerStr(char *str);

/*
 * Prints help output.
 * @param: void
 * @return: void
 */
void printHelp(void);

/*
 * TBD
 */
char checkFile(char *fileName, char *fileExtension, char terminate);

/*
 * TBD
 */
int indexOf(const char * str, char character);

/*
 * TBD
 */
void subString(char *str_source, char *str_destination, int startPos, int length);

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
int main(const int argc, char * const argv[])
{
	myLog(1, __FILE__, __LINE__, 0, "Program running: clearing log."); // This clears the log, do NOT clear it anymore.
	printf("Hello %s\nWelcome to TIED.\n\n", getenv("USERNAME"));
	// TESTING //
	char string[] = "TEST - test"; // MFR
	lowerStr(string); // MFR
	printf("%s\n", string); // MFR

	myLog(1, __FILE__, __LINE__, 1, "%s", "This is a test."); // MFR
	myLog(2, __FILE__, __LINE__, 1, "%s", "This is a test."); // MFR
	myLog(3, __FILE__, __LINE__, 1, "%s", "This is a test."); // MFR
	myLog(-1, __FILE__, __LINE__, 1, "%s", "This is a test."); // MFR

	printf("Characters counted: %i\n", fileCharacters("TestFile.txt")); // MFR

	FILE *image = NULL;
	char header[54];
	int width = 0;
	int height = 0;
	int size = 0;

	image = fopen("TestImage.bmp", "rb"); // Need a way to escape '\' from path. Or convert '\' to '/' (<- more portable).

	if (image == NULL)
	{
		printf("Failed to open image\n");
		myLog(3, __FILE__, __LINE__, 1, "Failed to open: %s", "TestImage.bmp"); // Change "TestImage.bmp" to string with custom image name.
		exit(EXIT_FAILURE);
	}

	fread(header, sizeof(unsigned char), 54, image);

	size = (int) decimalFromHeader(header, 2, 4); // MFR
	width = (int) decimalFromHeader(header, 18, 4); // MFR
	height = (int) decimalFromHeader(header, 22, 4); // MFR


	printf("Width: %i\nHeight: %i\nSize: %i\n", width, height, size); // MFR
	int test = decimalFromHeader(header, 0, 4); // MFR
	printf("%i\tSizeOf: %i\n", test, sizeof(unsigned long long)); // MFR
	myLog(1, __FILE__, __LINE__, 1, "%s", "-- END OF TEST --"); // MFR
	// END OF TESTING //

	char *imageP = NULL; // BMP image pointer input
	char *outputP = NULL; // Pointer output
	char *textP = NULL; // TXT file pointer, either in- or output

	unsigned char key = 0; // Caesar Cipher key, must be between 0 and 127
	char encrypt = -1; // Whether to encrypt or decrypt

	if (argc <= 1)
	{
		printf("To few arguments: %i.\nRun /help for more info.\nTerminating program.\n", argc);
		printHelp();
		myLog(3, __FILE__, __LINE__, 1, "User did not enter enough arguments: %i", argc);
		exit(EXIT_FAILURE);
	}
	else
	{
        for (int i = 1; i < argc; i++)
        {
            if (strstr(argv[i], "/"))
            {
                lowerStr(argv[i]);
            }
        }

		for (int i = 1; i < argc; i++) // Loading arguments into local variables
		{
			if (!strcmp(argv[i], "/help"))
			{
				myLog(1, __FILE__, __LINE__, 1, "User requested help. Printing & exiting.");
				printHelp();
				exit(EXIT_SUCCESS);
			}
			else if (!strcmp(argv[i], "/e"))
			{
				i++;
				imageP = argv[i];
				checkFile(imageP, "bmp", 1);
				encrypt = 1;
				myLog(1, __FILE__, __LINE__, 1, "Encrypting: %s.", imageP);
			}
			else if (!strcmp(argv[i], "/d"))
			{
				i++;
				imageP = argv[i];
				checkFile(imageP, "bmp", 1);
				encrypt = 0;
				myLog(1, __FILE__, __LINE__, 1, "Decrypting: %s.", imageP);
			}
			else if (!strcmp(argv[i], "/t"))
			{
				i++;
				textP = argv[i];
				checkFile(textP, "txt", 1);
				myLog(1, __FILE__, __LINE__, 1, "Text file: %s.", textP);
			}
			else if (!strcmp(argv[i], "/o"))
			{
				i++;
				outputP = argv[i];
				myLog(1, __FILE__, __LINE__, 1, "Output file: %s.", outputP);
			}
			else if (!strcmp(argv[i], "/k"))
			{
				i++;
				key = *argv[i];
				// Obviously storing the key as plain text is not a good idea, but it is not a safe/secure encryption anyway, this way you can find the key back if you forget it.
				myLog(1, __FILE__, __LINE__, 1, "Key: %i.", key);
			}
			else
			{
				printf("Unknown argument: %s.\nRun /help for more info.\nTerminating program.\n", argv[i]);
				printHelp();
				myLog(3, __FILE__, __LINE__, 1, "Unknown argument: %s.", argv[i]);
				exit(EXIT_FAILURE);
			}
		}
	}
}

unsigned int fileCharacters(char *fileName)
{
	FILE *file = NULL;
	unsigned int counter = 0;
	char charStorage = '\0';

	file = fopen(fileName, "r");

	if (file == NULL)
	{
		myLog(2, __FILE__, __LINE__, 1, "Could not open: %s", fileName);
		return 0;
	}

	for (charStorage = (char) fgetc(file); charStorage != EOF; charStorage = (char) fgetc(file))
	{
		counter++;
	}

	fclose(file);

	myLog(1, __FILE__, __LINE__, 1, "Characters counted in: %s: %i", fileName, counter);

	return counter;
}

int decimalFromHeader(const char *header, unsigned char startPos, unsigned char bytesToRead) // This works but is overcomplicated and uses large data types // MFR
{
	int decimal = 0;
	// https://docs.oracle.com/cd/E19455-01/806-0477/chapter3-10/index.html
	uintptr_t  base = 1; // Needs long long because base can go to 68719476736 if 4 bytes are being read, long gives only 4 bytes on 32 bit OS
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

	myLog(1, __FILE__, __LINE__, 1, "Returning from decimalFromHeader: %i", decimal);
	return decimal;
}

void lowerStr(char *str)
{
	for (size_t i = 0; i < strlen(str); i++)
	{
		str[i] = (char) tolower(str[i]);
	}
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
	printf("- /k [KEY] :: encryption key, a key between 0 and 127 for the basic Caesar Cipher,\n\ta offset for the characters in ASCII format (0 - 127)\n");
}

char checkFile(char *fileName, char *fileExtension, char terminate)
{
	myLog(1, __FILE__, __LINE__, 1, "Checking file: %s.", _fullpath(NULL, fileName, _MAX_PATH));

	int index = indexOf(fileName, '.');
	if (index < 0)
	{
		printf("Unable to check file: %s.\nTerminating program.\n", _fullpath(NULL, fileName, _MAX_PATH));
		myLog(3, __FILE__, __LINE__, 1, "Unable to check file: %s.", _fullpath(NULL, fileName, _MAX_PATH));
		exit(EXIT_FAILURE);
	}
	char extension[(strlen(fileName) - index)];

	subString(fileName, extension, index + 1, (int) (strlen(fileName) - index));

	if ((strcmp(extension, fileExtension) != 0) || (access(fileName, F_OK) != 0))
	{
		myLog(2, __FILE__, __LINE__, 1, "Wrong file extension or non existing: %s :: %s.", _fullpath(NULL, fileName, _MAX_PATH), fileExtension);
		if (terminate)
		{
			myLog(3, __FILE__, __LINE__, 1, "Termination of program requested.");
			printf("File: %s,\ndoes not have %s as extension or does not exist.\nTerminating program.\n", _fullpath(NULL, fileName, _MAX_PATH), fileExtension);
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

int indexOf(const char * str, char character)
{
	int index = -1;
	char tmpCharacterStorage = '\0';
	printf("%i\n", strlen(str));
	for (int i = 0; i < (int) strlen(str); i++)
	{
		tmpCharacterStorage = str[i];
		if (tmpCharacterStorage == character) // !strcmp(&tmpCharacterStorage, &character) <== this does not work, use == instead.
		{
			index = i;
		}
	}
	myLog(1, __FILE__, __LINE__, 1, "Index of: \"%c\" in: %s: %i.", character, str, index);
	return index;
}

void subString(char *str_source, char *str_destination, int startPos, int length)
{
	memcpy(str_destination, &str_source[startPos], length + 1);
	myLog(1, __FILE__, __LINE__, 1, "Substring from: %s: %s.", str_source, str_destination);
}

