from qstrip import strip_markdown

def strip_and_compare(file_path, expected_path):
    with open(file_path, 'r') as f:
        md_text = f.read()

    with open(expected_path, 'r') as f:
        expected_text = f.read()

    assert strip_markdown(md_text) == expected_text

def test_markdown_bold():
    assert strip_markdown("pre **bold** post") == "pre bold post"

def test_markdown_italic():
    assert strip_markdown("pre *italic* post") == "pre italic post"

def test_markdown_strikethrough():
    assert strip_markdown("pre ~~strikethrough~~ post") == "pre strikethrough post"

def test_markdown_code():
    assert strip_markdown("pre `code` post") == "pre code post"

def test_markdown_code_block():
    assert strip_markdown("```\ncode block\n```") == "code block\n"

def test_markdown_link():
    assert strip_markdown("[link](http://example.com)") == "link"

def test_markdown_image():
    assert strip_markdown("![alt text](image.jpg)") == "alt text"

def test_headings_1():
    assert strip_markdown("# Heading 1\n## Heading 2\n### Heading 3") == "Heading 1\nHeading 2\nHeading 3"

def test_headings_2():
    assert strip_markdown("Heading 1\n========\nHeading 2\n--------\nNon heading ====") == "Heading 1\nHeading 2\nNon heading ===="

def test_code_mixed():
    assert strip_markdown("Regular **bold** `code **bold in code**` *italic*") == "Regular bold code **bold in code** italic"

def test_style_mixed():
    assert strip_markdown("Regular **~~bold and strikethrough with some *italic*~~**") == "Regular bold and strikethrough with some italic"

def test_markdown_empty():
    assert strip_markdown("") == ""

def test_markdown_short():
    strip_and_compare('tests/assets/short.md', 'tests/assets/stripped/short.txt')

def test_markdown_medium():
    strip_and_compare('tests/assets/medium.md', 'tests/assets/stripped/medium.txt')

def test_markdown_long():
    strip_and_compare('tests/assets/long.md', 'tests/assets/stripped/long.txt')

def test_markdown_extralong():
    markdown_text = "# This is a heading\n\n"
    stripped_text = "This is a heading\n"

    print("Generating long markdown text...")
    for i in range(1000):
        markdown_text += f"Regular **bold** *italic* ~~strikethrough~~ `code` [link](http://example.com) ![alt text](image.jpg)\n"
        stripped_text += f"Regular bold italic strikethrough code link alt text\n"

    output = strip_markdown(markdown_text)
    matches = output == stripped_text
    assert matches, "Failed to match the expected output for long markdown text."

