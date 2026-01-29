# Inheritance Example

This example demonstrates how to document class hierarchies and inheritance relationships in Doxygen.

## Features Demonstrated

- **Abstract base classes**: Pure virtual functions with `= 0`
- **Virtual functions**: Using `virtual` and `override` keywords
- **Class hierarchy documentation**: Using `@see` to link related classes
- **Inheritance relationships**: Doxygen automatically generates class diagrams
- **Pure virtual functions**: Abstract interface definition
- **Method overriding**: Implementing virtual functions in derived classes

## Files

- `include/shape.hpp` - Shape base class with Rectangle and Triangle derivatives
- `src/shape.cpp` - Implementation

## Generated Documentation Features

When documentation is generated, Doxygen will create:
- **Class hierarchy diagrams** showing inheritance relationships
- **Collaboration diagrams** showing class usage
- **Links between base and derived classes**
- **Virtual function indicators** in the documentation

## Generate Documentation

```bash
doxygen -g
# Edit Doxyfile:
#   INPUT = include src
#   HAVE_DOT = YES
#   CLASS_GRAPH = YES
#   COLLABORATION_GRAPH = YES
doxygen
```

Or using the action:

```yaml
- uses: sentenz/actions/doxygen@latest
  with:
    source-dir: tests/doxygen/inheritance-example/include
    project-name: Shape Hierarchy
    extract-all: true
```
