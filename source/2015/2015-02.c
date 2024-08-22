#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

int compareUlls(const void* pFirstVoid, const void* pSecondVoid)
{
	const unsigned long long* pFirst = pFirstVoid;
	const unsigned long long* pSecond = pSecondVoid;

	return (*pFirst > *pSecond) - (*pFirst < *pSecond);
}

#define LEN(x) \
(sizeof(x) / sizeof((x)[0]))

int main(void)
{
	Input input = getInput(2015, 2);
	if(!input.string)
	{
		fprintf(stderr, "Could not get input.\n");
		return EXIT_FAILURE;
	}

	const char* delimiter = "\n";
	char* token = strtok(input.string, delimiter);
	unsigned long long wrapSum = 0, ribbonSum = 0;
	while(token)
	{
		unsigned long long dims[3];
		if(sscanf(token, "%llux%llux%llu", &dims[0], &dims[1], &dims[2]) != 3)
		{
			free(input.string);
			fprintf(stderr, "Something went wrong trying to get the dimensions.\n");
			return EXIT_FAILURE;
		}
		qsort(dims, LEN(dims), sizeof(dims[0]), compareUlls);

		const unsigned long long a = dims[0] * dims[1];
		const unsigned long long b = dims[1] * dims[2];
		const unsigned long long c = dims[2] * dims[0];

		unsigned long long m = a < b ? a : b;
		m = m < c ? m : c;

		wrapSum += 2 * (a + b + c) + m;

		ribbonSum += dims[0] * dims[1] * dims[2] + 2 * (dims[0] + dims[1]);

		token = strtok(NULL, delimiter);
	}

	printf("Solution 1: %llu\n", wrapSum);
	printf("Solution 2: %llu\n", ribbonSum);

	free(input.string);

	printf("Exiting without problem.\n");
	return EXIT_SUCCESS;
}
