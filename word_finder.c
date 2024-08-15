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

static void addWordResult(DynamicWordArray *dwa, WordResult word) {
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
	
	for (int letter = 0; letter < 26; letter++) {
		if (!(node->children & (1U << letter))) continue;
		
		TrieNode *child = node->childPtrs[letter];
		char currentLetter = 'A' + letter;
		bool isSwap = (currentLetter != grid->letters[row * grid->size + col]);

		if (isSwap && remainingSwaps == 0) continue;

		currentWord[depth] = currentLetter;
		currentPositions[depth] = (Position){row, col};

		if (isSwap) {
			swapPositions[swapDepth] = (Position){row, col};
		}

		if (child->isWord && depth > 0) {
			currentWord[depth + 1] = '\0';
			WordResult wordResult = {
				.word = strdup(currentWord),
				.positions = malloc((depth + 1) * sizeof(Position)),
				.length = depth + 1,
				.score = calculateWordScore(currentWord, currentPositions, grid),
				.swapPositions = malloc(maxWordLength * sizeof(Position)),
				.numSwaps = isSwap ? swapDepth + 1 : swapDepth
			};
			memcpy(wordResult.positions, currentPositions, (depth + 1) * sizeof(Position));
			memcpy(wordResult.swapPositions, swapPositions, wordResult.numSwaps * sizeof(Position));
			addWordResult(words, wordResult);
		}

		const int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
									  {0, 1},   {1, -1}, {1, 0},  {1, 1}};
		for (int i = 0; i < 8; i++) {
			int newRow = row + directions[i][0];
			int newCol = col + directions[i][1];
			if (newRow >= 0 && newRow < grid->size && newCol >= 0 && newCol < grid->size) {
				dfs(grid, newRow, newCol, child, words, visited, currentWord,
					currentPositions, depth + 1, maxWordLength, 
					isSwap ? remainingSwaps - 1 : remainingSwaps,
					swapPositions, isSwap ? swapDepth + 1 : swapDepth);
			}
		}

		if (isSwap) {
			swapPositions[swapDepth] = (Position){-1, -1};  // Reset swap position
		}
	}

	visited[row * grid->size + col] = false;
}

DynamicWordArray findWords(const Grid *grid, TrieNode *trie, int maxWordLength, int maxSwaps) {
	DynamicWordArray words = initDynamicWordArray();
	bool *visited = calloc(grid->size * grid->size, sizeof(bool));
	char *currentWord = malloc((maxWordLength + 1) * sizeof(char));
	Position *currentPositions = malloc(maxWordLength * sizeof(Position));
	Position *swapPositions = malloc(maxWordLength * sizeof(Position));

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

	return words;
}