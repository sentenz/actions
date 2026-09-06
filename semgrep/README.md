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

Run on a Linux runner with Docker and GNU `realpath`. Local scan targets, configuration files, reports, and cache directories must resolve inside `GITHUB_WORKSPACE`. Relative paths use the current working directory; absolute workspace paths are preserved inside the container. Symlinks that resolve outside the workspace are rejected.

`jq` is also required to read the JSON report. `severity` is a minimum: `WARNING` includes `WARNING` and `ERROR`, while an empty value or `INFO` includes all severities. `findings-count` is the length of the JSON `results` array, including `0` for a clean scan. It is empty when the scan fails without a valid report; a missing or invalid report cannot produce a successful action result. Scanner exit codes are preserved.

### 2.1. Inputs

| Input      | Description                                         | Required | Default           |
| ---------- | --------------------------------------------------- | -------- | ----------------- |
| `path`     | Path to scan                                        | No       | `.`               |
| `image`    | Semgrep Docker image with version tag and digest    | No       | `semgrep/semgrep` |
| `config`   | Semgrep configuration (auto, p/default, path, etc.) | No       | `auto`            |
| `severity` | Minimum severity to report (INFO, WARNING, ERROR)   | No       | ``                |
| `exclude`  | Patterns to exclude (comma-separated)               | No       | ``                |
| `include`  | Patterns to include (comma-separated)               | No       | ``                |
| `sarif-output` | Workspace path for a SARIF report | No | `` |
| `json-output` | Workspace path for a JSON report | No | `` |
| `gitlab-sast-output` | Workspace path for a GitLab SAST report | No | `` |

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
      - uses: actions/checkout@9c091bb21b7c1c1d1991bb908d89e4e9dddfe3e0 # v7.0.0
      - uses: sentenz/actions/semgrep@eeb59ec6f18d51aee1f04136bfedceaa02d346f9
        with:
          path: "./src"
          config: "p/default"
          severity: "WARNING"
          sarif-output: "reports/semgrep.sarif"
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
