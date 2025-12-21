# Regal Composite Action

Linting for Rego policies using [Regal](https://github.com/StyraInc/regal).

## Description

This action runs Regal to lint Rego policy files, ensuring they follow best practices and coding standards.

## Inputs

| Input           | Description                                          | Required | Default    |
| --------------- | ---------------------------------------------------- | -------- | ---------- |
| `path`          | Path to the policy directory                         | No       | `./policy` |
| `regal-version` | Regal version                                        | No       | `latest`   |
| `output-format` | Output format (pretty, compact, json, github, sarif) | No       | `github`   |
| `config-file`   | Path to Regal configuration file                     | No       | ``         |
| `rules`         | Comma-separated list of rules to enable/disable      | No       | ``         |

## Outputs

| Output      | Description       |
| ----------- | ----------------- |
| `result`    | Regal lint result |
| `exit-code` | Regal exit code   |

## Usage

```yaml
jobs:
  lint-rego:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/regal@main
        with:
          path: "./policies"
          output-format: "github"
```

## External Configuration

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

## References

- [Regal Documentation](https://docs.styra.com/regal)
- [Regal Rules](https://docs.styra.com/regal/rules)
