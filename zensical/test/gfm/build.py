# SPDX-License-Identifier: Apache-2.0
"""Build the integration fixture and check the HTML that will be uploaded."""

import subprocess
import sys
from pathlib import Path

import html5lib


subprocess.run([sys.executable, "-m", "zensical", *sys.argv[1:], "--clean"], check=True)
html = Path("site/index.html").read_text(encoding="utf-8")
assert "<!-- GFM fixture comment -->" in html
assert 'xlink:href="#icon"' in html
page = html5lib.parse(html, namespaceHTMLElements=False)
article = page.find(".//article")
assert article is not None
assert article.findtext(".//th[@align='right']") == "Status"
assert article.findtext(".//td/del") == "draft"
assert article.findtext(".//li/ul/li").strip() == "Nested item"
checked, pending = article.findall(".//li/input[@type='checkbox']")
assert "checked" in checked.attrib and "checked" not in pending.attrib
assert "disabled" in checked.attrib and "disabled" in pending.attrib
assert article.findtext(".//a[@href='https://example.org/docs']") == "https://example.org/docs"
assert article.findtext(".//a[@href='mailto:docs@example.org']") == "docs@example.org"
assert article.findtext(".//aside[@class='admonition note']/p") == "Note"
assert "> [!WARNING]\n> ~~literal~~" in "".join(article.find(".//pre/code").itertext())
assert article.findtext(".//kbd") == "Enter"
assert article.find(".//script") is None
assert "<script>gfm_literal</script>" in "".join(article.itertext())
assert article.findtext(".//h2[@id='rendering']") == "Rendering"
assert page.find(".//nav//a[@href='#rendering']") is not None
assert article.findtext(".//a[@href='guide/']") == "guide"
assert Path("site/guide/index.html").is_file()
