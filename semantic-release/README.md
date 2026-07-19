# Semantic-Release Composite Action

Run [semantic-release](https://github.com/semantic-release/semantic-release) with validated inputs and Conventional Commits defaults.

- [1. Behavior](#1-behavior)
- [2. Inputs](#2-inputs)
- [3. Outputs](#3-outputs)
- [4. Usage](#4-usage)
- [5. Configuration](#5-configuration)
  - [5.1. Repository configuration](#51-repository-configuration)
  - [5.2. Default configuration](#52-default-configuration)
  - [5.3. Breaking changes](#53-breaking-changes)

## 1. Behavior

The action wraps [`cycjimmy/semantic-release-action`](https://github.com/cycjimmy/semantic-release-action) and:

- validates boolean inputs and the working directory;
- preserves repository-level semantic-release settings unless an explicit override is supplied;
- installs the Conventional Commits preset and the changelog and Git plugins by default;
- uses [`config/.releaserc.json`](./config/.releaserc.json) when no semantic-release configuration exists in the working directory;
- removes the temporary fallback configuration after semantic-release completes.

A full Git history is required so semantic-release can inspect tags and commits.

## 2. Inputs

| Input | Description | Required | Default |
| --- | --- | --- | --- |
| `github-token` | GitHub token used by semantic-release | No | `${{ github.token }}` |
| `semantic-version` | semantic-release version or version range | No | `25` |
| `branches` | Release branches; empty preserves repository configuration | No | Empty |
| `dry-run` | Override `dryRun`: `true`, `false`, or empty | No | Empty |
| `ci` | Override CI mode: `true`, `false`, or empty | No | Empty |
| `unset-gha-env` | Unset `GITHUB_ACTIONS` before semantic-release runs | No | `false` |
| `extends` | Shareable semantic-release configurations, one package per line | No | Empty |
| `extra-plugins` | Additional npm packages installed before semantic-release | No | See `action.yml` |
| `use-default-config` | Use the bundled configuration when the repository has no release configuration | No | `true` |
| `working-directory` | Repository-relative directory in which semantic-release runs | No | `.` |
| `tag-format` | Override `tagFormat`; empty preserves repository configuration | No | Empty |
| `repository-url` | Override the Git repository URL | No | Empty |

Supplying `branches`, `dry-run`, `ci`, `tag-format`, or `repository-url` overrides the corresponding repository configuration. Leaving these inputs empty allows semantic-release to resolve them from the repository configuration or its own defaults.

When `extra-plugins` is replaced, include every package required by the active semantic-release configuration. The bundled configuration requires `@semantic-release/changelog`, `@semantic-release/git`, and `conventional-changelog-conventionalcommits`.

## 3. Outputs

| Output | Description |
| --- | --- |
| `new-release-published` | Whether a new release was published |
| `new-release-version` | New release version |
| `new-release-major-version` | New release major version |
| `new-release-minor-version` | New release minor version |
| `new-release-patch-version` | New release patch version |
| `new-release-channel` | New release distribution channel |
| `new-release-notes` | Generated release notes |
| `new-release-git-head` | Git commit included in the new release |
| `new-release-git-tag` | Git tag associated with the new release |
| `last-release-version` | Previous release version |
| `last-release-git-head` | Git commit associated with the previous release |
| `last-release-git-tag` | Git tag associated with the previous release |

## 4. Usage

```yaml
jobs:
  release:
    runs-on: ubuntu-latest
    permissions:
      contents: write
      issues: write
      pull-requests: write
    steps:
      - name: Checkout full history
        uses: actions/checkout@v6
        with:
          fetch-depth: 0

      - name: Semantic Release
        id: release
        uses: sentenz/actions/semantic-release@latest
        with:
          github-token: ${{ secrets.GITHUB_TOKEN }}
```

For protected branches, use credentials permitted to push release commits and tags. When using `@semantic-release/git` with an alternative token, configure checkout credentials accordingly.

## 5. Configuration

### 5.1. Repository configuration

Repository configuration takes precedence. Supported semantic-release configuration locations include:

- `.releaserc`, `.releaserc.json`, `.releaserc.yaml`, `.releaserc.yml`, `.releaserc.js`, or `.releaserc.cjs`;
- `release.config.js` or `release.config.cjs`;
- the `release` property in `package.json`.

Example:

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

Set `use-default-config: "false"` to require an explicit repository configuration.

### 5.2. Default configuration

When no repository configuration exists and `use-default-config` is `true`, the action temporarily copies [`config/.releaserc.json`](./config/.releaserc.json) into the working directory.

The bundled configuration:

- releases `feat` commits as minor versions;
- releases `fix`, `perf`, and `revert` commits as patch versions;
- releases breaking changes as major versions;
- supports `main`, `next`, `beta`, and `alpha` release branches;
- generates `CHANGELOG.md` and GitHub releases.

### 5.3. Breaking changes

The Conventional Commits preset recognizes both breaking-change forms:

```text
refactor!: replace the deployment backend
```

```text
refactor: replace the deployment backend

BREAKING CHANGE: local development now requires Kind instead of K3s.
```

For the most portable commit history, include a `BREAKING CHANGE:` footer even when the `!` marker is present.
