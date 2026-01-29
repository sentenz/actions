# C++ API Documentation Example

This directory contains comprehensive C++ example code for demonstrating Doxygen documentation generation.

## Contents

- `include/` - Header files with extensive Doxygen comments
  - `math_operations.hpp` - Mathematical operations library
  - `geometry.hpp` - Geometric shapes and calculations
- `src/` - Implementation files
  - `math_operations.cpp` - Implementation of math operations
  - `geometry.cpp` - Implementation of geometric shapes
- `docs/` - Generated documentation output (after running Doxygen)

## Features Demonstrated

### Documentation Elements

1. **File Documentation**: Each file has comprehensive header comments
2. **Namespace Documentation**: Documented namespaces for organizing code
3. **Class Documentation**: Detailed class descriptions with usage examples
4. **Method Documentation**: Complete function documentation with:
   - Parameters (`@param`)
   - Return values (`@return`)
   - Exceptions (`@throw`)
   - Warnings (`@warning`)
   - Notes (`@note`)
   - Code examples (`@code`)
5. **Mathematical Formulas**: LaTeX formula rendering (`\f$`)
6. **Cross-references**: Links between related classes and functions
7. **Enumerations**: Documented enum values
8. **Inheritance**: Documented class hierarchies

### Code Examples

The library includes:

- **Calculator Class**: Basic and advanced mathematical operations
- **Statistics Class**: Statistical calculations (mean, median, std dev, etc.)
- **Geometric Shapes**: Abstract base class with concrete implementations
  - Circle
  - Rectangle
  - Triangle
- **Utility Functions**: Helper functions for geometric calculations

## Generating Documentation

### Using the Doxygen Action

```yaml
- name: Generate Documentation
  uses: sentenz/actions/doxygen@latest
  with:
    source-dir: tests/doxygen/cpp-example/include
    output-dir: tests/doxygen/cpp-example/docs
    project-name: C++ API Example
    project-version: 1.0.0
    recursive: true
    extract-all: true
```

### Using Doxygen Directly

```bash
# From the cpp-example directory
cd tests/doxygen/cpp-example

# Generate documentation using provided Doxyfile
doxygen Doxyfile

# Open the generated documentation
open docs/html/index.html
```

### Using Custom Doxyfile

```yaml
- name: Generate Documentation
  uses: sentenz/actions/doxygen@latest
  with:
    doxyfile-path: tests/doxygen/cpp-example/Doxyfile
    source-dir: tests/doxygen/cpp-example
    output-dir: tests/doxygen/cpp-example/docs
```

## Documentation Best Practices

This example demonstrates these best practices:

1. **Comprehensive Comments**: All public APIs are documented
2. **Usage Examples**: Code snippets show how to use the API
3. **Mathematical Notation**: Formulas are rendered properly
4. **Parameter Documentation**: All parameters explained
5. **Exception Documentation**: All thrown exceptions documented
6. **Cross-linking**: Related functions and classes are linked
7. **Namespace Organization**: Logical grouping of related functionality
8. **Version Information**: Files include version and author information

## Expected Output

The generated documentation will include:

- Class hierarchy diagrams
- Collaboration diagrams
- Include dependency graphs
- Call graphs (if enabled)
- Full API reference with searchable index
- Example code snippets
- Mathematical formula rendering

## Testing the Documentation

After generating documentation:

1. Open `docs/html/index.html` in a browser
2. Navigate through the class hierarchy
3. Check that all formulas render correctly
4. Verify that code examples are highlighted
5. Test the search functionality
6. Validate all cross-references work
