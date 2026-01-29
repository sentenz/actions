# Basic Doxygen Example

This example demonstrates basic Doxygen documentation features for simple functions.

## Features Demonstrated

- **File documentation**: Header comments with `@file`, `@brief`, `@author`, `@version`, `@date`
- **Function documentation**: Complete function docs with `@param`, `@return`
- **Exception documentation**: `@throw` for documenting exceptions
- **Warnings**: Using `@warning` for important notes
- **Code examples**: Inline code with `@code` blocks

## Files

- `include/calculator.hpp` - Header with basic arithmetic functions
- `src/calculator.cpp` - Implementation

## Generate Documentation

```bash
doxygen -g
# Edit Doxyfile: INPUT = include src
doxygen
```

Or using the action:

```yaml
- uses: sentenz/actions/doxygen@latest
  with:
    source-dir: tests/doxygen/basic-example/include
    project-name: Basic Calculator
    extract-all: true
```
