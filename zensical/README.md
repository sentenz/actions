# Zensical Composite Action

Build a Zensical documentation site and upload the generated files as the `github-pages` artifact. The [publishing workflow](../examples/workflows/zensical.yml) builds pull requests and deploys successful builds from pushes or manual runs on `main`.

## Usage

Select **GitHub Actions** under **Settings → Pages → Build and deployment → Source** in the site repository. Copy the [publishing workflow](../examples/workflows/zensical.yml) to `.github/workflows/zensical.yml`. If the publishing branch differs from `main`, update both the push filter and the deployment condition.

The defaults match [sentenz/convention](https://github.com/sentenz/convention) and [sentenz/skills](https://github.com/sentenz/skills). After checkout, one step installs the project's dependencies, runs its documentation tests, invokes its build script, and uploads `public/`.

```yaml
- name: Build and upload documentation
  uses: sentenz/actions/zensical@2f6b68c5e328e64714fed6e6363cbff4230c75c2
```

For a standard Zensical project, include Zensical in `requirements-docs.txt` and invoke its CLI directly. This example uses the default Zensical output directory.

```yaml
- name: Build and upload documentation
  uses: sentenz/actions/zensical@2f6b68c5e328e64714fed6e6363cbff4230c75c2
  with:
    tests-directory: ''
    build-script: ''
    site-directory: site
```

## Inputs

All paths are relative to the checked-out project root.

| Input | Default | Purpose |
| --- | --- | --- |
| `python-version` | `3.12` | Python version used to install and run the documentation tools. |
| `requirements-file` | `requirements-docs.txt` | Pip requirements file containing Zensical and any project-specific extensions. |
| `tests-directory` | `tests/docs` | Directory for Python unittest discovery. An empty string skips the test step. |
| `build-script` | `scripts/docs.py` | Python script called with the argument `build`. An empty string runs `python -m zensical build --clean`. |
| `site-directory` | `public` | Directory to upload. It must match the configured site output and contain a nonempty `index.html`. |

## Deployment

The build job needs `contents: read` for checkout. A separate job depends on the build, grants `pages: write` and `id-token: write` to `actions/deploy-pages`, and attaches the resulting URL to the `github-pages` environment. The example serializes deployments through the `pages` concurrency group.
