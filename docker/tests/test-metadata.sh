#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
# Exercise the Docker composite action's metadata/input validation without a registry.

set -euo pipefail
cd "$(dirname "$0")/../.."

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

yq eval -r '.runs.steps[] | select(.id == "metadata") | .run' docker/action.yml > "$tmpdir/metadata.sh"
bash -n "$tmpdir/metadata.sh"

export INPUT_GITHUB_TOKEN=""
export INPUT_REGISTRY_USERNAME="sentenz"
export INPUT_VERSION="1.2.3"
export INPUT_CONTEXT="."
export INPUT_FILE="Dockerfile"
export INPUT_IMAGE_NAME="api"
export INPUT_PLATFORMS="linux/amd64,linux/arm64"
export INPUT_TARGET=""
export INPUT_PUSH="false"
export INPUT_PUBLISH_LATEST="false"
export INPUT_PROVENANCE="auto"
export INPUT_SBOM="auto"
export REPOSITORY="sentenz/test"
export REPOSITORY_OWNER="sentenz"
export RUNNER_ARCH="X64"

pass() {
  : > "$tmpdir/output"
  if ! env GITHUB_OUTPUT="$tmpdir/output" "$@" bash "$tmpdir/metadata.sh" > "$tmpdir/log" 2>&1; then
    cat "$tmpdir/log" >&2
    echo "Expected valid Docker metadata inputs" >&2
    exit 1
  fi
}

fail() {
  local expected="$1"
  shift
  : > "$tmpdir/output"
  if env GITHUB_OUTPUT="$tmpdir/output" "$@" bash "$tmpdir/metadata.sh" > "$tmpdir/log" 2>&1; then
    echo "Expected input validation to fail: $expected" >&2
    exit 1
  fi
  if ! grep -Fq -- "$expected" "$tmpdir/log"; then
    cat "$tmpdir/log" >&2
    echo "Expected failure message: $expected" >&2
    exit 1
  fi
}

pass
grep -Fxq "image=ghcr.io/sentenz/api" "$tmpdir/output"
grep -Fxq "qemu-platforms=arm64" "$tmpdir/output"
grep -Fxq "provenance=" "$tmpdir/output"
grep -Fxq "sbom=" "$tmpdir/output"
grep -Fxq "ghcr.io/sentenz/api:1.2.3" "$tmpdir/output"
! grep -Fxq "ghcr.io/sentenz/api:latest" "$tmpdir/output"

pass INPUT_PUBLISH_LATEST=true INPUT_PROVENANCE=mode=max INPUT_SBOM=true
grep -Fxq "ghcr.io/sentenz/api:latest" "$tmpdir/output"
grep -Fxq "provenance=mode=max" "$tmpdir/output"
grep -Fxq "sbom=true" "$tmpdir/output"

pass INPUT_SBOM=false
 grep -Fxq "sbom=false" "$tmpdir/output"

pass INPUT_PUSH=true INPUT_GITHUB_TOKEN=test-token
grep -Fxq "push=true" "$tmpdir/output"

fail "provenance must be" INPUT_PROVENANCE=mode=unknown
fail "sbom must be auto, true, or false" INPUT_SBOM=invalid
fail "publish-latest must be true or false" INPUT_PUBLISH_LATEST=invalid
fail "push must be true or false" INPUT_PUSH=invalid
fail "reserved latest tag" INPUT_VERSION=latest
fail "github-token must not be empty" INPUT_PUSH=true

echo "Docker metadata and attestation input tests passed"
