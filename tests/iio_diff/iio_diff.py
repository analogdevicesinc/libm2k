import iio
import argparse

from utils import (
    Color,
    conditional_str,
    get_attrs,
    get_channels,
    get_conditional_color,
    get_union,
    to_input_output,
)


def main(args):
    # Reference context
    ctx_reference = iio.XMLContext(args.reference_file)
    if not ctx_reference:
        print(
            f"Failed to create a context from the reference file: {args.reference_file}"
        )
        return
    # Target context
    if args.uri:
        ctx_target = iio.Context(args.uri)
    else:
        ctx_target = iio.XMLContext(args.target_file)
    if not ctx_target:
        print(f"Failed to create a context from the target file: {args.target_file}")
        return

    # Handle context attributes
    ctx_attrs_ref, ctx_attrs_target, ctx_attrs_union = get_union(
        ctx_reference.attrs, ctx_target.attrs
    )
    print(f"{Color.WHITE}IIO context has {len(ctx_attrs_union)} attributes:")
    for ctx_attr in ctx_attrs_union:
        color = get_conditional_color(ctx_attr, ctx_attrs_ref, ctx_attrs_target)
        print(f"{color}\t{ctx_attr}")

    dev_names_reference, dev_names_target, dev_names_union = get_union(
        {dev.name for dev in ctx_reference.devices},
        {dev.name for dev in ctx_target.devices},
    )

    print(f"{Color.WHITE}IIO context has {len(dev_names_union)} devices:")
    for dev_name in sorted(dev_names_union):
        if dev_name in dev_names_reference and dev_name in dev_names_target:
            handle_common_device(ctx_target, ctx_reference, dev_name)
        else:
            handle_unique_device(
                dev_name,
                dev_names_target,
                ctx_target,
                dev_names_reference,
                ctx_reference,
            )


def handle_unique_device(
    dev_name, dev_names_target, ctx_target, dev_names_reference, ctx_reference
):
    # Device not present in both contexts -> deletion or addition with respect to reference
    if dev_name in dev_names_target:
        color = Color.GREEN
        dev = ctx_target.find_device(dev_name)
    if dev_name in dev_names_reference:
        color = Color.RED
        dev = ctx_reference.find_device(dev_name)

    msg = f"\t{color}iio:device: {dev.name}"
    msg += f"{color}\n\t\t\t{len(dev.channels)} channels found:"
    for channel in dev.channels:
        msg += f"{color}\n\t\t\t{conditional_str(channel.id)}: {conditional_str(channel.name)} ({to_input_output(channel.output)})"

        msg += f"{color}\n\t\t\t{len(channel.attrs)} channel-specific attributes found:"
        for idx, attr in enumerate(channel.attrs):
            msg += f"{color}\n\t\t\t\t\tattr {idx}: {attr}"
    print(msg)


def handle_common_device(ctx_target, ctx_reference, dev_name):
    device_reference = ctx_reference.find_device(dev_name)
    device_target = ctx_target.find_device(dev_name)

    _, _, channels_union = get_channels(device_reference, device_target)
    print(f"\t{Color.WHITE}iio:device: {dev_name}")
    print(f"{Color.WHITE}\t\t{len(channels_union)} channels found:")
    for chn in channels_union:
        color = Color.WHITE
        chn_id, chn_output = chn
        # Devices can have different channels
        chn_reference = device_reference.find_channel(chn_id, chn_output)
        channels_target = device_target.find_channel(chn_id, chn_output)
        # Get attributes for current channel
        attrs_reference, attrs_target, attrs_union = get_attrs(
            chn_reference, channels_target
        )
        print(f"\t\t\t{Color.WHITE}Channel: {chn_id} ({to_input_output(chn_output)})")
        if len(attrs_union) > 0:
            print(f"\t\t\t{color}{len(attrs_union)} channel-specific attributes found:")
            for idx, attr in enumerate(sorted(attrs_union)):
                color = get_conditional_color(attr, attrs_reference, attrs_target)
                print(f"\t\t\t\t{color}attr {idx}: {attr}")
    # ====================================================================================
    dev_attrs_reference, dev_attrs_target, dev_attrs_union = get_union(
        device_reference.attrs, device_target.attrs
    )
    if len(dev_attrs_union) > 0:
        print(
            f"{Color.WHITE}\t\t{len(dev_attrs_union)} device-specific attributes found:"
        )
        for idx, dev_attr in enumerate(sorted(dev_attrs_union)):
            color = get_conditional_color(
                dev_attr, dev_attrs_reference, dev_attrs_target
            )
            print(f"{color}\t\t\tattr {idx}: {dev_attr}")
    # ====================================================================================
    buffer_attrs_reference, buffer_attrs_target, buffer_attrs_union = get_union(
        device_reference.buffer_attrs, device_target.buffer_attrs
    )
    if len(buffer_attrs_union) > 0:
        print(
            f"{Color.WHITE}\t\t{len(buffer_attrs_union)} buffer-specific attributes found:"
        )
        for idx, buffer_attr in enumerate(sorted(buffer_attrs_union)):
            color = get_conditional_color(
                buffer_attr, buffer_attrs_reference, buffer_attrs_target
            )
            print(f"{color}\t\t\tattr {idx}: {buffer_attr}")
    # ====================================================================================
    debug_attrs_reference, debug_attrs_target, debug_attrs_all = get_union(
        device_reference.debug_attrs, device_target.debug_attrs
    )
    if len(debug_attrs_all) > 0:
        print(f"{Color.WHITE}\t\t\t{len(debug_attrs_all)} debug attributes found:")
        for idx, debug_attr in enumerate(sorted(debug_attrs_all)):
            color = get_conditional_color(
                debug_attr, debug_attrs_reference, debug_attrs_target
            )
            print(f"{color}\t\t\t\tdebug attr {idx}: {debug_attr}")
    # ====================================================================================
    try:
        trig_reference, trig_target, trig_all = get_union(
            device_reference.trigger, device_target.trigger
        )
        assert len(trig_all) > 0, "No trigger on this device"

        print(f"{Color.WHITE}\n\t\t\t{len(trig_all)} trigger attributes found:")
        for idx, trig in enumerate(trig_all):
            color = get_conditional_color(trig, trig_reference, trig_target)
            print(f"{color}\t\t\t\ttrigger {idx}: {trig}")
    except Exception as e:
        print(f"\t\t{Color.WHITE}No trigger on this device")


def get_parser():
    parser = argparse.ArgumentParser(
        prog="iio_diff",
        description="Utility used to compare changes across iio attributes.",
    )

    parser.add_argument(
        "--reference_file",
        type=str,
        required=True,
        help="Path to the reference XML file.",
    )

    target_group = parser.add_argument_group(
        title="Target ctx",
        description="Specify the target context to compare against the reference file",
    )
    target_group = target_group.add_mutually_exclusive_group(required=True)
    target_group.add_argument(
        "--uri", type=str, required=False, help="Source of the iio context"
    )

    target_group.add_argument(
        "--target_file",
        type=str,
        required=False,
        help="Path to the target XML file used to compare against the reference file",
    )
    return parser


if __name__ == "__main__":
    parser = get_parser()
    args = parser.parse_args()
    main(args)
