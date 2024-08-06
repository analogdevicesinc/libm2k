import argparse
import os
import iio
import sys
from utils import validate_xml_extension


def get_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="xml_generator",
        description="Save the iio state of an M2K device in xml format",
    )

    parser.add_argument(
        "--uri",
        type=str,
        default="ip:192.168.2.1",
        required=True,
        help="Source of the iio context. Defaults to ip:192.168.2.1",
    )

    parser.add_argument(
        "--filename",
        type=str,
        required=False,
        help="Output XML save file. Defaults to the current firmware version of the board",
    )
    return parser


def main(args):
    try:
        ctx = iio.Context(args.uri)
        if not ctx:
            raise ValueError(f"Failed to create a context from the URI: {args.uri}")

        if not os.path.exists("results"):
            os.makedirs("results", exist_ok=True)

        if args.filename:
            args.filename = validate_xml_extension(args.filename)
        else:
            fw_version = ctx.attrs["fw_version"].replace(".", "_")
            args.filename = f"{fw_version}.xml"

        save_path = os.path.join("results", args.filename)
        with open(save_path, "w") as file:
            file.write(ctx.xml)
    except Exception as e:
        print(e)
        sys.exit(1)


if __name__ == "__main__":
    parser = get_parser()
    args = parser.parse_args()
    main(args)
