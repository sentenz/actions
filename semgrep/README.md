# Semgrep Composite Action

Static analysis security scanning using Semgrep.

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
- [4. Configuration](#4-configuration)
  - [4.1. Internal Configuration](#41-internal-configuration)
  - [4.2. External Configuration](#42-external-configuration)

## 1. Details

- [Semgrep](https://github.com/semgrep/semgrep)
  > A fast, open-source, static analysis tool that finds bugs and enforces code standards by scanning source code for patterns.

- [Semgrep Documentation](https://semgrep.dev/docs/)
  > Comprehensive documentation for using and configuring Semgrep.

- [Semgrep Rules Registry](https://semgrep.dev/explore)
  > A collection of pre-defined Semgrep rules for various languages and frameworks.

- [Semgrep Rule Syntax](https://semgrep.dev/docs/writing-rules/rule-syntax)
  > A guide to writing custom Semgrep rules using structured pattern syntax.

## 2. Action

The [Semgrep Action](./action.yml) runs Semgrep for static application security testing (SAST), detecting bugs, security vulnerabilities, and anti-patterns in code.

### 2.1. Inputs

| Input           | Description                                         | Required | Default                                    |
| --------------- | --------------------------------------------------- | -------- | ------------------------------------------ |
| `path`          | Path to scan                                        | No       | `.`                                        |
| `semgrep-image` | Semgrep Docker image with version tag and digest    | No       | `semgrep/semgrep:1.145.2@sha256:791...7c9` |
| `config`        | Semgrep configuration (auto, p/default, path, etc.) | No       | `auto`                                     |
| `severity`      | Minimum severity to report (INFO, WARNING, ERROR)   | No       | `WARNING`                                  |
| `exclude`       | Patterns to exclude (comma-separated)               | No       | ``                                         |
| `include`       | Patterns to include (comma-separated)               | No       | ``                                         |

### 2.2. Outputs

| Output           | Description         |
| ---------------- | ------------------- |
| `result`         | Semgrep scan result |
| `exit-code`      | Semgrep exit code   |
| `findings-count` | Number of findings  |

## 3. Usage

```yaml
jobs:
  security-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/semgrep@latest
        with:
          path: "./src"
          config: "p/default"
          severity: "WARNING"
```

## 4. Configuration

### 4.1. Internal Configuration

<!-- TODO Loading internal configuration from actions `config/` directory in caller repository. -->

### 4.2. External Configuration

Create a `.semgrep.yml` file in your repository:

```yaml
rules:
  - id: custom-rule
    pattern: $X == $X
    message: Comparison is always true
    severity: WARNING
    languages: [python]
```
