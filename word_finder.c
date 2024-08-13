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
		letterScore *= grid->letterMultiplier[row][col];
		baseScore += letterScore;
		wordMultiplier *= grid->wordMultiplier[row][col];
	}

	return baseScore * wordMultiplier;
}

static void dfs(const Grid *grid, int row, int col, TrieNode *node, DynamicWordArray *words,
				bool **visited, char *currentWord, Position *currentPositions, int depth, int maxWordLength) {
	if (depth >= maxWordLength || visited[row][col])
		return;

	char letter = grid->letters[row][col];
	TrieNode *child = node->children[letter - 'A'];
	if (!child)
		return;

	visited[row][col] = true;
	currentWord[depth] = letter;
	currentPositions[depth] = (Position){row, col};

	if (child->isWord && depth > 0) {
		currentWord[depth + 1] = '\0';
		WordResult wordResult = {
			.word = strdup(currentWord),
			.positions = malloc((depth + 1) * sizeof(Position)),
			.length = depth + 1,
			.score = calculateWordScore(currentWord, currentPositions, grid),
			.swapPositions = NULL,
			.numSwaps = 0
		};
		memcpy(wordResult.positions, currentPositions, (depth + 1) * sizeof(Position));
		addWordResult(words, wordResult);
	}

	const int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
								  {0, 1},   {1, -1}, {1, 0},  {1, 1}};
	for (int i = 0; i < 8; i++) {
		int newRow = row + directions[i][0];
		int newCol = col + directions[i][1];
		if (newRow >= 0 && newRow < grid->size && newCol >= 0 && newCol < grid->size) {
			dfs(grid, newRow, newCol, child, words, visited, currentWord,
				currentPositions, depth + 1, maxWordLength);
		}
	}

	visited[row][col] = false;
}

DynamicWordArray findWords(const Grid *grid, TrieNode *trie, int maxWordLength) {
	DynamicWordArray words = initDynamicWordArray();
	bool **visited = malloc(grid->size * sizeof(bool *));
	for (int i = 0; i < grid->size; i++) {
		visited[i] = calloc(grid->size, sizeof(bool));
	}
	char *currentWord = malloc((maxWordLength + 1) * sizeof(char));
	Position *currentPositions = malloc(maxWordLength * sizeof(Position));

	for (int r = 0; r < grid->size; r++) {
		for (int c = 0; c < grid->size; c++) {
			dfs(grid, r, c, trie, &words, visited, currentWord,
				currentPositions, 0, maxWordLength);
		}
	}

	for (int i = 0; i < grid->size; i++) {
		free(visited[i]);
	}
	free(visited);
	free(currentWord);
	free(currentPositions);

	return words;
}


static void findBestWordWithSwapsRecursive(char *word, const Position *originalPositions, TrieNode *trie,
										   const Grid *grid, WordResult *bestResults, int currentSwaps,
										   int maxSwaps, int maxWordLength, Position *currentSwapPositions) {
	TrieNode *node = trie;
	for (int i = 0; word[i]; i++) {
		node = node->children[word[i] - 'A'];
		if (!node)
			return; // Word not in dictionary
	}

	if (node->isWord) {
		unsigned short score = calculateWordScore(word, originalPositions, grid);
		if (score > bestResults[currentSwaps].score) {
			strncpy(bestResults[currentSwaps].word, word, maxWordLength);
			bestResults[currentSwaps].word[maxWordLength] = '\0';
			bestResults[currentSwaps].score = score;
			memcpy(bestResults[currentSwaps].positions, originalPositions, strlen(word) * sizeof(Position));
			bestResults[currentSwaps].length = strlen(word);
			bestResults[currentSwaps].numSwaps = currentSwaps;
			memcpy(bestResults[currentSwaps].swapPositions, currentSwapPositions, currentSwaps * sizeof(Position));
		}
	}

	if (currentSwaps >= maxSwaps)
		return;

	for (int i = 0; word[i]; i++) {
		char original = word[i];
		for (char newC = 'A'; newC <= 'Z'; newC++) {
			if (newC != original) {
				word[i] = newC;
				currentSwapPositions[currentSwaps] = originalPositions[i];
				findBestWordWithSwapsRecursive(word, originalPositions, trie, grid, bestResults,
											   currentSwaps + 1, maxSwaps, maxWordLength, currentSwapPositions);
			}
		}
		word[i] = original; // Restore original letter
	}
}

void findBestWordWithSwaps(const WordResult *word, TrieNode *trie, const Grid *grid,
						   WordResult *bestResults, int maxSwaps, int maxWordLength) {
	char *swappedWord = malloc((maxWordLength + 1) * sizeof(char));
	strncpy(swappedWord, word->word, maxWordLength);
	swappedWord[maxWordLength] = '\0';

	Position *currentSwapPositions = malloc(maxSwaps * sizeof(Position));

	findBestWordWithSwapsRecursive(swappedWord, word->positions, trie, grid, bestResults,
								   0, maxSwaps, maxWordLength, currentSwapPositions);

	free(swappedWord);
	free(currentSwapPositions);
}