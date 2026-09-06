# Docker Composite Action

Build OCI container images with Docker Buildx and optionally publish versioned and `latest` tags to GitHub Container Registry (GHCR).

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
  - [3.1. Publish Using the Repository Name](#31-publish-using-the-repository-name)
  - [3.2. Publish Using a Custom Image Name](#32-publish-using-a-custom-image-name)
  - [3.3. Build Without Publishing](#33-build-without-publishing)
  - [3.4. Build for Multiple Platforms](#34-build-for-multiple-platforms)
  - [3.5. Build a Dockerfile Target](#35-build-a-dockerfile-target)
- [4. Configuration](#4-configuration)
  - [4.1. Workflow Permissions](#41-workflow-permissions)
  - [4.2. Image Naming](#42-image-naming)
  - [4.3. Registry Authentication](#43-registry-authentication)
  - [4.4. Validation](#44-validation)

## 1. Details

- [Docker Buildx](https://docs.docker.com/build/buildx/)
  > A Docker CLI extension for advanced builds, including multi-platform images, build caching, and BuildKit features.

- [GitHub Container Registry](https://docs.github.com/packages/working-with-a-github-packages-registry/working-with-the-container-registry)
  > GitHub's OCI-compatible container registry for publishing and managing container images.

- [Publishing Docker Images](https://docs.github.com/actions/publishing-packages/publishing-docker-images)
  > GitHub Actions guidance for authenticating, building, and publishing container images.

- [Docker Build Push Action](https://github.com/docker/build-push-action)
  > A GitHub Action for building and publishing container images with Docker Buildx.

Scalar metadata inputs and working-directory paths must be single-line values. CR/LF characters are rejected before downstream publishing or release steps.

## 2. Action

The [Docker Action](./action.yml) builds an OCI container image and optionally publishes it to the calling repository owner's GHCR namespace.

A publishing run applies `latest` and the supplied version tag to the same build result. The action uses the calling repository name as the default package name, supports an optional package name under the same owner namespace, and provides multi-platform builds, build arguments, Dockerfile targets, per-image GitHub Actions cache storage, and build-only validation through `push: false`.

All third-party actions are pinned to immutable commit SHAs.

### 2.1. Inputs

| Input               | Description                                                                | Required | Default                 |
| ------------------- | -------------------------------------------------------------------------- | -------- | ----------------------- |
| `github-token`      | GitHub token used to authenticate to `ghcr.io`                             | No       | `${{ github.token }}`   |
| `registry-username` | GHCR username associated with `github-token`                               | No       | `${{ github.actor }}`   |
| `version`           | Current Docker tag, such as `1.4.2` or `v1.4.2`; must differ from `latest` | Yes      | —                       |
| `context`           | Docker build context                                                       | No       | `.`                     |
| `file`              | Path to the Dockerfile                                                     | No       | `Dockerfile`            |
| `image-name`        | GHCR package name without registry, owner, tag, or digest                  | No       | Calling repository name |
| `platforms`         | Comma-separated target platforms                                           | No       | `linux/amd64`           |
| `build-args`        | Newline-delimited Docker build arguments                                   | No       | ``                      |
| `target`            | Dockerfile build target                                                    | No       | ``                      |
| `push`              | Publish the image to GHCR; set to `false` for build-only validation        | No       | `true`                  |

### 2.2. Outputs

| Output   | Description                                                                                  |
| -------- | -------------------------------------------------------------------------------------------- |
| `image`  | Fully qualified resolved image name, such as `ghcr.io/sentenz/example`                       |
| `tags`   | Newline-delimited `latest` and version tags                                                  |
| `digest` | Image manifest digest returned by Docker Buildx; may be empty for some build-only operations |

## 3. Usage

### 3.1. Publish Using the Repository Name

```yaml
name: Docker

on:
  release:
    types: [published]

permissions:
  contents: read
  packages: write

jobs:
  publish:
    name: Docker Publish
    if: github.event_name == 'release'
    runs-on: ubuntu-latest
    steps:
      - name: Checkout
        uses: actions/checkout@9c091bb21b7c1c1d1991bb908d89e4e9dddfe3e0 # v7.0.0

      - name: Build and Publish Image
        id: image
        uses: sentenz/actions/docker@eeb59ec6f18d51aee1f04136bfedceaa02d346f9
        with:
          version: ${{ github.event.release.tag_name }}
          github-token: ${{ secrets.GITHUB_TOKEN }}

      - name: Report Digest
        run: echo "${{ steps.image.outputs.digest }}"
```

### 3.2. Publish Using a Custom Image Name

```yaml
- name: Build and Publish API Image
  uses: sentenz/actions/docker@eeb59ec6f18d51aee1f04136bfedceaa02d346f9
  with:
    version: ${{ github.event.release.tag_name }}
    image-name: api
    github-token: ${{ secrets.GITHUB_TOKEN }}
```

### 3.3. Build Without Publishing

Use build-only mode for pull requests and other untrusted or non-release events:

```yaml
permissions:
  contents: read

steps:
  - name: Checkout
    uses: actions/checkout@9c091bb21b7c1c1d1991bb908d89e4e9dddfe3e0 # v7.0.0

  - name: Validate container build
    uses: sentenz/actions/docker@eeb59ec6f18d51aee1f04136bfedceaa02d346f9
    with:
      version: 0.0.0-pr
      push: false
```

### 3.4. Build for Multiple Platforms

```yaml
- name: Build and Publish Multi-Platform Image
  uses: sentenz/actions/docker@eeb59ec6f18d51aee1f04136bfedceaa02d346f9
  with:
    version: ${{ github.event.release.tag_name }}
    platforms: linux/amd64,linux/arm64
    build-args: |
      APP_VERSION=${{ github.event.release.tag_name }}
      VCS_REF=${{ github.sha }}
    github-token: ${{ secrets.GITHUB_TOKEN }}
```

### 3.5. Build a Dockerfile Target

```yaml
- name: Build Production Target
  uses: sentenz/actions/docker@eeb59ec6f18d51aee1f04136bfedceaa02d346f9
  with:
    version: ${{ github.event.release.tag_name }}
    target: production
    github-token: ${{ secrets.GITHUB_TOKEN }}
```

## 4. Configuration

### 4.1. Workflow Permissions

Publishing requires package write access in the calling workflow:

```yaml
permissions:
  contents: read
  packages: write
```

A composite action cannot grant workflow permissions. Build-only jobs generally require only `contents: read`.

### 4.2. Image Naming

The action derives the registry owner from `${{ github.repository_owner }}` and normalizes the owner and package names to lowercase.

With no `image-name`, the calling repository name is used:

```text
ghcr.io/<owner>/<repository>:latest
ghcr.io/<owner>/<repository>:<version>
```

With `image-name: api`, the package remains in the same owner namespace:

```text
ghcr.io/<owner>/api:latest
ghcr.io/<owner>/api:<version>
```

The `image-name` input accepts only a package-name component. Values containing a registry prefix, owner path, tag, digest, whitespace, or unsupported characters are rejected before authentication and build execution.

### 4.3. Registry Authentication

The package is created under the calling repository owner's GHCR namespace. Package visibility and repository access are managed in GitHub package settings, and organization policies may restrict package creation or visibility.

For private repositories, the supplied token must have access to the repository and permission to write packages. The default `${{ github.token }}` is normally sufficient when the calling workflow declares `packages: write`.

A personal access token is necessary only when publishing across permission boundaries not covered by the workflow token. When supplying a PAT, set `registry-username` to the account that owns the token.

### 4.4. Validation

The action fails before authentication or build execution when:

- `version`, `context`, `file`, or `platforms` is empty;
- `version` is not a valid Docker tag or resolves to `latest`;
- `push` is not exactly `true` or `false`;
- `registry-username` is empty while `push` is `true`; or
- `image-name` contains a registry prefix, owner path, tag, digest, whitespace, or unsupported characters.
