#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include "grid.h"
#include "trie.h"
#include "word_finder.h"

#define TEST(name) void test_##name()
#define RUN_TEST(name) printf("Running %s...\n", #name); test_##name(); printf("%s passed\n", #name)

char* create_temp_file(const char* content) {
	char template[] = "/tmp/gridtest_XXXXXX";
	int fd = mkstemp(template);
	if (fd == -1) {
		perror("Failed to create temporary file");
		exit(1);
	}

	FILE* temp_file = fdopen(fd, "w");
	if (temp_file == NULL) {
		perror("Failed to open temporary file");
		close(fd);
		exit(1);
	}

	fprintf(temp_file, "%s", content);
	fclose(temp_file);

	return strdup(template);
}

TEST(grid_creation) {
	Grid *grid = createGrid(5);
	assert(grid != NULL);
	assert(grid->size == 5);
	assert(grid->letters != NULL);
	assert(grid->letterMultiplier != NULL);
	assert(grid->wordMultiplier != NULL);
	freeGrid(grid);
}

TEST(grid_loading) {
	// Test case 1: Basic grid with single special characters
	{
		Grid *grid = createGrid(5);
		char* temp_filename = create_temp_file("ABCDE\nFGHIJ\nKLM*NO\nPQR^ST\nUVWXY\n");

		loadGrid(temp_filename, grid);
		assert(grid->letters[2 * grid->size + 2] == 'M');
		assert(grid->letterMultiplier[2 * grid->size + 2] == 2);
		assert(grid->wordMultiplier[2 * grid->size + 2] == 1);
		assert(grid->letters[3 * grid->size + 2] == 'R');
		assert(grid->letterMultiplier[3 * grid->size + 2] == 1);
		assert(grid->wordMultiplier[3 * grid->size + 2] == 2);

		freeGrid(grid);
		unlink(temp_filename);
		free(temp_filename);
	}

	// Test case 2: Grid with combined special characters
	{
		Grid *grid = createGrid(5);
		char* temp_filename = create_temp_file("ABCDE\nFGHIJ\nKLM*^NO\nPQRST\nUVWXY\n");

		loadGrid(temp_filename, grid);
		assert(grid->letters[2 * grid->size + 2] == 'M');
		assert(grid->letterMultiplier[2 * grid->size + 2] == 2);
		assert(grid->wordMultiplier[2 * grid->size + 2] == 2);

		freeGrid(grid);
		unlink(temp_filename);
		free(temp_filename);
	}

	// Test case 3: Grid with multiple special characters
	{
		Grid *grid = createGrid(5);
		char* temp_filename = create_temp_file("ABCDE\nFGH**IJ\nKL^^^MNO\nPQR**^ST\nUVWXY\n");

		loadGrid(temp_filename, grid);
		assert(grid->letters[1 * grid->size + 2] == 'H');
		assert(grid->letterMultiplier[1 * grid->size + 2] == 3);
		assert(grid->wordMultiplier[1 * grid->size + 2] == 1);
		assert(grid->letters[2 * grid->size + 1] == 'L');
		assert(grid->letterMultiplier[2 * grid->size + 1] == 1);
		assert(grid->wordMultiplier[2 * grid->size + 1] == 4);
		assert(grid->letters[3 * grid->size + 2] == 'R');
		assert(grid->letterMultiplier[3 * grid->size + 2] == 3);
		assert(grid->wordMultiplier[3 * grid->size + 2] == 2);

		freeGrid(grid);
		unlink(temp_filename);
		free(temp_filename);
	}

	// Test case 4: Grid with interleaved special characters
	{
		Grid *grid = createGrid(5);
		char* temp_filename = create_temp_file("ABCDE\nFGHIJ\nKLM*^*^NO\nPQRST\nUVWXY\n");

		loadGrid(temp_filename, grid);
		assert(grid->letters[2 * grid->size + 2] == 'M');
		assert(grid->letterMultiplier[2 * grid->size + 2] == 3);
		assert(grid->wordMultiplier[2 * grid->size + 2] == 3);

		freeGrid(grid);
		unlink(temp_filename);
		free(temp_filename);
	}

	// Test case 5: Grid with special characters at the end of a line
	{
		Grid *grid = createGrid(5);
		char* temp_filename = create_temp_file("ABCDE*\nFGHIJ^\nKLMNO\nPQRST\nUVWXY\n");

		loadGrid(temp_filename, grid);
		assert(grid->letters[0 * grid->size + 4] == 'E');
		assert(grid->letterMultiplier[0 * grid->size + 4] == 2);
		assert(grid->wordMultiplier[0 * grid->size + 4] == 1);
		assert(grid->letters[1 * grid->size + 4] == 'J');
		assert(grid->letterMultiplier[1 * grid->size + 4] == 1);
		assert(grid->wordMultiplier[1 * grid->size + 4] == 2);

		freeGrid(grid);
		unlink(temp_filename);
		free(temp_filename);
	}
}

