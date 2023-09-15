# Parallel Directory File Calculator

- **Unit**: Unix Systems Programming (`COMP2002`) — Assignment 1
- **Study Period**: 2023 Trimester 2A
- **Degree Programme**: Bachelor of Information Technology — Awarded with Distinction by Curtin University
- **Overall Unit Mark**: 88 (High Distinction)

> **README for Assignment Submission**: [README for Assignment Submission](ORIGINAL_README.md)

## Assignment context

The assignment focused on Unix process creation and inter-process communication using `fork()`, `pipe()`, `open()`, `close()`, `read()` and `write()`. The program had to find `.usp` files in its current directory, create one child process per file, send each filename to its child, and collect the calculation results into `result.txt`.

Each input contains an identifier, two non-negative integers and one of four arithmetic operators. The expected result is a non-negative integer; division uses integer division. File I/O was required to use file descriptors rather than related stdio file functions. The parent had to collect all children with `wait()` and release its resources.

## Implementation

- `directory_retriever.c` checks for the literal `.usp` suffix. `main.c` enumerates files in the current directory with `opendir()` and `readdir()`.
- The parent collects matching names, creates pipes and forks a child for each calculation.
- Each child receives a length-prefixed filename, opens the input with `open()`, reads its contents and calculates the result through `file_reader.c`.
- The child sends back a length-prefixed result string. The parent appends one result per line to `result.txt`, then waits for the child processes.

The parent uses a one-second delay between sending filenames through the pipes and reading the calculation results.

## Input and output

A `.usp` file has four lines:

```text
#00015261
10
+
2
```

The internal identifier need not match the filename. This example produces `#00015261:12`. Supported operators are `+`, `-`, `*` and `/`. The supplied sample files provide the expected exercise format.

`result.txt` is opened in append mode. Repeated runs add further result lines to an existing file, as described by the original task.

## Build and run

Use GCC and Make in a Linux or WSL environment. Run from this directory, where the sample `.usp` inputs reside:

```bash
make
./calculator
```

The application takes no command-line directory argument. To inspect memory handling in a session, if Valgrind is installed:

```bash
valgrind --leak-check=full ./calculator
```

`make clean` removes the build outputs.
