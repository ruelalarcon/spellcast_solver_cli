#include <stdio.h>
#include "output.h"

static void printGridWithHighlights(const Grid *grid, const Position *positions, int wordLength,
									const Position *swapPositions, int numSwaps, const char *word) {
	for (int i = 0; i < grid->size; i++) {
		for (int j = 0; j < grid->size; j++) {
			bool isWordPos = false;
			bool isSwapPos = false;
			int wordIndex = -1;

			for (int k = 0; k < wordLength; k++) {
				if (positions[k].row == i && positions[k].col == j) {
					isWordPos = true;
					wordIndex = k;
					break;
				}
			}

			for (int k = 0; k < numSwaps; k++) {
				if (swapPositions[k].row == i && swapPositions[k].col == j) {
					isSwapPos = true;
					break;
				}
			}

			if (isSwapPos) {
				printf("\033[34m%c\033[0m ", word[wordIndex]);
			} else if (isWordPos) {
				printf("\033[32m%c\033[0m ", word[wordIndex]);
			} else {
				printf("%c ", grid->letters[i][j]);
			}
		}
		printf("\n");
	}
}

void outputResults(const WordResult *bestResults, int maxSwaps, const Grid *grid, bool useJson) {
	if (useJson) {
		printf("[\n");
		for (int i = 0; i <= maxSwaps; i++) {
			if (i > 0) printf(",\n");
			printf("  {\n");
			printf("    \"swaps\": %d,\n", i);
			printf("    \"word\": \"%s\",\n", bestResults[i].word);
			printf("    \"score\": %d,\n", bestResults[i].score);
			printf("    \"positions\": [");
			for (int j = 0; j < bestResults[i].length; j++) {
				if (j > 0) printf(", ");
				printf("[%d, %d]", bestResults[i].positions[j].row, bestResults[i].positions[j].col);
			}
			printf("],\n");
			printf("    \"swap_positions\": [");
			for (int j = 0; j < i; j++) {
				if (j > 0) printf(", ");
				printf("[%d, %d]", bestResults[i].swapPositions[j].row, bestResults[i].swapPositions[j].col);
			}
			printf("]\n  }");
		}
		printf("\n]\n");
	} else {
		for (int i = 0; i <= maxSwaps; i++) {
			if (i > 0) printf("\n");
			printf("For %d swap%s:\n", i, i == 1 ? "" : "s");
			printf("Word: %s\n", bestResults[i].word);
			printf("Score: %d\n", bestResults[i].score);
			printGridWithHighlights(grid, bestResults[i].positions, bestResults[i].length,
									bestResults[i].swapPositions, i, bestResults[i].word);
		}
	}
}