#include "aoc.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>

#if (defined(UNICODE) && !defined(_UNICODE)) || (defined(_UNICODE) && !defined(UNICODE))
#error "UNICODE and _UNICODE should be defined together"
#endif
#endif

static const Input errorInput = {NULL, 0};

#define LOG(...) \
logError(__FILE__, __LINE__, __func__, __VA_ARGS__);

static void logError(const char* file, int line, const char* function, const char* format, ...)
{
	const char* errorFormat = "In file %s at line %d in function %s: %s\n";

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
		return;
	}

	char* const string = malloc(stringSize + 1);
	if(!string)
	{
		va_end(argsString);
		fprintf(stderr, "Could not allocate string.\n");
		return;
	}

	if(vsnprintf(string, stringSize + 1, format, argsString) != stringSize)
	{
		va_end(argsString);
		fprintf(stderr, "Could not format string.\n");
		return;
	}
	va_end(argsString);

	fprintf(stderr, errorFormat, file, line, function, string);

	free(string);
}

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
		LOG("Invalid argument.")
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
		LOG("Could not get string size.")
		return NULL;
	}

	char* const string = malloc(stringSize + 1);
	if(!string)
	{
		va_end(argsString);
		LOG("Could not allocate string.")
		return NULL;
	}

	if(vsnprintf(string, stringSize + 1, format, argsString) != stringSize)
	{
		free(string);
		va_end(argsString);
		LOG("Could not format string.")
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
		LOG("Invalid argument.")
		return false;
	}

	char* const newData = realloc(*pData, newSize);
	if(!newData)
	{
		LOG("Could not reallocate data.")
		return false;
	}
	*pData = newData;

	return true;
}

#ifdef _WIN32
#ifdef  UNICODE
static WCHAR* multiByteToWideChar(const char* string)
{
	const int wideStringSize = MultiByteToWideChar(CP_UTF8, 0, string, -1, NULL, 0);
	if(wideStringSize <= 0)
	{
		LOG("Could not get wide string size.")
		return NULL;
	}

	WCHAR* const wideString = malloc(wideStringSize * sizeof(wideString[0]));
	if(!wideString)
	{
		LOG("Could not allocate wide string.")
		return NULL;
	}

	if(MultiByteToWideChar(CP_UTF8, 0, string, -1, wideString, wideStringSize) != wideStringSize)
	{
		free(wideString);
		LOG("Could not convert string.")
		return NULL;
	}

	return wideString;
}
#endif

static Input readHttp(HINTERNET internetUrl)
{
	if(!internetUrl)
	{
		LOG("Invalid argument.")
		return errorInput;
	}

	DWORD bufferCapacity = 10;
	char* input = malloc(bufferCapacity * sizeof(input[0]));
	if(!input)
	{
		LOG("Could not allocate buffer to read file.")
		return errorInput;
	}

	DWORD toRead = bufferCapacity;
	DWORD read;
	DWORD inputLength = 0;
	while(true)
	{
		if(!InternetReadFile(internetUrl, input + inputLength, toRead, &read))
		{
			free(input);
			LOG("Could not read HTTP.")
			return errorInput;
		}

		if(read == 0)
		{
			if(!resize(&input, inputLength + 1))
			{
				free(input);
				LOG("Could not resize input.")
				return errorInput;
			}
			input[inputLength] = '\0';

			return (Input){input, inputLength};
		}

		inputLength += read;

		// Resize buffer
		bufferCapacity *= 2;
		if(!resize(&input, bufferCapacity))
		{
			free(input);
			LOG("Could not resize input.")
			return errorInput;
		}

		toRead = bufferCapacity / 2;
	}
}

static Input getHttp(const char* url)
{
	const char* const sessionCookie = getenv("aoc-session");
	if(!sessionCookie)
	{
		LOG("Session cookie is not set.")
		return errorInput;
	}

	char* const sessionCookieHeader = formatString("Cookie: session=%s", sessionCookie);
	if(!sessionCookieHeader)
	{
		LOG("Could not format session cookie header.")
		return errorInput;
	}

	const HINTERNET internet = InternetOpen(
		TEXT("aoc"),
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0
	);
	if(!internet)
	{
		free(sessionCookieHeader);
		LOG("Could not open internet.")
		return errorInput;
	}

#ifdef UNICODE
	WCHAR* const wideUrl = multiByteToWideChar(url);
	if(!wideUrl)
	{
		free(sessionCookieHeader);
		InternetCloseHandle(internet);
		LOG("Could not get wide URL.")
		return errorInput;
	}

	WCHAR* const wideSessionCookieHeader = multiByteToWideChar(sessionCookieHeader);
	if(!wideSessionCookieHeader)
	{
		free(sessionCookieHeader);
		free(wideUrl);
		LOG("Could not get wide session cookie header.");
		return errorInput;
	}
#endif

	const HINTERNET internetUrl = InternetOpenUrl(
		internet,
#ifdef UNICODE
		wideUrl,
		wideSessionCookieHeader,
#else
		url,
		sessionCookieHeader,
#endif
		(DWORD)-1,
		INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI,
		0
	);
#ifdef UNICODE
	free(wideUrl);
	free(wideSessionCookieHeader);
#endif
	free(sessionCookieHeader);
	if(!internetUrl)
	{
		InternetCloseHandle(internet);
		LOG("Could not open URL.");
		return errorInput;
	}

	Input input = readHttp(internetUrl);

	if(!InternetCloseHandle(internetUrl))
	{
		free(input.string);
		InternetCloseHandle(internet);
		LOG("Could not close URL.")
		return errorInput;
	}

	if(!InternetCloseHandle(internet))
	{
		free(input.string);
		LOG("Could not close internet.")
		return errorInput;
	}

	return input;
}
#endif

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
		LOG("Invalid argument.")
		return errorInput;
	}

	size_t bufferCapacity = 1024;
	char* input = malloc(bufferCapacity * sizeof(input[0]));
	if(!input)
	{
		LOG("Could not allocate buffer to read file.")
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
				LOG("Could not read file.")
				return errorInput;
			}

			if(!resize(&input, inputLength + 1))
			{
				free(input);
				LOG("Could not resize input.")
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
			LOG("Could not resize input.")
			return errorInput;
		}

		toRead = bufferCapacity / 2;
	}
}

Input getInput(unsigned year, unsigned day)
{
	if(year < 2015 || day < 1 || day > 25)
	{
		LOG("Invalid argument.")
		return errorInput;
	}

	// Create file name
	char* const fileName = formatString("%u-%02u.txt", year, day);
	if(!fileName)
	{
		LOG("Could not format file name.")
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

	char* const url = formatString("https://adventofcode.com/%u/day/%u/input", year, day);
	if(!url)
	{
		free(fileName);
		LOG("Could not format URL.")
		return errorInput;
	}

#ifdef _WIN32
	Input input = getHttp(url);
	free(url);
	if(!input.string)
	{
		free(fileName);
		LOG("Could not get input over HTTP.")
		return errorInput;
	}
#else
	char* const result = NULL;
#endif

	file = fopen(fileName, "w");
	free(fileName);
	if(!file)
	{
		LOG("Could not open file.")
		return input;
	}

	if(fprintf(file, "%s", input.string) <= 0)
	{
		LOG("Could not write input to file.")
	}

	fclose(file);

	return input;
}
