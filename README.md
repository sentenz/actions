# Actions

[![License](https://img.shields.io/github/license/sentenz/percent)](https://opensource.org/licenses/Apache-2.0)

A centralized repository containing reusable GitHub Composite Actions for CI/CD pipelines.

- [1. Composite Actions](#1-composite-actions)
  - [1.1. Conftest](#11-conftest)
  - [1.2. Regal](#12-regal)
  - [1.3. Renovate](#13-renovate)
  - [1.4. Semantic Release](#14-semantic-release)
  - [1.5. Semgrep](#15-semgrep)
  - [1.6. Trivy](#16-trivy)
  - [1.7. Docker](#17-docker)
  - [1.8. VS Code Extension](#18-vs-code-extension)
- [2. References](#2-references)

## 1. Composite Actions

### 1.1. Conftest

- [Conftest Action](./conftest/README.md)
  > Validate configuration files using Open Policy Agent (OPA) and Rego policies.

### 1.2. Regal

- [Regal Action](./regal/README.md)
  > Lint Rego policies to ensure best practices and coding standards.

### 1.3. Renovate

- [Renovate Action](./renovate/README.md)
  > Automated dependency updates using Renovate.

### 1.4. Semantic Release

- [Semantic-Release Action](./semantic-release/README.md)
  > Automated semantic versioning and releases using semantic-release.

### 1.5. Semgrep

- [Semgrep Action](./semgrep/README.md)
  > Static analysis security testing (SAST) using Semgrep.

### 1.6. Trivy

- [Trivy Action](./trivy/README.md)
  > Comprehensive security scanning with Trivy for SBOM generation, vulnerability scanning, and license compliance.

### 1.7. Docker

- [Docker Action](./docker/README.md)
  > Build OCI container images and publish versioned and latest tags to GitHub Container Registry.

### 1.8. VS Code Extension

- [VS Code Extension Action](./vscode-extension/README.md)
  > Package and publish VS Code extensions to Visual Studio Marketplace.

## 2. References

- GitHub [Composite Actions](https://docs.github.com/en/actions/creating-actions/creating-a-composite-action) page.

## Development checks

Run the local metadata and regression checks with Python, Bash, Node.js, GNU
`realpath`, and `jq` installed:

```sh
python -m pip install PyYAML==6.0.3
python scripts/validate_actions.py
python -m unittest discover -s tests -p 'test_*.py' -v
```

The regression suite runs the actual composite shell blocks with inert scanner
commands. The action-specific workflows run the pinned scanner images and check
real Semgrep reports and positive/negative Conftest policy fixtures. Changes to
`lib/actions.sh` must be tested with every action that sources it.
