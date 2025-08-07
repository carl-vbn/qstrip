from ._core import strip_markdown as _strip_markdown
from typing import Optional, Sequence


def strip_markdown(text: str, *, mask: Optional[Sequence[str]] = None) -> str:
    """
    Strip markdown from the given text.

    Parameters:
    text (str): The input text containing markdown.
    mask (Sequence[str] | None): Optional list/tuple of element names to strip.
        Allowed values: 'table', 'link', 'image', 'code'. If omitted or None,
        strips all elements. If empty, strips nothing.

    Returns:
    str: The stripped text.
    """
    if mask is None:
        return _strip_markdown(text)

    bits = 0
    for item in mask:
        item_lower = str(item).lower()
        if item_lower == "table":
            bits |= _MASK_TABLE
        elif item_lower == "link":
            bits |= _MASK_LINK
        elif item_lower == "image":
            bits |= _MASK_IMAGE
        elif item_lower == "code":
            bits |= _MASK_CODE
        elif item_lower == "all":
            bits |= _MASK_ALL
        else:
            continue

    if bits == 0:
        return text

    return _strip_markdown(text, bits)


_MASK_TABLE = 1
_MASK_LINK = 2
_MASK_IMAGE = 4
_MASK_CODE = 8
_MASK_ALL = 15

__all__ = [
    "strip_markdown",
]
