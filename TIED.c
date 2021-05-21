/*
 * Created by Leander on 30/04/2021.
 * Main file for TIED program.
 *
 * MFR = Mark For Removal (AKA DEBUG), must be removed out of release version.
 *
 * gcc -Wall -pedantic -c lilog.c -o lilog.o
 * gcc -Wall -pedantic -c lilib.c -o lilib.o
 * gcc -Wall -pedantic -c TIEDlib.c -o TIEDlib.o
 * gcc -Wall -pedantic TIED.c liLog.o lilib.o TIEDlib.o -o TIED.exe
 *
 * /e TestImage10x10.bmp /o Output.bmp /k 50 /t TestFile_Small.txt
 * /d Output.bmp /o Output.txt /k 50
 *
 * -e TestImageSquare.bmp -o Output.bmp -t TestFile_ABC.txt -k 120
 * -d Output.bmp -o Output.txt -k 120
 *
 * Enable extended logging: -DETXLOG
 *
 * http://www.libsdl.org/index.php
 */

// Libraries
// Own
/*
 * This header contains the following headers:
 * - stdio.h
 * - stdlib.h
 * - time.h
 * - stdarg.h
 * - string.h
 * - ctype.h
 * - unistd.h
 * - math.h
 */
#include "TIEDlib.h"

// Standard
#include <stdint.h>

// Definitions
// DO NOT DEFINE EXTLOG HERE OR ANYWHERE IN HEADERS OR C FILES USED BY THE PROGRAM, USE -DEXTLOG

// Functions

// Main
/*
 * Commands: (minimum of 5 arguments is required for either function of the program)
 * - /help :: commands overview
 * - /e [IMAGE] :: encrypt :: REQUIRES: /t, /o
 * - /d [IMAGE] :: decrypt :: REQUIRES: /o
 * - /t [TEST FILE] :: text file
 * - /o [OUTPUT FILE] :: output file, either .txt OR .bmp
 * - /k [KEY] :: encryption key, a key between 0 and 123 for the basic Caesar Cipher, an offset for the characters in ASCII format (0 - 123)
 */
