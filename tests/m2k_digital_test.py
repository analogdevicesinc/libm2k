import unittest
import libm2k
from digital_functions import set_digital_trigger, check_digital_channels_state, check_digital_output, \
    check_digital_trigger, check_open_drain_mode, test_kernel_buffers, test_pattern_generator_pulse
from digital_functions import test_digital_cyclic_buffer
import reset_def_values as reset
from open_context import ctx, dig, d_trig
import logger
from repeat_test import repeat

class D_DigitalTests(unittest.TestCase):
    #  Class where are defined tests for the digital segment

    @classmethod
    def setUpClass(self):
        log = logger.myLogger()
        log.info("Digital Segment")

    def test_digital_objects(self):
        with self.subTest(msg='test if Digital and Digital Trigger objects were retrieved'):
            self.assertIsNot((dig, d_trig), (None, None), 'Digital objects: dig, d_trig ')

    def test_state_digital_channels(self):
        for i in range(16):
            result = check_digital_channels_state(dig, i)
            for j in range(len(result)):
                with self.subTest(i):
                    self.assertEqual(result[j], 1, "Channel: " + str(i))

    def test_digital_output_channels(self):
        return_data = check_digital_output(dig)
        for i in range(16):
            with self.subTest(i):
                self.assertEqual(return_data[i], 1, "Channel: " + str(i))

    def test_trig_conditions(self):
        for i in range(16):
            trig_test = check_digital_trigger(i, dig, d_trig)
            for j in range(len(trig_test)):
                with self.subTest(j):
                    self.assertEqual(trig_test[j], 1, "Digital channel: "+ str(i))

    def test_cyclic_buffer(self):
        for i in range(16):
            with self.subTest(i):
                self.assertEqual(test_digital_cyclic_buffer(dig, d_trig, i), 0, "Channel: " + str(i))

    def test_kernel_buffers(self):
        # Verifies if the kernel buffer count can be set without throwing runtime error (busy retry works)
        test_err = test_kernel_buffers(dig, 4)
        with self.subTest(
            msg='Set kernel buffers count on Digital In without raising an error '):
            self.assertEqual(test_err, False, 'Error occured')

    @unittest.skipIf(ctx.getFirmwareVersion() < 'v0.33', 'Test applicable for firmware v0.33 and later.')
    def test_pattern_generator_pulse(self):
        # Verifies that the pattern generator does not generate any additional edges prior to the pattern set.
        # At the end it holds the value of the last sample at the output.
        # The measured pattern should be the same as the one set.

        # Single channel
        for i in range(16):
            test_result = test_pattern_generator_pulse(dig, d_trig, i)
            with self.subTest(i):
                self.assertEqual(test_result, 0, "Found " + str(test_result) + " aditional edges on  Channel: " + str(i))
        # All channels
        test_result = test_pattern_generator_pulse(dig, d_trig, -1)
        with self.subTest(-1):
            self.assertEqual(test_result, 0, "Found " + str(test_result) + " aditional edges in multi-channel test")

    @unittest.skipIf(ctx.getFirmwareVersion() < 'v0.33', 'Test applicable for firmware v0.33 and later.')
    def test_last_sample_hold(self):
        # Tests the last sample and hold functionality of the digital interface.
        # - After the pattern is sent, the last sample should be held at the output.

        # Single channel
        for DIO_chn in range(16):
            result_ok = test_last_sample_hold(dig, d_trig, ctx, DIO_chn)
            with self.subTest(msg=f"DIO{str(DIO_chn)}"):
                self.assertEqual(result_ok, True, f"Failed to hold the last sample on DIO{str(DIO_chn)}")
        # All channels
        result_ok = test_last_sample_hold(dig, d_trig, ctx, None)
        with self.subTest(msg="DIO all"):
            self.assertEqual(result_ok, True, f"Failed to hold the last sample on all DIO test")