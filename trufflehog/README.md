# TruffleHog Composite Action

Scan Git repositories for leaked credentials through the upstream [TruffleHog action](https://github.com/trufflesecurity/trufflehog), pinned to a reviewed commit.

- [1. Requirements](#1-requirements)
- [2. Inputs](#2-inputs)
- [3. Usage](#3-usage)
- [4. Scan Ranges and Results](#4-scan-ranges-and-results)
- [5. Testing](#5-testing)

## 1. Requirements

Use a Linux runner with Docker, Git, and jq available, such as `ubuntu-latest`. Check out the repository before invoking the action and use `fetch-depth: 0` so the scan range is available. The workflow only needs `contents: read`; this action does not post comments or upload artifacts.

## 2. Inputs

| Input | Description | Required | Default |
| --- | --- | --- | --- |
| `path` | Path to the checked-out Git repository. | No | `./` |
| `base` | Optional starting commit or ref. Leave both range inputs empty for automatic selection. | No | Empty |
| `head` | Optional ending commit or ref. Leave both range inputs empty for automatic selection. | No | Empty |
| `trufflehog-version` | Container version tag, optionally followed by `@sha256:<digest>`. | No | `latest` |
| `image` | Container repository without a version tag; supports registry mirrors. | No | `ghcr.io/trufflesecurity/trufflehog` |
| `extra-args` | Additional space-separated TruffleHog CLI arguments. | No | Empty |

The upstream action revision and scanner container version are independent. Set `trufflehog-version` to select a specific scanner version or a version plus image digest. The default follows the upstream scanner image's `latest` tag.

`extra-args` follows upstream argument splitting; embedded shell quoting is not interpreted. Prefer flags such as `--results=verified,unknown` when an option accepts a single value. Flags that filter result types also change which findings fail the step.

## 3. Usage

```yaml
name: TruffleHog

on:
  push:
    branches: [main]
  pull_request:

permissions:
  contents: read

jobs:
  trufflehog:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@3d3c42e5aac5ba805825da76410c181273ba90b1 # v7.0.1
        with:
          fetch-depth: 0
          persist-credentials: false

      - uses: sentenz/actions/trufflehog@0cd66856de0b52048989564908d897c75f0b7e08
```

An executable example is available in [examples/workflows/trufflehog.yml](../examples/workflows/trufflehog.yml).

## 4. Scan Ranges and Results

With `base` and `head` both empty, the pinned upstream action selects the range from the workflow event:

| Event | Scan range |
| --- | --- |
| `pull_request` | PR base commit through PR head commit. |
| `push` | Previous commit through the pushed head; an initial push scans available history. A push without commits is skipped. |
| `workflow_dispatch` or `schedule` | Available repository history. |

For other events or a custom range, pass explicit `base` and `head` values that exist in the checkout. Setting either input activates upstream's manual range mode. In particular, passing only `github.sha` on a PR disables automatic PR range selection.

TruffleHog emits GitHub Actions annotations and fails the step when a finding matches the selected result types or when scanning encounters an error. Failures propagate directly to the calling job. There are no custom outputs; callers that intentionally use `continue-on-error` can inspect the step's `outcome`.

## 5. Testing

[TruffleHog Test](../.github/workflows/trufflehog-test.yml) invokes the local composite action against the workflow's automatic range and a temporary Git repository. The fixture checks a clean range and failure propagation for a synthetic URI credential. Verification is disabled for the synthetic credential; no service credentials are required.
