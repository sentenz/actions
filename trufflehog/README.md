# TruffleHog Composite Action

Secret scanning for Git repositories using [TruffleHog](https://github.com/trufflesecurity/trufflehog).

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
  - [2.3. Permissions](#23-permissions)
- [3. Usage](#3-usage)
- [4. Configuration](#4-configuration)
  - [4.1. Internal Configuration](#41-internal-configuration)
  - [4.2. External Configuration](#42-external-configuration)

## 1. Details

- [TruffleHog](https://github.com/trufflesecurity/trufflehog)
  > An open-source tool for detecting and verifying leaked credentials in Git repositories.

- [TruffleHog GitHub Action](https://github.com/trufflesecurity/trufflehog#octocat-trufflehog-github-action)
  > GitHub Actions integration with automatic commit ranges for pull requests and pushes.

- [TruffleHog Test](../.github/workflows/trufflehog-test.yml)
  > Integration workflow covering automatic ranges, clean commits, and failure propagation for synthetic URI credentials with verification disabled.

## 2. Action

The [TruffleHog Action](./action.yml) wraps the SHA-pinned upstream GitHub action with automatic scan ranges, configurable scanner inputs, and failure propagation.

A Linux runner with Docker, Git, and jq is required, such as `ubuntu-latest`. Check out the repository before invoking the action and use `fetch-depth: 0` so the scan range is available.

### 2.1. Inputs

| Input                | Description                                                                       | Required | Default                             |
| -------------------- | --------------------------------------------------------------------------------- | -------- | ----------------------------------- |
| `path`               | Path to the checked-out Git repository                                             | No       | `./`                                |
| `base`               | Starting commit or ref; empty with `head` preserves automatic range selection      | No       | `""`                                |
| `head`               | Ending commit or ref; empty with `base` preserves automatic range selection        | No       | `""`                                |
| `trufflehog-version` | Container version tag, optionally followed by `@sha256:<digest>`                    | No       | `latest`                            |
| `image`              | Container repository without a version tag; supports registry mirrors              | No       | `ghcr.io/trufflesecurity/trufflehog` |
| `extra-args`         | Additional space-separated TruffleHog CLI arguments                                | No       | `""`                                |

### 2.2. Outputs

The action does not define custom outputs. TruffleHog emits GitHub Actions annotations and fails the step when a finding matches the selected result types or when scanning encounters an error. Failures propagate directly to the calling job.

Callers that intentionally use `continue-on-error` can inspect the step's `outcome`.

### 2.3. Permissions

The calling workflow must grant the `GITHUB_TOKEN` permission required to check out the repository being scanned.

| Permission | Access | Description                                    |
| ---------- | ------ | ---------------------------------------------- |
| `contents` | `read` | Allows checkout of the repository being scanned |

```yaml
jobs:
  trufflehog:
    permissions:
      contents: read
```

## 3. Usage

```yaml
jobs:
  trufflehog:
    runs-on: ubuntu-latest
    permissions:
      contents: read
    steps:
      - uses: actions/checkout@3d3c42e5aac5ba805825da76410c181273ba90b1 # v7.0.1
        with:
          fetch-depth: 0
          persist-credentials: false
      - uses: sentenz/actions/trufflehog@e59be3f004f11bc57b50715d04baa6fe139487bb
        with:
          path: "./"
```

A complete workflow is available in [examples/workflows/trufflehog.yml](../examples/workflows/trufflehog.yml).

## 4. Configuration

### 4.1. Internal Configuration

The action uses the upstream TruffleHog revision pinned in [`action.yml`](./action.yml). No internal scanner configuration file is supplied.

With `base` and `head` both empty, the pinned upstream action selects the range from the workflow event:

| Event                             | Scan range                                                                                                        |
| --------------------------------- | ----------------------------------------------------------------------------------------------------------------- |
| `pull_request`                    | PR base commit through PR head commit                                                                             |
| `push`                            | Previous commit through the pushed head; an initial push scans available history. A push without commits is skipped |
| `workflow_dispatch` or `schedule` | Available repository history                                                                                      |

The upstream action revision and scanner container version are independent. The `trufflehog-version` input defaults to `latest`, following the upstream scanner image's latest tag.

### 4.2. External Configuration

Override scanner settings through inputs in the calling workflow:

```yaml
- uses: sentenz/actions/trufflehog@e59be3f004f11bc57b50715d04baa6fe139487bb
  with:
    path: "./"
    trufflehog-version: "3.97.4"
    extra-args: --debug
```

Set `trufflehog-version` to a specific scanner version or a version followed by `@sha256:<digest>`. Set `image` to use a different container repository or registry mirror.

For other events or a custom scan range, pass explicit `base` and `head` values that exist in the checkout. Setting either input activates upstream's manual range mode. In particular, passing only `github.sha` on a PR disables automatic PR range selection.

`extra-args` follows upstream argument splitting; embedded shell quoting is not interpreted. Prefer flags such as `--results=verified,unknown` when an option accepts a single value. Flags that filter result types also change which findings fail the step.
