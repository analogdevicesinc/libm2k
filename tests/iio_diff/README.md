# iio-diff

## Prerequisites

Before running the scripts in this project, make sure that the Python bindings for libiio **(pylibiio)** are properly installed on your system. For installation instructions and additional information refer to the [official wiki page](https://wiki.analog.com/resources/tools-software/linux-software/pyadi-iio)

## XML generation: `xml_generator.py`

The xml_generator.py script generates an XML representation of the current libiio context.


- Stores the current libiio context in XML format.
- By default, the resulting XML file is stored in a `./results` directory relative to the path where xml_generator.py is located. The directory is created if it does not exist.

Usage example:
```bash
python3 xml_generator.py \
    --uri ip:192.168.2.1 \
    --filename v0_XX.xml
```

## Compare iio state: `iio_diff.py`

The `iio_diff.py` script compares a reference context with a target context using XML files. It identifies and highlights the differences between the two contexts:

- **Common Attributes**: Attributes present in both the reference and target contexts.
New Attributes: Attributes added in the target context but not present in the reference context.
- **Missing Attributes**: Attributes present in the reference context but missing in the target context.
- **Note**: The reference context can be compared against a target context loaded from an XML file or by providing a URI to a connected board.

The `iio_diff.py` script is designed to compare a reference context and a target context using XML files.


Usage:
```bash
# Target context created from a URI
python3 iio_diff.py \
        --reference_file ./results/v0_32.xml \
        --uri ip:192.168.2.2    

# Target context created from an XML file
python3 iio_diff.py \
        --reference_file ./results/v0_32.xml \
        --target_file  ./results/v0_31.xml
```