import argparse
import sys
from . import strip_markdown


def main():
    parser = argparse.ArgumentParser(description="Strip markdown")
    parser.add_argument("-i", "--input", type=str,
                        help="Input file to strip markdown from. "
                        "Defaults to stdin.")
    parser.add_argument("-o", "--output", type=str,
                        help="Output file to write the stripped "
                        "text to. Defaults to stdout.")
    parser.add_argument("--mask", type=str, default=None,
                        help="Comma-separated elements to strip: table, link,"
                             " image, code. Default: all")
    args = parser.parse_args()

    if args.input:
        with open(args.input, "r") as f:
            text = f.read()
    else:
        text = sys.stdin.read()

    if args.mask is None:
        stripped_text = strip_markdown(text)
    else:
        items = [s.strip() for s in args.mask.split(',') if s.strip()]
        stripped_text = strip_markdown(text, mask=items)

    if args.output:
        with open(args.output, "w") as f:
            f.write(stripped_text)
    else:
        sys.stdout.write(stripped_text)


if __name__ == "__main__":
    main()
