/*
	Created by Leander on 30/04/2021.
	Main file for TIED program.
*/

// Libraries
#include "myLog.h"

// Definitions
// TBD

// Functions
// TBD

int main(int argc, char const *argv[])
{
	myLog(1, __FILE__, __LINE__, 0, "%s", "This is a test.");
	myLog(2, __FILE__, __LINE__, 1, "%s", "This is a test.");
	myLog(3, __FILE__, __LINE__, 1, "%s", "This is a test.");
	myLog(-1, __FILE__, __LINE__, 1, "%s", "This is a test.");
}

