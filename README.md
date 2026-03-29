# Scientific Conference Organization Tool
Design of Algorithms (DA) — Spring 2026 — L.EIC016
Group <GN> | TP <TN>


## - GENERAL CONSIDERATIONS -
- All subtopics under section 4.2 are preoperly adressed and explained  in our PowerPoint presentation;
- The "non-trivial input example stated under subsecton [T2.2] Basic Formulation with Risk Analysis = 1" is located in the input/ directory, named "datasettopic2-2.csv";
## Group Members

- Duarte Pinto (up number)
- João Maia (up number)
- Tiago Alves (up number)

## Project Overview
A tool to assign paper submissions to reviewers for a scientific
conference, formulated as a Maximum Flow problem using the
Edmonds-Karp algorithm.

## How to Compile


To create an executable for the program to run, both in Interactive and Batch modes, it is required to first build and compile the source code. The following script does so: 
### Linux
```bash
mkdir build
cd build
cmake ..
make
```

### Windows
```bash
mkdir build
cmake -B build
cmake --build build
```
## How to Run

## Interactive Mode 

### Linux
If running from the build directory:
```bash
./main

#note that, in this case, relative paths must be specified in the format: ../code/intended/directory
```
If running from the root directory:
```bash
./build/main

#note that, in this case, relative paths must be specified in the format: code/intended/directory
```

### Windows 
If running from the Debug directory (located inside /build):
```bash
./main.exe

#note that, in this case, relative paths must be specified in the format: ../code/intended/directory
```
If running from the root directory:
```bash
./build/Debug/main.exe

#note that, in this case, relative paths must be specified in the format: code/intended/directory
```
Launches a text menu with the following options:
1. Load input file
2. Display loaded data
3. Run assignment
4. Run risk analysis
5. Save output
0. Exit

## Batch Mode
```bash
./main -b /path/to/input.csv /path/to/output.csv
```
Automatically runs the full pipeline and writes
results to the specified output file.

## Input Format
Input files must be `.csv` files structured in four sections:
- `#Submissions` — paper submissions with ID, title,
  author, email, primary and optional secondary topic
- `#Reviewers` — reviewers with ID, name, email,
  primary and optional secondary expertise
- `#Parameters` — algorithm parameters
- `#Control` — output control parameters

See `code/data/input/` for example datasets.

## Output Format
Results are written to a CSV file with up to three sections:
- Successful assignment; 
- Failed assignment;
- Risk analysis results.

## Project Structure
```
code/
├── main.cpp          — entry point, CLI menu and batch mode
├── Parser.h/.cpp     — CSV input parsing and validation
├── FlowNetwork.h/.cpp — flow network construction
├── EdmondsKarp.h     — Edmonds-Karp max flow algorithm
├── Scheduler.h/.cpp  — assignment extraction and risk analysis
├── Output.h/.cpp     — output file formatting
├── Types.h           — shared data structures
└── utils.h           — utility functions (trim)
```

## Documentation
Full Doxygen documentation is available in `documentation/html/`.
Open `documentation/html/index.html` in a browser to view it.

To avoid going inside the
`docs/` folder, executing the following command from the project's root will automatically open a browser window on the documentation's main page:

```bash
xdg-open docs/html/index.html
```
## Notes
- Requires C++20 or later
- Tested on Ubuntu 24 and Windows 11 