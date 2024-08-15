#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "grid.h"

#define MAX_LINE_LENGTH 1024

Grid* createGrid(int size) {
	Grid *grid = malloc(sizeof(Grid));
	grid->size = size;
	grid->letters = malloc(size * size * sizeof(char));
	grid->letterMultiplier = malloc(size * size * sizeof(int));
	grid->wordMultiplier = malloc(size * size * sizeof(int));
	for (int i = 0; i < size * size; i++) {
		grid->letterMultiplier[i] = 1;
		grid->wordMultiplier[i] = 1;
	}
	return grid;
}

void loadGrid(const char *filePath, Grid *grid) {
	FILE *file = fopen(filePath, "r");
	if (!file) {
		perror("Error opening grid file");
		exit(1);
	}

	char line[MAX_LINE_LENGTH];
	int row = 0;

	while (fgets(line, sizeof(line), file) && row < grid->size) {
		int col = 0;
		for (int i = 0; line[i] && col < grid->size; i++) {
			if (isalpha(line[i])) {
				grid->letters[row * grid->size + col] = toupper(line[i]);
				
				// Count total '*' and '^' characters
				int letterMultiplier = 0;
				int wordMultiplier = 0;
				while (line[i+1] == '*' || line[i+1] == '^') {
					if (line[i+1] == '*') letterMultiplier++;
					if (line[i+1] == '^') wordMultiplier++;
					i++;
				}
				
				grid->letterMultiplier[row * grid->size + col] = letterMultiplier + 1;
				grid->wordMultiplier[row * grid->size + col] = wordMultiplier + 1;
				
				col++;
			}
		}
		row++;
	}

	if (row < grid->size) {
		fprintf(stderr, "Error: Not enough rows in grid file\n");
		exit(1);
	}

	fclose(file);
}

void freeGrid(Grid *grid) {
	free(grid->letters);
	free(grid->letterMultiplier);
	free(grid->wordMultiplier);
	free(grid);
}