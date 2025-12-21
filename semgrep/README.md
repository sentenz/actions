# Semgrep Composite Action

Static analysis security scanning using [Semgrep](https://semgrep.dev/).

## Description

This action runs Semgrep for static application security testing (SAST), detecting bugs, security vulnerabilities, and anti-patterns in code.

## Inputs

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

## Outputs

| Output           | Description         |
| ---------------- | ------------------- |
| `result`         | Semgrep scan result |
| `exit-code`      | Semgrep exit code   |
| `findings-count` | Number of findings  |

## Usage

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

## External Configuration

Create a `.semgrep.yml` file in your repository:

```yaml
rules:
  - id: custom-rule
    pattern: $X == $X
    message: Comparison is always true
    severity: WARNING
    languages: [python]
```

## References

- [Semgrep Documentation](https://semgrep.dev/docs/)
- [Semgrep Rules Registry](https://semgrep.dev/explore)