TEST(trie_operations) {
	TrieNode *root = createNode();
	insertWord(root, "HELLO", 5);
	insertWord(root, "WORLD", 5);

	assert(root->children & (1U << ('H' - 'A')));
	TrieNode *node = root->childPtrs['H' - 'A'];
	assert(node->children & (1U << ('E' - 'A')));
	node = node->childPtrs['E' - 'A'];
	assert(node->children & (1U << ('L' - 'A')));
	node = node->childPtrs['L' - 'A'];
	assert(node->children & (1U << ('L' - 'A')));
	node = node->childPtrs['L' - 'A'];
	assert(node->children & (1U << ('O' - 'A')));
	node = node->childPtrs['O' - 'A'];
	assert(node->isWord);

	freeTrie(root);
}

TEST(word_finding) {
	Grid *grid = createGrid(3);
	// Create a simple 3x3 grid
	char gridLetters[9] = {'C', 'A', 'T', 'D', 'O', 'G', 'R', 'A', 'T'};
	memcpy(grid->letters, gridLetters, 9 * sizeof(char));
	for (int i = 0; i < 9; i++) {
		grid->letterMultiplier[i] = 1;
		grid->wordMultiplier[i] = 1;
	}

	TrieNode *trie = createNode();
	insertWord(trie, "CAT", 3);
	insertWord(trie, "DOG", 3);
	insertWord(trie, "RAT", 3);

	DynamicWordArray words = findWords(grid, trie, 3, 0);  // 0 swaps allowed
	assert(words.size == 3);

	bool foundCat = false, foundDog = false, foundRat = false;
	for (int i = 0; i < words.size; i++) {
		if (strcmp(words.array[i].word, "CAT") == 0) foundCat = true;
		if (strcmp(words.array[i].word, "DOG") == 0) foundDog = true;
		if (strcmp(words.array[i].word, "RAT") == 0) foundRat = true;
	}
	assert(foundCat && foundDog && foundRat);

	freeDynamicWordArray(&words);
	freeTrie(trie);
	freeGrid(grid);
}

