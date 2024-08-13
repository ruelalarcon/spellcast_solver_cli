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
 * @return A DynamicWordArray containing all found words.
 */
DynamicWordArray findWords(const Grid *grid, TrieNode *trie, int maxWordLength);

/**
 * Finds the best word with swaps for a given initial word.
 *
 * @param word The initial WordResult to start from.
 * @param trie The Trie containing the dictionary.
 * @param grid The game grid.
 * @param bestResults Array to store the best results for each number of swaps.
 * @param maxSwaps The maximum number of swaps allowed.
 * @param maxWordLength The maximum allowed word length.
 */
void findBestWordWithSwaps(const WordResult *word, TrieNode *trie, const Grid *grid,
						   WordResult *bestResults, int maxSwaps, int maxWordLength);

/**
 * Frees the memory allocated for the dynamic array.
 *
 * @param dwa Pointer to the DynamicWordArray.
 */
void freeDynamicWordArray(DynamicWordArray *dwa);

#endif // WORD_FINDER_H