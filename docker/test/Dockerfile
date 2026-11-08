# SPDX-License-Identifier: Apache-2.0
FROM alpine:3.24@sha256:3f4d6b32e26703c6082839234f43769d5046b3ac7c910033911d1e71d8cd630a AS emulation-check
RUN uname -m >/dev/null

FROM scratch AS fixture

ARG FIXTURE_VALUE=default

COPY --from=emulation-check /etc/alpine-release /alpine-release

LABEL org.opencontainers.image.title="docker-action-ci" \
      org.opencontainers.image.description="Build-only fixture for the container image composite action" \
      io.github.sentenz.actions.fixture-value="${FIXTURE_VALUE}"
