# Test: Dual-Channel Waveform Synchronization

## Overview

| Field                | Value                                                                              |
| -------------------- | ---------------------------------------------------------------------------------- |
| **Test Name**        | `test_dual_channel_waveform_sync`                                                  |
| **Test File**        | `tests/m2k_analog_test.py`                                                         |
| **Data Generation**  | Programmatic (waveforms generated in test code)                                    |
| **Related Issue**    | [analogdevicesinc/m2k-fw#20](https://github.com/analogdevicesinc/m2k-fw/issues/20) |
| **Minimum Firmware** | v0.34+                                                                             |


## Problem Statement

Firmware v0.33 exhibits a synchronization issue when both DAC channels operate simultaneously with different hardware oversampling ratios. The phase relationship between channels deviates from the configured settings, causing misalignment in the output waveforms.

**Root Cause:** Incorrect handling of IIO oversampling attributes in the hardware configuration. When libm2k's `setOversamplingRatio` API configures different oversampling ratios for each DAC channel, the channels lose their temporal synchronization.

**Manifestation:** The issue occurs when:
- Both DAC channels generate waveforms concurrently
- Channels use different oversampling ratios
- The expected phase relationship between channels is not maintained


## Test Configuration

This test validates hardware oversampling functionality using pre-generated waveform data from a CSV file. The test deliberately stresses the synchronization logic by using drastically different oversampling ratios on each channel.

### Data File Structure

| Column | Samples | Signal   | Description                              |
| ------ | ------- | -------- | ---------------------------------------- |
| CH0    | 750,000 | Triangle | Full resolution, no HW oversampling      |
| CH1    | 20      | Ramp     | Minimal samples, maximum HW oversampling |

### Channel 0: Triangle Wave (Native Resolution)

| Parameter      | Value   | Rationale                                    |
| -------------- | ------- | -------------------------------------------- |
| Sample Count   | 750,000 | One complete period at native DAC resolution |
| Sample Rate    | 75 MHz  | Maximum DAC sample rate                      |
| Oversampling   | 1       | No hardware oversampling (native samples)    |
| Effective Rate | 75 MHz  | Full resolution output                       |
| Period         | 10 ms   | 750,000 samples ÷ 75 MHz = 10 ms per cycle   |

### Channel 1: Ramp Signal (Maximum Hardware Oversampling)

| Parameter      | Value   | Rationale                                      |
| -------------- | ------- | ---------------------------------------------- |
| Sample Count   | 20      | Minimal buffer, maximum hardware interpolation |
| Sample Rate    | 75 MHz  | Maximum DAC sample rate                        |
| Oversampling   | 750,000 | Each sample repeated 750,000 times by hardware |
| Effective Rate | 100 Hz  | 75 MHz ÷ 750,000 = 100 Hz effective rate       |

> **Note on terminology:** The CSV contains a 20-sample "ramp" waveform, but due to hardware sample-and-hold behavior with 750,000x oversampling, each sample is held constant for 10 ms. This creates a "stair step" pattern when captured by the ADC, which is why the validation algorithm refers to "stair step falling edges."

## Design Rationale

### Why Hardware Oversampling?

The original bug report ([m2k-fw#20](https://github.com/analogdevicesinc/m2k-fw/issues/20)) specifically identifies issues with the firmware's IIO oversampling attribute handling. Testing with software-generated high-resolution waveforms would **not** reproduce the bug because:

1. **Software generation bypasses the problematic code path** - The issue is in the hardware oversampling logic, not the data itself
2. **IIO attribute configuration is the trigger** - The bug only manifests when oversampling ratios are configured through IIO attributes
3. **Different ratios stress synchronization** - Using drastically different oversampling ratios on each channel exercises the synchronization logic that was broken in v0.33

### Why Programmatic Waveform Generation?

The waveforms are generated programmatically in the test code to ensure:
- **Self-contained tests** - No external data files required
- **Reproducibility** - Waveform parameters are explicitly defined in code
- **Consistency** - IIO oversampling and sample rate configurations are applied via libm2k API calls, isolating the hardware synchronization behavior

The generated waveforms match the original buffer layout used in Scopy when the issue was discovered:
- **CH0 (Triangle)**: 750,000 samples, 0-5V amplitude, symmetric rising/falling edges
- **CH1 (Ramp/Stair)**: 20 samples, pattern [0,1,2,3,4] repeated 4 times


### Synchronization Validation

The test validates that the triangle wave (CH0) and stair step signal (CH1) maintain proper phase alignment. When synchronized correctly, the triangle wave's local minima should align with the stair step's falling edges.


## Expected Results

### Pass (firmware v0.34+)

- Both channels display expected waveform characteristics in captured data
- Triangle minima is located around the stair step falling edges

### Fail (firmware <= v0.34)

- By design the test is skipped on older firmware versions since it's a known bug.
  It can still be executed by removing the `@unittest.skipIf` decorator in the test class.

## Running the Test

Prerequisites:
- Loopback connection from `W1 -> 1+`, `W2 -> 2+`, `{1-,2-} -> GND`.

From the `tests/` directory run:

```bash
python3 main.py A_AnalogTests.test_dual_channel_waveform_sync
```

## References

- [m2k-fw Issue #20](https://github.com/analogdevicesinc/m2k-fw/issues/20) - Original bug report and discussion