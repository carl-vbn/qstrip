# qstrip

![GitHub stars](https://img.shields.io/github/stars/carl-vbn/qstrip)
![Build Status](https://github.com/carl-vbn/qstrip/actions/workflows/python-package.yml/badge.svg)
![License](https://img.shields.io/github/license/carl-vbn/qstrip)

A fast Markdown stripper with a C backend.

## Installation

```bash
pip install qstrip
```

## Usage

```python
from qstrip import strip_markdown

with open('markdown_file.md', 'r') as f:
    content = f.read()

stripped_content = strip_markdown(content)
print(stripped_content)
```

## Current and planned features
- [x] Strip headings
- [x] Strip bold tags
- [x] Strip italic tags
- [x] Strip strikethrough tags
- [x] Strip code blocks
- [x] Strip inline code
- [x] Strip links
- [x] Strip images
- [x] Strip tables
- [x] Handle images inside links
- [ ] Strip lists
- [ ] Strip blockquotes
- [ ] Handle escape sequences
- [ ] Support other markup formats (e.g., reStructuredText, HTML/XML)
