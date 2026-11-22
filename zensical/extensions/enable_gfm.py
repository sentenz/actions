#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Create a temporary Zensical configuration enabling GitHub Flavored Markdown."""

from __future__ import annotations

import re
import tempfile
from pathlib import Path

try:
    import tomllib
except ModuleNotFoundError:
    import tomli as tomllib


def main() -> None:
    config_path = Path("zensical.toml")
    if not config_path.is_file():
        raise SystemExit("flavor=gdm requires zensical.toml in the checkout root")

    content = config_path.read_text(encoding="utf-8")
    project = tomllib.loads(content).get("project", {})
    extensions = project.get("markdown_extensions", {})

    if "sentenz_gfm" not in extensions:
        section = re.search(
            r"(?m)^[ \t]*\[project\.markdown_extensions\][ \t]*(?:#.*)?$", content
        )
        if section:
            offset = section.end()
            content = content[:offset] + "\nsentenz_gfm = {}" + content[offset:]
        elif re.search(r"(?m)^[ \t]*markdown_extensions[ \t]*=", content):
            raise SystemExit(
                "flavor=gdm requires [project.markdown_extensions] table syntax "
                "when markdown_extensions is already configured"
            )
        else:
            content = content.rstrip() + "\n\n[project.markdown_extensions]\nsentenz_gfm = {}\n"

    tomllib.loads(content)  # Validate the temporary configuration before building.
    with tempfile.NamedTemporaryFile(
        mode="w", encoding="utf-8", prefix=".zensical-gdm-", suffix=".toml",
        dir=config_path.parent, delete=False,
    ) as config:
        config.write(content)
        print(config.name)


if __name__ == "__main__":
    main()
