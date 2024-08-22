#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

int main(void)
{
	Input input = getInput(2015, 1);
	if(!input.string)
	{
		fprintf(stderr, "Could not get input.\n");
		return EXIT_FAILURE;
	}

	printf("My input:\n%s\n", input.string);

	long long sum = 0;
	for(size_t i = 0; i < input.length; ++i)
	{
		if(input.string[i] == '(')
		{
			++sum;
		}
		else if(input.string[i] == ')')
		{
			--sum;
		}
	}

	printf("Solution 1: %lld\n", sum);

	sum = 0;
	for(size_t i = 0; i < input.length; ++i)
	{
		if(input.string[i] == '(')
		{
			++sum;
		}
		else if(input.string[i] == ')')
		{
			--sum;
		}

		if(sum == -1)
		{
			printf("Solution 2: %zu\n", i + 1);
			break;
		}
	}
	free(input.string);
	if(sum != -1)
	{
		fprintf(stderr, "Problem: no solution found for part two.\n");
		return EXIT_FAILURE;
	}

	printf("Exiting without problem.\n");
	return EXIT_SUCCESS;
}
