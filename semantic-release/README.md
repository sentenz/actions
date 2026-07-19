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

When no semantic-release configuration exists and `use-default-config` is `true`, the action temporarily copies [`config/release.config.cjs`](./config/release.config.cjs) into the working directory.

The internal configuration supports the `main`, `next`, `beta`, and `alpha` release branches, generates `CHANGELOG.md` and GitHub releases, and explicitly parses breaking-change headers in both forms:

```text
refactor!: replace the deployment backend
```

```text
refactor(scope)!: replace the deployment backend
```

Set `use-default-config: "false"` to require an external repository configuration.

### 4.2. External Configuration

Create a `release.config.cjs` file in your repository:

```js
const parserOpts = {
  headerPattern: /^(\w*)(?:\((.*)\))?!?: (.*)$/,
  breakingHeaderPattern: /^(\w*)(?:\((.*)\))?!: (.*)$/,
  headerCorrespondence: ["type", "scope", "subject"],
  noteKeywords: ["BREAKING CHANGE", "BREAKING-CHANGE"],
};

module.exports = {
  branches: ["main"],
  plugins: [
    [
      "@semantic-release/commit-analyzer",
      {
        preset: "conventionalcommits",
        parserOpts,
      },
    ],
    [
      "@semantic-release/release-notes-generator",
      {
        preset: "conventionalcommits",
        parserOpts,
      },
    ],
    "@semantic-release/changelog",
    "@semantic-release/github",
    "@semantic-release/git",
  ],
};
```

A preset declaration alone does not reliably parse bang-marked breaking headers in current `@semantic-release/commit-analyzer` releases. The explicit parser options work around [semantic-release/commit-analyzer#759](https://github.com/semantic-release/commit-analyzer/issues/759).

Avoid negative rules such as `{ type: "refactor", release: false }` when the same type may contain a breaking marker. Such rules can suppress the major release; see [semantic-release/commit-analyzer#805](https://github.com/semantic-release/commit-analyzer/issues/805).

A `BREAKING CHANGE:` footer remains the most portable form:

```text
refactor: replace the deployment backend

BREAKING CHANGE: local development now requires Kind instead of K3s.
```