TEST(score_calculation) {
	Grid *grid = createGrid(5);
	// Create a 5x5 grid with various multipliers
	char gridLetters[25] = {
		'C', 'A', 'T', 'S', 'P',
		'D', 'O', 'G', 'E', 'L',
		'R', 'A', 'T', 'E', 'A',
		'F', 'I', 'S', 'H', 'Y',
		'Q', 'U', 'I', 'Z', 'Z'
	};
	memcpy(grid->letters, gridLetters, 25 * sizeof(char));
	for (int i = 0; i < 25; i++) {
		grid->letterMultiplier[i] = 1;
		grid->wordMultiplier[i] = 1;
	}

	// Set up some multipliers
	grid->letterMultiplier[2] = 2;    // Double letter score on 'T' in CAT
	grid->wordMultiplier[12] = 2;     // Double word score on 'T' in RAT
	grid->letterMultiplier[18] = 3;   // Triple letter score on 'H' in FISH
	grid->wordMultiplier[23] = 3;     // Triple word score on 'Z' in QUIZ
	grid->letterMultiplier[23] = 4;   // Quadruple letter score on 'Z' in QUIZ

	// Test case 1: CAT
	WordResult result1 = {
		.word = strdup("CAT"),
		.positions = malloc(3 * sizeof(Position)),
		.length = 3,
		.score = 0,
		.swapPositions = NULL,
		.numSwaps = 0
	};
	result1.positions[0] = (Position){0, 0};
	result1.positions[1] = (Position){0, 1};
	result1.positions[2] = (Position){0, 2};

	result1.score = calculateWordScore(result1.word, result1.positions, grid);
	// Expected score: (5 + 1 + 2*2) = 10
	assert(result1.score == 10);
	freeWordResult(&result1);

	// Test case 2: RAT (with double word score)
	WordResult result2 = {
		.word = strdup("RAT"),
		.positions = malloc(3 * sizeof(Position)),
		.length = 3,
		.score = 0,
		.swapPositions = NULL,
		.numSwaps = 0
	};
	result2.positions[0] = (Position){2, 0};
	result2.positions[1] = (Position){2, 1};
	result2.positions[2] = (Position){2, 2};

	result2.score = calculateWordScore(result2.word, result2.positions, grid);
	// Expected score: (2 + 1 + 2) * 2 = 10
	assert(result2.score == 10);
	freeWordResult(&result2);

	// Test case 3: FISH (with triple letter score)
	WordResult result3 = {
		.word = strdup("FISH"),
		.positions = malloc(4 * sizeof(Position)),
		.length = 4,
		.score = 0,
		.swapPositions = NULL,
		.numSwaps = 0
	};
	result3.positions[0] = (Position){3, 0};
	result3.positions[1] = (Position){3, 1};
	result3.positions[2] = (Position){3, 2};
	result3.positions[3] = (Position){3, 3};

	result3.score = calculateWordScore(result3.word, result3.positions, grid);
	// Expected score: 5 + 1 + 2 + (3 * 4) = 20
	assert(result3.score == 20);
	freeWordResult(&result3);

	// Test case 4: QUIZ (with triple word score and quadruple letter score)
	WordResult result4 = {
		.word = strdup("QUIZ"),
		.positions = malloc(4 * sizeof(Position)),
		.length = 4,
		.score = 0,
		.swapPositions = NULL,
		.numSwaps = 0
	};
	result4.positions[0] = (Position){4, 0};
	result4.positions[1] = (Position){4, 1};
	result4.positions[2] = (Position){4, 2};
	result4.positions[3] = (Position){4, 3};

	result4.score = calculateWordScore(result4.word, result4.positions, grid);
	// Expected score: (8 + 4 + 1 + (4 * 8)) * 3 = 135
	assert(result4.score == 135);
	freeWordResult(&result4);

	// Test case 5: LONGWORD (testing the long word bonus)
	WordResult result5 = {
		.word = strdup("LONGWORD"),
		.positions = malloc(8 * sizeof(Position)),
		.length = 8,
		.score = 0,
		.swapPositions = NULL,
		.numSwaps = 0
	};
	for (int i = 0; i < 8; i++) {
		result5.positions[i] = (Position){i % 5, i / 5};
	}

	result5.score = calculateWordScore(result5.word, result5.positions, grid);
	// Expected score: (3 + 1 + 2 + 3 + 5 + 1 + 2 + 3) + 10 (long word bonus) = 30
	assert(result5.score == 30);
	freeWordResult(&result5);

	freeGrid(grid);
}

