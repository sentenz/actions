# Container Image Action

Build an OCI container image with Docker Buildx and optionally publish it to the calling repository owner's GitHub Container Registry namespace. A publishing run applies both `latest` and the supplied current version to the same build result.

## Features

- Uses the calling repository name as the default GHCR package name.
- Supports an optional custom package name under the same owner namespace.
- Publishes `latest` and the supplied version tag from one Buildx invocation.
- Supports multi-platform builds, build arguments, Dockerfile targets, and GitHub Actions cache storage.
- Provides build-only validation through `push: false`.
- Pins all third-party actions to immutable commit SHAs.

## Required workflow permissions

Publishing requires package write access in the calling workflow:

```yaml
permissions:
  contents: read
  packages: write
```

A composite action cannot grant these permissions. Build-only jobs generally require only `contents: read`.

## Inputs

| Name | Required | Default | Description |
| --- | --- | --- | --- |
| `github-token` | No | `${{ github.token }}` | Token used to authenticate to `ghcr.io`. |
| `registry-username` | No | `${{ github.actor }}` | GHCR username associated with `github-token`. |
| `version` | Yes | — | Current Docker tag, such as `1.4.2` or `v1.4.2`. The value must differ from `latest`. |
| `context` | No | `.` | Docker build context. |
| `file` | No | `Dockerfile` | Path to the Dockerfile. |
| `image-name` | No | Repository name | GHCR package name without registry, owner, tag, or digest. |
| `platforms` | No | `linux/amd64` | Comma-separated target platforms. |
| `build-args` | No | Empty | Newline-delimited Docker build arguments. |
| `target` | No | Empty | Optional Dockerfile build target. |
| `push` | No | `true` | Publish to GHCR. Set to `false` for build-only validation. |

## Outputs

| Name | Description |
| --- | --- |
| `image` | Fully qualified resolved image name, such as `ghcr.io/sentenz/example`. |
| `tags` | Newline-delimited `latest` and version tags. |
| `digest` | Image manifest digest returned by Docker Buildx. This may be empty for some build-only exports. |

## Image naming

The action always derives the registry owner from `${{ github.repository_owner }}`.

With no `image-name`, the calling repository name is used:

```text
ghcr.io/<owner>/<repository>:latest
ghcr.io/<owner>/<repository>:<version>
```

With `image-name: api`, the custom package remains in the same owner namespace:

```text
ghcr.io/<owner>/api:latest
ghcr.io/<owner>/api:<version>
```

Owner and package names are normalized to lowercase. `image-name` accepts only a package-name component; values containing a registry, owner path, tag, or digest are rejected before authentication and build execution.

## Usage

### Publish using the repository name

```yaml
name: Publish Container Image

on:
  release:
    types: [published]

permissions:
  contents: read
  packages: write

jobs:
  publish:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout
        uses: actions/checkout@9c091bb21b7c1c1d1991bb908d89e4e9dddfe3e0 # v7.0.0

      - name: Build and publish image
        id: image
        uses: sentenz/actions/container-image@main
        with:
          version: ${{ github.event.release.tag_name }}
          github-token: ${{ secrets.GITHUB_TOKEN }}

      - name: Report digest
        run: echo "${{ steps.image.outputs.digest }}"
```

### Publish with a custom image name

```yaml
- name: Build and publish API image
  uses: sentenz/actions/container-image@main
  with:
    version: ${{ github.event.release.tag_name }}
    image-name: api
    github-token: ${{ secrets.GITHUB_TOKEN }}
```

### Build without publishing

Use build-only mode for pull requests and other untrusted or non-release events:

```yaml
permissions:
  contents: read

steps:
  - name: Checkout
    uses: actions/checkout@9c091bb21b7c1c1d1991bb908d89e4e9dddfe3e0 # v7.0.0

  - name: Validate container build
    uses: sentenz/actions/container-image@main
    with:
      version: 0.0.0-pr
      push: false
```

### Multi-platform build

```yaml
- name: Build and publish multi-platform image
  uses: sentenz/actions/container-image@main
  with:
    version: ${{ github.event.release.tag_name }}
    platforms: linux/amd64,linux/arm64
    build-args: |
      APP_VERSION=${{ github.event.release.tag_name }}
      VCS_REF=${{ github.sha }}
    github-token: ${{ secrets.GITHUB_TOKEN }}
```

### Dockerfile target

```yaml
- name: Build production target
  uses: sentenz/actions/container-image@main
  with:
    version: ${{ github.event.release.tag_name }}
    target: production
    github-token: ${{ secrets.GITHUB_TOKEN }}
```

## GHCR package access

The package is created under the calling repository owner's GHCR namespace. Package visibility and repository access are managed in GitHub package settings. Organization policies can restrict package creation or visibility. For private repositories, the supplied token must have access to the repository and permission to write packages.

The default `${{ github.token }}` is normally sufficient when the calling workflow declares `packages: write`. A personal access token is necessary only when publishing across permission boundaries not covered by the workflow token. When supplying a PAT, set `registry-username` to the account that owns that token.

## Validation

The action fails before authentication or build execution when:

- `version`, `context`, `file`, or `platforms` is empty;
- `version` is not a valid Docker tag or resolves to `latest`;
- `push` is not exactly `true` or `false`; or
- `registry-username` is empty while `push` is `true`; or
- `image-name` contains a registry prefix, owner path, tag, digest, whitespace, or unsupported characters.

## References

- [Publishing Docker images](https://docs.github.com/actions/publishing-packages/publishing-docker-images)
- [Working with the Container registry](https://docs.github.com/packages/working-with-a-github-packages-registry/working-with-the-container-registry)
- [Docker Build Push Action](https://github.com/docker/build-push-action)
