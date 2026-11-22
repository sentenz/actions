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

- [GitHub Pages](https://docs.github.com/en/pages/getting-started-with-github-pages/using-custom-workflows-with-github-pages)
  > GitHub's hosting service deploys the site from an artifact produced by a workflow.

- [Zensical Test](../.github/workflows/zensical-test.yml)
  > Integration workflow that builds the Convention and Skills documentation and a standalone site using the bundled CLI.

## 2. Action

The [Zensical Action](./action.yml) installs project dependencies, runs documentation tests, builds the site, and uploads the generated files. The defaults follow the project layouts in [sentenz/convention](https://github.com/sentenz/convention) and [sentenz/skills](https://github.com/sentenz/skills).

Check out the site repository on a Linux runner with Docker, such as `ubuntu-latest`, before invoking the action. All input paths are relative to the checkout root.

### 2.1. Dependencies

| Actions | Reference | Purpose |
| --- | --- | --- |
| [actions/upload-pages-artifact](https://github.com/actions/upload-pages-artifact) | `actions/upload-pages-artifact` | Package and upload the generated site for GitHub Pages. |

| Images | Reference | Purpose |
| --- | --- | --- |
| [zensical/zensical](https://hub.docker.com/r/zensical/zensical) | `docker.io/zensical/zensical` | Provide Python and the Zensical CLI for tests and builds. |

### 2.2. Inputs

| Input               | Description                                                                                               | Required | Default                                                                         |
| ------------------- | --------------------------------------------------------------------------------------------------------- | -------- | ------------------------------------------------------------------------------- |
| `image-version`     | Zensical image tag, optionally followed by `@sha256:<digest>`                                             | No       | `0.1.0@sha256:a9512943ddf49c645fd3c0861e5347f9c889abe7f5fdaad05e047582b8069b70` |
| `requirements-file` | Pip requirements file. An empty string skips dependency installation.                                     | No       | `requirements-docs.txt`                                                         |
| `tests-directory`   | Directory for Python unittest discovery. An empty string skips tests.                                     | No       | `tests/docs`                                                                    |
| `build-script`      | Python script invoked with the argument `build`. An empty string runs `python -m zensical build --clean`. | No       | `scripts/docs.py`                                                               |
| `site-directory`    | Generated site directory containing a nonempty `index.html`                                               | No       | `public`                                                                        |
| `artifact-name`     | Name of the uploaded Pages artifact                                                                       | No       | `github-pages`                                                                  |

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
      - uses: sentenz/actions/zensical@b942f9f9f086ad5d89686573b99f0c2e02c87399
```

Select **GitHub Actions** under **Settings → Pages → Build and deployment → Source** in the site repository. Copy the complete [publishing workflow](../examples/workflows/zensical.yml) to `.github/workflows/zensical.yml`. It builds pull requests and deploys successful builds from pushes or manual runs on `main`. The deployment job uses the `github-pages` environment and serializes deployments through the `pages` concurrency group.

## 4. Configuration

### 4.1. Internal Configuration

The action pins the container image by digest in [action.yml](./action.yml). It mounts the checkout at `/docs` and runs the [build script](./build.sh) as the runner user so subsequent steps can read the generated files. The upload starts after the build succeeds and the action verifies that `site-directory` contains a nonempty `index.html`.

### 4.2. External Configuration

For a standalone project with no extra dependencies or tests, use the CLI bundled in the image and upload Zensical's default `site/` directory.

```yaml
- uses: sentenz/actions/zensical@b942f9f9f086ad5d89686573b99f0c2e02c87399
  with:
    requirements-file: ''
    tests-directory: ''
    build-script: ''
    site-directory: site
```

For projects with a requirements file, keep any Zensical version pin aligned with `image-version`. Set `site-directory` to the output directory configured by the project. When overriding `artifact-name`, pass the same name to `actions/deploy-pages` in the deployment job.

To publish from a branch other than `main`, update both the push branch filter and the deployment condition in the calling workflow.