TEST(specific_word_finding) {
	// Test dictionary
	const char* test_words[] = {
		"DOGFISH", "JACKFISH", "JACKSCREW",
		"XYST", "CHINTZY", "TOUCHBACK",
		"DOG", "FISH", "JACK", "SCREW", // Similar but lesser words
		"CHIN", "TOUCH", "BACK", "TINT" // Similar but lesser words
	};
	int num_words = sizeof(test_words) / sizeof(test_words[0]);

	TrieNode *trie = createNode();
	for (int i = 0; i < num_words; i++) {
		insertWord(trie, test_words[i], strlen(test_words[i]));
	}

	// Test Grid 1
	{
		Grid *grid = createGrid(5);
		char gridLetters[25] = {
			'Q', 'W', 'E', 'R', 'T',
			'Y', 'U', 'I', 'O', 'P',
			'A', 'S', 'D', 'F', 'G',
			'H', 'J', 'K', 'L', 'Z',
			'X', 'C', 'V', 'B', 'N'
		};
		memcpy(grid->letters, gridLetters, 25 * sizeof(char));
		for (int i = 0; i < 25; i++) {
			grid->letterMultiplier[i] = 1;
			grid->wordMultiplier[i] = 1;
		}

		WordResult bestResults[3] = {0};  // For 0, 1, and 2 swaps
		for (int swaps = 0; swaps <= 2; swaps++) {
			DynamicWordArray words = findWords(grid, trie, 9, swaps);  // Max word length is 9
			for (int i = 0; i < words.size; i++) {
				if (words.array[i].score > bestResults[swaps].score) {
					freeWordResult(&bestResults[swaps]);
					bestResults[swaps] = words.array[i];
					words.array[i].word = NULL;  // Prevent double free
					words.array[i].positions = NULL;
					words.array[i].swapPositions = NULL;
				}
			}
			freeDynamicWordArray(&words);
		}

		assert(strcmp(bestResults[0].word, "DOGFISH") == 0);
		assert(strcmp(bestResults[1].word, "JACKFISH") == 0);
		assert(strcmp(bestResults[2].word, "JACKSCREW") == 0);

		for (int i = 0; i < 3; i++) {
			freeWordResult(&bestResults[i]);
		}
		freeGrid(grid);
	}

	// Test Grid 2
	{
		Grid *grid = createGrid(5);
		char gridLetters[25] = {
			'A', 'B', 'C', 'D', 'E',
			'F', 'G', 'H', 'I', 'J',
			'K', 'L', 'M', 'N', 'O',
			'P', 'Q', 'R', 'S', 'T',
			'U', 'V', 'W', 'X', 'Y'
		};
		memcpy(grid->letters, gridLetters, 25 * sizeof(char));
		for (int i = 0; i < 25; i++) {
			grid->letterMultiplier[i] = 1;
			grid->wordMultiplier[i] = 1;
		}

		WordResult bestResults[3] = {0};  // For 0, 1, and 2 swaps
		for (int swaps = 0; swaps <= 2; swaps++) {
			DynamicWordArray words = findWords(grid, trie, 9, swaps);  // Max word length is 9
			for (int i = 0; i < words.size; i++) {
				if (words.array[i].score > bestResults[swaps].score) {
					freeWordResult(&bestResults[swaps]);
					bestResults[swaps] = words.array[i];
					words.array[i].word = NULL;  // Prevent double free
					words.array[i].positions = NULL;
					words.array[i].swapPositions = NULL;
				}
			}
			freeDynamicWordArray(&words);
		}

		assert(strcmp(bestResults[0].word, "XYST") == 0);
		assert(strcmp(bestResults[1].word, "CHINTZY") == 0);
		assert(strcmp(bestResults[2].word, "TOUCHBACK") == 0);

		for (int i = 0; i < 3; i++) {
			freeWordResult(&bestResults[i]);
		}
		freeGrid(grid);
	}

	freeTrie(trie);
}

int main() {
	RUN_TEST(grid_creation);
	RUN_TEST(grid_loading);
	RUN_TEST(trie_operations);
	RUN_TEST(word_finding);
	RUN_TEST(score_calculation);
	RUN_TEST(specific_word_finding);
	printf("All tests passed!\n");
	return 0;
}