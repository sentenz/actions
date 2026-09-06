"""Run the real action shell blocks with inert scanner commands.

These tests check command construction, input boundaries, and output contracts.
The action-specific GitHub workflows exercise the real scanner binaries.
"""

import copy
import json
import os
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

import yaml

from scripts.validate_actions import validate

ROOT = Path(__file__).resolve().parents[1]
STUB = '''#!/usr/bin/env python3
import json, os, pathlib, sys
args = sys.argv[1:]
pathlib.Path(os.environ["STUB_LOG"]).write_text(json.dumps(args))
if "semgrep" in args:
    for flag, body in (("--json-output", os.environ.get("STUB_JSON", '{"results":[]}')),
                       ("--sarif-output", '{"runs":[]}'),
                       ("--gitlab-sast-output", '{"vulnerabilities":[]}')):
        if flag in args and body != "__missing__":
            pathlib.Path(args[args.index(flag) + 1]).write_text(body)
print(os.environ.get("STUB_STDOUT", "scanner diagnostic"))
sys.exit(int(os.environ.get("STUB_EXIT", "0")))
'''


def outputs(path):
    result = {}
    lines = iter(path.read_text().splitlines())
    for line in lines:
        if "<<" in line:
            name, delimiter = line.split("<<", 1)
            value = []
            for line in lines:
                if line == delimiter:
                    break
                value.append(line)
            else:
                raise AssertionError("unterminated output record")
            result[name] = "\n".join(value)
        elif "=" in line:
            name, value = line.split("=", 1)
            result[name] = value
        elif line:
            raise AssertionError(f"invalid output record: {line!r}")
    return result


class CompositeTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.base = Path(self.temporary.name)
        self.workspace = self.base / "workspace with spaces"
        self.workspace.mkdir()
        self.bin = self.base / "bin"
        self.bin.mkdir()
        for name in ("docker", "regal"):
            path = self.bin / name
            path.write_text(STUB)
            path.chmod(0o755)
        (self.workspace / "policy").mkdir()
        (self.workspace / "src").mkdir()
        (self.workspace / "package.json").write_text(json.dumps({
            "name": "fixture", "publisher": "sentenz", "version": "1.0.0",
            "engines": {"vscode": "^1.96.0"},
        }))
        self.output = self.base / "output"
        self.log = self.base / "command.json"

    def run_step(self, action, step_id, inputs=None, extra_env=None, cwd=None):
        metadata = yaml.safe_load((ROOT / action / "action.yml").read_text())
        step = next(s for s in metadata["runs"]["steps"] if s.get("id", s.get("name")) == step_id)
        values = {k: str(v.get("default", "")) for k, v in metadata.get("inputs", {}).items()}
        values.update(inputs or {})
        contexts = {
            "github.action_path": str(ROOT / action), "github.repository": "sentenz/actions",
            "github.repository_owner": "sentenz", "github.actor": "fixture-actor",
            "github.token": "workflow-token", "runner.arch": "X64",
        }
        env = dict(os.environ)
        env.update({
            "PATH": f"{self.bin}:{env['PATH']}", "GITHUB_WORKSPACE": str(self.workspace),
            "RUNNER_TEMP": str(self.base), "GITHUB_OUTPUT": str(self.output),
            "GITHUB_ENV": str(self.output), "GITHUB_REPOSITORY": "sentenz/actions",
            "STUB_LOG": str(self.log),
        })
        for key, value in step.get("env", {}).items():
            match = re.fullmatch(r"\$\{\{\s*(.*?)\s*\}\}", str(value))
            if match:
                expression = match[1]
                env[key] = values[expression[7:]] if expression.startswith("inputs.") else contexts[expression]
            else:
                env[key] = str(value)
        env.update(extra_env or {})
        self.output.write_text("")
        self.log.unlink(missing_ok=True)
        self.completed = subprocess.run(
            ["bash", "-e", "-o", "pipefail", "-c", step["run"]],
            cwd=cwd or self.workspace, env=env, text=True, capture_output=True, timeout=15,
        )
        return self.completed, outputs(self.output)

    def assert_success(self, completed):
        self.assertEqual(completed.returncode, 0, completed.stdout + completed.stderr)

    def command(self):
        return json.loads(self.log.read_text())

    def test_output_records_do_not_create_additional_keys(self):
        payload = "report\nEOF\npush=true\npublish=true\nresult<<EOF\nmore"
        env = dict(os.environ, GITHUB_OUTPUT=str(self.output), PAYLOAD=payload)
        subprocess.run(["bash", "-e", "-c", 'source "$1"; actions_output result "$PAYLOAD"',
                        "test", str(ROOT / "lib/actions.sh")], env=env, check=True)
        self.assertEqual(outputs(self.output), {"result": payload})

    def test_docker_rejects_output_injection(self):
        for payload in ("actor\npush=true", "actor\rpush=true"):
            with self.subTest(payload=repr(payload)):
                completed, result = self.run_step("docker", "metadata", {
                    "version": "1.0.0", "push": "false", "registry-username": payload,
                    "github-token": "unused-fixture-token",
                })
                self.assertNotEqual(completed.returncode, 0)
                self.assertNotEqual(result.get("push"), "true")
                self.assertIn("single line", completed.stderr)

    def test_docker_build_only_outputs_stay_false(self):
        completed, result = self.run_step("docker", "metadata", {"version": "1.0.0", "push": "false"})
        self.assert_success(completed)
        self.assertEqual(result["push"], "false")
        self.assertIn("ghcr.io/sentenz/actions:1.0.0", result["tags"])

    def test_vscode_rejects_working_directory_output_injection(self):
        directory = "extension\npublish=true"
        (self.workspace / directory).mkdir()
        completed, result = self.run_step("vscode-extension", "validate", {
            "working-directory": directory, "publish": "false", "marketplace-token": "unused-fixture-token",
        })
        self.assertNotEqual(completed.returncode, 0)
        self.assertNotEqual(result.get("publish"), "true")
        self.assertIn("single line", completed.stderr)

    def test_vscode_package_only_outputs_stay_false(self):
        completed, result = self.run_step("vscode-extension", "validate", {"publish": "false"})
        self.assert_success(completed)
        self.assertEqual(result["publish"], "false")

    def test_vscode_output_symlink_cannot_escape_workspace(self):
        outside = self.base / "outside.vsix"
        # RUNNER_TEMP is independently bounded so the target is outside both roots.
        runner_temp = self.base / "runner-temp"
        runner_temp.mkdir()
        (self.workspace / "escaped.vsix").symlink_to(outside)
        completed, _ = self.run_step("vscode-extension", "validate", {"output-path": "escaped.vsix"},
                                     {"RUNNER_TEMP": str(runner_temp)})
        self.assertNotEqual(completed.returncode, 0)
        self.assertFalse(outside.exists())

    def test_semantic_release_rejects_multiline_working_directory(self):
        completed, _ = self.run_step("semantic-release", "validate", {
            "github-token": "unused-fixture-token", "working-directory": "dir\ncreated=true",
        })
        self.assertNotEqual(completed.returncode, 0)
        self.assertIn("single line", completed.stderr)

    def test_explicit_missing_scanner_files_fail_before_execution(self):
        cases = [("conftest", "config-file"), ("conftest", "data-files"),
                 ("conftest", "policy-path"), ("regal", "config-file"),
                 ("trivy", "trivy-config"), ("trivy", "trivyignore-file")]
        for action, field in cases:
            with self.subTest(action=action, field=field):
                completed, _ = self.run_step(action, action, {field: "missing-file"})
                self.assertNotEqual(completed.returncode, 0)
                self.assertFalse(self.log.exists(), "scanner must not run after invalid configuration")

    def test_conftest_preserves_absolute_configuration_and_data_paths(self):
        config = self.workspace / "custom.toml"
        config.write_text("")
        # data-files is explicitly space-separated; use a relative filename here.
        (self.workspace / "data.json").write_text("{}")
        completed, _ = self.run_step("conftest", "conftest", {"config-file": str(config), "data-files": "data.json"})
        self.assert_success(completed)
        args = self.command()
        self.assertEqual(args[args.index("--config-file") + 1], str(config))
        self.assertEqual(args[args.index("--data") + 1], str(self.workspace / "data.json"))
        self.assertIn(f"{self.workspace}:{self.workspace}", args)

    def test_semgrep_honors_target_and_minimum_severity(self):
        completed, _ = self.run_step("semgrep", "semgrep", {"path": "src", "severity": "WARNING"})
        self.assert_success(completed)
        args = self.command()
        self.assertEqual(args[-1], str(self.workspace / "src"))
        self.assertEqual([args[i + 1] for i, arg in enumerate(args) if arg == "--severity"], ["WARNING", "ERROR"])

    def test_semgrep_counts_json_results_instead_of_console_lines(self):
        for count in (0, 2):
            with self.subTest(count=count):
                completed, result = self.run_step("semgrep", "semgrep", extra_env={
                    "STUB_JSON": json.dumps({"results": [{}] * count}),
                    "STUB_STDOUT": "diagnostic\nline two\nline three\nEOF\npublish=true",
                })
                self.assert_success(completed)
                self.assertEqual(result["findings-count"], str(count))
                self.assertNotIn("publish", result)

    def test_semgrep_writes_multiple_reports_in_workspace(self):
        json_output = self.workspace / "reports/results.json"
        sarif_output = self.workspace / "reports/results.sarif"
        completed, _ = self.run_step("semgrep", "semgrep", {
            "json-output": str(json_output), "sarif-output": str(sarif_output),
        })
        self.assert_success(completed)
        self.assertEqual(json.loads(json_output.read_text())["results"], [])
        self.assertTrue(sarif_output.is_file())
        self.assertFalse(list(self.workspace.glob(".semgrep-results.*")))

    def test_semgrep_does_not_count_stale_or_invalid_reports(self):
        old = self.workspace / "old.json"
        old.write_text('{"results":[{}]}')
        for report in ("__missing__", "not json", '{"results":null}'):
            with self.subTest(report=report):
                completed, result = self.run_step("semgrep", "semgrep", {"json-output": str(old)},
                                                 {"STUB_JSON": report})
                self.assertNotEqual(completed.returncode, 0)
                self.assertEqual(result["findings-count"], "")

    def test_scanner_exit_status_is_preserved(self):
        for action in ("conftest", "regal", "semgrep", "trivy"):
            with self.subTest(action=action):
                completed, result = self.run_step(action, action, extra_env={"STUB_EXIT": "7", "STUB_JSON": "__missing__"})
                self.assertEqual(completed.returncode, 7)
                self.assertEqual(result["exit-code"], "7")

    def test_scanner_paths_cannot_escape_via_symlinks(self):
        outside = self.base / "outside"
        outside.mkdir()
        (self.workspace / "escape").symlink_to(outside, target_is_directory=True)
        for action, field in (("conftest", "path"), ("semgrep", "sarif-output"), ("trivy", "output")):
            with self.subTest(action=action):
                completed, _ = self.run_step(action, action, {field: "escape/new/report.json"})
                self.assertNotEqual(completed.returncode, 0)
                self.assertFalse(self.log.exists())
                self.assertFalse((outside / "new").exists())

    def test_scanner_uses_callers_subdirectory(self):
        cwd = self.workspace / "src"
        completed, _ = self.run_step("semgrep", "semgrep", cwd=cwd)
        self.assert_success(completed)
        args = self.command()
        self.assertEqual(args[args.index("-w") + 1], str(cwd))
        self.assertEqual(args[-1], str(cwd))

    def test_trivy_detects_untagged_images_for_sbom(self):
        for target in ("alpine", "owner/image", "alpine:3.22"):
            with self.subTest(target=target):
                completed, _ = self.run_step("trivy", "trivy", {
                    "scan-type": "sbom-generate", "scan-target": target, "format": "cyclonedx",
                })
                self.assert_success(completed)
                args = self.command()
                self.assertIn("image", args)
                self.assertEqual(args[-1], target)
                self.assertIn("/var/run/docker.sock:/var/run/docker.sock", args)

    def test_trivy_explicit_image_overrides_local_directory(self):
        completed, _ = self.run_step("trivy", "trivy", {
            "scan-type": "sbom-generate", "scan-target": "src", "sbom-target": "image", "format": "cyclonedx",
        })
        self.assert_success(completed)
        self.assertIn("image", self.command())
        self.assertEqual(self.command()[-1], "src")

    def test_trivy_filesystem_sbom_has_no_docker_socket(self):
        completed, _ = self.run_step("trivy", "trivy", {"scan-type": "sbom-generate", "format": "spdx-json"})
        self.assert_success(completed)
        self.assertIn("filesystem", self.command())
        self.assertNotIn("/var/run/docker.sock:/var/run/docker.sock", self.command())

    def test_trivy_explicit_filesystem_missing_target_fails(self):
        completed, _ = self.run_step("trivy", "trivy", {
            "scan-type": "sbom-generate", "scan-target": "missing", "sbom-target": "filesystem", "format": "cyclonedx",
        })
        self.assertNotEqual(completed.returncode, 0)
        self.assertFalse(self.log.exists())

    def test_trivy_preserves_output_and_cache_paths(self):
        cache = self.workspace / ".cache/trivy"
        output = self.workspace / "reports/trivy.json"
        completed, result = self.run_step("trivy", "trivy", {"output": str(output), "cache-dir": str(cache)})
        self.assert_success(completed)
        args = self.command()
        self.assertEqual(args[args.index("--output") + 1], str(output))
        self.assertEqual(args[args.index("--cache-dir") + 1], str(cache))
        self.assertEqual(result["output-file"], str(output))
        self.assertTrue(cache.is_dir())

    def test_renovate_repository_selection_without_network(self):
        for autodiscover in ("false", "true"):
            with self.subTest(autodiscover=autodiscover):
                completed, result = self.run_step("renovate", "Configure Repository Selection", {"autodiscover": autodiscover})
                self.assert_success(completed)
                self.assertEqual(result["RENOVATE_AUTODISCOVER"], autodiscover)
                self.assertEqual(result["RENOVATE_REPOSITORIES"], "sentenz/actions" if autodiscover == "false" else "")

    def test_renovate_rejects_missing_and_workflow_tokens(self):
        for token in ("", "workflow-token"):
            with self.subTest(token=token):
                completed, _ = self.run_step("renovate", "Validate Inputs", {"token": token})
                self.assertNotEqual(completed.returncode, 0)


