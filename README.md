# Student SGPA Calculator (C++)

A command-line C++ program that reads structured student assessment records,
converts each subject percentage to grade points, and calculates the
credit-hour-weighted Semester Grade Point Average (SGPA).

## Features

- Reads multiple student and subject records from a text file.
- Calculates subject percentages from obtained and total marks.
- Applies the grade-point bands retained from the original coursework.
- Computes SGPA using subject credit hours.
- Reports total and earned credit hours.
- Validates the input structure and marks, and reports clear file/format errors.
- Supports custom input and output file paths.

## Grade-point scale

| Percentage | Grade | Grade points |
| --- | --- | ---: |
| 85 and above | A | 4.00 |
| 80–84.99 | A- | 3.75 |
| 75–79.99 | B+ | 3.50 |
| 70–74.99 | B- | 3.00 |
| 65–69.99 | C+ | 2.50 |
| 60–64.99 | C | 2.00 |
| 55–59.99 | D+ | 1.50 |
| 50–54.99 | D | 1.00 |
| Below 50 | F | 0.00 |

SGPA is calculated as the sum of (subject grade points × subject credit hours)
divided by total credit hours.

## Build and run

Requires a C++17-compatible compiler.

```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o sgpa-calculator
./sgpa-calculator
```

By default, the application reads `data/academic_records.example.txt` and
writes `student_gpas.txt`. Custom paths can be supplied:

```bash
./sgpa-calculator path/to/records.txt path/to/report.txt
```

## Input format

Each student starts with a header. Each subject includes its credit hours and
a `Total: obtained/maximum` line. Other assessment-component lines are ignored
by the parser, so the source format can contain quizzes, assignments, labs,
and examination details. See the synthetic example in `data/`.

```text
Student: Demo Student (DEMO-001), Semester: Example Semester
Subject: Programming Fundamentals (CS101), Credit Hours: 3
  Final: 34/40
  Total: 132/160
```

## Privacy note

The archive supplied for review contained student-number-like identifiers and
individual grade records. Those records have **not** been copied into this
public-ready version. The included example dataset is synthetic and uses demo
identifiers only. Do not commit real student records or identifiers to a
public repository.

## Smoke test

```bash
./tests/smoke_test.sh
```
