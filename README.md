# Parallel Directory File Calculator

- **Unit**: Unix Systems Programming (`COMP2002`) — Assignment 1
- **Study Period**: 2023 Trimester 2A
- **Degree Programme**: Bachelor of Information Technology — Awarded with Distinction by Curtin University
- **Overall Unit Mark**: 88 (High Distinction)

> **README for Assignment Submission**: [README for Assignment Submission](ORIGINAL_README.md)

<table>
  <thead>
    <tr>
      <th colspan="6">Curtin University &bull; Bachelor of Information Technology</th>
    </tr>
  </thead>
  <tbody>
    <tr align="center" style="font-weight: bold; background-color: #f6f8fa;">
      <td>2022 T3A</td>
      <td>2023 T1A</td>
      <td>2023 T2A</td>
      <td>2023 T3A</td>
      <td>2024 T1A</td>
      <td>2024 T2A</td>
    </tr>
    <tr>
      <td valign="top">
        <abbr title="Fundamentals of Programming (92 HD)"><b>COMP1005</b></abbr><br>
        &bull; <a href="https://github.com/honneground/COMP1005-Vampire-Ecosystem-Simulation">Vampire Eco</a><br>        &bull; <a href="https://github.com/honneground/COMP1005-Vehicle-Emissions-Analyzer">Emissions</a><br>      </td>
      <td valign="top">
        <abbr title="Data Structures and Algorithms (89 HD)"><b>COMP1002</b></abbr><br>
        &bull; <a href="https://github.com/honneground/COMP1002-StudyHelp-Academic-Network">StudyHelp</a><br>        &bull; <a href="https://github.com/honneground/COMP1002-Advanced-Data-Structures-and-Visualizer">Visualizer</a><br>        <abbr title="Unix and C Programming (90 HD)"><b>COMP1000</b></abbr><br>
        &bull; <a href="https://github.com/honneground/COMP1000-Terminal-Maze-Survival-Game">Maze Survival</a><br>        &bull; <a href="https://github.com/honneground/COMP1000-Terminal-Maze-Darkness-Game">Maze Darkness</a><br>        &bull; <a href="https://github.com/honneground/COMP1000-Bookstore-Transaction-Simulator">Bookstore</a><br>        <abbr title="Introduction to Software Engineering (95 HD)"><b>ISAD1000</b></abbr><br>
        &bull; <a href="https://github.com/honneground/ISAD1000-Software-Testing-and-Pocket-Beast-Game">Pocket Beast</a><br>      </td>
      <td valign="top" style="background-color: #f1f8ff; border: 2px solid #0969da;">
        <abbr title="Unix Systems Programming (88 HD)"><b>COMP2002</b></abbr><br>
        &bull; <b>&#128073; Dir Calculator</b><br>        <abbr title="Operating Systems (80 HD)"><b>COMP2006</b></abbr><br>
        &bull; <a href="https://github.com/honneground/COMP2006-Customer-Queue-Concurrency-Simulation">Queue Sim</a><br>      </td>
      <td valign="top">
        <abbr title="Database Systems (80 HD)"><b>ISYS2014</b></abbr><br>
        &bull; <a href="https://github.com/honneground/ISYS2014-Relational-Database-Management-System">FootyStats DB</a><br>        <abbr title="Quantitative Techniques for Business (87 HD)"><b>ECOM2001</b></abbr><br>
        &bull; <a href="https://github.com/honneground/ECOM2001-Econometric-Statistical-Analysis-R">Econometric R</a><br>      </td>
      <td valign="top">
        <abbr title="Financial Decision Making (93 HD)"><b>ACCT1002</b></abbr><br>
        &bull; <a href="https://github.com/honneground/ACCT1002-Financial-Decision-Calculators">Finance Tools</a><br>        <abbr title="Human Computer Interface (71 D)"><b>ICTE3002</b></abbr><br>
        &bull; <a href="https://github.com/honneground/ICTE3002-Lumen-Interaction-Design">Lumen UI</a><br>        <abbr title="Engineering Management and Professional Practice (79 D)"><b>MGMT3000</b></abbr><br>
        &bull; <a href="https://github.com/honneground/MGMT3000-Phone-Router-Repurposing-Design">Phone Router</a><br>      </td>
      <td valign="top">
        <abbr title="Capstone Computing Project 1 (94 HD)"><b>ISAD3000</b></abbr><br>
        &bull; <a href="https://github.com/honneground/ISAD3000-CurtinSG-App-Design">CurtinSG UI</a><br>        <abbr title="Capstone Computing Project 2 (95 HD)"><b>ISAD3001</b></abbr><br>
        &bull; <a href="https://github.com/honneground/ISAD3001-CurtinSG-Student-App">CurtinSG App</a><br>        <abbr title="Advanced Computer Communications (88 HD)"><b>CNCO3002</b></abbr><br>
        &bull; <a href="https://github.com/honneground/CNCO3002-Multi-Service-Socket-Server">Socket Server</a><br>      </td>
    </tr>
  </tbody>
</table>


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

---

## Portfolio note

This Git repository was created retrospectively from the original project code, with assistance from OpenAI Codex and Antigravity in curating the repository and writing this README for publication. The initial commit timestamp is backdated to the original coursework submission date, followed by the final release commit.


