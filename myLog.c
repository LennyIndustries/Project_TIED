/*
 * Created by Leander on 30/04/2021.
 * Log function, use this to log output to a file.
 */

// Libraries
#include "myLog.h"

void myLog(char logLevel, char *file, int line, char append, const char *message, ...)
{
	// A big ball of wibbly wobbly, timey wimey stuff.
	char dateTime[22] = {0};
	time_t myTime = time(NULL);
	// Log file.
	FILE *myLog = NULL;
	char *logLevelString = NULL;
	// Arguments
	va_list arg;

	switch (logLevel) // Sets the log level from given number
	{
		case 1:
			logLevelString = "INFO";
			break;
		case 2:
			logLevelString = "WARN";
			break;
		case 3:
			logLevelString = "CRIT";
			break;
		default:
			logLevelString = "!ERR";
			break;
	}

	myLog = fopen(OUTPUTFILENAME, append ? "a" : "w"); // Opens the log file

	if (myLog == NULL) // Check if the file is opened
	{
		printf("-!!!- CRITICAL ERROR -!!!-\nCan not open log file!\nEXITING\n");
		exit(EXIT_FAILURE);
	}

	strftime(dateTime, 22, "%H:%M:%S - %d/%m/%Y", localtime(&myTime)); // Creates date and time string for log file

	fprintf(myLog, "%s :: %s :: File: %s (line: %d) :: ", logLevelString, dateTime, file, line); // Prints all needed info to the log file

	va_start(arg, message); // Prints the message to the log file
	vfprintf(myLog, message, arg);
	va_end(arg);
	fprintf(myLog, "\n"); // Prints a new line at the end

	fclose(myLog); // Closes the log file
}
