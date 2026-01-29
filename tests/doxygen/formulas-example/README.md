# Formulas Example

This example demonstrates how to document mathematical formulas using LaTeX notation in Doxygen.

## Features Demonstrated

- **Inline formulas**: Using `\f$ formula \f$` for inline math
- **Display formulas**: Using `\f[ formula \f]` for block equations
- **Mathematical symbols**: Greek letters, subscripts, superscripts, square roots
- **Summation notation**: \sum with indices
- **Fractions**: Using \frac{numerator}{denominator}
- **Complex expressions**: Multi-line formulas with proper formatting

## Files

- `include/statistics.hpp` - Statistical functions with LaTeX formulas
- `src/statistics.cpp` - Implementation

## Formula Rendering

The formulas will be rendered as images or MathJax in the generated HTML documentation.

Examples from the code:
- Mean: x̄ = (1/n)∑xᵢ
- Variance: σ² = (1/n)∑(xᵢ - x̄)²
- Quadratic formula: x = (-b ± √(b² - 4ac)) / 2a

## Generate Documentation

```bash
doxygen -g
# Edit Doxyfile:
#   INPUT = include src
#   USE_MATHJAX = YES  (for MathJax rendering)
#   or
#   FORMULA_FONTSIZE = 10  (for image rendering)
doxygen
```

Or using the action:

```yaml
- uses: sentenz/actions/doxygen@latest
  with:
    source-dir: tests/doxygen/formulas-example/include
    project-name: Statistical Formulas
    extract-all: true
```
