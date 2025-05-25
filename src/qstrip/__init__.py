from ._core import strip_markdown as _strip_markdown

def strip_markdown(text):
    """
    Strip markdown from the given text.

    Parameters:
    text (str): The input text containing markdown.

    Returns:
    str: The stripped text.
    """
    return _strip_markdown(text)