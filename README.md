# qstrip

A fast Markdown stripper with a C backend.

## Installation

```bash
pip install git+https://github.com/carl-vbn/qstrip.git
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
- [ ] Strip lists
- [ ] Strip tables
- [ ] Strip blockquotes
- [ ] Handle escape sequences
- [ ] Support other markup formats (e.g., reStructuredText, HTML/XML)