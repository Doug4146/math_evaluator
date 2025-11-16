# math_evaluator

This project is a simple command-line mathematical expression evaluator that evaluates expressions such as 
`35* sin(cos(tan(e / 3)))`. 

## Features
- Tokenizes the user-inputted string and uses the shunting-yard algorithm to parse the list of tokens and convert into reverse polish form.
  This is then evaluated directly.
- Supports integers (`2, 190`), floats (`2.2, 190.190`) and numbers in scientific form (`2.2E+2, 4E-4`) (Note there must be a `+` or `-` infront of E)
- Supports `e` and `pi` as predefined constants
- Supports basic operators `+` ,`-`, `*`, `/` and parentheses `(`, `)`
- Supports functions: `sin`, `cos`, `tan`, `ln`, `log`, `exp` (Note that a `(` must always be written directly in front of a function name)