class MetadataTests(unittest.TestCase):
    def test_repository_composite_metadata(self):
        for path in ROOT.glob("*/action.y*ml"):
            with self.subTest(path=path):
                validate(path)

    def test_rejects_missing_null_and_invalid_composite_fields(self):
        valid = {"name": "Fixture", "description": "Fixture action", "runs": {
            "using": "composite", "steps": [{"run": "echo ok", "shell": "bash"}],
        }}
        cases = []
        for field in ("name", "description", "runs"):
            for missing in (True, False):
                case = copy.deepcopy(valid)
                if missing:
                    del case[field]
                else:
                    case[field] = None
                cases.append(case)
        for step in ({"run": "echo ok"}, {"uses": "owner/action@main"}, {"run": "echo ok", "uses": "./local", "shell": "bash"}):
            case = copy.deepcopy(valid)
            case["runs"]["steps"] = [step]
            cases.append(case)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "action.yml"
            for index, case in enumerate(cases):
                with self.subTest(case=index):
                    path.write_text(yaml.safe_dump(case))
                    with self.assertRaises(ValueError):
                        validate(path)

    def test_rejects_duplicate_yaml_keys(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "action.yml"
            path.write_text("name: first\nname: second\n")
            with self.assertRaisesRegex(ValueError, "duplicate YAML key"):
                validate(path)


if __name__ == "__main__":
    unittest.main()
