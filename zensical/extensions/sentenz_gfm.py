# SPDX-License-Identifier: Apache-2.0
"""Render GFM while leaving the document tree available for Zensical navigation."""

import comrak
import html5lib
from html5lib.constants import unadjustForeignAttributes
from markdown.extensions import Extension
from markdown.preprocessors import Preprocessor
from markdown.treeprocessors import Treeprocessor


def serialize_html(element):
    return html5lib.serialize(
        element, tree="etree", omit_optional_tags=False, quote_attr_values="always",
        escape_lt_in_attrs=True,
    )


class GfmSource(Preprocessor):
    def run(self, lines):
        # Capture source before Python-Markdown extensions can rewrite lists or fences.
        self.lines = lines
        return []


class GfmTree(Treeprocessor):
    def run(self, root):
        options = comrak.ExtensionOptions()
        options.table = True
        options.tasklist = True
        options.strikethrough = True
        options.autolink = True
        options.tagfilter = True
        options.alerts = True
        render = comrak.RenderOptions()
        render.alert_style = comrak.AlertStyle.Semantic
        # Preserve inline HTML such as <kbd> while applying GFM's tag filter.
        render.unsafe_ = True
        html = comrak.render_markdown(
            "\n".join(self.md.preprocessors["sentenz_gfm"].lines),
            extension_options=options, render_options=render,
        )

        # Python-Markdown's TOC expects string tag names. Keep HTML comments
        # in its raw HTML stash until the tree has been serialized.
        stash = self.md.htmlStash

        class FragmentTree(html5lib.getTreeBuilder("etree")):
            def insertComment(self, token, parent=None):
                self.insertText(stash.store("<!--" + token["data"] + "-->"), parent)

        parser = html5lib.HTMLParser(tree=FragmentTree, namespaceHTMLElements=False)
        fragment = parser.parseFragment(html)
        # html5lib's HTML serializer otherwise drops prefixes such as xlink:href.
        for element in fragment.iter():
            for name in list(element.attrib):
                if name.startswith("{"):
                    namespace, local = name[1:].split("}", 1)
                    qualified = unadjustForeignAttributes[namespace, local]
                    element.set(qualified, element.attrib.pop(name))
        fragment.tag = root.tag
        return fragment


class GfmExtension(Extension):
    def extendMarkdown(self, md):
        md.preprocessors.register(GfmSource(md), "sentenz_gfm", 1000)
        # Replace the inline pass with the parsed tree before TOC and link processing.
        md.treeprocessors.register(GfmTree(md), "inline", 20)
        md.treeprocessors.deregister("prettify")
        md.output_formats = dict.fromkeys(md.output_formats, serialize_html)


def makeExtension(**kwargs):
    return GfmExtension(**kwargs)
