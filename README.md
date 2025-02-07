# math_evaluatpr

This project is a simple math engine that performs **lexical analysis** on mathematical expressions. 
It breaks down the input expression into tokens (like numbers, operators, and parentheses) that can be processed further.

## Features
- **Lexical analysis** of math expressions like `3 + 5 * (2 - 8)`
- Supports operators: `+`, `-`, `*`, `/`
- Supports parentheses `()` and single-digit integer literals

## Requirements
- **MinGW** (tested with version 14.2.0, includes GCC as the C compiler)
- **CMake** (version >= 3.30.1)
- **mingw32-make** (for building)

## Build Instructions

1. **Fork the repository**:
   - Go to https://github.com/Doug4146/Math-Engine
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
- To run the program with an math expression:
   ```bash
   .\math_engine.exe  "2 + 1 - 2"
- Example output
   ```bash
   Token(type: TOKEN_NUMBER, value: '2')
   Token(type: TOKEN_OPERATOR_PLUS, value: '+')
   Token(type: TOKEN_NUMBER, value: '1')
   Token(type: OPERATOR_MINUS, value: '-')
   Token(type: LITERAL_INT, value: '2')
   Token(type: TOKEN_EOF, value: '\0')
    