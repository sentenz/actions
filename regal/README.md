# Regal Composite Action

Linting for Rego policies using [Regal](https://github.com/StyraInc/regal).

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
- [4. Configuration](#4-configuration)
  - [4.1. Internal Configuration](#41-internal-configuration)
  - [4.2. External Configuration](#42-external-configuration)

## 1. Details

- [Regal](https://github.com/open-policy-agent/regal)
    > An open-source linter for Rego policies that ensure best practices and coding standards.

- [Rego Style Guide](https://www.openpolicyagent.org/docs/style-guide)
    > A style guide for writing Rego policies, providing best practices and conventions to improve code quality and maintainability.

## 2. Action

The [Regal Action](./action.yml) runs Regal to lint Rego policy files, ensuring best practices and coding standards are enforced.

### 2.1. Inputs

| Input           | Description                                          | Required | Default    |
| --------------- | ---------------------------------------------------- | -------- | ---------- |
| `path`          | Path to the policy directory                         | No       | `./policy` |
| `regal-version` | Regal version                                        | No       | `latest`   |
| `output-format` | Output format (pretty, compact, json, github, sarif) | No       | `github`   |
| `config-file`   | Path to Regal configuration file                     | No       | ``         |

### 2.2. Outputs

| Output      | Description       |
| ----------- | ----------------- |
| `result`    | Regal lint result |
| `exit-code` | Regal exit code   |

## 3. Usage

```yaml
jobs:
  lint-rego:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/regal@latest
        with:
          path: "./tests/policy"
          output-format: "github"
```

## 4. Configuration

### 4.1. Internal Configuration

<!-- TODO Loading internal configuration from actions `config/` directory in caller repository. -->

### 4.2. External Configuration

Create a `.regal/config.yaml` file in your repository:

```yaml
rules:
  style:
    opa-fmt:
      level: error
  bugs:
    rule-shadows-builtin:
      level: error
```