int main(const int argc, char *argv[])
{
	char input = '\0';
	// Argument pointers
	char encrypt = -1; // Whether to encrypt or decrypt
	char *imageP = NULL; // BMP image pointer input
	char *outputP = NULL; // Pointer output
	char *textP = NULL; // TXT file pointer input
	unsigned char key = 0; // Caesar Cipher key, must be between 0 and 127

	liLog(1, __FILE__, __LINE__, 0, "Program running: clearing log."); // This clears the log, do NOT clear it anymore.
	printf("Hello %s\nWelcome to TIED.\n\n", getenv("USERNAME")); // Welcomes to user

	// Resolving arguments
	if (argc <= 1) // Making sure there are arguments
	{
		printf("To few arguments: %i.\nRun '//' or '--' help for more info.\nTerminating program.\n", argc);
		printHelp();
		liLog(3, __FILE__, __LINE__, 1, "User did not enter enough arguments: %i", argc);
		exit(EXIT_FAILURE);
	}
	else
	{
		for (int i = 1; i < argc; i++) // Setting any command to lowercase
		{
			if ((argv[i][0] == '/') || (argv[i][0] == '-')) // Checking the first position so it is a command and not a path that contains '/', '-' is also allowed to prefix commands
			{
				strToLower(argv[i]);
			}
		}

		printf("Commands:\n");

		for (int i = 1; i < argc; i++) // Loading arguments into local variables
		{
			if ((strcmp(argv[i] + 2, "help") == 0) && (((argv[i][0] == '/') && (argv[i][1] == '/')) || ((argv[i][0] == '-') && (argv[i][1] == '-')))) // strcmp returns 0 if strings match // User want's help, does not matter where it is requested, this exits to program
			{
				liLog(1, __FILE__, __LINE__, 1, "User requested help. Printing & exiting.");
				printHelp();
				exit(EXIT_SUCCESS); // Since only help was requested this was successful
			}
			else if (((strcmp(argv[i] + 1, "e") == 0) && (encrypt == -1)) && ((argv[i][0] == '/') || (argv[i][0] == '-'))) // Encrypting, if encrypt is not -1 it is already decrypting
			{
				i++;
				imageP = argv[i];
				encrypt = 1;
				printf("Encrypting: %s.\n", _fullpath(NULL, imageP, _MAX_PATH));
				liLog(1, __FILE__, __LINE__, 1, "Encrypting: %s.", _fullpath(NULL, imageP, _MAX_PATH));
			}
			else if (((strcmp(argv[i] + 1, "d") == 0) && (encrypt == -1)) && ((argv[i][0] == '/') || (argv[i][0] == '-'))) // Decrypting, if encrypt is not -1 it is already encrypting
			{
				i++;
				imageP = argv[i];
				encrypt = 0;
				printf("Decrypting: %s.\n", _fullpath(NULL, imageP, _MAX_PATH));
				liLog(1, __FILE__, __LINE__, 1, "Decrypting: %s.", _fullpath(NULL, imageP, _MAX_PATH));
			}
			else if ((strcmp(argv[i] + 1, "t") == 0) && ((argv[i][0] == '/') || (argv[i][0] == '-'))) // Text file as input, only used while encrypting
			{
				i++;
				textP = argv[i];
				printf("Text file: %s.\n", _fullpath(NULL, textP, _MAX_PATH));
				liLog(1, __FILE__, __LINE__, 1, "Text file: %s.", _fullpath(NULL, textP, _MAX_PATH));
			}
			else if ((strcmp(argv[i] + 1, "o") == 0) && ((argv[i][0] == '/') || (argv[i][0] == '-'))) // Output file, either bmp or txt, depends on encryption setting
			{
				i++;
				outputP = argv[i];
				printf("Output file: %s.\n", _fullpath(NULL, outputP, _MAX_PATH));
				liLog(1, __FILE__, __LINE__, 1, "Output file: %s.", _fullpath(NULL, outputP, _MAX_PATH));
			}
			else if ((strcmp(argv[i] + 1, "k") == 0) && ((argv[i][0] == '/') || (argv[i][0] == '-'))) // Key for Caesar Cipher (shifting), range: 0 - 123, optional argument
			{
				i++;
				key = strtol(argv[i], NULL, 10);
				// Obviously storing the key as plain text is not a good idea, but it is not a safe/secure encryption anyway, this way you can find the key back if you forget it.
				#ifdef EXTLOG // Only log with extended logging defined, gcc: -DEXTLOG
					liLog(1, __FILE__, __LINE__, 1, "Key: %d.", key);
				#endif
			}
			else // Argument is not known, exiting
			{
				printf("\nUnknown argument: %s.\nRun '//' or '--' help for more info.\nTerminating program.\n", argv[i]);
				printHelp();
				liLog(3, __FILE__, __LINE__, 1, "Unknown argument: %s.", argv[i]);
				exit(EXIT_FAILURE);
			}
		}
	}

	// Checking arguments
	// Required arguments
	if ((imageP == NULL) || (outputP == NULL) || (encrypt ? (textP == NULL) : 0))
	{
		printf("Missing arguments.\n");
		printHelp();
		liLog(3, __FILE__, __LINE__, 1, "Missing arguments.");
		exit(EXIT_FAILURE);
	}
	// File extension & existence
	checkFile(imageP, "bmp", 1, 1); // If wrong program is terminated, must exist
	encrypt ? checkFile(textP, "txt", 1, 1) : 0;
	if (!(encrypt ? checkFile(outputP, "bmp", 0, 0) : checkFile(outputP, "txt", 0, 0))) // Does not need to exist, does not terminate in checkFile
	{
		printf("File: %s,\ndoes not have the correct extension.\nTerminating program.\n", _fullpath(NULL, outputP, _MAX_PATH));
		liLog(3, __FILE__, __LINE__, 1, "Output file is not correct.");
		exit(EXIT_FAILURE);
	}
	// Key range
	if (key > 123) // 0 - 123 provide encryption (Except 0), 124 does not, 124+ loops back to 0 - 123, if key + character go over 255 it breaks the function
	{
		printf("Key (%d) out of bounds: 0 - 123 (incl).\nTerminating program.\n", key);
		liLog(3, __FILE__, __LINE__, 1, "Key (%d) out of bounds.", key);
		exit(EXIT_FAILURE);
	}

	// Confirming settings
	printf("Key: %i\n", key);
	printf("Do you want to continue with these settings? (y/n)\n");
	do
	{
		scanf(" %c", &input);
		fflush(stdin);
		strToLower(&input);
		if (input == 'n')
		{
			printf("Terminating program");
			liLog(1, __FILE__, __LINE__, 1, "User requested termination of program.");
			exit(EXIT_SUCCESS); // Since the user asked for it
		}
	} while (input != 'y');
	printf("\n");

	printf("\n%s %s.\n", (encrypt ? "Encryption" : "Decryption"), ((encrypt ? encryptImage(imageP, textP, outputP, key) : decryptImage(imageP, outputP, key)) ? "successful" : "failed"));

	// Exit
	printf("\nExiting TIED.\nGoodbye %s\n", getenv("USERNAME"));
	return EXIT_SUCCESS;
}
