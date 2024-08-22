#ifndef AOC_H
#define AOC_H

#include <stddef.h>

/*
 * An Advent of Code Input.
 *
 * Fields:
 * - string: A null-terminated string representing the input.
 * - length: The length of the input.
 */
typedef struct Input
{
	char* const string;
	const size_t length;
} Input;

/*
 * Retrieve the input for an Advent of Code problem.
 * If a file "<year>-<day>.txt" exists, the input is read from that file.
 * Otherwise, the input is fetched from adventofcode.com and saved to such a file.
 *
 * Parameters:
 * - year: The year of the problem.
 * - day: The day of the problem.
 * 
 * Returns:
 * - {NULL, 0} if something went wrong.
 * - The problem input if everything went correctly.
 */
Input getInput(unsigned year, unsigned day);

#endif
