# Sigstore Cosign Composite Action

Sign OCI image digests and local files with keyless GitHub Actions OIDC, then
require immediate signature verification. The action installs Cosign v3.1.4
through a commit-pinned installer and uses ephemeral signing keys.

- [1. Action](#1-action)
  - [1.1. Inputs](#11-inputs)
  - [1.2. Outputs](#12-outputs)
  - [1.3. Permissions](#13-permissions)
- [2. Usage](#2-usage)
  - [2.1. Images](#21-images)
  - [2.2. Files](#22-files)
- [3. Trust and Verification](#3-trust-and-verification)
- [4. Troubleshooting](#4-troubleshooting)
- [5. Development](#5-development)

## 1. Action

The [Sigstore action](./action.yml) accepts one image or a list of files per
invocation. Both signing modes use `--yes` and verify the exact supplied
certificate identity and OIDC issuer. Signing or verification failures fail
the action.

| Mode | Signing command | Verification command | Signature storage |
| --- | --- | --- | --- |
| Image | `cosign sign` | `cosign verify` | OCI registry and local bundle |
| Files | `cosign sign-blob` | `cosign verify-blob` | Adjacent local bundles |

Signing an SBOM file authenticates that document. Binding an SBOM to an image
requires a separately produced OCI attestation, such as `cosign attest`.
Artifact production, release publication, and deployment verification belong
to the calling workflow and consuming systems.

### 1.1. Inputs

| Input | Default | Required | Purpose |
| --- | --- | --- | --- |
| `image` | Empty | In image mode | Fully qualified OCI reference ending in `@sha256:<64 lowercase hex characters>`. |
| `files` | Empty | In file mode | Newline-delimited exact paths to existing, nonempty regular files. Globs are not expanded. |
| `identity` | None | Yes | Exact expected certificate identity, including the signing workflow path and Git ref. |
| `issuer` | `https://token.actions.githubusercontent.com` | No | Exact expected certificate OIDC issuer. |
| `bundle` | `image.sigstore.json` | No | Output bundle path for an image signature. File mode uses adjacent bundles. |
| `deployment` | `public` | No | Selects `public` or `private` Sigstore infrastructure. |
| `signing-config` | Empty | In private mode | Path to an approved, nonempty signing configuration file. |
| `trusted-root` | Empty | In private mode | Path to an approved, nonempty trusted root file for signing and verification. |

Supply exactly one of `image` and `files`. Leave `signing-config` and
`trusted-root` empty in public mode. The action rejects mutable image tags,
empty file lists, missing or empty files, artifact symlinks, file inputs ending
in `.sigstore.json`, and symlinked bundle destinations.

### 1.2. Outputs

The action declares no GitHub Actions step outputs. Subsequent steps consume
these artifacts by path or registry reference.

| Mode | Result |
| --- | --- |
| Image | Registry signature and a local bundle at `bundle`, defaulting to `image.sigstore.json`. |
| Files | A bundle at `<file>.sigstore.json` adjacent to each input file. |

Bundles contain signatures, signing certificates, and verification evidence.
The action checks that each expected bundle exists and is nonempty. Retain
registry signatures and distribute files with their bundles for the artifact
support lifetime. Configure upload destinations and retention in the caller.

### 1.3. Permissions

Grant permissions at job scope according to the signing mode and publication
steps. Authenticate to the image registry before invoking image signing.

| Permission | Access | Purpose |
| --- | --- | --- |
| `id-token` | `write` | Request a GitHub Actions OIDC token for both signing modes. |
| `contents` | `read` | Check out repository files using `GITHUB_TOKEN`. |
| `contents` | `write` | Check out files and publish GitHub release assets when the caller performs release publication. |
| `packages` | `write` | Publish image signatures to GHCR using `GITHUB_TOKEN`. File signing alone does not require this permission. |

> [!NOTE]
> A reusable workflow's calling job must grant `id-token: write` so the called
> signing job can use it. The expected identity names the workflow performing
> signing, including the called workflow when reusable workflows are used.

## 2. Usage

Replace `<commit-sha>` with a reviewed commit of `sentenz/actions` containing
`sigstore/action.yml`. Examples use Ubuntu runners. Set `identity` to the
approved workflow path and ref for the consuming repository.

### 2.1. Images

After a `sentenz/actions/docker` step with `id: docker-build` has published the
image and authenticated to GHCR, use its `image` and `digest` outputs. Grant
`contents: read`, `packages: write`, and `id-token: write` to this job.

```yaml
- name: Sign and verify image
  uses: sentenz/actions/sigstore@<commit-sha>
  with:
    image: ${{ steps.docker-build.outputs.image }}@${{ steps.docker-build.outputs.digest }}
    identity: https://github.com/${{ github.repository }}/.github/workflows/docker.yml@refs/heads/main

- name: Retain image bundle
  uses: actions/upload-artifact@ea165f8d65b6e75b540449e92b4886f43607fa02 # v4
  with:
    name: image-signature
    path: image.sigstore.json
    if-no-files-found: error
    retention-days: 90
```

The Docker action publishes the image before signing. If verification fails,
the image remains in the registry. Consumers must verify the approved digest
before deployment.

### 2.2. Files

Produce the files before invoking this step. Grant `id-token: write` and any
permissions needed by the producer or publisher. Archives, SBOMs, and firmware
use the same interface.

```yaml
- name: Sign and verify release files
  uses: sentenz/actions/sigstore@<commit-sha>
  with:
    identity: https://github.com/${{ github.repository }}/.github/workflows/release.yml@refs/heads/main
    files: |
      dist/source.tar.gz
      dist/sbom.spdx.json
      dist/firmware.bin

- name: Retain files and bundles
  uses: actions/upload-artifact@ea165f8d65b6e75b540449e92b4886f43607fa02 # v4
  with:
    name: signed-release-files
    path: |
      dist/source.tar.gz
      dist/source.tar.gz.sigstore.json
      dist/sbom.spdx.json
      dist/sbom.spdx.json.sigstore.json
      dist/firmware.bin
      dist/firmware.bin.sigstore.json
    if-no-files-found: error
```

Only the supplied local files are signed. Publish the corresponding bundles
alongside release assets when distributing through GitHub Releases.

## 3. Trust and Verification

Public Sigstore is the default. Signing metadata is disclosed to public
Sigstore, and Cosign uses its embedded public TUF trust anchor and authenticated
updates. For private infrastructure, select `deployment: private` and provide
both `signing-config` and `trusted-root` paths. Review and distribute those
files independently of the artifacts. Configure approved Fulcio, CT, and Rekor
services without public fallback endpoints. Private Fulcio must accept GitHub
Actions OIDC with audience `sigstore`.

The caller supplies the approved identity and issuer. Protect the signing
workflow, its referenced actions, and approved branches or tags with review
requirements. The action validates the supplied values during verification.

Consumers can verify an immutable image digest with:

```bash
cosign verify "$IMAGE_DIGEST" \
  --certificate-identity 'https://github.com/ORG/REPO/.github/workflows/docker.yml@refs/heads/main' \
  --certificate-oidc-issuer 'https://token.actions.githubusercontent.com'
```

For a local file and its bundle:

```bash
cosign verify-blob dist/firmware.bin \
  --bundle dist/firmware.bin.sigstore.json \
  --certificate-identity 'https://github.com/ORG/REPO/.github/workflows/release.yml@refs/heads/main' \
  --certificate-oidc-issuer 'https://token.actions.githubusercontent.com'
```

Proceed with deployment or installation only after successful verification.
Private consumers must also pass their approved `--trusted-root` file.

## 4. Troubleshooting

### Signing action cannot be found for an older release tag

A local reference such as `uses: ./.github/actions/sigstore` loads the action
from the job workspace. Checking out a release tag created before that action
existed can cause a missing `action.yml` error before Cosign starts.

Use `sentenz/actions/sigstore@<commit-sha>` with a reviewed commit containing
this action. GitHub loads that action revision independently of the artifact
repository checkout. Confirm both the `sigstore` path and commit when action
lookup fails. The certificate identity continues to name the signing workflow
and its ref in the consuming repository.

### OIDC authentication or identity verification fails

Check the signing job's `id-token: write` permission and any reusable workflow
caller permissions. Confirm that `identity` matches the actual signing workflow
and ref, and that `issuer` matches the certificate issuer. For private deployments,
check that Fulcio accepts the GitHub Actions token with audience `sigstore`.

### Image signing cannot write to the registry

Authenticate to the target registry before signing. For GHCR, grant
`packages: write` and confirm the token has access to the package. The shared
Docker action establishes registry authentication when publishing an image.

## 5. Development

Run `python3 sigstore/test/test_action.py` from the repository root. The tests
execute the action's Bash script with a mock Cosign executable to check input
validation, bundle handling, command arguments, and verification failures.
The [test workflow](../.github/workflows/sigstore-test.yml) runs these checks
with read-only repository access. Live OIDC and registry signing require a
trusted workflow run with the permissions described above.

The action was imported from
[template-k8s PR #152](https://github.com/sentenz/template-k8s/pull/152)
at commit `3f626d9b5eec82d4fe85bfed29b3234aa92b694e`.
