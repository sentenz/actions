# SPDX-License-Identifier: Apache-2.0
# Shared shell helpers for repository-local and downloaded composite actions.

actions_require_single_line() {
  case "$2" in
    *$'\r'* | *$'\n'*)
      printf '::error::%s must be a single line\n' "$1" >&2
      return 1
      ;;
  esac
}

actions_output() {
  local name="$1" value="$2" delimiter
  delimiter="gha-output-${RANDOM}-${RANDOM}-${RANDOM}-${RANDOM}"
  while grep -Fqx "${delimiter}" <<< "${value//$'\r'/}"; do
    delimiter="gha-output-${RANDOM}-${RANDOM}-${RANDOM}-${RANDOM}"
  done
  printf '%s<<%s\n%s\n%s\n' "${name}" "${delimiter}" "${value}" "${delimiter}" >> "${GITHUB_OUTPUT}"
}

actions_workspace() {
  ACTIONS_WORKSPACE="$(cd -- "${GITHUB_WORKSPACE:-${PWD}}" && pwd -P)"
  ACTIONS_WORKDIR="$(pwd -P)"
  actions_require_single_line workspace "${ACTIONS_WORKSPACE}"
  actions_require_single_line working-directory "${ACTIONS_WORKDIR}"
  case "${ACTIONS_WORKDIR}/" in
    "${ACTIONS_WORKSPACE}/"*) ;;
    *) echo '::error::Working directory must be inside GITHUB_WORKSPACE' >&2; return 1 ;;
  esac
}

actions_workspace_path() {
  local value="$1" label="$2" resolved
  actions_require_single_line "${label}" "${value}" || return 1
  if [[ -z "${value}" ]]; then
    printf '::error::%s must not be empty\n' "${label}" >&2
    return 1
  fi
  resolved="$(realpath -m -- "${value}")" || return 1
  actions_require_single_line "${label}" "${resolved}" || return 1
  case "${resolved}/" in
    "${ACTIONS_WORKSPACE}/"*) printf '%s\n' "${resolved}" ;;
    *) printf '::error::%s must resolve inside GITHUB_WORKSPACE\n' "${label}" >&2; return 1 ;;
  esac
}

actions_require_file() {
  if [[ ! -f "$1" || ! -r "$1" ]]; then
    printf '::error::Cannot read requested %s: %q\n' "$2" "$1" >&2
    return 1
  fi
}
