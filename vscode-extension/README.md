# VS Code Extension Composite Action

Package a VS Code extension as a VSIX file and optionally publish that exact artifact to Visual Studio Marketplace.

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
  - [3.1. Publish with a Marketplace Token](#31-publish-with-a-marketplace-token)
  - [3.2. Package Without Publishing](#32-package-without-publishing)
  - [3.3. Publish a Pre-Release](#33-publish-a-pre-release)
  - [3.4. Package for a Target Platform](#34-package-for-a-target-platform)
  - [3.5. Publish with Microsoft Entra Authentication](#35-publish-with-microsoft-entra-authentication)
- [4. Configuration](#4-configuration)
  - [4.1. Build Preparation](#41-build-preparation)
  - [4.2. Authentication](#42-authentication)
  - [4.3. Output Artifact](#43-output-artifact)
  - [4.4. Validation](#44-validation)

## 1. Details

- [Publishing Extensions](https://code.visualstudio.com/api/working-with-extensions/publishing-extension)
  > Official guidance for packaging and publishing extensions with `@vscode/vsce`.

- [VS Code Extension Manager](https://github.com/microsoft/vscode-vsce)
  > The command-line tool used by this action to create and publish VSIX packages.

- [Extension Manifest](https://code.visualstudio.com/api/references/extension-manifest)
  > Reference for the extension `package.json` fields used by Visual Studio Code and the Marketplace.

## 2. Action

The [VS Code Extension Action](./action.yml) validates an extension manifest, packages the extension with `@vscode/vsce`, and optionally publishes the generated VSIX to Visual Studio Marketplace.

Packaging and publishing use the same VSIX file. This makes package-only CI and release publishing exercise the same artifact construction path. The action does not install project dependencies; callers retain control over dependency installation, tests, compilation, and bundling before packaging.

### 2.1. Inputs

| Input               | Description                                                                      | Required | Default |
| ------------------- | -------------------------------------------------------------------------------- | -------- | ------- |
| `marketplace-token` | Visual Studio Marketplace token exposed to `vsce` as `VSCE_PAT`                  | No       | ``      |
| `authentication`    | Publishing authentication method: `pat` or `azure-credential`                    | No       | `pat`   |
| `working-directory` | Repository-relative directory containing the extension `package.json`            | No       | `.`     |
| `output-path`       | VSIX path relative to `working-directory`; empty uses runner temporary storage    | No       | ``      |
| `vsce-version`      | `@vscode/vsce` version, major version, or `latest`                                | No       | `3`     |
| `target`            | Optional target platform, such as `linux-x64`, `darwin-arm64`, or `web`           | No       | ``      |
| `pre-release`       | Package and publish the extension as a pre-release                                | No       | `false` |
| `no-dependencies`   | Pass `--no-dependencies` to `vsce package`                                        | No       | `false` |
| `publish`           | Publish the packaged VSIX to Visual Studio Marketplace                            | No       | `true`  |

### 2.2. Outputs

| Output         | Description                                                   |
| -------------- | ------------------------------------------------------------- |
| `vsix-path`    | Absolute path to the packaged VSIX file                       |
| `extension-id` | Extension identifier in `publisher.name` format               |
| `version`      | Extension version read from `package.json`                    |
| `published`    | Whether the VSIX was published to Visual Studio Marketplace   |

## 3. Usage

### 3.1. Publish with a Marketplace Token

Store the Marketplace credential as the `VSCE_PAT` repository or environment secret.

```yaml
name: Publish VS Code Extension

on:
  release:
    types: [published]

permissions:
  contents: read

jobs:
  publish:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout
        uses: actions/checkout@9c091bb21b7c1c1d1991bb908d89e4e9dddfe3e0 # v7.0.0

      - name: Install dependencies
        run: npm ci

      - name: Test
        run: npm test

      - name: Publish extension
        id: extension
        uses: sentenz/actions/vscode-extension@latest
        with:
          marketplace-token: ${{ secrets.VSCE_PAT }}
```

### 3.2. Package Without Publishing

Package-only mode is suitable for pull requests and other events that must not access release credentials.

```yaml
- name: Package extension
  id: extension
  uses: sentenz/actions/vscode-extension@latest
  with:
    publish: false
    output-path: artifacts/extension.vsix

- name: Inspect package path
  run: echo "${{ steps.extension.outputs.vsix-path }}"
```

### 3.3. Publish a Pre-Release

```yaml
- name: Publish pre-release extension
  uses: sentenz/actions/vscode-extension@latest
  with:
    marketplace-token: ${{ secrets.VSCE_PAT }}
    pre-release: true
```

The extension manifest must use a version distinct from any regular release already uploaded to the Marketplace.

### 3.4. Package for a Target Platform

```yaml
- name: Publish Linux x64 extension
  uses: sentenz/actions/vscode-extension@latest
  with:
    marketplace-token: ${{ secrets.VSCE_PAT }}
    target: linux-x64
    output-path: artifacts/extension-linux-x64.vsix
```

Supported targets are `win32-x64`, `win32-arm64`, `linux-x64`, `linux-arm64`, `linux-armhf`, `alpine-x64`, `alpine-arm64`, `darwin-x64`, `darwin-arm64`, and `web`.

Run the action once per target when publishing platform-specific packages.

### 3.5. Publish with Microsoft Entra Authentication

After authenticating Azure CLI with an identity authorized for the Marketplace publisher, select `azure-credential` authentication:

```yaml
- name: Publish extension
  uses: sentenz/actions/vscode-extension@latest
  with:
    authentication: azure-credential
```

The caller is responsible for establishing the Azure CLI credential before this action runs. No Marketplace token is required in this mode.

## 4. Configuration

### 4.1. Build Preparation

`@vscode/vsce` requires Node.js 20 or newer. The action validates the runtime but does not install Node.js or project dependencies.

Install dependencies and run release gates before invoking the action:

```yaml
- run: npm ci
- run: npm run lint
- run: npm test
- run: npm run build
```

During packaging, `vsce` also invokes the extension's `vscode:prepublish` script when one is defined.

Use `no-dependencies: true` only when runtime dependencies have already been bundled into the extension or are otherwise intentionally excluded from dependency discovery.

### 4.2. Authentication

With `authentication: pat`, the action exposes `marketplace-token` only through the `VSCE_PAT` environment variable and fails before packaging when publishing is enabled without a token.

The Marketplace publisher and the extension manifest's `publisher` value must already be configured. The token must have Marketplace management permission for that publisher.

Microsoft has announced retirement of global Azure DevOps Personal Access Tokens on December 1, 2026. New publishing pipelines should evaluate Microsoft Entra authentication and use `authentication: azure-credential` when the required identity and Azure CLI session are available.

### 4.3. Output Artifact

When `output-path` is empty, the action creates a unique VSIX under `RUNNER_TEMP`. A configured path is resolved relative to `working-directory` and must remain inside `GITHUB_WORKSPACE` or `RUNNER_TEMP`.

The `vsix-path` output can be passed to artifact upload, release attachment, signing, or downstream validation steps.

### 4.4. Validation

The action fails before packaging when:

- Node.js or `npx` is unavailable, or Node.js is older than version 20;
- `working-directory` resolves outside `GITHUB_WORKSPACE` or lacks `package.json`;
- `publisher`, `name`, or `version` is absent from the extension manifest;
- `publish`, `pre-release`, or `no-dependencies` is not exactly `true` or `false`;
- `authentication` is not `pat` or `azure-credential`;
- token authentication is selected for publishing without `marketplace-token`;
- `vsce-version` is not `latest`, a major version, or an exact version;
- `target` is not a supported VS Code extension target; or
- `output-path` does not end in `.vsix` or resolves outside the permitted directories.
