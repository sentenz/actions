# Workflow examples

Copy the selected YAML files into `.github/workflows/` in the consuming repository. Adjust branch filters to match the repository's development and release branches.

| Task | Workflow | Setup and behavior |
| --- | --- | --- |
| Build a container image | [docker.yml](docker.yml) | Validates the root Dockerfile on pull requests and publishes to GHCR when a release is published. |
| Sign a container image | [docker-sigstore.yml](docker-sigstore.yml) | Validates pull requests, then publishes a multi-platform image on a version tag, signs its digest and retains the verification bundle. Set the Docker action's `context` and `file` inputs in both jobs for a different build layout. |
| Test a Kubernetes deployment | [kind.yml](kind.yml) | Applies the Kustomization in `deploy/` to a disposable cluster and checks Deployment, StatefulSet and DaemonSet rollouts. Set the repository variable `KUSTOMIZE_PATH` for a different overlay. |
| Check Rego policies | [conftest.yml](conftest.yml) and [regal.yml](regal.yml) | Both expect policies in `policy/`. Set the repository variable `POLICY_PATH` for a different directory. |
| Scan a change | [trivy.yml](trivy.yml) | Scans the filesystem, configuration, licenses and a generated CycloneDX SBOM. The filesystem and configuration jobs upload SARIF to GitHub code scanning. |
| Attach SBOMs during semantic release | [semantic-release.yml](semantic-release.yml) and [sbom.yml](sbom.yml) | Copy both files. The release workflow passes the published Git tag to the reusable SBOM workflow. |
| Publish signed release SBOMs | [trivy-sbom-sigstore.yml](trivy-sbom-sigstore.yml) | Generates and scans CycloneDX and SPDX documents, signs and verifies each file, then attaches both documents and their verification bundles to the tagged release. Creates the release if needed. |

Choose one image publisher per GHCR package and one SBOM publisher per release. The two tag-triggered signing examples accept stable tags such as `1.2.3` and `v1.2.3`. For a manual run, select the release tag as the workflow ref. Signing uses the public Sigstore service.

Tags created by a workflow using its built-in `GITHUB_TOKEN` do not start tag-push workflows. The paired semantic-release and SBOM examples handle this through a direct workflow call. For automated tag-triggered signing, the tag publisher can use a GitHub App installation token or a personal access token. [GitHub workflow trigger documentation](https://docs.github.com/en/actions/how-tos/write-workflows/choose-when-workflows-run/trigger-a-workflow)
