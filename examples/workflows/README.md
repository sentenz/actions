# Example Workflow Files

This directory contains example workflow files demonstrating how to use the centralized GitHub Actions.

## 1. Available Examples

| Workflow                                       | Description                                     |
| ---------------------------------------------- | ----------------------------------------------- |
| [conftest.yml](./conftest.yml)                 | Policy-as-Code (PaC) validation workflow              |
| [regal.yml](./regal.yml)                       | Rego policy linting workflow                    |
| [renovate.yml](./renovate.yml)                 | Automated dependency updates workflow           |
| [semantic-release.yml](./semantic-release.yml) | Automated release workflow                      |
| [semgrep.yml](./semgrep.yml)                   | Security scanning workflow                      |
| [ci.yml](./ci.yml)                             | Complete CI pipeline combining multiple actions |

## 2. Usage

Copy these workflow files to your repository's `.github/workflows/` directory and modify as needed.

## 3. Notes

- Replace `@main` with a specific version tag for production use
- Ensure required secrets are configured in your repository settings
- Review and adjust permissions as needed for your security requirements
