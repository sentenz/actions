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

## 2. References

- GitHub [Composite Actions](https://docs.github.com/en/actions/creating-actions/creating-a-composite-action) page.
