# SPDX-License-Identifier: Apache-2.0
FROM alpine:3.24@sha256:28bd5fe8b56d1bd048e5babf5b10710ebe0bae67db86916198a6eec434943f8b AS emulation-check
RUN uname -m >/dev/null

FROM scratch AS fixture

ARG FIXTURE_VALUE=default

COPY --from=emulation-check /etc/alpine-release /alpine-release

LABEL org.opencontainers.image.title="docker-action-ci" \
      org.opencontainers.image.description="Build-only fixture for the container image composite action" \
      io.github.sentenz.actions.fixture-value="${FIXTURE_VALUE}"
