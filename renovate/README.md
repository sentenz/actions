# Renovate Composite Action

Run self-hosted Renovate with explicit authentication and repository selection.

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Authentication](#22-authentication)
- [3. Usage](#3-usage)
  - [3.1. Personal Access Token](#31-personal-access-token)
  - [3.2. GitHub App Installation Token](#32-github-app-installation-token)
- [4. Configuration](#4-configuration)
  - [4.1. Repository Configuration](#41-repository-configuration)
  - [4.2. Self-hosted Configuration](#42-self-hosted-configuration)

## 1. Details

- [Renovate](https://github.com/renovatebot/renovate)
  > An open-source dependency automation tool that creates pull requests for dependency updates.

- [Renovate Documentation](https://docs.renovatebot.com/)
  > Configuration, managers, datasources, and self-hosting guidance.

- [Renovate GitHub Action](https://github.com/renovatebot/github-action)
  > The upstream action used by this composite action.

## 2. Action

The [Renovate Action](./action.yml) validates authentication, selects either the current repository or autodiscovery, and delegates execution to `renovatebot/github-action`.

### 2.1. Inputs

| Input          | Description                                              | Required | Default  |
| -------------- | -------------------------------------------------------- | -------- | -------- |
| `token`        | Platform access token used by Renovate                   | Yes      | —        |
| `autodiscover` | Discover all repositories accessible to the token        | No       | `false`  |
| `config-file`  | Path to a self-hosted/global Renovate configuration file | No       | ``       |
| `platform`     | Renovate platform identifier                             | No       | `github` |
| `log-level`    | Renovate log level                                       | No       | `info`   |

The Renovate CLI version is delegated to `renovatebot/github-action` and follows the default provided by the pinned upstream action release.

### 2.2. Authentication

For GitHub, provide either:

- a dedicated personal access token; or
- a GitHub App installation token generated in the caller workflow.

GitHub's workflow `GITHUB_TOKEN` is intentionally rejected. It is too restrictive for self-hosted Renovate and PRs or pushes created with it do not trigger normal downstream workflow events.

Store a personal access token as an Actions secret such as `RENOVATE_TOKEN`. A classic token generally requires `repo` for repository access and `workflow` when Renovate must update files under `.github/workflows`. For a fine-grained token, grant the selected repositories the Renovate permissions required for contents, pull requests, issues, and commit statuses.

The caller workflow may retain `permissions: contents: read`; those permissions apply to the workflow's `GITHUB_TOKEN`, not to the dedicated token supplied to Renovate.

## 3. Usage

### 3.1. Personal Access Token

With `autodiscover: "false"`, the action limits Renovate to `${{ github.repository }}`.

```yaml
name: Renovate

on:
  workflow_dispatch:
  schedule:
    - cron: "0 4 * * 0"

concurrency:
  group: renovate
  cancel-in-progress: false

permissions:
  contents: read

jobs:
  renovate:
    name: Renovate
    runs-on: ubuntu-latest
    timeout-minutes: 30

    steps:
      - name: Checkout
        uses: actions/checkout@3d3c42e5aac5ba805825da76410c181273ba90b1 # v7.0.1
        with:
          persist-credentials: false

      - name: Run Renovate
        uses: sentenz/actions/renovate@latest
        with:
          token: ${{ secrets.RENOVATE_TOKEN }}
          autodiscover: "false"
          platform: github
          log-level: info
```

### 3.2. GitHub App Installation Token

Configure the GitHub App with access to the target repositories and the repository permissions Renovate requires.

```yaml
steps:
  - name: Create Renovate token
    id: renovate-token
    uses: actions/create-github-app-token@v2
    with:
      app-id: ${{ secrets.RENOVATE_APP_ID }}
      private-key: ${{ secrets.RENOVATE_APP_PRIVATE_KEY }}
      owner: ${{ github.repository_owner }}
      repositories: ${{ github.event.repository.name }}

  - name: Checkout
    uses: actions/checkout@3d3c42e5aac5ba805825da76410c181273ba90b1 # v7.0.1
    with:
      persist-credentials: false

  - name: Run Renovate
    uses: sentenz/actions/renovate@latest
    with:
      token: ${{ steps.renovate-token.outputs.token }}
      autodiscover: "false"
      platform: github
```

## 4. Configuration

### 4.1. Repository Configuration

Renovate automatically reads a repository configuration file such as `renovate.json`, `renovate.json5`, or `renovate.jsonc` from the target repository. This file controls dependency rules for that repository and does not need to be supplied through `config-file`.

```json
{
  "$schema": "https://docs.renovatebot.com/renovate-schema.json",
  "extends": ["config:recommended"]
}
```

### 4.2. Self-hosted Configuration

Use `config-file` only for Renovate's self-hosted/global configuration:

```yaml
- name: Run Renovate
  uses: sentenz/actions/renovate@latest
  with:
    token: ${{ secrets.RENOVATE_TOKEN }}
    config-file: renovate/config/renovate.json
```

The path is resolved from the checked-out caller repository. Checkout is therefore required when `config-file` references a repository file.
