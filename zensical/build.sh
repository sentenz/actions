#!/bin/sh
# SPDX-License-Identifier: Apache-2.0
set -eu

export PYTHONPATH="/opt/zensical-extensions${PYTHONPATH:+:${PYTHONPATH}}"
export PATH="${HOME}/.local/bin:${PATH}"

case "${GFM:-false}" in
  true)
    python -m pip install --user --disable-pip-version-check -r /opt/zensical-extensions/requirements-gfm.txt
    ;;
  false)
    ;;
  *)
    printf 'Invalid gfm input: %s (expected true or false)\n' "$GFM" >&2
    exit 1
    ;;
esac
exec python -m zensical build --clean
