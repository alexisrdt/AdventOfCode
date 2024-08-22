#include <stdio.h>
#include <stdlib.h>

#include "aoc.h"

int main(void)
{
	Input input = getInput(${YEAR}, ${DAY});
	if(!input.string)
	{
		fprintf(stderr, "Could not get input.\n");
		return EXIT_FAILURE;
	}

	free(input.string);

	return EXIT_SUCCESS;
}
