#include "aoc.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#endif

static const Input errorInput = {NULL, 0};

/*
 * Format a string.
 *
 * Parameters:
 * - format: A format string, followed by its arguments.
 * 
 * Returns:
 * - A dynamically allocated string on success.
 * - NULL on failure.
 */
static char* formatString(const char* format, ...)
{
	if(!format)
	{
		fprintf(stderr, "Invalid argument.\n");
		return NULL;
	}

	va_list argsSize;
	va_start(argsSize, format);

	va_list argsString;
	va_copy(argsString, argsSize);

	const int stringSize = vsnprintf(NULL, 0, format, argsSize);
	va_end(argsSize);
	if(stringSize <= 0)
	{
		va_end(argsString);
		fprintf(stderr, "Could not get string size.\n");
		return NULL;
	}

	char* const string = malloc(stringSize + 1);
	if(!string)
	{
		va_end(argsString);
		fprintf(stderr, "Could not allocate string.\n");
		return NULL;
	}

	if(vsnprintf(string, stringSize + 1, format, argsString) != stringSize)
	{
		free(string);
		va_end(argsString);
		fprintf(stderr, "Could not format string.\n");
		return NULL;
	}
	va_end(argsString);

	return string;
}

/*
 * Resize a buffer.
 *
 * Parameters:
 * - pData: Pointer to the buffer.
 * - newSize: The new size of the buffer.
 * 
 * Returns:
 * - true on success.
 * - false on failure.
 */
static bool resize(char** pData, size_t newSize)
{
	if(!pData || !*pData || !newSize)
	{
		fprintf(stderr, "Invalid argument.\n");
		return false;
	}

	char* const newData = realloc(*pData, newSize);
	if(!newData)
	{
		return false;
	}
	*pData = newData;

	return true;
}

/*
 * Read the content of a text file.
 *
 * Parameters:
 * - file: A file handle opened in text mode.
 * 
 * Returns:
 * - {NULL, 0} if something went wrong.
 * - The problem input if everything went correctly.
 */
static Input readFile(FILE* file)
{
	if(!file)
	{
		fprintf(stderr, "Invalid argument.\n");
		return errorInput;
	}

	size_t bufferCapacity = 1024;
	char* input = malloc(bufferCapacity * sizeof(input[0]));
	if(!input)
	{
		fprintf(stderr, "Could not allocate buffer to read file.\n");
		return errorInput;
	}

	size_t toRead = bufferCapacity;
	size_t read;
	size_t inputLength = 0;
	while(true)
	{
		read = fread(input + inputLength, sizeof(input[0]), toRead, file);
		inputLength += read;

		if(read < toRead)
		{
			if(!feof(file))
			{
				free(input);
				fprintf(stderr, "Could not read file.\n");
				return errorInput;
			}

			if(!resize(&input, inputLength + 1))
			{
				free(input);
				return errorInput;
			}
			input[inputLength] = '\0';

			return (Input){input, inputLength};
		}

		// Resize buffer
		bufferCapacity *= 2;
		if(!resize(&input, bufferCapacity))
		{
			free(input);
			return errorInput;
		}

		toRead = bufferCapacity / 2;
	}
}

Input getInput(unsigned year, unsigned day, const char* sessionCookie)
{
	// Create file name
	char* const fileName = formatString("%u-%02u.txt", year, day);
	if(!fileName)
	{
		fprintf(stderr, "Could not format file name.\n");
		return errorInput;
	}

	// Check if file already exists
	FILE* file = fopen(fileName, "r");
	if(file)
	{
		free(fileName);

		Input input = readFile(file);
		fclose(file);

		return input;
	}

#ifdef _WIN32
	const HINTERNET internet = InternetOpen(
		TEXT("aoc"),
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0
	);
	if(!internet)
	{
		free(fileName);
		fprintf(stderr, "Could not open internet.\n");
		return errorInput;
	}

	char* const url = formatString("https://adventofcode.com/%u/day/%u/input", year, day);
	if(!url)
	{
		free(fileName);
		InternetCloseHandle(internet);
		fprintf(stderr, "Could not format URL.\n");
		return errorInput;
	}

	// Create session cookie header
	char* const sessionCookieHeader = formatString("Cookie: session=%s", sessionCookie);
	if(!sessionCookieHeader)
	{
		free(fileName);
		free(url);
		InternetCloseHandle(internet);
		fprintf(stderr, "Could not format session cookie header.\n");
		return errorInput;
	}

	const HINTERNET internetUrl = InternetOpenUrlA(
		internet,
		url,
		sessionCookieHeader,
		(DWORD)-1,
		INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI,
		0
	);
	free(url);
	free(sessionCookieHeader);
	if(!internetUrl)
	{
		free(fileName);
		InternetCloseHandle(internet);
		fprintf(stderr, "Could not open URL.\n");
		return errorInput;
	}

	char* const result = malloc(8192);
	DWORD bytesRead = 0;
	InternetReadFile(internetUrl, result, 8192, &bytesRead);
	result[bytesRead] = '\0';

	if(!InternetCloseHandle(internetUrl))
	{
		free(fileName);
		fprintf(stderr, "Could not close URL.\n");
		return errorInput;
	}

	if(!InternetCloseHandle(internet))
	{
		free(fileName);
		fprintf(stderr, "Could not close internet.\n");
		return errorInput;
	}
#else
	free(sessionCookieHeader);
#endif

	file = fopen(fileName, "w");
	free(fileName);
	if(!file)
	{
		fprintf(stderr, "Could not open file.\n");
		return errorInput;
	}

	fprintf(file, "%s", result);

	fclose(file);

	return (Input){result, strlen(result)};
}
