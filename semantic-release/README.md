# Semantic-Release Composite Action

Automated semantic versioning and releases using [semantic-release](https://github.com/semantic-release/semantic-release).

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
- [4. Configuration](#4-configuration)
  - [4.1. Internal Configuration](#41-internal-configuration)
  - [4.2. External Configuration](#42-external-configuration)

## 1. Details

- [Semantic-Release](https://github.com/semantic-release/semantic-release)
    > An open-source tool that automates versioning and releases based on Semantic Versioning and Conventional Commits.

- [Semantic-Release Configuration](https://semantic-release.gitbook.io/semantic-release/usage/configuration)
    > Configuration options for release branches, tags, plugins, and repository-specific behavior.

- [Conventional Commits](https://www.conventionalcommits.org/)
    > A specification for adding human-readable and machine-readable meaning to commit messages.

- [Semantic Versioning](https://semver.org/)
    > A versioning scheme that communicates compatibility through major, minor, and patch versions.

## 2. Action

The [Semantic-Release Action](./action.yml) runs semantic-release with validated inputs, repository-level configuration support, and Conventional Commits defaults.

### 2.1. Inputs

| Input                | Description                                                        | Required | Default               |
| -------------------- | ------------------------------------------------------------------ | -------- | --------------------- |
| `github-token`       | GitHub token used by semantic-release                              | No       | `${{ github.token }}` |
| `semantic-version`   | semantic-release version or version range                          | No       | `25`                  |
| `branches`           | Release branches; empty preserves repository configuration         | No       | ``                    |
| `dry-run`            | Override dry-run mode (`true`, `false`, or empty)                   | No       | ``                    |
| `ci`                 | Override CI mode (`true`, `false`, or empty)                        | No       | ``                    |
| `unset-gha-env`      | Unset the `GITHUB_ACTIONS` environment variable                    | No       | `false`               |
| `extends`            | Shareable semantic-release configurations (multiline)              | No       | ``                    |
| `extra-plugins`      | Additional npm packages installed before semantic-release          | No       | See `action.yml`      |
| `use-default-config` | Use the internal configuration when no repository config is found  | No       | `true`                |
| `working-directory`  | Repository-relative directory in which semantic-release runs       | No       | `.`                   |
| `tag-format`         | Override the tag format; empty preserves repository configuration  | No       | ``                    |
| `repository-url`     | Override the Git repository URL                                    | No       | ``                    |

### 2.2. Outputs

| Output                      | Description                         |
| --------------------------- | ----------------------------------- |
| `new-release-published`     | Whether a new release was published |
| `new-release-version`       | New release version                 |
| `new-release-major-version` | New release major version           |
| `new-release-minor-version` | New release minor version           |
| `new-release-patch-version` | New release patch version           |
| `new-release-channel`       | New release distribution channel    |
| `new-release-notes`         | Generated release notes             |
| `new-release-git-head`      | Git commit included in the release  |
| `new-release-git-tag`       | Git tag associated with the release |
| `last-release-version`      | Previous release version            |
| `last-release-git-head`     | Previous release Git commit         |
| `last-release-git-tag`      | Previous release Git tag            |

## 3. Usage

```yaml
jobs:
  release:
    runs-on: ubuntu-latest
    permissions:
      contents: write
      issues: write
      pull-requests: write
    steps:
      - uses: actions/checkout@v6.0.1
        with:
          fetch-depth: 0
      - uses: sentenz/actions/semantic-release@latest
        with:
          github-token: ${{ secrets.GITHUB_TOKEN }}
```

## 4. Configuration

### 4.1. Internal Configuration

When no semantic-release configuration exists and `use-default-config` is `true`, the action temporarily copies [`config/.releaserc.json`](./config/.releaserc.json) into the working directory.

The internal configuration uses the `conventionalcommits` preset, supports the `main`, `next`, `beta`, and `alpha` release branches, and generates `CHANGELOG.md` and GitHub releases.

Set `use-default-config: "false"` to require an external repository configuration.

### 4.2. External Configuration

Create a `.releaserc.json` file in your repository:

```json
{
  "branches": ["main"],
  "plugins": [
    [
      "@semantic-release/commit-analyzer",
      {
        "preset": "conventionalcommits"
      }
    ],
    [
      "@semantic-release/release-notes-generator",
      {
        "preset": "conventionalcommits"
      }
    ],
    "@semantic-release/changelog",
    "@semantic-release/github",
    "@semantic-release/git"
  ]
}
```

When `extra-plugins` is overridden, include all packages required by the active semantic-release configuration. The internal configuration requires `@semantic-release/changelog`, `@semantic-release/git`, and `conventional-changelog-conventionalcommits`.
