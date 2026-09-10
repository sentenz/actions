#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail

fixture_dir="$(mktemp -d "${RUNNER_TEMP}/trufflehog-test.XXXXXX")"
git -C "${fixture_dir}" init --quiet --initial-branch=main
git -C "${fixture_dir}" config user.name "TruffleHog CI"
git -C "${fixture_dir}" config user.email "trufflehog-ci@example.invalid"

printf 'TruffleHog integration fixture\n' > "${fixture_dir}/README.md"
git -C "${fixture_dir}" add README.md
git -C "${fixture_dir}" commit --quiet -m "Initial fixture"
base="$(git -C "${fixture_dir}" rev-parse HEAD)"

printf 'A clean change\n' >> "${fixture_dir}/README.md"
git -C "${fixture_dir}" add README.md
git -C "${fixture_dir}" commit --quiet -m "Clean change"
clean="$(git -C "${fixture_dir}" rev-parse HEAD)"

# Assemble an inert URI credential only in the temporary fixture repository.
printf 'https://%s:%s@scanner.invalid/resource\n' \
  'ci-fixture' 'synthetic-credential-93e4' > "${fixture_dir}/credentials.txt"
git -C "${fixture_dir}" add credentials.txt
git -C "${fixture_dir}" commit --quiet -m "Synthetic credential"
finding="$(git -C "${fixture_dir}" rev-parse HEAD)"
git -C "${fixture_dir}" checkout --quiet "${clean}"

{
  printf 'path=%s\n' "${fixture_dir}"
  printf 'base=%s\n' "${base}"
  printf 'clean=%s\n' "${clean}"
  printf 'finding=%s\n' "${finding}"
} >> "${GITHUB_OUTPUT}"
