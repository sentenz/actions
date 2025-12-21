# Renovate Composite Action

Automated dependency updates using [Renovate](https://github.com/renovatebot/renovate).

## Description

This action runs Renovate to automatically create pull requests for dependency updates across your repositories.

## Inputs

| Input              | Description                               | Required | Default               |
| ------------------ | ----------------------------------------- | -------- | --------------------- |
| `token`            | GitHub token for Renovate                 | Yes      | `${{ github.token }}` |
| `renovate-version` | Renovate Docker image version             | No       | `42.64.1`             |
| `renovate-image`   | Renovate Docker image with digest         | No       | See action.yml        |
| `autodiscover`     | Autodiscover repositories                 | No       | `true`                |
| `config-file`      | Path to renovate configuration file       | No       | ``                    |
| `platform`         | Platform to run on (github, gitlab, etc.) | No       | `github`              |
| `dry-run`          | Run in dry-run mode                       | No       | `false`               |
| `log-level`        | Log level (debug, info, warn, error)      | No       | `info`                |

## Outputs

| Output      | Description               |
| ----------- | ------------------------- |
| `result`    | Renovate execution result |
| `exit-code` | Renovate exit code        |

## Usage

```yaml
jobs:
  renovate:
    runs-on: ubuntu-latest
    permissions:
      contents: read
      pull-requests: write
      issues: write
      checks: read
      id-token: write
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/renovate@main
        with:
          token: ${{ secrets.GITHUB_TOKEN }}
          autodiscover: "true"
```

## External Configuration

Create a `renovate.json` file in your repository:

```json
{
  "$schema": "https://docs.renovatebot.com/renovate-schema.json",
  "extends": ["config:recommended"]
}
```

## References

- [Renovate Documentation](https://docs.renovatebot.com/)
- [Renovate Configuration Options](https://docs.renovatebot.com/configuration-options/)
