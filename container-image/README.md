# Container Image Composite Action

Build, publish, and promote OCI container images with Docker Buildx and GitHub Container Registry (GHCR).

The action owns container-release mechanics so calling workflows only declare triggers, permissions, and image-specific build inputs.

## Operations

| Operation | Behavior |
| --- | --- |
| `build` | Builds the configured platforms without publishing. |
| `publish` | Builds and publishes the immutable version tag and, when enabled, `latest`. |
| `promote` | Retags an existing immutable version as `latest` without rebuilding. |
| `auto` | Publishes on tag refs and builds on other refs. |

When `operation` is omitted, the legacy `push` input remains supported: `push: true` resolves to `publish`, and `push: false` resolves to `build`.

## Inputs

| Input | Description | Default |
| --- | --- | --- |
| `operation` | `auto`, `build`, `publish`, or `promote`; supersedes `push` when set | `` |
| `version` | Docker tag. `auto`/`build` derives `sha-<commit>` when omitted; tag refs derive the ref name | `` |
| `publish-latest` | `auto`, `true`, or `false`. `auto` publishes `latest` only for stable semantic-version tag refs | `true` |
| `immutable` | Refuse to overwrite an existing version tag during publication | `false` |
| `github-token` | Token used to authenticate to GHCR | `${{ github.token }}` |
| `registry-username` | GHCR username associated with the token | `${{ github.actor }}` |
| `context` | Docker build context | `.` |
| `file` | Dockerfile path | `Dockerfile` |
| `image-name` | Package name within the calling repository owner's GHCR namespace | Calling repository name |
| `platforms` | Comma-separated target platforms | `linux/amd64` |
| `build-args` | Newline-delimited Docker build arguments | `` |
| `labels` | Additional newline-delimited OCI image labels | `` |
| `target` | Optional Dockerfile target | `` |
| `push` | Legacy build/publish selector used only when `operation` is omitted | `true` |

## Outputs

| Output | Description |
| --- | --- |
| `operation` | Resolved operation |
| `version` | Resolved version |
| `image` | Fully qualified GHCR image name |
| `tags` | Newline-delimited tags used for the operation |
| `digest` | Buildx digest for build/publish operations; empty for promotion-only operations |

## Recommended workflow

```yaml
name: Container Image

on:
  pull_request:
  push:
    branches: [main]
    tags: ["v*.*.*"]
  workflow_dispatch:
    inputs:
      operation:
        type: choice
        options: [build, publish, promote]
        default: build
      version:
        type: string
        required: false
      publish_latest:
        type: choice
        options: [auto, "true", "false"]
        default: auto

permissions:
  contents: read
  packages: write

jobs:
  image:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@9c091bb21b7c1c1d1991bb908d89e4e9dddfe3e0 # v7.0.0

      - uses: sentenz/actions/container-image@main
        with:
          operation: ${{ inputs.operation || 'auto' }}
          version: ${{ inputs.version }}
          publish-latest: ${{ inputs.publish_latest || 'auto' }}
          immutable: true
          platforms: linux/amd64,linux/arm64
          github-token: ${{ secrets.GITHUB_TOKEN }}
```

For normal pull-request and branch runs, `auto` performs a build-only validation. Tag runs publish the tag and update `latest` only for stable semantic versions. Manual runs explicitly select build, publish, or promotion behavior.

## Publication guarantees

With `immutable: true`, the action checks GHCR before publishing and fails when the requested version tag already exists. Promotion verifies that the immutable source tag exists before retagging it as `latest`.

Published builds enable maximum provenance and SBOM generation. Build-only validation disables attestations. The action applies standard OCI source, URL, version, and revision labels and accepts additional labels through `labels`. Buildx cache storage is scoped by the resolved image name.

## Permissions

Build-only operations require:

```yaml
permissions:
  contents: read
```

Publish and promote operations additionally require:

```yaml
permissions:
  packages: write
```

A composite action cannot grant workflow permissions. The supplied token must have access to the calling repository and its GHCR package namespace.

## Validation

The action rejects:

- unsupported operations;
- missing explicit versions for manual publish and promote operations;
- invalid Docker tags or the reserved `latest` version;
- invalid `publish-latest`, `immutable`, or legacy `push` values;
- malformed package names containing registry, owner, tag, or digest components;
- missing registry credentials for publish and promote operations;
- empty build context, Dockerfile, or platforms for build and publish operations.
