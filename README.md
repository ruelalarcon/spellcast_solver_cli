# spellcast_solver_cli

> A high-performance C-based CLI tool for solving the Discord game Spellcast, optimizing word choices based on letter scores and special tiles. Compatible with double/triple (and more than triple) letter/word scores, custom grid sizes, etc.

## Installation

To use spellcast_solver_cli, you need to have a C compiler (gcc) installed on your system. Clone this repository and compile the project:

```bash
git clone https://github.com/ruelalarcon/spellcast_solver_cli.git
cd spellcast_solver_cli
make
```

## Usage

The basic usage of spellcast_solver_cli is as follows:

```bash
./spellcast_solver <grid_file> [options]
```

- `<grid_file>`: Path to the input grid file.

### Optional Arguments

You can customize the solver's behavior using the following optional arguments:

```bash
--maxwordlength <value>  Maximum word length (default: 14)
--maxswaps <value>       Maximum number of swaps (default: 2)
--gridsize <value>       Grid size (default: 5)
--dict <file>            Dictionary file path (default: resources/dictionary.txt)
--json <true/false>      Output in JSON format (default: false)
```

### Usage Examples

Solve a Spellcast grid with default settings:

```bash
./spellcast_solver grid.txt
```

Solve a grid with custom parameters and JSON output:

```bash
./spellcast_solver grid.txt --maxwordlength 10 --maxswaps 3 --json true
```

## Input Format

The input for this solver is a text file representing the Spellcast grid. The file should follow this format:

- One line per row of the grid
- Letters are uppercase
- Special tiles are indicated with `*` for double letter, `^` for double word immediately following the letter
- Multiple special characters can be used for higher multipliers (e.g., `**` for triple letter, `^^` for triple word)

### Grid File Example

```
ABCDE
FGH*IJ
KLM^NO
PQRST
UVWXY
```

In this example:
- The 'H' is a double letter tile
- The 'M' is a double word tile

You can also combine and stack special characters:

```
ABCDE
FGH***IJ
KLM*^NO
PQR^^ST
UVWXY
```

Here:
- The 'H' is a quadruple letter tile
- The 'M' is both a double letter and double word tile
- The 'R' is a triple word tile

## Parallelization with OpenMP

This solver utilizes OpenMP for parallelization, improving performance on multi-core systems.

You can control the number of threads used by the solver using the `OMP_NUM_THREADS` environment variable. Here are some examples:

Set a specific number of threads:
```bash
export OMP_NUM_THREADS=4
./spellcast_solver grid.txt
```

Use all available cores:
```bash
export OMP_NUM_THREADS=$(nproc)
./spellcast_solver grid.txt
```

Set threads for a single run:
```bash
OMP_NUM_THREADS=8 ./spellcast_solver grid.txt
```

## Output Examples

### JSON Output

When using the `--json true` option, the output will be in JSON format:

```json
[
  {
    "swaps": 0,
    "word": "XYST",
    "score": 15,
    "positions": [[4, 3], [4, 4], [3, 3], [3, 4]],
    "swap_positions": []
  },
  {
    "swaps": 1,
    "word": "CHINTZY",
    "score": 36,
    "positions": [[0, 2], [1, 2], [1, 3], [2, 3], [3, 4], [3, 3], [4, 4]],
    "swap_positions": [[3, 3]]
  },
  {
    "swaps": 2,
    "word": "TOUCHBACK",
    "score": 42,
    "positions": [[3, 4], [2, 4], [1, 3], [0, 2], [1, 2], [0, 1], [0, 0], [1, 0], [2, 0]],
    "swap_positions": [[1, 3], [1, 0]]
  }
]
```

### Standard Output

For the standard output format (when `--json` is not used), the solver will print the results in a human-readable format, including a visual representation of the grid with the word highlighted in green, and letters that require swapping in blue.

If a word in our dictionary is incompatible with Spellcast, try deleting it from your dictionary and running the program again. Additionally, feel free to make a pull request with the incompatible words removed.

![output](https://github.com/user-attachments/assets/9b86f90e-fae5-4713-8176-ee2074c6bf34)

## Contributing

Contributions are welcome! Here's how you can contribute to the project:

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

If you're adding new features or making significant changes, consider adding new test cases to cover your additions. The tests are located in the `tests.c` file.

### Running Tests

This project includes a suite of unit tests to ensure code quality and functionality. To run the tests, use:
   ```bash
   make test
   ```
