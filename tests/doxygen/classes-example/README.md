# Classes Example

This example demonstrates Doxygen documentation for classes, namespaces, and object-oriented code.

## Features Demonstrated

- **Namespace documentation**: Organizing code with `@namespace`
- **Struct documentation**: Simple data structures
- **Class documentation**: Complete class with methods and members
- **Member variables**: Documented with inline `///` comments
- **Getter methods**: Inline implementation with documentation
- **Constructor documentation**: With parameter and exception docs
- **Private members**: Documented private member variables

## Files

- `include/circle.hpp` - Circle class with Point struct
- `src/circle.cpp` - Implementation

## Generate Documentation

```bash
doxygen -g
# Edit Doxyfile: INPUT = include src, EXTRACT_ALL = YES
doxygen
```

Or using the action:

```yaml
- uses: sentenz/actions/doxygen@latest
  with:
    source-dir: tests/doxygen/classes-example/include
    project-name: Geometry Classes
    extract-all: true
```
