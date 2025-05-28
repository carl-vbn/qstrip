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
    args = parser.parse_args()

    if args.input:
        with open(args.input, "r") as f:
            text = f.read()
    else:
        text = sys.stdin.read()

    stripped_text = strip_markdown(text)

    if args.output:
        with open(args.output, "w") as f:
            f.write(stripped_text)
    else:
        sys.stdout.write(stripped_text)


if __name__ == "__main__":
    main()
