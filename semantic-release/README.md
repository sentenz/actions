# Semantic-Release Composite Action

Automated semantic versioning and releases using [semantic-release](https://github.com/semantic-release/semantic-release).

## Description

This action runs semantic-release to automate version management and package publishing based on conventional commits.

## Inputs

| Input              | Description                            | Required | Default               |
| ------------------ | -------------------------------------- | -------- | --------------------- |
| `github-token`     | GitHub token for releases              | Yes      | `${{ github.token }}` |
| `semantic-version` | semantic-release version               | No       | `25.0.2`              |
| `branches`         | Release branches (JSON array)          | No       | `["main"]`            |
| `dry-run`          | Run in dry-run mode                    | No       | `false`               |
| `extra-plugins`    | Extra plugins to install (multiline)   | No       | See action.yml        |
| `config-file`      | Path to semantic-release configuration | No       | ``                    |

## Outputs

| Output                      | Description                         |
| --------------------------- | ----------------------------------- |
| `new-release-published`     | Whether a new release was published |
| `new-release-version`       | The new release version             |
| `new-release-major-version` | The new release major version       |
| `new-release-minor-version` | The new release minor version       |
| `new-release-patch-version` | The new release patch version       |
| `new-release-channel`       | The new release channel             |
| `new-release-notes`         | The new release notes               |
| `last-release-version`      | The last release version            |

## Usage

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
      - uses: sentenz/actions/semantic-release@main
        with:
          github-token: ${{ secrets.GITHUB_TOKEN }}
```

## External Configuration

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

## References

- [semantic-release Documentation](https://semantic-release.gitbook.io/)
- [Conventional Commits](https://www.conventionalcommits.org/)
