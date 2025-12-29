# Semantic-Release Composite Action

Automated semantic versioning and releases using semantic-release.

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
    > An open-source tool that automates the versioning and release process of software projects based on semantic versioning and conventional commits.

- [Semantic-Release Documentation](https://semantic-release.gitbook.io/)
  > Comprehensive documentation for using and configuring semantic-release.

- [Semantic-Release Configuration](https://semantic-release.gitbook.io/semantic-release/usage/configuration)
  > Configuration options for customizing semantic-release behavior.

- [Semantic-Release Plugins](https://semantic-release.gitbook.io/semantic-release/usage/plugins)
  > A list of official and community plugins for extending semantic-release functionality.

- [Conventional Commits](https://www.conventionalcommits.org/)
  > A specification for adding human and machine-readable meaning to commit messages.

- [Semantic Versioning](https://semver.org/)
  > A versioning scheme for software that conveys meaning about the underlying changes.

- [Keeping a Changelog](https://keepachangelog.com/en/1.0.0/)
  > A guide for maintaining a changelog that is easy to read and understand.

## 2. Action

The [Semantic-Release Action](./action.yml) runs semantic-release to automate version management and package publishing based on conventional commits.

### 2.1. Inputs

| Input               | Description                          | Required | Default               |
| ------------------- | ------------------------------------ | -------- | --------------------- |
| `github-token`      | GitHub token for releases            | Yes      | `${{ github.token }}` |
| `semantic-version`  | semantic-release version             | No       | `25.0.2`              |
| `branches`          | Release branches (JSON array)        | No       | `["main"]`            |
| `dry-run`           | Run in dry-run mode                  | No       | `false`               |
| `extra-plugins`     | Extra plugins to install (multiline) | No       | See action.yml        |
| `working-directory` | Working directory                    | No       | `.`                   |
| `tag-format`        | Tag format                           | No       | `${version}`          |

### 2.2. Outputs

| Output                      | Description                         |
| --------------------------- | ----------------------------------- |
| `new-release-published`     | Whether a new release was published |
| `new-release-version`       | The new release version             |
| `new-release-major-version` | The new release major version       |
| `new-release-minor-version` | The new release minor version       |
| `new-release-patch-version` | The new release patch version       |
| `new-release-channel`       | The new release channel             |
| `new-release-notes`         | The new release notes               |
| `new-release-git-head`      | The git HEAD of the new release     |
| `new-release-git-tag`       | The git tag of the new release      |
| `last-release-version`      | The last release version            |
| `last-release-git-head`     | The git HEAD of the last release    |
| `last-release-git-tag`      | The git tag of the last release     |

## 3. Usage

```yaml
jobs:
  release:
    runs-on: ubuntu-latest
    permissions:
      contents: write
      issues: write
      pull-requests: write
      id-token: write
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

<!-- TODO Loading internal configuration from actions `config/` directory in caller repository. -->

### 4.2. External Configuration

Create a `.releaserc.json` file in your repository:

```json
{
  "branches": ["main"],
  "plugins": [
    "@semantic-release/commit-analyzer",
    "@semantic-release/release-notes-generator",
    "@semantic-release/changelog",
    "@semantic-release/github",
    "@semantic-release/git"
  ]
}
```
