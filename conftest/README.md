# Conftest Composite Action

Policy-as-Code (PaC) testing using Conftest from Open Policy Agent (OPA).

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
- [4. Configuration](#4-configuration)
  - [4.1. Internal Configuration](#41-internal-configuration)
  - [4.2. External Configuration](#42-external-configuration)

## 1. Details

- [Conftest](https://www.conftest.dev/)
    > A Policy-as-Code (PaC) tool for writing tests against structured configuration data using the Rego language from Open Policy Agent (OPA).

- [Open Policy Agent (OPA)](https://www.openpolicyagent.org/)
  > An open-source, general-purpose policy engine that enables unified, context-aware policy enforcement across the entire stack.

- [Rego Language Reference](https://www.openpolicyagent.org/docs/latest/policy-reference/)
  > The policy language used by OPA and Conftest to define rules and policies.

## 2. Action

The [Conftest Action](./action.yml) runs to validate configuration files against Rego policies.

### 2.1. Inputs

| Input            | Description                                             | Required | Default                                       |
| ---------------- | ------------------------------------------------------- | -------- | --------------------------------------------- |
| `path`           | Path to the files/directories to test                   | No       | `.`                                           |
| `policy-path`    | Path to the policy directory                            | No       | `policy`                                      |
| `conftest-image` | Conftest Docker image with version tag and digest       | No       | `openpolicyagent/conftest:v0.65.0@sha256:...` |
| `output-format`  | Output format (stdout, json, tap, table, junit, github) | No       | `github`                                      |
| `fail-on-warn`   | Fail on warnings                                        | No       | `false`                                       |
| `all-namespaces` | Use all namespaces                                      | No       | `true`                                        |

### 2.2. Outputs

| Output      | Description                |
| ----------- | -------------------------- |
| `result`    | Conftest validation result |
| `exit-code` | Conftest exit code         |

## 3. Usage

```yaml
jobs:
  policy-check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/conftest@latest
        with:
          path: "./config"
          policy-path: "./tests/policy"
          output-format: "github"
```

## 4. Configuration

### 4.1. Internal Configuration

<!-- TODO Loading internal configuration from actions `config/` directory in caller repository. -->

### 4.2. External Configuration

Referencing policies from an caller repository.

```yaml
- uses: actions/checkout@v6.0.1
  with:
    repository: your-org/policies
    path: external-policies

- uses: sentenz/actions/conftest@latest
  with:
    policy-path: "./external-policies"
```
