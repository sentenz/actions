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

Run on a Linux runner with Docker and GNU `realpath`. Local scan targets, configuration files, reports, and cache directories must resolve inside `GITHUB_WORKSPACE`. Relative paths use the current working directory; absolute workspace paths are preserved inside the container. Symlinks that resolve outside the workspace are rejected.

An explicitly supplied `config-file` or `data-files` entry must be a readable file. A missing `policy-path` fails validation; when the default `policy` directory is absent and `config-file` is supplied, Conftest may load its policy location from that configuration. `data-files` accepts one line of space-separated paths; individual paths cannot contain spaces.

### 2.1. Inputs

| Input            | Description                                             | Required | Default                    |
| ---------------- | ------------------------------------------------------- | -------- | -------------------------- |
| `path`           | Path to the files/directories to test                   | No       | `.`                        |
| `policy-path`    | Path to the policy directory                            | No       | `policy`                   |
| `image`          | Conftest Docker image with version tag and digest       | No       | `openpolicyagent/conftest` |
| `output-format`  | Output format (stdout, json, tap, table, junit, github) | No       | `github`                   |
| `fail-on-warn`   | Fail on warnings                                        | No       | `false`                    |
| `all-namespaces` | Use all namespaces                                      | No       | `true`                     |
| `config-file` | Path to a readable Conftest configuration file | No | `` |
| `data-files` | Space-separated paths to readable data files | No | `` |

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
      - uses: actions/checkout@9c091bb21b7c1c1d1991bb908d89e4e9dddfe3e0 # v7.0.0
      - uses: sentenz/actions/conftest@eeb59ec6f18d51aee1f04136bfedceaa02d346f9
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
- uses: actions/checkout@9c091bb21b7c1c1d1991bb908d89e4e9dddfe3e0 # v7.0.0
  with:
    repository: your-org/policies
    path: external-policies

- uses: sentenz/actions/conftest@eeb59ec6f18d51aee1f04136bfedceaa02d346f9
  with:
    policy-path: "./external-policies"
```
