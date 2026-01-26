# Test Design Documentation

This directory contains design documentation for tests in the libm2k test suite. Each document explains the rationale behind test design decisions, making it easier to understand, maintain, and extend the test suite.

## Purpose

- **Document design decisions** - Explain why tests are structured the way they are
- **Capture domain knowledge** - Record hardware-specific considerations
- **Enable maintenance** - Help future developers understand test intent
- **Track issue references** - Link tests to the bugs/features they validate

## Document Structure

Each test document should follow this template:

```markdown
# Test: [Test Name]

## Overview
| Field                | Value                                |
| -------------------- | ------------------------------------ |
| **Test Name**        | `test_function_name`                 |
| **Test File**        | `tests/file.py`                      |
| **Related Issue**    | Link to GitHub issue (if applicable) |
| **Minimum Firmware** | Version requirement (if applicable)  |

## Problem Statement
What problem or feature does this test validate?

## Test Configuration
What parameters are used and why?

## Design Rationale
Why was this approach chosen? What alternatives were considered?

## Expected Results
What indicates pass/fail? What error messages mean what?

## Running the Test
Command-line examples for running the test.

## Running the Test
Instructions for running the test.

## References
Links to issues, documentation, datasheets, etc.
```

## Naming Convention

Documentation files should be named after the test method they document:
- `test_dual_channel_waveform_sync.md`

## Index

| Document                                                                 | Test                              | Description                                  |
| ------------------------------------------------------------------------ | --------------------------------- | -------------------------------------------- |
| [test_dual_channel_waveform_sync.md](test_dual_channel_waveform_sync.md) | `test_dual_channel_waveform_sync` | Validates dual-channel phase synchronization |

