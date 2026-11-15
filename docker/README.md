# Docker Composite Action

Build OCI container images with Docker Buildx, configure provenance and SBOM attestations, and optionally publish versioned and `latest` tags to GitHub Container Registry (GHCR).

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Dependencies](#21-dependencies)
  - [2.2. Inputs](#22-inputs)
  - [2.3. Outputs](#23-outputs)
  - [2.4. Permissions](#24-permissions)
- [3. Usage](#3-usage)
  - [3.1. Publish Using the Repository Name](#31-publish-using-the-repository-name)
  - [3.2. Publish Using a Custom Image Name](#32-publish-using-a-custom-image-name)
  - [3.3. Build Without Publishing](#33-build-without-publishing)
  - [3.4. Build for Multiple Platforms](#34-build-for-multiple-platforms)
  - [3.5. Build a Dockerfile Target](#35-build-a-dockerfile-target)
  - [3.6. Publish Attested Versioned Images](#36-publish-attested-versioned-images)
- [4. Configuration](#4-configuration)
  - [4.1. Image Naming](#41-image-naming)
  - [4.2. Registry Authentication](#42-registry-authentication)
  - [4.3. Validation](#43-validation)
  - [4.4. Provenance and SBOM Attestations](#44-provenance-and-sbom-attestations)

## 1. Details

- [Docker Buildx](https://docs.docker.com/build/buildx/)
  > A Docker CLI extension for advanced builds, including multi-platform images, build caching, and BuildKit features.

- [GitHub Container Registry](https://docs.github.com/packages/working-with-a-github-packages-registry/working-with-the-container-registry)
  > GitHub's OCI-compatible container registry for publishing and managing container images.

- [Publishing Docker Images](https://docs.github.com/actions/publishing-packages/publishing-docker-images)
  > GitHub Actions guidance for authenticating, building, and publishing container images.

- [Docker Build Push Action](https://github.com/docker/build-push-action)
  > A GitHub Action for building and publishing container images with Docker Buildx.

## 2. Action

The [Docker Action](./action.yml) builds an OCI container image and optionally publishes it to the calling repository owner's GHCR namespace.

By default, a publishing run applies `latest` and the supplied version tag to the same build result. Set `publish-latest: false` to push only the immutable version tag, which is useful when promotion to `latest` must happen after release verification. The action uses the calling repository name as the default package name, supports an optional package name under the same owner namespace, and provides multi-platform builds, build arguments, Dockerfile targets, per-image GitHub Actions cache storage, and build-only validation through `push: false`. BuildKit can embed provenance and SBOM attestations in the pushed OCI image index.

### 2.1. Dependencies

| Actions | Reference | Purpose |
| --- | --- | --- |
| [docker/setup-qemu-action](https://github.com/docker/setup-qemu-action) | `docker/setup-qemu-action` | Enable QEMU emulation for non-native target architectures when needed. |
| [docker/setup-buildx-action](https://github.com/docker/setup-buildx-action) | `docker/setup-buildx-action` | Configure Docker Buildx for building container images. |
| [docker/login-action](https://github.com/docker/login-action) | `docker/login-action` | Authenticate to GHCR when image publishing is enabled. |
| [docker/build-push-action](https://github.com/docker/build-push-action) | `docker/build-push-action` | Build the image and optionally publish its versioned and `latest` tags. |

### 2.2. Inputs

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
| `publish-latest`    | Publish the `latest` tag as well as the versioned tag                       | No       | `true`                  |
| `provenance`        | BuildKit provenance: `auto`, `true`, `false`, `mode=min`, or `mode=max`      | No       | `auto`                  |
| `sbom`              | Image SBOM attestation: `auto`, `true`, or `false`                          | No       | `auto`                  |
| `attests`           | Advanced newline-delimited BuildKit attestation parameters                 | No       | ``                      |

### 2.3. Outputs

| Output   | Description                                                                                  |
| -------- | -------------------------------------------------------------------------------------------- |
| `image`  | Fully qualified resolved image name, such as `ghcr.io/sentenz/example`                       |
| `tags`   | Newline-delimited published tags (`latest` is omitted when `publish-latest: false`)           |
| `digest` | Image manifest digest returned by Docker Buildx; may be empty for some build-only operations |

### 2.4. Permissions

The calling workflow must grant the `GITHUB_TOKEN` permissions required by the active Docker configuration. BuildKit provenance and SBOM generation do not require OIDC permissions; a separate Sigstore signing step requires `id-token: write`.

| Permission | Access  | Description                                                |
| ---------- | ------- | ---------------------------------------------------------- |
| `contents` | `read`  | Allows `GITHUB_TOKEN` to read repository contents          |
| `packages` | `write` | Allows `GITHUB_TOKEN` to publish container images to GHCR  |

```yaml
jobs:
  publish:
    permissions:
      contents: read
      packages: write
```

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
        uses: sentenz/actions/docker@latest
        with:
          version: ${{ github.event.release.tag_name }}
          github-token: ${{ secrets.GITHUB_TOKEN }}

      - name: Report Digest
        run: echo "${{ steps.image.outputs.digest }}"
```

### 3.2. Publish Using a Custom Image Name

```yaml
- name: Build and Publish API Image
  uses: sentenz/actions/docker@latest
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
    uses: sentenz/actions/docker@latest
    with:
      version: 0.0.0-pr
      push: false
```

### 3.4. Build for Multiple Platforms

```yaml
- name: Build and Publish Multi-Platform Image
  uses: sentenz/actions/docker@latest
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
  uses: sentenz/actions/docker@latest
  with:
    version: ${{ github.event.release.tag_name }}
    target: production
    github-token: ${{ secrets.GITHUB_TOKEN }}
```

### 3.6. Publish Attested Versioned Images

Release workflows can explicitly request maximum-detail provenance and an image SBOM while deferring `latest` promotion:

```yaml
- name: Publish attested versioned image
  id: docker-build
  uses: sentenz/actions/docker@latest
  with:
    version: ${{ github.ref_name }}
    context: .
    file: container/k8s/Dockerfile
    image-name: k8s
    platforms: linux/amd64,linux/arm64
    push: true
    publish-latest: false
    provenance: mode=max
    sbom: true

- name: Report the immutable image reference
  env:
    IMAGE: ${{ steps.docker-build.outputs.image }}
    DIGEST: ${{ steps.docker-build.outputs.digest }}
  run: echo "${IMAGE}@${DIGEST}"
```

Grant `packages: write` in the publishing job, and pin the action to a reviewed commit SHA in production. The `digest` output identifies the OCI image index, including BuildKit-generated attestation manifests, for subsequent signing and verification with `sentenz/actions/sigstore`.

## 4. Configuration

### 4.1. Image Naming

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

### 4.2. Registry Authentication

The package is created under the calling repository owner's GHCR namespace. Package visibility and repository access are managed in GitHub package settings, and organization policies may restrict package creation or visibility.

For private repositories, the supplied token must have access to the repository and permission to write packages. The default `${{ github.token }}` is normally sufficient when the calling workflow declares `packages: write`.

A personal access token is necessary only when publishing across permission boundaries not covered by the workflow token. When supplying a PAT, set `registry-username` to the account that owns the token.

### 4.3. Validation

The action fails before authentication or build execution when:

- `version`, `context`, `file`, or `platforms` is empty;
- `version` is not a valid Docker tag or resolves to `latest`;
- `push` or `publish-latest` is not exactly `true` or `false`;
- `sbom` is not `auto`, `true`, or `false`;
- `provenance` is not `auto`, `true`, `false`, `mode=min`, or `mode=max`;
- `registry-username` is empty while `push` is `true`; or
- `image-name` contains a registry prefix, owner path, tag, digest, whitespace, or unsupported characters.

### 4.4. Provenance and SBOM Attestations

Both attestations are produced by BuildKit during the image build and stored in GHCR as part of the OCI image index when `push: true`. They are not independent workflow artifact files. Build-only validation (`push: false`) does not persist them to a registry.

- `provenance: auto` (default) leaves the upstream `docker/build-push-action` behavior unchanged: public GitHub repositories receive `mode=max` by default; private repositories receive `mode=min`. Set `mode=max` explicitly to require rich build provenance on release images, regardless of repository visibility.
- `provenance: mode=min` limits details, while `mode=max` includes richer build parameters. **Do not pass secrets as Docker build arguments:** max-level provenance can disclose build argument values. Use BuildKit secret mounts for credentials.
- `provenance: false` disables the BuildKit provenance attestation; `provenance: true` uses BuildKit's enabled behavior.
- `sbom: true` generates an additional SBOM attestation. The default `auto` leaves the SBOM shortcut unset (no automatic SBOM), while allowing advanced SBOM configuration through `attests`. Set `false` to explicitly disable it.
- `attests` passes advanced parameters to the upstream build action (for example `type=provenance,mode=max,version=v1` or `type=sbom,generator=image`). Explicit `provenance` and `sbom` inputs take precedence over the corresponding types in `attests`; keep their values at `auto` when configuring the same types using `attests`.
- `publish-latest: false` is independent of attestation settings and publishes only the specified version tag. It is intended for release pipelines that promote `latest` after policy checks, image signing, and release asset publication.

BuildKit attestations describe the build; they are not a replacement for a separate cryptographic signature or verification policy. Use the `image` and `digest` outputs with the Sigstore action to sign the immutable image reference. Consult [Docker's GitHub Actions attestation guide](https://docs.docker.com/build/ci/github-actions/attestations/) and [provenance specification](https://docs.docker.com/build/metadata/attestations/slsa-provenance/) for details.
