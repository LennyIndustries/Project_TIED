/*
 * Created by Leander on 07/05/2021.
 * Lenny Industries Library Header
 */

#ifndef PROJECT_TIED_LILIB_H
#define PROJECT_TIED_LILIB_H

// Libraries
#include "lilog.h"
/*
 * This header contains the following headers:
 * - stdio.h
 * - stdlib.h
 * - time.h
 * - stdarg.h
 */
#include <string.h>
#include <ctype.h>

// Definitions
#define B(x) S_to_binary_(#x) // https://stackoverflow.com/a/15114188/4620857

// Functions
/*
 * Allows binary notation
 * https://stackoverflow.com/a/15114188/4620857
 */
unsigned long long S_to_binary_(const char *s);

/*
 * Converts a string to all lowercase, string is edited directly if you would like to keep a copy of the original string you need to make it yourself.
 * @param: char * (str) a pointer to the string
 * @return: void
 */
void lowerStr(char *str);

/*
 * TBD
 */
int indexOf(const char *str, char character);

/*
 * TBD
 */
void subString(char *str_source, char *str_destination, int startPos, int length);

#endif //PROJECT_TIED_LILIB_H
