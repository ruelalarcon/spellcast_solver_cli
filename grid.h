#ifndef GRID_H
#define GRID_H

#include <stdbool.h>

typedef struct {
	int row;
	int col;
} Position;

typedef struct {
	char **letters;
	int **letterMultiplier;
	int **wordMultiplier;
	int size;
} Grid;

/**
 * Creates a new Grid structure.
 *
 * @param size The size of the grid (assuming it's square).
 * @return A pointer to the newly created Grid structure.
 */
Grid* createGrid(int size);

/**
 * Loads the game grid from a file.
 *
 * @param filePath The path to the grid file.
 * @param grid Pointer to the Grid structure to store the loaded grid.
 */
void loadGrid(const char *filePath, Grid *grid);

/**
 * Frees the memory allocated for the Grid structure.
 *
 * @param grid Pointer to the Grid structure to be freed.
 */
void freeGrid(Grid *grid);

#endif // GRID_H