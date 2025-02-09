# math_evaluator

This project is a simple command-line mathematical expression evaluator that evaluates expressions such as 
`35* sin(cos(tan(e / 3)))`. 

## Features
- Tokenizes the user-inputted string and uses the shunting-yard algorithm to parse the list of tokens and convert into reverse polish form.
  This is then evaluated directly.
- Supports integers (`2, 190`), floats (`2.2, 190.190`) and numbers in scientific form (`2.2E+2, 4E-4`) (Note there must be a `+` or `-` infront of E)
- Supports `e` and `pi` as predefined constants
- Supports basic operatoros `+` ,`-`, `*`, `/` and parentheses `(`, `)`
- Supports functions: `sin`, `cos`, `tan`, `ln`, `log`, `exp` (Note that a `(` must always be written directly in front of a function name)

## Requirements
- **MinGW** (tested with version 14.2.0, includes GCC as the C compiler)
- **CMake** (version >= 3.30.1)
- **mingw32-make** (for building)

## Build Instructions

1. **Fork the repository**:
   - Go to https://github.com/Doug4146/math_evaluator
   - Click on the "Fork" button at the top-right corner to create your own copy of the repository
1. **Clone the forked repository**:
   ```bash
   git clone https://github.com/yourusername/your-forked-repo.git
   cd your-forked-repo
2. **Create a build directory**:
   ```bash
   mkdir build
   cd build
3. **Generate build files with CMake**
   ```bash
   cmake ..
4. **Build the program**
   ```bash
   mingw32-make
   
## Usage
- Run the program exectutable with precisly one math expression in string format:
   ```bash
   .\math_evaluator.exe  "2*sin(cos(e*pi) / 1 - 2"
    
