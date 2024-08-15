#ifndef WORD_FINDER_H
#define WORD_FINDER_H

#include "grid.h"
#include "trie.h"

typedef struct {
	char *word;
	unsigned short score;
	Position *positions;
	int length;
	Position *swapPositions;
	int numSwaps;
} WordResult;

typedef struct {
	WordResult *array;
	int size;
	int capacity;
} DynamicWordArray;

/**
 * Finds all valid words in the grid using depth-first search.
 *
 * @param grid The game grid.
 * @param trie The Trie containing the dictionary.
 * @param maxWordLength The maximum allowed word length.
 * @param maxSwaps The maximum amount of swaps.
 * @return A DynamicWordArray containing all found words.
 */
DynamicWordArray findWords(const Grid *grid, TrieNode *trie, int maxWordLength, int maxSwaps);

/**
 * Frees the memory allocated for the dynamic array of WordResults.
 *
 * This function frees all the memory associated with the DynamicWordArray,
 * including the memory for each WordResult it contains.
 *
 * @param dwa Pointer to the DynamicWordArray to be freed.
 */
void freeDynamicWordArray(DynamicWordArray *dwa);

/**
 * Frees the memory allocated for a single WordResult.
 *
 * This function frees all the dynamically allocated memory within a WordResult,
 * including the word, positions, and swapPositions.
 *
 * @param wr Pointer to the WordResult to be freed.
 */
void freeWordResult(WordResult *wr);

/**
 * Calculates the score for a given word based on its positions in the grid.
 *
 * @param word The word to calculate the score for.
 * @param positions The positions of the letters in the grid.
 * @param grid The game grid.
 * @return The calculated score for the word.
 */
unsigned short calculateWordScore(const char *word, const Position *positions, const Grid *grid);

#endif // WORD_FINDER_H