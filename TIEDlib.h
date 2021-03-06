/*
 * Created by Leander on 07/05/2021.
 * TIED Library Header
 */

#ifndef PROJECT_TIED_TIEDLIB_H
#define PROJECT_TIED_TIEDLIB_H

// Libraries
/*
 * This header contains the following headers:
 * - stdio.h
 * - stdlib.h
 * - time.h
 * - stdarg.h
 * - string.h
 * - ctype.h
 */
#include "lilib.h"
#include <unistd.h>
#include <math.h>

// Definitions
#define ENCRYPTED 0x18 // CAN (Cancel) :: 0001 1000 :: 24 // Place this in BMP header @6 - 10, unused data <- check if there to prevent encrypting an already encrypted image
#define SOD 0x2 // STX (Start Of Text) :: 0000 0010 :: 2 // Decrypt first, the encryption is applied to this as well, if not the text might be encrypted to one of these characters
#define EOD 0x3 // ETX (End Of Text) :: 0000 0011 :: 3 // Encryption does not allow for the use of the first 4 ASCII characters in text file to be encrypted // only NULL now not allowed
// Use this to call the caesarCipherF function, it will add the size for you. If you use malloc, be sure to fill the memory with something not NULL!
// Arrays
#define caesarCipherA(input, output, encrypt, key) caesarCipherF(input, sizeOfArray(input), output, sizeOfArray(output), encrypt, key)
// Strings
#define caesarCipherS(input, output, encrypt, key) caesarCipherF(input, strlen(input), output, strlen(output), encrypt, key)

// Functions
/*
 * Encrypts an image.
 * @param: char * (imageP) the image to decrypt, char * (textP) where the text file is, char * (output) where the output files need to be, unsigned char (key) the key to decrypt the image
 * @return: char 0 if failed, 1 if successful
 */
char encryptImage(char *imageP, char *textP, char *outputP, unsigned char key);

/*
 * Decrypts an image.
 * @param: char * (imageP) the image to decrypt, char * (outputP) where the output files need to be, unsigned char (key) the key to decrypt the image
 * @return: char 0 if failed, 1 if successful
 */
char decryptImage(char *imageP, char *outputP, unsigned char key);

/*
 * Gets data from the image, assigns memory, make sure to free it! Returns 0 if failed.
 * @param: char * (imageP) pointer to image, char ** (headerReturn) header data to return, memory gets assigned, char ** (dataReturn) image data to return, memory gets assigned
 * @return: char 0 if failed, 1 if successful
 */
char getImageData(char *imageP, char **headerReturn, char **dataReturn);

/*
 * Counts the character in a file.
 * @param: char * (fileName) the name of the file.
 * @return: int (counter) the total counted characters
 */
unsigned int countFileCharacters(char *fileName);

/*
 * Prints help output.
 * @param: void
 * @return: void
 */
void printHelp(void);

/*
 * Checks a file, if it has the right extension and if it exists.
 * @param: char * (fileName) the name of the file you would like to check, char * (fileExtension) the extension the file should have, char (terminate) if it should terminate the program if the file check fails, char (existing) if the file needs to exist
 * @return: char 0 if it failed the check, 1 if it passes, if terminating, nothing
 */
char checkFile(char *fileName, char *fileExtension, char terminate, char existing);

/*
 * Applies a caesar cipher to the input and send it to the output.
 * 0 - 123 provide encryption (Except 0), 124 does not, 124+ loops back to 0 - 123, if key + character go over 255 it breaks the function.
 * @param: char * (input) input text, unsigned int (inputLength) length of the input text, char * (output) output text, unsigned int (outputLength) length of the output text, unsigned char (encrypt) 0 decrypt 1 encrypt, unsigned char (key) encryption key
 * @return: char 0 if input and output are not of the same size, 1 if it has finished
 */
char caesarCipherF(char *input, unsigned int inputLength, char *output, unsigned int outputLength, unsigned char encrypt, unsigned char key);

#endif // PROJECT_TIED_TIEDLIB_H
