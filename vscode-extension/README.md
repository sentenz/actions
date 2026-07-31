# VS Code Extension Composite Action

Package a Visual Studio Code extension as a VSIX file and optionally publish the packaged artifact to Visual Studio Marketplace.

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
  - [3.1. Publish with a Marketplace Token](#31-publish-with-a-marketplace-token)
  - [3.2. Publish with a Release Version](#32-publish-with-a-release-version)
  - [3.3. Package Without Publishing](#33-package-without-publishing)
  - [3.4. Publish a Pre-Release](#34-publish-a-pre-release)
  - [3.5. Publish for a Target Platform](#35-publish-for-a-target-platform)
  - [3.6. Publish with Microsoft Entra Authentication](#36-publish-with-microsoft-entra-authentication)
- [4. Configuration](#4-configuration)
  - [4.1. Build Preparation](#41-build-preparation)
  - [4.2. Versioning](#42-versioning)
  - [4.3. Authentication](#43-authentication)
  - [4.4. Output Artifact](#44-output-artifact)
  - [4.5. Validation](#45-validation)

## 1. Details

- [Publishing Extensions](https://code.visualstudio.com/api/working-with-extensions/publishing-extension)
  > Official guidance for packaging and publishing extensions with `@vscode/vsce`.

- [VS Code Extension Manager](https://github.com/microsoft/vscode-vsce)
  > The command-line tool used by this action to create and publish VSIX packages.

- [Extension Manifest](https://code.visualstudio.com/api/references/extension-manifest)
  > Reference for the extension `package.json` fields used by Visual Studio Code and Visual Studio Marketplace.

## 2. Action

The [VS Code Extension Action](./action.yml) validates an extension manifest, optionally resolves a release version,
packages the extension with `@vscode/vsce`, and optionally publishes the generated VSIX to Visual Studio Marketplace.

Packaging and publishing use the same VSIX file. The action does not install project dependencies; dependency
installation, tests, compilation, and bundling remain the responsibility of the calling workflow.

### 2.1. Inputs

| Input               | Description                                                                                           | Required | Default |
| ------------------- | ----------------------------------------------------------------------------------------------------- | -------- | ------- |
| `marketplace-token` | Visual Studio Marketplace token exposed to `vsce` as `VSCE_PAT`                                       | No       | ``      |
| `authentication`    | Publishing authentication method: `pat` or `azure-credential`                                         | No       | `pat`   |
| `working-directory` | Repository-relative directory containing the extension `package.json`                                 | No       | `.`     |
| `output-path`       | VSIX path relative to `working-directory`; empty uses runner temporary storage                         | No       | ``      |
| `vsce-version`      | `@vscode/vsce` version, major version, or `latest`                                                     | No       | `3`     |
| `version`           | `patch`, `minor`, `major`, or an exact `major.minor.patch` release version with optional `v` prefix   | No       | ``      |
| `target`            | Optional target platform, such as `linux-x64`, `darwin-arm64`, or `web`                                | No       | ``      |
| `pre-release`       | Package and publish the extension as a pre-release                                                     | No       | `false` |
| `no-dependencies`   | Pass `--no-dependencies` to `vsce package`                                                             | No       | `false` |
| `skip-duplicate`    | Treat an already-published extension version as a successful publish                                   | No       | `false` |
| `publish`           | Publish the packaged VSIX to Visual Studio Marketplace                                                 | No       | `true`  |

### 2.2. Outputs

| Output         | Description                                                        |
| -------------- | ------------------------------------------------------------------ |
| `vsix-path`    | Absolute path to the packaged VSIX file                            |
| `extension-id` | Extension identifier in `publisher.name` format                    |
| `version`      | Normalized version contained in the packaged VSIX                  |
| `published`    | Whether the VSIX was published to Visual Studio Marketplace        |

## 3. Usage

### 3.1. Publish with a Marketplace Token

Store the Marketplace credential as the `VSCE_PAT` repository or environment secret.

```yaml
jobs:
  vscode-extension:
    runs-on: ubuntu-latest
    permissions:
      contents: read
    steps:
      - uses: actions/checkout@v7

      - uses: actions/setup-node@v7
        with:
          node-version: "22"

      - run: npm ci
      - run: npm run check
      - run: npm run build

      - name: VS Code Extension
        uses: sentenz/actions/vscode-extension@latest
        with:
          marketplace-token: ${{ secrets.VSCE_PAT }}
```

### 3.2. Publish with a Release Version

`version` accepts relative increments (`patch`, `minor`, `major`) and exact release versions. Exact versions can be
provided either as `1.0.0` or as a Git-tag-style value such as `v1.0.0`.

```yaml
- name: VS Code Extension
  id: extension
  uses: sentenz/actions/vscode-extension@latest
  with:
    marketplace-token: ${{ secrets.VSCE_PAT }}
    version: "v1.5.0"
    skip-duplicate: "true"

- name: Inspect Published Version
  run: echo "${{ steps.extension.outputs.version }}"
```

A leading `v` is normalized away before packaging, so both `version: "v1.5.0"` and `version: "1.5.0"` produce a VSIX
with version `1.5.0`. This makes GitHub release tags directly consumable:

```yaml
with:
  version: ${{ github.event.release.tag_name }}
```

Relative increments remain available. For example, a manifest version of `1.4.2` with `version: "minor"` packages and
publishes `1.5.0`.

### 3.3. Package Without Publishing

Package-only mode is suitable for pull requests and other events that must not access release credentials.

```yaml
- name: VS Code Extension
  id: extension
  uses: sentenz/actions/vscode-extension@latest
  with:
    publish: "false"
    output-path: "artifacts/extension.vsix"

- name: Inspect Package Path
  run: echo "${{ steps.extension.outputs.vsix-path }}"
```

### 3.4. Publish a Pre-Release

```yaml
- name: VS Code Extension
  uses: sentenz/actions/vscode-extension@latest
  with:
    marketplace-token: ${{ secrets.VSCE_PAT }}
    pre-release: "true"
```

The extension manifest must use a version distinct from any regular release already uploaded to the Marketplace.

### 3.5. Publish for a Target Platform

```yaml
- name: VS Code Extension
  uses: sentenz/actions/vscode-extension@latest
  with:
    marketplace-token: ${{ secrets.VSCE_PAT }}
    target: "linux-x64"
    output-path: "artifacts/extension-linux-x64.vsix"
```

Supported targets are `win32-x64`, `win32-arm64`, `linux-x64`, `linux-armhf`, `linux-arm64`, `alpine-x64`,
`alpine-arm64`, `darwin-x64`, `darwin-arm64`, and `web`.

Run the action once per target when publishing platform-specific packages.

### 3.6. Publish with Microsoft Entra Authentication

After authenticating Azure CLI with an identity authorized for the Marketplace publisher, select
`azure-credential` authentication:

```yaml
- name: VS Code Extension
  uses: sentenz/actions/vscode-extension@latest
  with:
    authentication: "azure-credential"
```

The calling workflow is responsible for establishing the Azure CLI credential before this action runs. No
Marketplace token is required in this mode.

## 4. Configuration

### 4.1. Build Preparation

Current `@vscode/vsce` 3.x releases require Node.js 22 or newer. The action validates the runtime but does not install
Node.js or project dependencies.

Install dependencies and run release gates before invoking the action:

```yaml
- uses: actions/setup-node@v7
  with:
    node-version: "22"

- run: npm ci
- run: npm run lint
- run: npm test
- run: npm run build
```

During packaging, `vsce` invokes the extension's `vscode:prepublish` script when one is defined.

Use `no-dependencies: "true"` only when runtime dependencies have already been bundled into the extension or are
otherwise intentionally excluded from dependency discovery.

### 4.2. Versioning

When `version` is empty, the action packages the version already present in `package.json`.

When `version` is `patch`, `minor`, or `major`, the action resolves that increment against the manifest version. An
exact `major.minor.patch` value can also be supplied directly. Exact versions may optionally use a single lowercase
`v` prefix, which is normalized away before invoking `vsce`; `v1.0.0` therefore becomes package version `1.0.0`.

The resolved version is passed to `vsce package` with `--no-git-tag-version` and `--no-update-package-json`. This is
deliberate for CI: the generated VSIX receives the requested version without modifying the checked-out manifest,
creating a version commit, or creating a Git tag. Release commits and tags remain the responsibility of the release
workflow or release-management tooling.

The `version` output always reports the normalized version contained in the generated VSIX.

### 4.3. Authentication

With `authentication: "pat"`, the action exposes `marketplace-token` through the `VSCE_PAT` environment variable and
fails before packaging when publishing is enabled without a token.

The Marketplace publisher and the extension manifest's `publisher` value must already be configured. The token must
have Marketplace management permission for that publisher.

With `authentication: "azure-credential"`, the action passes `--azure-credential` to `vsce publish`. The calling
workflow must authenticate Azure CLI with an identity authorized for the Marketplace publisher.

### 4.4. Output Artifact

When `output-path` is empty, the action creates a unique VSIX under `RUNNER_TEMP`. A configured path is resolved
relative to `working-directory` and must remain inside `GITHUB_WORKSPACE` or `RUNNER_TEMP`.

The `vsix-path` output can be passed to artifact upload, release attachment, signing, or downstream validation steps.

When `skip-duplicate: "true"`, publishing passes `--skip-duplicate` to `vsce`, allowing idempotent release workflows
to succeed when the exact extension version is already present in the Marketplace.

### 4.5. Validation

The action fails before packaging when:

- Node.js or `npx` is unavailable, or Node.js is older than version 22;
- `working-directory` resolves outside `GITHUB_WORKSPACE` or lacks `package.json`;
- `publisher`, `name`, or `version` is absent from the extension manifest;
- `publish`, `pre-release`, `no-dependencies`, or `skip-duplicate` is not exactly `true` or `false`;
- `authentication` is not `pat` or `azure-credential`;
- token authentication is selected for publishing without `marketplace-token`;
- `vsce-version` is not `latest`, a major version, or an exact version;
- `version` is not empty, `patch`, `minor`, `major`, or an exact `major.minor.patch` version with an optional lowercase `v` prefix;
- a relative version increment is requested when the manifest version is not `major.minor.patch`;
- `target` is not a supported VS Code extension target; or
- `output-path` does not end in `.vsix` or resolves outside the permitted directories.
