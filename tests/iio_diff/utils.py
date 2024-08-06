import os
import re
from ast import Set
from typing import Any, Iterable, List, Tuple

import iio


def validate_xml_extension(file_name):
    _, ext = os.path.splitext(file_name)
    if not ext:
        return file_name + ".xml"
    if ext == ".xml":
        return file_name
    raise ValueError("File must have a .xml extension")


class Color:
    """
    Class to hold ANSI color constants that can be used to colorize text in terminal.
    """

    WHITE = "\033[97m"  # Found with same name
    GREEN = "\033[92m"  # Exists only in target
    RED = "\033[91m"  # Found in
    BLUE = "\033[94m"


def get_conditional_color(value: Any, reference: Iterable, target: Iterable) -> Color:
    """
    Determines the color based on the presence of a value in the reference and target.

    Parameters:
    - value (Any): The value to check.
    - reference (Iterable): The reference set or list.
    - target (Iterable): The target set or list.

    Returns:
    Color: ANSI color code for text in terminal

    Color decision rules:
    - If the value is present in both the reference and target, returns WHITE.
    - If the value is new and coming from the target (i.e., it's in the target but not in the reference), returns GREEN.
    - If the value was in the reference but is missing from the target, returns RED.
    """
    if value in reference and value in target:
        return Color.WHITE
    if value in reference and value not in target:
        return Color.RED
    if value not in reference and value in target:
        return Color.GREEN


def extract_channel_index(input: str) -> int:
    """
    Extracts the channel index from a string.

    The function searches for patterns like 'voltage', 'temp', or 'altvoltage' followed by a number in the input string.
    It returns the number as an integer.

    Parameters:
    - input (str): The string to extract the channel index from.

    Returns:
    int: The extracted channel index.

    Raises:
    ValueError: If no valid number is found in the string.
    """
    match = re.search(r"(voltage|temp|altvoltage)(\d+)", input)
    if match:
        return int(match.group(2))
    else:
        raise ValueError("No valid number found in the string")


def get_channels(device1: iio.Device, device2: iio.Device) -> List[Tuple[str, bool]]:
    """
    Combines the channels of two devices into a set of tuples of channel id and output status that
    can be used to identify shared and unique channels to each device.

    Parameters:
    - device1 (iio.Device)
    - device2 (iio.Device)

    Returns:
    - List[Tuple[str, bool]]: A sorted list of tuples, where each tuple contains a channel id and a boolean indicating
      whether the channel is an output channel. The list is sorted by the channel index.
    """
    channels_1 = {(dev.id, dev.output) for dev in device1.channels}
    channels_2 = {(dev.id, dev.output) for dev in device2.channels}
    channels_all = channels_1 | channels_2
    return (
        channels_1,
        channels_2,
        sorted(channels_all, key=lambda x: extract_channel_index(x[0])),
    )


def get_union(input1: Any, input2: Any) -> Tuple[Set, Set, Set]:
    """
    Converts two inputs into sets and returns their individual sets and the union of both sets.

    This function takes two inputs, converts them into sets, and then returns a tuple containing the first set,
    the second set, and the union of both sets.

    Parameters:
    - input1 (Any): The first input to convert into a set.
    - input2 (Any): The second input to convert into a set.

    Returns:
    - Tuple[Set, Set, Set]: A tuple containing the first set, the second set, and the union of both sets.
    """
    s1 = set(input1)
    s2 = set(input2)
    return s1, s2, s1 | s2


def get_attrs(channel_reference, channel_target):
    if channel_reference and channel_target:
        attrs_reference = set(channel_reference.attrs)
        attrs_target = set(channel_target.attrs)
    if channel_reference and not channel_target:
        attrs_reference = set(channel_reference.attrs)
    if not channel_reference and channel_target:
        attrs_target = set(channel_target.attrs)
    attrs_union = attrs_reference | attrs_target
    return attrs_reference, attrs_target, attrs_union


def to_input_output(value):
    return "output" if value else "input"


def conditional_str(value):
    return f"{value}" if value else ""
