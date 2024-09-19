#
# Copyright (c) 2024 Analog Devices Inc.
#
# This file is part of libm2k
# (see http://www.github.com/analogdevicesinc/libm2k).
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

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
