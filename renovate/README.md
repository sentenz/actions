# Renovate Composite Action

Automated dependency updates using Renovate.

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
  - [3.1. Internal Configuration](#31-internal-configuration)
  - [3.2. External Configuration](#32-external-configuration)

## 1. Details

- [Renovate](https://github.com/renovatebot/renovate)
    > An open-source tool that automates the process of updating dependencies in software projects by creating pull requests for version updates.

- [Renovate Documentation](https://docs.renovatebot.com/)
  > Comprehensive documentation for using and configuring Renovate.

- [Renovate Configuration Options](https://docs.renovatebot.com/configuration-options/)
  > A detailed list of configuration options available for customizing Renovate's behavior.

- [Renovate Managers](https://docs.renovatebot.com/modules/manager/)
  > A list of supported package managers and ecosystems that Renovate manage.

## 2. Action

The [Renovate Action](./renovate/action.yml) runs Renovate to automatically create pull requests for dependency updates across your repositories.

### 2.1. Inputs

| Input            | Description                                       | Required | Default                                                 |
| ---------------- | ------------------------------------------------- | -------- | ------------------------------------------------------- |
| `token`          | GitHub token for Renovate                         | Yes      | `${{ github.token }}`                                   |
| `renovate-image` | Renovate Docker image with version tag and digest | No       | `ghcr.io/renovatebot/renovate:42.64.1@sha256:e09...65a` |
| `autodiscover`   | Autodiscover repositories                         | No       | `true`                                                  |
| `config-file`    | Path to renovate configuration file               | No       | ``                                                      |
| `platform`       | Platform to run on (github, gitlab, etc.)         | No       | `github`                                                |
| `dry-run`        | Run in dry-run mode                               | No       | `false`                                                 |
| `log-level`      | Log level (debug, info, warn, error)              | No       | `info`                                                  |

### 2.2. Outputs

| Output      | Description               |
| ----------- | ------------------------- |
| `result`    | Renovate execution result |
| `exit-code` | Renovate exit code        |

## 3. Usage

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
      - uses: sentenz/actions/renovate@latest
        with:
          token: ${{ secrets.GITHUB_TOKEN }}
          autodiscover: "true"
```

### 3.1. Internal Configuration

<!-- TODO Loading internal configuration from actions `config/` directory in caller repository. -->

### 3.2. External Configuration

Create a `renovate.json` file in your repository:

```json
{
  "$schema": "https://docs.renovatebot.com/renovate-schema.json",
  "extends": ["config:recommended"]
}
```
