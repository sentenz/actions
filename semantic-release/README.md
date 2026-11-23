# Semantic-Release Composite Action

Automated semantic versioning and releases using [semantic-release](https://github.com/semantic-release/semantic-release).

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Dependencies](#21-dependencies)
  - [2.2. Inputs](#22-inputs)
  - [2.3. Outputs](#23-outputs)
  - [2.4. Permissions](#24-permissions)
- [3. Usage](#3-usage)
  - [3.1. Using the workflow-provided `GITHUB_TOKEN`](#31-using-the-workflow-provided-github_token)
  - [3.2. Using `RELEASE_PAT` for automatic tag-triggered workflows](#32-using-release_pat-for-automatic-tag-triggered-workflows)
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

- [Keeping a Changelog](https://keepachangelog.com/en/1.0.0/)
  > A guide for maintaining a changelog that is easy to read and understand.

## 2. Action

### 2.1. Dependencies

| Actions | Reference | Purpose |
| --- | --- | --- |
| [cycjimmy/semantic-release-action](https://github.com/cycjimmy/semantic-release-action) | `cycjimmy/semantic-release-action` | Run semantic-release to determine versions and publish releases. |

### 2.2. Inputs

| Input                | Description                                                                                                       | Required | Default               |
| -------------------- | ----------------------------------------------------------------------------------------------------------------- | -------- | --------------------- |
| `github-token`       | GitHub token used by semantic-release                                                                             | No       | `${{ github.token }}` |
| `semantic-version`   | semantic-release version or version range                                                                         | No       | `25`                  |
| `branches`           | Release branches; empty preserves repository configuration                                                        | No       | ``                    |
| `dry-run`            | Override dry-run mode (`true`, `false`, or empty)                                                                 | No       | ``                    |
| `ci`                 | Override CI mode (`true`, `false`, or empty)                                                                      | No       | ``                    |
| `unset-gha-env`      | Unset the `GITHUB_ACTIONS` environment variable                                                                   | No       | `false`               |
| `extends`            | Shareable semantic-release configurations (multiline)                                                             | No       | ``                    |
| `extra-plugins`      | Additional npm packages installed before semantic-release                                                         | No       | See `action.yml`      |
| `use-default-config` | Use the internal configuration when no external config is found                                                   | No       | `true`                |
| `working-directory`  | Repository-relative directory in which semantic-release runs. CR/LF characters are rejected before release steps. | No       | `.`                   |
| `tag-format`         | Override the tag format; empty preserves repository configuration                                                 | No       | ``                    |
| `repository-url`     | Override the Git repository URL                                                                                   | No       | ``                    |

### 2.3. Outputs

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

### 2.4. Permissions

The `github-token` input is forwarded to semantic-release as `GITHUB_TOKEN` for Git authentication and GitHub API operations. The **credential supplied to that input** must be able to create tags/releases and, when `@semantic-release/git` is enabled, push release commits.

The caller's `permissions:` block controls **only the workflow-provided `GITHUB_TOKEN`**. It does **not** grant permissions to a Personal Access Token (PAT) or GitHub App installation token; those are configured independently.

#### Workflow-provided `GITHUB_TOKEN`

When passing `${{ secrets.GITHUB_TOKEN }}` (or using the action's default), grant permissions appropriate to the enabled plugins:

| Permission | Access | When needed |
| --- | --- | --- |
| `contents` | `write` | Create Git tags, GitHub Releases, and release commits. |
| `issues` | `write` | Comment on or update released issues with `@semantic-release/github`. |
| `pull-requests` | `write` | Comment on or update related pull requests with `@semantic-release/github`. |

```yaml
jobs:
  release:
    permissions:
      contents: write
      issues: write
      pull-requests: write
```

> [!NOTE]
> Workflow-triggering Limitation
>
> Tags or commits pushed using the workflow-provided `GITHUB_TOKEN` do not trigger subsequent `push` workflows. To automatically start tag-based publication workflows, authenticate the release push with a PAT or GitHub App installation token, or explicitly trigger the next workflow using `workflow_dispatch` / `repository_dispatch`. See [GitHub's workflow-triggering documentation](https://docs.github.com/en/actions/how-tos/write-workflows/choose-when-workflows-run/trigger-a-workflow).

#### Fine-grained Personal Access Token (PAT)

For automatic downstream `push.tags` workflows, create a fine-grained PAT, restrict it to the release repository, and store it as an Actions repository secret such as `RELEASE_PAT`.

| Repository permission | Access | When needed |
| --- | --- | --- |
| **Contents** | **Read and write** | Required to push release tags/commits and create GitHub Releases. |
| Issues | Read and write | When the release configuration updates or comments on issues. |
| Pull requests | Read and write | When the release configuration updates or comments on pull requests. |
| Workflows | Read and write | Only if the release commit itself modifies GitHub Actions workflow files; **not** required merely to trigger existing workflows by pushing a tag. |

GitHub grants fine-grained PATs `Metadata: read` automatically. For classic PATs, `repo` (private repositories) or `public_repo` (public repositories) is generally required instead. The token owner's access and the repository's branch/tag rulesets must also authorize release pushes. A PAT does not bypass branch protections automatically.

When an external token is used, the job can keep the built-in `GITHUB_TOKEN` at `contents: read` for checkout; the external token supplies release write access. Prefer narrowly scoped, short-lived credentials and rotate stored PATs. See [semantic-release authentication guidance](https://semantic-release.gitbook.io/semantic-release/usage/ci-configuration).

#### GitHub App installation token

A GitHub App installation token is another option for automatic downstream `push` workflows. Install the App on the release repository and grant **Contents: read and write**, plus **Issues** and **Pull requests: read and write** when the release plugins need them. **Workflows: read and write** is only needed when the release process modifies workflow files, not for ordinary tag creation.

GitHub App permissions are granted to the App installation and cannot be increased using the calling job's `permissions:` block. For repositories with protected release branches, configure the applicable ruleset permissions or an approved App bypass explicitly. Short-lived installation tokens are preferable to long-lived PATs where feasible.

## 3. Usage

### 3.1. Using the workflow-provided `GITHUB_TOKEN`

Suitable when downstream `push` workflows are not required, or when they are dispatched explicitly:

```yaml
jobs:
  release:
    runs-on: ubuntu-latest
    permissions:
      contents: write
      issues: write
      pull-requests: write
    steps:
      - uses: actions/checkout@3d3c42e5aac5ba805825da76410c181273ba90b1 # v7.0.1
        with:
          fetch-depth: 0
          persist-credentials: false
      - uses: sentenz/actions/semantic-release@latest
        with:
          github-token: ${{ secrets.GITHUB_TOKEN }}
```

### 3.2. Using `RELEASE_PAT` for automatic tag-triggered workflows

A tag created using `RELEASE_PAT` can trigger separately defined `on: push: tags:` workflows:

```yaml
jobs:
  release:
    runs-on: ubuntu-latest
    permissions:
      contents: read
    steps:
      - uses: actions/checkout@3d3c42e5aac5ba805825da76410c181273ba90b1 # v7.0.1
        with:
          fetch-depth: 0
          persist-credentials: false
      - uses: sentenz/actions/semantic-release@latest
        with:
          github-token: ${{ secrets.RELEASE_PAT }}
```

`persist-credentials: false` prevents the checkout step's built-in token credentials from taking precedence over the release credential. The composite action passes the supplied token to semantic-release for both Git and GitHub API authentication. If the release configuration commits generated files back to a protected branch, that push must also be allowed by the branch's protection rules.

## 4. Configuration

### 4.1. Internal Configuration

When `use-default-config` is `true`, the action uses [`config/.releaserc.json`](./config/.releaserc.json) only when no semantic-release configuration exists in the working directory or its repository ancestors and no `extends` input is supplied.

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
