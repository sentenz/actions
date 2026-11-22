#!/bin/sh
# SPDX-License-Identifier: Apache-2.0
set -eu

export PYTHONPATH="/opt/zensical-extensions${PYTHONPATH:+:${PYTHONPATH}}"
export PATH="${HOME}/.local/bin:${PATH}"

case "${FLAVOR:-zensical}" in
  zensical|gdm) ;;
  *)
    echo "Unsupported Markdown flavor: ${FLAVOR}. Expected zensical or gdm." >&2
    exit 2
    ;;
esac

# Keep installing the bundled renderer for projects that explicitly configure
# sentenz_gfm, even when the flavor input is omitted.
python -m pip install --user --disable-pip-version-check -r /opt/zensical-extensions/requirements-gfm.txt

if [ "${FLAVOR:-zensical}" = "gdm" ]; then
  config="$(python /opt/zensical-extensions/enable_gfm.py)"
  trap 'rm -f "${config}"' EXIT
  python -m zensical build --clean --config-file "${config}"
else
  exec python -m zensical build --clean
fi
