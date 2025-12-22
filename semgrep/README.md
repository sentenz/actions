# Semgrep Composite Action

Static analysis security scanning using Semgrep.

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
  - [3.1. Internal Configuration](#31-internal-configuration)
  - [3.2. External Configuration](#32-external-configuration)

## 1. Details

- [Semgrep](hhttps://github.com/semgrep/semgrep)
  > A fast, open-source, static analysis tool that finds bugs and enforces code standards by scanning source code for patterns.

- [Semgrep Documentation](https://semgrep.dev/docs/)
  > Comprehensive documentation for using and configuring Semgrep.

- [Semgrep Rules Registry](https://semgrep.dev/explore)
  > A collection of pre-defined Semgrep rules for various languages and frameworks.

- [Semgrep Writing Rules](https://semgrep.dev/docs/writing-rules/)
  > A guide on how to write custom Semgrep rules to tailor scans to your specific needs.

## 2. Action

The [Semgrep Action](./semgrep/action.yml) runs Semgrep for static application security testing (SAST), detecting bugs, security vulnerabilities, and anti-patterns in code.

### 2.1. Inputs

| Input             | Description                                         | Required | Default        |
| ----------------- | --------------------------------------------------- | -------- | -------------- |
| `path`            | Path to scan                                        | No       | `.`            |
| `semgrep-version` | Semgrep Docker image version                        | No       | `1.145.2`      |
| `semgrep-image`   | Semgrep Docker image with digest                    | No       | See action.yml |
| `config`          | Semgrep configuration (auto, p/default, path, etc.) | No       | `auto`         |
| `output-format`   | Output format (text, json, sarif, gitlab-sast)      | No       | `github`       |
| `severity`        | Minimum severity to report (INFO, WARNING, ERROR)   | No       | `WARNING`      |
| `exclude`         | Patterns to exclude (comma-separated)               | No       | ``             |
| `include`         | Patterns to include (comma-separated)               | No       | ``             |

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
      - uses: sentenz/actions/semgrep@main
        with:
          path: "./src"
          config: "auto"
          severity: "WARNING"
```

### 3.1. Internal Configuration

<!-- TODO Loading internal configuration from actions `config/` directory in caller repository. -->

### 3.2. External Configuration

Create a `.semgrep.yml` file in your repository:

```yaml
rules:
  - id: custom-rule
    pattern: $X == $X
    message: Comparison is always true
    severity: WARNING
    languages: [python]
```
