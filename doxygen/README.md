# Doxygen Composite Action

Generate API documentation from source code using Doxygen and publish to GitHub Pages.

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
  - [3.1. Basic Documentation Generation](#31-basic-documentation-generation)
  - [3.2. Custom Doxyfile Configuration](#32-custom-doxyfile-configuration)
  - [3.3. Publishing to GitHub Pages](#33-publishing-to-github-pages)
  - [3.4. Multi-Language Projects](#34-multi-language-projects)
  - [3.5. Complete Workflow with Release](#35-complete-workflow-with-release)
- [4. Configuration](#4-configuration)
  - [4.1. Doxyfile Configuration](#41-doxyfile-configuration)
  - [4.2. GitHub Pages Setup](#42-github-pages-setup)

## 1. Details

- [Doxygen](https://www.doxygen.nl/)
  > A documentation generator that creates an online documentation browser (in HTML) and/or an off-line reference manual (in LaTeX) from annotated C++, C, C#, Objective-C, Python, Java, PHP, and other source code.

- [Doxygen Documentation](https://www.doxygen.nl/manual/)
  > Official manual covering all Doxygen features, configuration options, and special commands.

- [Doxygen GitHub Action](https://github.com/mattnotmitt/doxygen-action)
  > A popular GitHub Action for running Doxygen in CI/CD pipelines.

- [GitHub Pages](https://docs.github.com/en/pages)
  > GitHub Pages is a static site hosting service that takes files from a repository and publishes a website.

## 2. Action

The [Doxygen Action](./action.yml) generates API documentation from source code using Doxygen and optionally publishes it to GitHub Pages.

### 2.1. Inputs

| Input             | Description                                         | Required | Default                                        |
| ----------------- | --------------------------------------------------- | -------- | ---------------------------------------------- |
| `source-dir`      | Directory containing source code to document        | No       | `.`                                            |
| `output-dir`      | Directory where documentation will be generated     | No       | `docs/html`                                    |
| `doxyfile-path`   | Path to Doxyfile configuration file                 | No       | ``                                             |
| `doxygen-version` | Doxygen version to use                              | No       | `1.12.0`                                       |
| `project-name`    | Project name for documentation                      | No       | ``                                             |
| `project-version` | Project version for documentation                   | No       | ``                                             |
| `generate-html`   | Generate HTML documentation                         | No       | `true`                                         |
| `generate-latex`  | Generate LaTeX documentation                        | No       | `false`                                        |
| `recursive`       | Recursively scan subdirectories                     | No       | `true`                                         |
| `extract-all`     | Extract all entities (even undocumented)            | No       | `false`                                        |
| `file-patterns`   | File patterns to document (space-separated)         | No       | `*.c *.cc *.cxx *.cpp *.c++ *.h *.hh *.hpp...` |
| `publish-pages`   | Publish documentation to GitHub Pages               | No       | `false`                                        |
| `pages-branch`    | Branch to publish GitHub Pages to                   | No       | `gh-pages`                                     |
| `github-token`    | GitHub token for publishing to GitHub Pages         | No       | `${{ github.token }}`                          |

### 2.2. Outputs

| Output               | Description                                      |
| -------------------- | ------------------------------------------------ |
| `documentation-path` | Path to generated documentation                  |
| `pages-url`          | URL of published GitHub Pages (if published)     |
| `success`            | Whether documentation generation was successful  |

## 3. Usage

### 3.1. Basic Documentation Generation

Generate documentation for a C++ project in the current directory.

```yaml
jobs:
  documentation:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1

      - name: Generate Documentation
        uses: sentenz/actions/doxygen@latest
        with:
          source-dir: "src"
          project-name: "My Project"
          project-version: "1.0.0"

      - name: Upload Documentation
        uses: actions/upload-artifact@v6.0.0
        with:
          name: documentation
          path: docs/html
```

### 3.2. Custom Doxyfile Configuration

Use a custom Doxyfile for advanced configuration.

```yaml
jobs:
  documentation:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1

      - name: Generate Documentation
        uses: sentenz/actions/doxygen@latest
        with:
          doxyfile-path: "config/Doxyfile"
          source-dir: "src"
          output-dir: "build/docs"
```

### 3.3. Publishing to GitHub Pages

Generate documentation and automatically publish it to GitHub Pages.

```yaml
jobs:
  documentation:
    runs-on: ubuntu-latest
    permissions:
      contents: write
    steps:
      - uses: actions/checkout@v6.0.1

      - name: Generate and Publish Documentation
        uses: sentenz/actions/doxygen@latest
        with:
          source-dir: "src"
          project-name: "My API"
          project-version: ${{ github.ref_name }}
          publish-pages: "true"
          github-token: ${{ secrets.GITHUB_TOKEN }}
```

### 3.4. Multi-Language Projects

Document projects with multiple programming languages.

```yaml
jobs:
  documentation:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1

      - name: Generate Documentation
        uses: sentenz/actions/doxygen@latest
        with:
          source-dir: "."
          project-name: "Multi-Language Project"
          file-patterns: "*.cpp *.h *.py *.java *.php"
          recursive: "true"
          extract-all: "true"
```

### 3.5. Complete Workflow with Release

Full workflow that generates documentation on release and publishes to GitHub Pages.

```yaml
name: Documentation

on:
  push:
    branches:
      - main
  release:
    types: [published]

jobs:
  generate-docs:
    runs-on: ubuntu-latest
    permissions:
      contents: write
    steps:
      - name: Checkout
        uses: actions/checkout@v6.0.1

      - name: Extract version
        id: version
        run: |
          if [[ "${{ github.event_name }}" == "release" ]]; then
            echo "version=${{ github.event.release.tag_name }}" >> $GITHUB_OUTPUT
          else
            echo "version=development" >> $GITHUB_OUTPUT
          fi

      - name: Generate Documentation
        uses: sentenz/actions/doxygen@latest
        with:
          source-dir: "src"
          output-dir: "docs"
          project-name: "My API Documentation"
          project-version: ${{ steps.version.outputs.version }}
          generate-html: "true"
          generate-latex: "false"
          recursive: "true"
          extract-all: "false"
          publish-pages: "true"
          github-token: ${{ secrets.GITHUB_TOKEN }}

      - name: Upload Documentation Artifact
        if: github.event_name == 'release'
        uses: actions/upload-artifact@v6.0.0
        with:
          name: api-documentation
          path: docs/html

      - name: Attach Documentation to Release
        if: github.event_name == 'release'
        run: |
          tar -czf documentation.tar.gz -C docs/html .
          gh release upload ${{ github.event.release.tag_name }} documentation.tar.gz
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

## 4. Configuration

### 4.1. Doxyfile Configuration

Doxygen uses a configuration file called `Doxyfile` to control documentation generation. You can either:

1. **Use Action Defaults**: The action will generate a basic Doxyfile automatically
2. **Provide Custom Doxyfile**: Create your own Doxyfile with custom settings

Example minimal Doxyfile:

```text
# Doxyfile for My Project

# Project settings
PROJECT_NAME           = "My Project"
PROJECT_NUMBER         = "1.0.0"
PROJECT_BRIEF          = "A brief description of the project"

# Build settings
OUTPUT_DIRECTORY       = docs
INPUT                  = src include
RECURSIVE              = YES
EXTRACT_ALL            = NO
EXTRACT_PRIVATE        = NO
EXTRACT_STATIC         = YES

# Output formats
GENERATE_HTML          = YES
HTML_OUTPUT            = html
GENERATE_LATEX         = NO

# Documentation settings
JAVADOC_AUTOBRIEF      = YES
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO

# Diagram settings
HAVE_DOT               = YES
UML_LOOK               = YES
CALL_GRAPH             = YES
CALLER_GRAPH           = YES

# File patterns
FILE_PATTERNS          = *.cpp *.h *.hpp *.c *.cc
EXCLUDE_PATTERNS       = */test/* */tests/*
```

A sample Doxyfile is provided in [`doxygen/config/Doxyfile`](./config/Doxyfile).

### 4.2. GitHub Pages Setup

To publish documentation to GitHub Pages:

1. **Enable GitHub Pages** in your repository settings:
   - Go to Settings > Pages
   - Select "Deploy from a branch"
   - Choose the `gh-pages` branch and `/ (root)` folder
   - Click "Save"

2. **Configure Permissions** in your workflow:
   ```yaml
   permissions:
     contents: write  # Required to push to gh-pages branch
   ```

3. **Use the Action** with `publish-pages: "true"`:
   ```yaml
   - uses: sentenz/actions/doxygen@latest
     with:
       publish-pages: "true"
       github-token: ${{ secrets.GITHUB_TOKEN }}
   ```

Your documentation will be available at: `https://[username].github.io/[repository]/`

**Note**: The first deployment may take a few minutes to become available.
