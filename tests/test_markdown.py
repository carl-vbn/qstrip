from qstrip import strip_markdown
import os

ASSETS_LOCATION = os.path.join(os.path.dirname(__file__), 'assets')


def strip_file_and_compare(file_name, expected_file_name):
    file_path = os.path.join(ASSETS_LOCATION, file_name)
    expected_path = os.path.join(ASSETS_LOCATION, expected_file_name)

    with open(file_path, 'r') as f:
        md_text = f.read()

    with open(expected_path, 'r') as f:
        expected_text = f.read()

    assert strip_markdown(md_text) == expected_text


def strip_and_compare(md_text, expected_text):
    assert strip_markdown(md_text) == expected_text


def test_markdown_bold():
    strip_and_compare("pre **bold** post", "pre bold post")


def test_markdown_italic():
    strip_and_compare("pre *italic* post", "pre italic post")


def test_markdown_strikethrough():
    strip_and_compare("pre ~~strikethrough~~ post", "pre strikethrough post")


def test_markdown_code():
    strip_and_compare("pre `code` post", "pre code post")


def test_markdown_code_block():
    strip_and_compare("```\ncode block\n```", "code block\n")


def test_markdown_link():
    strip_and_compare("[link](http://example.com)", "link")


def test_markdown_image():
    strip_and_compare("![alt text](image.jpg)", "alt text")


def test_headings_1():
    strip_and_compare("# Heading 1\n## Heading 2\n### Heading 3",
                      "Heading 1\nHeading 2\nHeading 3")


def test_headings_2():
    strip_and_compare("Heading 1\n========\nHeading 2\n--------"
                      + "\nNon heading ====",
                      "Heading 1\nHeading 2\nNon heading ====")


def test_code_mixed():
    strip_and_compare("Regular **bold** `code **bold in code**` *italic*",
                      "Regular bold code **bold in code** italic")


def test_style_mixed():
    strip_and_compare("Regular **~~bold and strikethrough with "
                      + "some *italic*~~**",
                      "Regular bold and strikethrough with some italic")


def test_markdown_table():
    md_text = "| Header 1 | Header 2 |\n|----------|----------|\n"
    md_text += "| Row 1   | Data 1   |\n| Row 2   | Data 2   |\n"
    expected_text = "Header 1,Header 2\nRow 1,Data 1\nRow 2,Data 2\n"
    strip_and_compare(md_text, expected_text)


def test_markdown_link_image():
    md_text = "[![alt text](image.jpg)](http://example.com)"
    expected_text = "alt text"
    strip_and_compare(md_text, expected_text)


def test_markdown_link_image_2():
    md_text = "[pre ![image 1](image1.jpg) ![image 2](image2.jpg) " \
              "post](http://example.com)"
    expected_text = "pre image 1 image 2 post"
    strip_and_compare(md_text, expected_text)


def test_markdown_empty():
    strip_and_compare("", "")


def test_markdown_short():
    strip_file_and_compare('short.md', 'stripped/short.txt')


def test_markdown_medium():
    strip_file_and_compare('medium.md', 'stripped/medium.txt')


def test_markdown_long():
    strip_file_and_compare('long.md', 'stripped/long.txt')


def test_markdown_extralong():
    markdown = "# This is a heading\n\n"
    stripped = "This is a heading\n"

    print("Generating long markdown text...")
    for i in range(1000):
        markdown += "Regular **bold** *italic* ~~strikethrough~~ `code` "
        markdown += "[link](http://example.com) ![alt text](image.jpg)\n"
        stripped += "Regular bold italic strikethrough code link alt text\n"

    output = strip_markdown(markdown)
    matches = output == stripped

    err = "Failed to match the expected output for long markdown text."
    assert matches, err


def test_mask_empty_list_strips_nothing():
    md = "X [link](u) Y ![alt](img) Z `code`\n" \
         "| A | B |\n|---|---|\n| 1 | 2 |\n"
    assert strip_markdown(md, mask=[]) == md


def test_mask_only_link():
    md = "X [link](u) Y ![alt](img) Z `code`"
    expected = "X link Y ![alt](img) Z `code`"
    assert strip_markdown(md, mask=["link"]) == expected


def test_mask_only_image():
    md = "X ![alt](img) Y [link](u) Z `code`"
    expected = "X alt Y [link](u) Z `code`"
    assert strip_markdown(md, mask=["image"]) == expected


def test_mask_only_code():
    md = "pre `code` post [link](u) ![alt](img)"
    expected = "pre code post [link](u) ![alt](img)"
    assert strip_markdown(md, mask=["code"]) == expected


def test_mask_only_table():
    md = "| A | B |\n|---|---|\n| 1 | 2 |\n"
    expected = "A,B\n1,2\n"
    assert strip_markdown(md, mask=["table"]) == expected


def test_mask_link_and_image():
    md = "X [link](u) ![alt](img) `code`"
    expected = "X link alt `code`"
    assert strip_markdown(md, mask=["link", "image"]) == expected


def test_mask_all_string_equivalent_to_default():
    md = "[link](u) ![alt](img) `code`\n| A | B |\n|---|---|\n| 1 | 2 |\n"
    expected_default = strip_markdown(md)
    assert strip_markdown(md, mask=["all"]) == expected_default
