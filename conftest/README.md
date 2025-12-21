# Conftest Composite Action

Policy-as-Code testing using [Open Policy Agent Conftest](https://www.conftest.dev/).

## Description

This action runs Conftest to validate configuration files against Rego policies. It supports external policy and configuration files for flexible policy enforcement.

## Inputs

| Input              | Description                                             | Required | Default        |
| ------------------ | ------------------------------------------------------- | -------- | -------------- |
| `path`             | Path to the files/directories to test                   | No       | `.`            |
| `policy-path`      | Path to the policy directory                            | No       | `policy`       |
| `conftest-version` | Conftest Docker image version                           | No       | `v0.65.0`      |
| `conftest-image`   | Conftest Docker image digest                            | No       | See action.yml |
| `output-format`    | Output format (stdout, json, tap, table, junit, github) | No       | `github`       |
| `fail-on-warn`     | Fail on warnings                                        | No       | `false`        |
| `all-namespaces`   | Use all namespaces                                      | No       | `true`         |

## Outputs

| Output      | Description                |
| ----------- | -------------------------- |
| `result`    | Conftest validation result |
| `exit-code` | Conftest exit code         |

## Usage

```yaml
jobs:
  policy-check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/conftest@main
        with:
          path: "./config"
          policy-path: "./policies"
          output-format: "github"
```

## External Configuration

You can reference external policy files from a separate repository:

```yaml
- uses: actions/checkout@v6.0.1
  with:
    repository: your-org/policies
    path: external-policies

- uses: sentenz/actions/conftest@main
  with:
    policy-path: "./external-policies"
```

## References

- [Conftest Documentation](https://www.conftest.dev/)
- [Rego Language Reference](https://www.openpolicyagent.org/docs/latest/policy-reference/)
