# Zensical Composite Action

Test and build documentation inside the [official Zensical Docker image](https://hub.docker.com/r/zensical/zensical), then upload the generated site as the `github-pages` artifact. The [publishing workflow](../examples/workflows/zensical.yml) builds pull requests and deploys successful builds from pushes or manual runs on `main`.

## Usage

Select **GitHub Actions** under **Settings → Pages → Build and deployment → Source** in the site repository. Copy the [publishing workflow](../examples/workflows/zensical.yml) to `.github/workflows/zensical.yml`. If the publishing branch differs from `main`, update both the push filter and the deployment condition.

Run the build job on a Linux runner with Docker, such as `ubuntu-latest`. The composite action mounts the checkout at `/docs` and runs as the runner user so later workflow steps can access the generated files.

The defaults match [sentenz/convention](https://github.com/sentenz/convention) and [sentenz/skills](https://github.com/sentenz/skills). After checkout, one step installs the project's dependencies, runs its documentation tests, invokes its build script, and uploads `public/`.

```yaml
- name: Build and upload documentation
  uses: sentenz/actions/zensical@b942f9f9f086ad5d89686573b99f0c2e02c87399
```

For a standard Zensical project with no extra dependencies, use the CLI bundled in the image. This example publishes the default Zensical output directory.

```yaml
- name: Build and upload documentation
  uses: sentenz/actions/zensical@b942f9f9f086ad5d89686573b99f0c2e02c87399
  with:
    requirements-file: ''
    tests-directory: ''
    build-script: ''
    site-directory: site
```

## Inputs

All paths are relative to the checked-out project root.

| Input | Default | Purpose |
| --- | --- | --- |
| `image-version` | `0.1.0` pinned by digest | Docker Hub tag for `zensical/zensical`, optionally followed by `@sha256:…`. The image supplies Python and Zensical. |
| `requirements-file` | `requirements-docs.txt` | Pip requirements installed inside the container. Keep any Zensical pin aligned with the image version. An empty string skips installation. |
| `tests-directory` | `tests/docs` | Directory for Python unittest discovery. An empty string skips the test step. |
| `build-script` | `scripts/docs.py` | Python script called with the argument `build`. An empty string runs `python -m zensical build --clean`. |
| `site-directory` | `public` | Directory to upload. It must match the configured site output and contain a nonempty `index.html`. |
| `artifact-name` | `github-pages` | Name of the uploaded Pages artifact. Pass the same name to `actions/deploy-pages` when overriding it. |

## Deployment

The build job needs `contents: read` for checkout. A separate job depends on the build, grants `pages: write` and `id-token: write` to `actions/deploy-pages`, and attaches the resulting URL to the `github-pages` environment. The example serializes deployments through the `pages` concurrency group.
