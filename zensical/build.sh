#!/bin/sh
# SPDX-License-Identifier: Apache-2.0
set -eu

export PYTHONPATH="/opt/zensical-extensions${PYTHONPATH:+:${PYTHONPATH}}"
export PATH="${HOME}/.local/bin:${PATH}"

python -m pip install --user --disable-pip-version-check -r /opt/zensical-extensions/requirements-gfm.txt
exec python -m zensical build --clean
