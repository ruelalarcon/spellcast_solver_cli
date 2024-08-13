#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>
#include "word_finder.h"
#include "grid.h"

/**
 * Outputs the results of the word finding and optimization process.
 *
 * @param bestResults Array of the best WordResults for each number of swaps.
 * @param maxSwaps The maximum number of swaps allowed.
 * @param grid The game grid.
 * @param useJson Whether to output in JSON format.
 */
void outputResults(const WordResult *bestResults, int maxSwaps, const Grid *grid, bool useJson);

#endif // OUTPUT_H