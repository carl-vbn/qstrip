from qstrip import strip_markdown

snippets = [
    (
        "just text",
        "just text"
    ),
    (
        "regular **bold** `code **bold in code**` *italic*",
        "regular bold code **bold in code** italic"
    ),
    (
        "```\ncode block\n```",
        "code block"
    ),
    (
        "[link](http://example.com)",
        "link"
    ),
    (
        "![alt text](image.jpg)",
        "alt text"
    ),
    (
        "# Heading 1\n## Heading 2\n### Heading 3",
        "Heading 1\nHeading 2\nHeading 3"
    ),
    (
        "Heading 1\n========\nHeading 2\n--------\nNon heading ====",
        "Heading 1\nHeading 2\nNon heading ===="
    )
]

for i in range(10000):
    combined_markdown = "\n".join(snippets[(i+j) % len(snippets)][0] for j in range(1000))
    combined_stripped = "\n".join(snippets[(i+j) % len(snippets)][1] for j in range(1000))

    output = strip_markdown(combined_markdown)

    if output.strip('\n') != combined_stripped.strip('\n'):
        raise Exception(f"Mismatch at iteration {i}")

print("Stress test completed successfully without mismatches.")