#!/bin/sh
# SPDX-License-Identifier: Apache-2.0
set -eu

export PYTHONPATH="/opt/zensical-extensions${PYTHONPATH:+:${PYTHONPATH}}"

if [ -n "${REQUIREMENTS_FILE}" ]; then
  export PATH="${HOME}/.local/bin:${PATH}"
  python -m pip install --user --disable-pip-version-check -r "${REQUIREMENTS_FILE}"
fi

if [ -n "${TESTS_DIRECTORY}" ]; then
  python -m unittest discover -s "${TESTS_DIRECTORY}" -v
fi

if [ -n "${BUILD_SCRIPT}" ]; then
  exec python -- "${BUILD_SCRIPT}" build
fi
exec python -m zensical build --clean
