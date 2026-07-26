# Container Image Composite Action

Build, publish, or promote OCI container images with Docker Buildx and GitHub Container Registry (GHCR).

The action owns container build and registry mechanics. Calling workflows provide repository-specific triggers and image inputs.

## Recommended lifecycle

- Pull requests call `operation: build` for non-publishing validation.
- Published GitHub Releases call `operation: publish`.
- Release workflows provide `version: ${{ github.event.release.tag_name }}`.
- `immutable: true` prevents replacing an existing release image.
- `publish-latest` controls whether the release also updates `latest`.

## Operations

| Operation | Behavior |
| --- | --- |
| `build` | Builds the configured platforms without publishing. |
| `publish` | Builds and publishes the version tag and, when enabled, `latest`. |
| `promote` | Retags an existing immutable version as `latest` without rebuilding. |
| `auto` | Publishes on tag refs and builds on other refs. |

When `operation` is omitted, the legacy `push` input remains supported: `push: true` resolves to `publish`, and `push: false` resolves to `build`.

## Inputs

| Input | Description | Default |
| --- | --- | --- |
| `operation` | `auto`, `build`, `publish`, or `promote`; supersedes `push` when set | Empty |
| `version` | Docker image version | Context-derived where supported |
| `publish-latest` | `auto`, `true`, or `false` | `true` |
| `immutable` | Refuse to overwrite an existing version tag during publication | `false` |
| `github-token` | Token used to authenticate to GHCR | `${{ github.token }}` |
| `registry-username` | GHCR username associated with the token | `${{ github.actor }}` |
| `context` | Docker build context | `.` |
| `file` | Dockerfile path | `Dockerfile` |
| `image-name` | Package name within the calling repository owner's GHCR namespace | Repository name |
| `platforms` | Comma-separated target platforms | `linux/amd64` |
| `build-args` | Newline-delimited Docker build arguments | Empty |
| `labels` | Additional newline-delimited OCI image labels | Empty |
| `target` | Optional Dockerfile target | Empty |
| `push` | Legacy build/publish selector used when `operation` is omitted | `true` |

## Outputs

| Output | Description |
| --- | --- |
| `operation` | Resolved operation |
| `version` | Resolved version |
| `image` | Fully qualified GHCR image name |
| `tags` | Newline-delimited tags used for the operation |
| `digest` | Buildx digest for build/publish operations |

## Recommended workflow

```yaml
name: Container Image

on:
  pull_request:
  release:
    types: [published]

permissions:
  contents: read

jobs:
  validate:
    if: github.event_name == 'pull_request'
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v7
      - uses: sentenz/actions/container-image@main
        with:
          operation: build
          version: sha-${{ github.sha }}

  publish:
    if: github.event_name == 'release'
    runs-on: ubuntu-latest
    permissions:
      contents: read
      packages: write
    steps:
      - uses: actions/checkout@v7
      - uses: sentenz/actions/container-image@main
        with:
          operation: publish
          version: ${{ github.event.release.tag_name }}
          immutable: true
          github-token: ${{ secrets.GITHUB_TOKEN }}
```

## Publication guarantees

With `immutable: true`, the action checks GHCR before publishing and fails when the requested version already exists. Promotion verifies that the immutable source version exists before retagging it as `latest`.

Published builds enable maximum provenance and SBOM generation. Build-only validation disables attestations. The action applies standard OCI source, URL, version, and revision labels and accepts additional labels through `labels`.

## Latest-tag policy

`publish-latest` accepts:

- `true`: publish `latest` with the version;
- `false`: publish only the version;
- `auto`: publish `latest` only for stable semantic-version tag contexts.

## Permissions

Build-only operations require `contents: read`. Publish and promote operations additionally require `packages: write`. A composite action cannot grant workflow permissions.

## Validation

The action rejects unsupported operations, invalid versions, malformed image names, invalid policy inputs, missing registry credentials for write operations, and empty build configuration.
