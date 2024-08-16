#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "word_finder.h"

#define INITIAL_CAPACITY 128

const unsigned char SCORES[26] = {1, 4, 5, 3, 1, 5, 3, 4, 1, 7, 6, 3, 4, 2, 1, 1, 8, 2, 2, 2, 4, 5, 5, 7, 4, 8};

static DynamicWordArray initDynamicWordArray() {
	DynamicWordArray dwa;
	dwa.array = malloc(INITIAL_CAPACITY * sizeof(WordResult));
	if (!dwa.array) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}
	dwa.size = 0;
	dwa.capacity = INITIAL_CAPACITY;
	return dwa;
}

static WordResult createWordResult(const char *word, const Position *positions, int length, const Grid *grid, const Position *swapPositions, int numSwaps) {
	WordResult wordResult = {
		.word = strdup(word),
		.positions = malloc(length * sizeof(Position)),
		.length = length,
		.score = calculateWordScore(word, positions, grid),
		.swapPositions = malloc(numSwaps * sizeof(Position)),
		.numSwaps = numSwaps
	};
	memcpy(wordResult.positions, positions, length * sizeof(Position));
	memcpy(wordResult.swapPositions, swapPositions, numSwaps * sizeof(Position));
	return wordResult;
}

static void addWordResult(DynamicWordArray *dwa, WordResult word) {
	#pragma omp critical
	{
		if (dwa->size == dwa->capacity) {
			dwa->capacity *= 2;
			WordResult *temp = realloc(dwa->array, dwa->capacity * sizeof(WordResult));
			if (!temp) {
				fprintf(stderr, "Memory reallocation failed\n");
				exit(1);
			}
			dwa->array = temp;
		}
		dwa->array[dwa->size++] = word;
	}
}

void freeDynamicWordArray(DynamicWordArray *dwa) {
	for (int i = 0; i < dwa->size; i++) {
		freeWordResult(&dwa->array[i]);
	}
	free(dwa->array);
	dwa->array = NULL;
	dwa->size = 0;
	dwa->capacity = 0;
}

void freeWordResult(WordResult *wr) {
	free(wr->word);
	free(wr->positions);
	free(wr->swapPositions);
}

unsigned short calculateWordScore(const char *word, const Position *positions, const Grid *grid) {
	unsigned short baseScore = 0;
	unsigned short wordMultiplier = 1;

	for (int i = 0; word[i]; i++) {
		int row = positions[i].row;
		int col = positions[i].col;
		unsigned char letterScore = SCORES[word[i] - 'A'];
		letterScore *= grid->letterMultiplier[row * grid->size + col];
		baseScore += letterScore;
		wordMultiplier *= grid->wordMultiplier[row * grid->size + col];
	}

	int longWordBonus = (strlen(word) > 6) * 10;

	return baseScore * wordMultiplier + longWordBonus;
}

static void dfs(const Grid *grid, int row, int col, TrieNode *node, DynamicWordArray *words,
				bool *visited, char *currentWord, Position *currentPositions, int depth,
				int maxWordLength, int remainingSwaps, Position *swapPositions, int swapDepth) {
	if (depth >= maxWordLength || visited[row * grid->size + col])
		return;

	visited[row * grid->size + col] = true;
	
	unsigned int children = node->children;
	char gridLetter = grid->letters[row * grid->size + col];
	bool isGridLetterValid = (children & (1U << (gridLetter - 'A')));

	if (isGridLetterValid) {
		TrieNode *child = node->childPtrs[gridLetter - 'A'];
		currentWord[depth] = gridLetter;
		currentPositions[depth] = (Position){row, col};

		if (child->isWord && depth > 0) {
			currentWord[depth + 1] = '\0';
			addWordResult(words, createWordResult(currentWord, currentPositions, depth + 1, grid, swapPositions, swapDepth));
		}

		const int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
									  {0, 1},   {1, -1}, {1, 0},  {1, 1}};
		for (int i = 0; i < 8; i++) {
			int newRow = row + directions[i][0];
			int newCol = col + directions[i][1];
			if (newRow >= 0 && newRow < grid->size && newCol >= 0 && newCol < grid->size) {
				dfs(grid, newRow, newCol, child, words, visited, currentWord,
					currentPositions, depth + 1, maxWordLength, remainingSwaps,
					swapPositions, swapDepth);
			}
		}
	}

	if (remainingSwaps > 0) {
		while (children) {
			int letter = __builtin_ctz(children);
			char currentLetter = 'A' + letter;
			
			if (currentLetter != gridLetter) {
				TrieNode *child = node->childPtrs[letter];
				currentWord[depth] = currentLetter;
				currentPositions[depth] = (Position){row, col};
				swapPositions[swapDepth] = (Position){row, col};

				if (child->isWord && depth > 0) {
					currentWord[depth + 1] = '\0';
					addWordResult(words, createWordResult(currentWord, currentPositions, depth + 1, grid, swapPositions, swapDepth + 1));
				}

				const int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
											  {0, 1},   {1, -1}, {1, 0},  {1, 1}};
				for (int i = 0; i < 8; i++) {
					int newRow = row + directions[i][0];
					int newCol = col + directions[i][1];
					if (newRow >= 0 && newRow < grid->size && newCol >= 0 && newCol < grid->size) {
						dfs(grid, newRow, newCol, child, words, visited, currentWord,
							currentPositions, depth + 1, maxWordLength, remainingSwaps - 1,
							swapPositions, swapDepth + 1);
					}
				}
			}
			
			children &= (children - 1);
		}
	}

	visited[row * grid->size + col] = false;
}

DynamicWordArray findWords(const Grid *grid, TrieNode *trie, int maxWordLength, int maxSwaps) {
	DynamicWordArray words = initDynamicWordArray();

	#pragma omp parallel
	{
		bool *visited = calloc(grid->size * grid->size, sizeof(bool));
		char *currentWord = malloc((maxWordLength + 1) * sizeof(char));
		Position *currentPositions = malloc(maxWordLength * sizeof(Position));
		Position *swapPositions = malloc(maxWordLength * sizeof(Position));

		#pragma omp for collapse(2)
		for (int r = 0; r < grid->size; r++) {
			for (int c = 0; c < grid->size; c++) {
				dfs(grid, r, c, trie, &words, visited, currentWord,
					currentPositions, 0, maxWordLength, maxSwaps, swapPositions, 0);
			}
		}

		free(visited);
		free(currentWord);
		free(currentPositions);
		free(swapPositions);
	}

	return words;
}