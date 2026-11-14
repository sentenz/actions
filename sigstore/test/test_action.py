#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Exercise the composite action's shell using a local Cosign test double."""

import json
import os
from pathlib import Path
import subprocess
import tempfile
import textwrap
import unittest


ACTION = Path(__file__).resolve().parents[1] / "action.yml"
MOCK = '''#!/usr/bin/env python3
import json, os, pathlib, sys
args = sys.argv[1:]
with open(os.environ["CALL_LOG"], "a") as log:
    log.write(json.dumps(args) + "\\n")
if args[0].startswith("verify") and os.environ.get("FAIL_VERIFY") == "1":
    sys.exit(42)
if args[0] in ("sign", "sign-blob"):
    if os.environ.get("FAIL_SIGN") == "1":
        sys.exit(43)
    if os.environ.get("SKIP_BUNDLE") != "1":
        pathlib.Path(args[args.index("--bundle") + 1]).write_text("mock bundle")
'''


class SigningContract(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.log = self.root / "calls.jsonl"
        mock = self.root / "cosign"
        mock.write_text(MOCK)
        mock.chmod(0o755)
        # The imported action has one inline run block, in its final step.
        source = ACTION.read_text()
        self.assertEqual(source.count("      run: |\n"), 1)
        self.script = textwrap.dedent(source.split("      run: |\n", 1)[1])
        self.env = dict(
            os.environ,
            PATH=str(self.root) + os.pathsep + os.environ["PATH"],
            CALL_LOG=str(self.log),
            IMAGE="ghcr.io/example/image@sha256:" + "a" * 64,
            FILES="",
            IDENTITY="https://github.com/example/repo/.github/workflows/sign.yml@refs/heads/main",
            ISSUER="https://token.actions.githubusercontent.com",
            BUNDLE="image.sigstore.json",
            DEPLOYMENT="public",
            SIGNING_CONFIG="",
            TRUSTED_ROOT="",
            FAIL_SIGN="0",
            FAIL_VERIFY="0",
            SKIP_BUNDLE="0",
        )

    def run_action(self, **changes):
        return subprocess.run(
            ["bash", "-c", self.script], cwd=self.root,
            env=self.env | changes, capture_output=True, text=True,
        )

    def calls(self):
        return [json.loads(line) for line in self.log.read_text().splitlines()]

    def test_image_signs_then_verifies_exact_identity(self):
        result = self.run_action()
        self.assertEqual(result.returncode, 0, result.stderr)
        sign, verify = self.calls()
        self.assertEqual(sign, [
            "sign", "--yes", "--oidc-provider", "github-actions",
            "--oidc-client-id", "sigstore", "--bundle", "image.sigstore.json",
            self.env["IMAGE"],
        ])
        self.assertEqual(verify, [
            "verify", "--certificate-identity", self.env["IDENTITY"],
            "--certificate-oidc-issuer", self.env["ISSUER"], self.env["IMAGE"],
        ])
        self.assertTrue((self.root / "image.sigstore.json").is_file())

    def test_multiple_files_with_spaces(self):
        for name in ("firmware image.bin", "sbom.json"):
            (self.root / name).write_text("artifact")
        result = self.run_action(IMAGE="", FILES="firmware image.bin\nsbom.json")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual([call[0] for call in self.calls()],
                         ["sign-blob", "verify-blob", "sign-blob", "verify-blob"])
        for name in ("firmware image.bin", "sbom.json"):
            self.assertTrue((self.root / (name + ".sigstore.json")).is_file())

    def test_rejects_invalid_inputs_before_signing(self):
        cases = [
            {"IMAGE": "ghcr.io/example/image:latest"},
            {"IMAGE": "ghcr.io/example/image@sha256:abc"},
            {"IMAGE": ""},
            {"FILES": "artifact"},
            {"IDENTITY": ""},
            {"ISSUER": ""},
            {"DEPLOYMENT": "unknown"},
            {"DEPLOYMENT": "private"},
            {"SIGNING_CONFIG": "custom.json"},
            {"IMAGE": "", "FILES": "\n\n"},
            {"IMAGE": "", "FILES": "missing.bin"},
        ]
        for changes in cases:
            with self.subTest(changes=changes):
                self.assertNotEqual(self.run_action(**changes).returncode, 0)
                self.assertFalse(self.log.exists())

    def test_private_configuration_passes_trust_to_both_commands(self):
        (self.root / "config.json").write_text("{}")
        (self.root / "root.json").write_text("{}")
        result = self.run_action(DEPLOYMENT="private", SIGNING_CONFIG="config.json",
                                 TRUSTED_ROOT="root.json")
        self.assertEqual(result.returncode, 0, result.stderr)
        sign, verify = self.calls()
        self.assertEqual(sign[sign.index("--signing-config") + 1], "config.json")
        for call in (sign, verify):
            self.assertEqual(call[call.index("--trusted-root") + 1], "root.json")

    def test_rejects_empty_files_and_symlinks(self):
        (self.root / "empty.bin").touch()
        (self.root / "artifact.bin").write_text("artifact")
        (self.root / "link.bin").symlink_to("artifact.bin")
        for name in ("empty.bin", "link.bin"):
            self.assertNotEqual(self.run_action(IMAGE="", FILES=name).returncode, 0)
        (self.root / "artifact.bin.sigstore.json").symlink_to("artifact.bin")
        self.assertNotEqual(self.run_action(IMAGE="", FILES="artifact.bin").returncode, 0)
        (self.root / "image.sigstore.json").symlink_to("artifact.bin")
        self.assertNotEqual(self.run_action().returncode, 0)
        self.assertFalse(self.log.exists())

    def test_missing_bundle_stops_before_verification(self):
        result = self.run_action(SKIP_BUNDLE="1")
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual([call[0] for call in self.calls()], ["sign"])

    def test_signing_failure_stops_before_verification(self):
        result = self.run_action(FAIL_SIGN="1")
        self.assertEqual(result.returncode, 43)
        self.assertEqual([call[0] for call in self.calls()], ["sign"])

    def test_image_verification_failure_propagates(self):
        self.assertEqual(self.run_action(FAIL_VERIFY="1").returncode, 42)

    def test_file_verification_failure_stops_remaining_files(self):
        for name in ("first.bin", "second.bin"):
            (self.root / name).write_text("artifact")
        result = self.run_action(IMAGE="", FILES="first.bin\nsecond.bin", FAIL_VERIFY="1")
        self.assertEqual(result.returncode, 42)
        self.assertEqual([call[0] for call in self.calls()], ["sign-blob", "verify-blob"])
        self.assertFalse((self.root / "second.bin.sigstore.json").exists())


if __name__ == "__main__":
    unittest.main(verbosity=2)
