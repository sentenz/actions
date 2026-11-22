# Zensical Composite Action

Build documentation with [Zensical](https://zensical.org/) and upload the generated site as a GitHub Pages artifact.

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Dependencies](#21-dependencies)
  - [2.2. Inputs](#22-inputs)
  - [2.3. Outputs](#23-outputs)
  - [2.4. Permissions](#24-permissions)
- [3. Usage](#3-usage)
- [4. Configuration](#4-configuration)
  - [4.1. Internal Configuration](#41-internal-configuration)
  - [4.2. External Configuration](#42-external-configuration)

## 1. Details

- [Zensical](https://zensical.org/)
  > Static site generator used to build project documentation.

- [GitHub Flavored Markdown](https://github.github.com/gfm/)
  > CommonMark dialect with extensions for tables, task lists, strikethrough, autolinks, and raw HTML filtering.

- [GitHub Pages](https://docs.github.com/en/pages/getting-started-with-github-pages/using-custom-workflows-with-github-pages)
  > GitHub's hosting service deploys the site from an artifact produced by a workflow.

- [Zensical Test](../.github/workflows/zensical-test.yml)
  > Integration workflow that builds standalone and GFM fixtures, then verifies GFM in the generated HTML.

## 2. Action

The [Zensical Action](./action.yml) installs the bundled GFM renderer dependencies, builds the site using the Zensical CLI, and uploads the generated files. Project-specific preprocessing and tests belong in the calling workflow.

Check out the site repository on a Linux runner with Docker, such as `ubuntu-latest`, before invoking the action. The `site-directory` input is relative to the checkout root.

### 2.1. Dependencies

| Actions | Reference | Purpose |
| --- | --- | --- |
| [actions/upload-pages-artifact](https://github.com/actions/upload-pages-artifact) | `actions/upload-pages-artifact` | Package and upload the generated site for GitHub Pages. |

| Images | Reference | Purpose |
| --- | --- | --- |
| [zensical/zensical](https://hub.docker.com/r/zensical/zensical) | `docker.io/zensical/zensical` | Provide Python and the Zensical CLI for builds. |

### 2.2. Inputs

| Input | Description | Required | Default |
| --- | --- | --- | --- |
| `image` | Full Zensical Docker image reference, including registry and repository | No | `docker.io/zensical/zensical:0.1.0@sha256:a9512943ddf49c645fd3c0861e5347f9c889abe7f5fdaad05e047582b8069b70` |
| `flavor` | Optional Markdown flavor: empty for standard Zensical rendering; `gfm` to enable GitHub Flavored Markdown | No | Empty (`''`) |
| `site-directory` | Generated site directory containing a nonempty `index.html` | No | `public` |
| `artifact-name` | Name of the uploaded Pages artifact | No | `github-pages` |

### 2.3. Outputs

The action produces a workflow artifact named by `artifact-name`. The deployment step in the calling workflow exposes the published URL as `steps.deployment.outputs.page_url`.

### 2.4. Permissions

| Permission | Access  | Description                                                               |
| ---------- | ------- | ------------------------------------------------------------------------- |
| `contents` | `read`  | Allows the build job to check out the site repository.                    |
| `pages`    | `write` | Allows the deployment job to publish the artifact to GitHub Pages.        |
| `id-token` | `write` | Allows the deployment job to request the OIDC token used by GitHub Pages. |

## 3. Usage

```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    permissions:
      contents: read
    steps:
      - uses: actions/checkout@3d3c42e5aac5ba805825da76410c181273ba90b1 # v7.0.1
        with:
          persist-credentials: false
      - uses: sentenz/actions/zensical@df49b0c43c42e78efe9caae720ba455dd9e07a2b
        with:
          site-directory: site
```

Select **GitHub Actions** under **Settings → Pages → Build and deployment → Source** in the site repository. Copy the complete [publishing workflow](../examples/workflows/zensical.yml) to `.github/workflows/zensical.yml`. It builds pull requests and deploys successful builds from pushes or manual runs on `main`. The deployment job uses the `github-pages` environment and serializes deployments through the `pages` concurrency group.

## 4. Configuration

### 4.1. Internal Configuration

The action pins the container image by digest in [action.yml](./action.yml). It mounts the checkout at `/docs` and runs the [build script](./build.sh) as the runner user. The script installs the dependencies from [requirements-gfm.txt](./extensions/requirements-gfm.txt). When `flavor` is omitted or empty, it builds from the project's existing Zensical configuration. With `flavor: gfm`, it enables the bundled GFM renderer in a temporary `zensical.toml` and builds using that file, without modifying the checkout's configuration. The upload starts after the build succeeds and `site-directory` contains a nonempty `index.html`.

### 4.2. External Configuration

For a standalone project, set `site-directory` to Zensical's default output directory, `site/`.

```yaml
- uses: sentenz/actions/zensical@df49b0c43c42e78efe9caae720ba455dd9e07a2b
  with:
    site-directory: site
```

The action uses standard Zensical rendering when `flavor` is omitted (or set to `''`). To render **GitHub Flavored Markdown (GFM)**, select `flavor: gfm`. A `zensical.toml` file must be present at the checkout root.

```yaml
- uses: sentenz/actions/zensical@df49b0c43c42e78efe9caae720ba455dd9e07a2b
  with:
    flavor: gfm
    site-directory: site
```

The action adds `sentenz_gfm = {}` only to the temporary build configuration. Existing projects explicitly configuring `sentenz_gfm` retain that behavior when `flavor` is omitted.

The renderer uses Comrak for GFM syntax, including nested lists, fenced code, pipe tables, task lists, strikethrough, extended autolinks, and tag filtering. It also renders GitHub alerts such as `> [!NOTE]` as Zensical admonitions. Zensical continues to generate heading anchors, the table of contents, and site-relative links. Projects using Python-Markdown-specific source syntax can keep their existing renderer by leaving `sentenz_gfm` out of the configuration.

The GFM dependencies are installed automatically in the container for both modes to preserve compatibility with existing manually configured GFM projects.

For projects requiring additional Python packages during the build, provide a custom container through `image` with those packages preinstalled. Run project-specific preprocessing and tests separately in the calling workflow. For a local preview, install the dependencies in [requirements-gfm.txt](./extensions/requirements-gfm.txt) and add this repository's `zensical/extensions` directory to `PYTHONPATH` before starting Zensical.

To use a registry mirror, set `image` to the full image reference, for example `registry.example.org/docs/zensical:0.1.0`. Ensure a custom image contains a compatible Zensical CLI. Set `site-directory` to the output directory configured by the project. When overriding `artifact-name`, pass the same name to `actions/deploy-pages` in the deployment job.

To publish from a branch other than `main`, update both the push branch filter and the deployment condition in the calling workflow.
