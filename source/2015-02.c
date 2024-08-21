#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

int main(void)
{
	Input input = getInput(2015, 2, "session-cookie-here");
	if(!input.string)
	{
		fprintf(stderr, "Could not get input.\n");
		return EXIT_FAILURE;
	}

	printf("My input:\n%s\n", input.string);

	const char* delimiter = "\n";
	char* token = strtok(input.string, delimiter);
	unsigned long long sum = 0;
	while(token)
	{
		unsigned long long l, w, h;
		if(sscanf(token, "%llux%llux%llu", &l, &w, &h) != 3)
		{
			free(input.string);
			fprintf(stderr, "Something went wrong trying to get the dimensions.\n");
			return EXIT_FAILURE;
		}

		const unsigned long long a = l * w;
		const unsigned long long b = w * h;
		const unsigned long long c = h * l;

		unsigned long long m = a < b ? a : b;
		m = m < c ? m : c;

		sum += 2 * (a + b + c) + m;

		token = strtok(NULL, delimiter);
	}

	printf("Solution 1: %llu\n", sum);

	free(input.string);

	printf("Exiting without problem.\n");
	return EXIT_SUCCESS;
}
