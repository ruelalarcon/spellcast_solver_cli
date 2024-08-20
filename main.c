#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"
#include "trie.h"
#include "word_finder.h"
#include "output.h"

#define DEFAULT_MAX_WORD_LENGTH 14
#define DEFAULT_MAX_SWAPS 2
#define DEFAULT_GRID_SIZE 5
#define DEFAULT_DICT_FILE "resources/dictionary.txt"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <grid_file> [options]\n", argv[0]);
		fprintf(stderr, "Options:\n");
		fprintf(stderr, "  --maxwordlength <value>  Maximum word length (default: 14)\n");
		fprintf(stderr, "  --maxswaps <value>       Maximum number of swaps (default: 2)\n");
		fprintf(stderr, "  --gridsize <value>       Grid size (default: 5)\n");
		fprintf(stderr, "  --dict <file>            Dictionary file path (default: resources/dictionary.txt)\n");
		fprintf(stderr, "  --json <true/false>      Output in JSON format (default: false)\n");
		return 1;
	}

	char *gridFile = argv[1];
	int maxWordLength = DEFAULT_MAX_WORD_LENGTH;
	int maxSwaps = DEFAULT_MAX_SWAPS;
	int gridSize = DEFAULT_GRID_SIZE;
	char *dictFile = DEFAULT_DICT_FILE;
	bool useJson = false;

	int opt;
	static struct option longOptions[] = {
		{"maxwordlength", required_argument, 0, 'w'},
		{"maxswaps", required_argument, 0, 's'},
		{"gridsize", required_argument, 0, 'g'},
		{"dict", required_argument, 0, 'd'},
		{"json", required_argument, 0, 'j'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "w:s:g:d:j:", longOptions, NULL)) != -1) {
		switch (opt) {
			case 'w': maxWordLength = atoi(optarg); break;
			case 's': maxSwaps = atoi(optarg); break;
			case 'g': gridSize = atoi(optarg); break;
			case 'd': dictFile = optarg; break;
			case 'j': useJson = (strcmp(optarg, "true") == 0); break;
			default: fprintf(stderr, "Invalid option\n"); return 1;
		}
	}

	Grid *grid = createGrid(gridSize);
	loadGrid(gridFile, grid);

	TrieNode *trie = loadDictionary(dictFile, maxWordLength);

	DynamicWordArray words = findWords(grid, trie, maxWordLength, maxSwaps);

	// Find best words for each number of swaps
	WordResult *bestResults = calloc(maxSwaps + 1, sizeof(WordResult));
	for (int i = 0; i <= maxSwaps; i++) {
		bestResults[i].word = NULL;
		bestResults[i].positions = NULL;
		bestResults[i].swapPositions = NULL;
		bestResults[i].score = 0;
	}

	for (int i = 0; i < words.size; i++) {
		int swaps = words.array[i].numSwaps;
		if (swaps <= maxSwaps && words.array[i].score > bestResults[swaps].score) {
			if (bestResults[swaps].word != NULL) {
				freeWordResult(&bestResults[swaps]);
			}
			bestResults[swaps] = words.array[i];

			// Create deep copy of the word result
			bestResults[swaps].word = strdup(words.array[i].word);
			bestResults[swaps].positions = malloc(words.array[i].length * sizeof(Position));
			memcpy(bestResults[swaps].positions, words.array[i].positions, words.array[i].length * sizeof(Position));
			bestResults[swaps].swapPositions = malloc(words.array[i].numSwaps * sizeof(Position));
			memcpy(bestResults[swaps].swapPositions, words.array[i].swapPositions, words.array[i].numSwaps * sizeof(Position));
		}
	}

	outputResults(bestResults, maxSwaps, grid, useJson);

	// Free allocated memory
	freeDynamicWordArray(&words);
	for (int i = 0; i <= maxSwaps; i++) {
		if (bestResults[i].word != NULL) {
			freeWordResult(&bestResults[i]);
		}
	}
	free(bestResults);
	freeTrie(trie);
	freeGrid(grid);

	return 0;
}