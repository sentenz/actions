# Trivy Composite Action

Comprehensive security scanning with Trivy for vulnerability detection, SBOM generation, attestation, and license compliance.

- [1. Details](#1-details)
- [2. Action](#2-action)
  - [2.1. Inputs](#21-inputs)
  - [2.2. Outputs](#22-outputs)
- [3. Usage](#3-usage)
  - [3.1. Filesystem Vulnerability Scanning](#31-filesystem-vulnerability-scanning)
  - [3.2. Container Image Scanning](#32-container-image-scanning)
  - [3.3. SBOM Generation (CycloneDX)](#33-sbom-generation-cyclonedx)
  - [3.4. SBOM Generation (SPDX)](#34-sbom-generation-spdx)
  - [3.5. SBOM Scanning](#35-sbom-scanning)
  - [3.6. License Compliance Scanning](#36-license-compliance-scanning)
  - [3.7. IaC Misconfiguration Scanning](#37-iac-misconfiguration-scanning)
  - [3.8. Secret Scanning](#38-secret-scanning)
  - [3.9. Repository Scanning](#39-repository-scanning)
- [4. Advanced Configuration](#4-advanced-configuration)
  - [4.1. Custom Ignore Rules](#41-custom-ignore-rules)
  - [4.2. Offline Scanning](#42-offline-scanning)
  - [4.3. Publishing SBOM to Release Notes](#43-publishing-sbom-to-release-notes)

## 1. Details

- [Trivy](https://github.com/aquasecurity/trivy)
  > A comprehensive and versatile security scanner for vulnerabilities, misconfigurations, secrets, SBOM, and license compliance.

- [Trivy Documentation](https://aquasecurity.github.io/trivy/)
  > Official documentation covering all Trivy features and use cases.

- [Trivy-Action](https://github.com/aquasecurity/trivy-action)
  > The official GitHub Action for running Trivy scans in CI/CD pipelines.

- [SBOM with Trivy](https://aquasecurity.github.io/trivy/latest/docs/supply-chain/sbom/)
  > Guide for generating and scanning Software Bill of Materials using Trivy.

- [License Scanning](https://aquasecurity.github.io/trivy/latest/docs/scanner/license/)
  > Documentation on scanning for license compliance issues.

## 2. Action

The [Trivy Action](./action.yml) provides comprehensive security scanning capabilities including vulnerability detection, SBOM generation and scanning (SPDX and CycloneDX formats), license compliance checking, and more.

### 2.1. Inputs

| Input                       | Description                                                                          | Required | Default                               |
| --------------------------- | ------------------------------------------------------------------------------------ | -------- | ------------------------------------- |
| `scan-type`                 | Type of scan (fs, image, config, repository, rootfs, sbom, sbom-generate)           | No       | `fs`                                  |
| `scan-target`               | Target to scan (path, image name, repo URL)                                          | No       | `.`                                   |
| `trivy-image`               | Trivy Docker image with version tag and digest                                       | No       | `aquasec/trivy:0.68.2@sha256:...`     |
| `format`                    | Output format (table, json, sarif, cyclonedx, spdx, spdx-json, github, cosign-vuln) | No       | `json`                                |
| `output`                    | Output file path (relative to workspace)                                             | No       | ``                                    |
| `severity`                  | Severities to report (UNKNOWN,LOW,MEDIUM,HIGH,CRITICAL)                              | No       | `UNKNOWN,LOW,MEDIUM,HIGH,CRITICAL`    |
| `vuln-type`                 | Vulnerability types (os,library)                                                     | No       | `os,library`                          |
| `scanners`                  | Scanners to use (vuln,misconfig,secret,license)                                      | No       | `vuln,secret`                         |
| `skip-dirs`                 | Directories to skip (comma-separated)                                                | No       | ``                                    |
| `skip-files`                | Files to skip (comma-separated)                                                      | No       | ``                                    |
| `timeout`                   | Scan timeout duration                                                                | No       | `5m0s`                                |
| `ignore-unfixed`            | Ignore unfixed vulnerabilities                                                       | No       | `false`                               |
| `exit-code`                 | Exit code when vulnerabilities are found                                             | No       | `1`                                   |
| `list-all-pkgs`             | List all packages regardless of vulnerabilities                                      | No       | `false`                               |
| `cache-dir`                 | Cache directory                                                                      | No       | ``                                    |
| `db-repository`             | Custom database repository                                                           | No       | ``                                    |
| `offline-scan`              | Disable external API requests                                                        | No       | `false`                               |
| `license-full`              | Show full license information                                                        | No       | `false`                               |
| `ignored-licenses`          | Licenses to ignore (comma-separated)                                                 | No       | ``                                    |
| `license-confidence-level`  | License confidence level (0.0-1.0)                                                   | No       | `0.9`                                 |
| `trivyignore-file`          | Path to .trivyignore file                                                            | No       | ``                                    |
| `github-pat`                | GitHub Personal Access Token for private repositories                                | No       | ``                                    |

### 2.2. Outputs

| Output        | Description                         |
| ------------- | ----------------------------------- |
| `result`      | Trivy scan result                   |
| `exit-code`   | Trivy exit code                     |
| `output-file` | Path to the output file (if created)|

## 3. Usage

### 3.1. Filesystem Vulnerability Scanning

Scan the current directory for vulnerabilities in dependencies:

```yaml
jobs:
  security-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/trivy@latest
        with:
          scan-type: "fs"
          scan-target: "."
          format: "sarif"
          output: "trivy-results.sarif"
          severity: "CRITICAL,HIGH"
```

### 3.2. Container Image Scanning

Scan a Docker image for vulnerabilities:

```yaml
jobs:
  image-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - name: Build image
        run: docker build -t myapp:latest .
      - uses: sentenz/actions/trivy@latest
        with:
          scan-type: "image"
          scan-target: "myapp:latest"
          format: "json"
          output: "image-scan.json"
          severity: "CRITICAL,HIGH"
```

### 3.3. SBOM Generation (CycloneDX)

Generate a Software Bill of Materials in CycloneDX format:

```yaml
jobs:
  sbom-generate:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - name: Generate SBOM (CycloneDX)
        uses: sentenz/actions/trivy@latest
        with:
          scan-type: "sbom-generate"
          scan-target: "."
          format: "cyclonedx"
          output: "sbom.cdx.json"

      - name: Upload SBOM as artifact
        uses: actions/upload-artifact@v4
        with:
          name: sbom-cyclonedx
          path: sbom.cdx.json

      - name: Attach SBOM to release
        if: github.event_name == 'release'
        uses: softprops/action-gh-release@v2
        with:
          files: sbom.cdx.json
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

### 3.4. SBOM Generation (SPDX)

Generate a Software Bill of Materials in SPDX format:

```yaml
jobs:
  sbom-spdx:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - name: Generate SBOM (SPDX-JSON)
        uses: sentenz/actions/trivy@latest
        with:
          scan-type: "sbom-generate"
          scan-target: "."
          format: "spdx-json"
          output: "sbom.spdx.json"

      - name: Generate SBOM (SPDX)
        uses: sentenz/actions/trivy@latest
        with:
          scan-type: "sbom-generate"
          scan-target: "."
          format: "spdx"
          output: "sbom.spdx"

      - name: Upload SBOM artifacts
        uses: actions/upload-artifact@v4
        with:
          name: sbom-spdx
          path: |
            sbom.spdx.json
            sbom.spdx
```

### 3.5. SBOM Scanning

Scan an existing SBOM for vulnerabilities:

```yaml
jobs:
  sbom-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - name: Scan SBOM
        uses: sentenz/actions/trivy@latest
        with:
          scan-type: "sbom"
          scan-target: "sbom.cdx.json"
          format: "table"
          severity: "HIGH,CRITICAL"
```

### 3.6. License Compliance Scanning

Scan for license compliance issues:

```yaml
jobs:
  license-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/trivy@latest
        with:
          scan-type: "fs"
          scan-target: "."
          scanners: "license"
          format: "table"
          output: "license-report.txt"
          license-full: "true"
          ignored-licenses: "MIT,Apache-2.0"
```

### 3.7. IaC Misconfiguration Scanning

Scan Infrastructure-as-Code files for misconfigurations:

```yaml
jobs:
  iac-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/trivy@latest
        with:
          scan-type: "config"
          scan-target: "."
          format: "sarif"
          output: "trivy-iac.sarif"
          severity: "HIGH,CRITICAL"
```

### 3.8. Secret Scanning

Scan for hardcoded secrets and credentials:

```yaml
jobs:
  secret-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1
      - uses: sentenz/actions/trivy@latest
        with:
          scan-type: "fs"
          scan-target: "."
          scanners: "secret"
          format: "sarif"
          output: "trivy-secrets.sarif"
```

### 3.9. Repository Scanning

Scan a remote repository:

```yaml
jobs:
  repo-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: sentenz/actions/trivy@latest
        with:
          scan-type: "repository"
          scan-target: "https://github.com/example/repo"
          format: "json"
          output: "repo-scan.json"
          github-pat: ${{ secrets.GITHUB_TOKEN }}
```

## 4. Advanced Configuration

### 4.1. Custom Ignore Rules

Create a `.trivyignore` file in your repository to ignore specific vulnerabilities:

```
# .trivyignore
# Ignore a specific CVE
CVE-2021-12345

# Ignore all vulnerabilities in a specific package
pkg:golang/example.com/package@1.0.0
```

Use it in your workflow:

```yaml
- uses: sentenz/actions/trivy@latest
  with:
    scan-type: "fs"
    scan-target: "."
    trivyignore-file: ".trivyignore"
```

### 4.2. Offline Scanning

For air-gapped environments, use offline scanning:

```yaml
- uses: sentenz/actions/trivy@latest
  with:
    scan-type: "fs"
    scan-target: "."
    offline-scan: "true"
    cache-dir: ".trivy-cache"
```

### 4.3. Publishing SBOM to Release Notes

Complete workflow for generating and publishing SBOM:

```yaml
name: Release with SBOM

on:
  release:
    types: [created]

jobs:
  sbom-and-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6.0.1

      # Generate SBOM in both formats
      - name: Generate CycloneDX SBOM
        uses: sentenz/actions/trivy@latest
        with:
          scan-type: "sbom-generate"
          scan-target: "."
          format: "cyclonedx"
          output: "sbom.cdx.json"

      - name: Generate SPDX SBOM
        uses: sentenz/actions/trivy@latest
        with:
          scan-type: "sbom-generate"
          scan-target: "."
          format: "spdx-json"
          output: "sbom.spdx.json"

      # Scan the SBOM for vulnerabilities
      - name: Scan SBOM
        uses: sentenz/actions/trivy@latest
        with:
          scan-type: "sbom"
          scan-target: "sbom.cdx.json"
          format: "json"
          output: "sbom-scan-results.json"
        continue-on-error: true

      # License compliance check
      - name: License Compliance
        uses: sentenz/actions/trivy@latest
        with:
          scan-type: "sbom"
          scan-target: "sbom.cdx.json"
          scanners: "license"
          format: "table"
          output: "license-compliance.txt"
        continue-on-error: true

      # Upload to release
      - name: Upload SBOMs to Release
        uses: softprops/action-gh-release@v2
        with:
          files: |
            sbom.cdx.json
            sbom.spdx.json
            sbom-scan-results.json
            license-compliance.txt
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```
