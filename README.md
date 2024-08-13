# spellcast_solver_cli

> A high-performance C-based CLI tool for solving the Discord game Spellcast, optimizing word choices based on letter scores and special tiles. Compatible with double/triple (and more than triple) letter/word scores, custom grid sizes, etc.

## Installation

To use spellcast_solver_cli, you need to have a C compiler (such as gcc) installed on your system. Clone this repository and compile the project:

```bash
git clone https://github.com/yourusername/spellcast_solver_cli.git
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

## Output Examples

### JSON Output

When using the `--json true` option, the output will be in JSON format:

```json
[
  {
    "swaps": 0,
    "word": "EXAMPLE",
    "score": 24,
    "positions": [[0, 0], [1, 1], [2, 2], [3, 3], [4, 4], [3, 3], [2, 2]],
    "swap_positions": []
  },
  {
    "swaps": 1,
    "word": "BETTER",
    "score": 30,
    "positions": [[0, 1], [1, 1], [2, 2], [3, 3], [4, 4], [3, 3]],
    "swap_positions": [[0, 1]]
  }
]
```

### Standard Output

For the standard output format (when `--json` is not used), the solver will print the results in a human-readable format, including a visual representation of the grid with the word highlighted in green, and letters that require swapping in blue.

![output](https://github.com/user-attachments/assets/45971fde-7d4f-425c-acbb-39d2b69f988d)

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
