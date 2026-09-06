"""Validate the composite metadata and shell blocks maintained in this repository."""

import re
import subprocess
import sys
from pathlib import Path

import yaml


class UniqueLoader(yaml.SafeLoader):
    """Reject duplicate YAML keys instead of silently overwriting them."""


def unique_mapping(loader, node, deep=False):
    mapping = {}
    for key_node, value_node in node.value:
        key = loader.construct_object(key_node, deep=deep)
        if key in mapping:
            raise ValueError(f"duplicate YAML key: {key}")
        mapping[key] = loader.construct_object(value_node, deep=deep)
    return mapping


UniqueLoader.add_constructor(yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG, unique_mapping)


def nonempty_string(value, label):
    if not isinstance(value, str) or not value.strip():
        raise ValueError(f"{label} must be a nonempty string")


def validate(path):
    metadata = yaml.load(Path(path).read_text(), Loader=UniqueLoader)
    if not isinstance(metadata, dict):
        raise ValueError("action metadata must be a mapping")
    for field in ("name", "description"):
        nonempty_string(metadata.get(field), field)
    for section in ("inputs", "outputs"):
        definitions = metadata.get(section, {})
        if not isinstance(definitions, dict):
            raise ValueError(f"{section} must be a mapping")
        for key, definition in definitions.items():
            if not isinstance(definition, dict):
                raise ValueError(f"{section}.{key} must be a mapping")
            nonempty_string(definition.get("description"), f"{section}.{key}.description")
            if section == "outputs":
                nonempty_string(definition.get("value"), f"outputs.{key}.value")
            if "required" in definition and not isinstance(definition["required"], bool):
                raise ValueError(f"{section}.{key}.required must be a boolean")
    runs = metadata.get("runs")
    if not isinstance(runs, dict) or runs.get("using") != "composite":
        raise ValueError("runs.using must be composite")
    steps = runs.get("steps")
    if not isinstance(steps, list) or not steps:
        raise ValueError("runs.steps must be a nonempty list")
    ids = set()
    for index, step in enumerate(steps):
        label = f"runs.steps[{index}]"
        if not isinstance(step, dict) or ("run" in step) == ("uses" in step):
            raise ValueError(f"{label} must define exactly one of run or uses")
        if "id" in step:
            nonempty_string(step["id"], f"{label}.id")
            if step["id"] in ids:
                raise ValueError(f"duplicate step id: {step['id']}")
            ids.add(step["id"])
        if "run" in step:
            nonempty_string(step["run"], f"{label}.run")
            nonempty_string(step.get("shell"), f"{label}.shell")
            if step["shell"] == "bash":
                subprocess.run(["bash", "-n"], input=step["run"], text=True, check=True)
        else:
            nonempty_string(step["uses"], f"{label}.uses")
            if not step["uses"].startswith("./") and not re.search(r"@[a-f0-9]{40}$", step["uses"]):
                raise ValueError(f"{label}.uses must pin an external action to a full commit SHA")
    return metadata


if __name__ == "__main__":
    root = Path(__file__).resolve().parents[1]
    paths = [Path(p) for p in sys.argv[1:]] or sorted(root.glob("*/action.y*ml"))
    if not paths:
        sys.exit("No composite action metadata found")
    for path in paths:
        try:
            validate(path)
        except (ValueError, yaml.YAMLError, subprocess.CalledProcessError) as error:
            sys.exit(f"{path}: {error}")
    print(f"Validated {len(paths)} composite actions and their Bash syntax")
