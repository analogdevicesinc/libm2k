import sys
import unittest

import libm2k

import logger
from analog_functions import compare_in_out_frequency
from analog_functions import set_samplerates_for_shapetest
from analog_functions import test_amplitude, test_offset
from bug_checks_functions import test_dac_artifact
from open_context import ain, aout, trig
from open_context import dig, d_trig
from shapefile import shape_gen, ref_shape_gen


class E_EmulatorTests(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        log = logger.myLogger()
        log.info("Emulator tests")

    def test_analog_objects(self):
        # Verify through open_context() function if the analog objects AnalogIn, AnalogOut and Trigger were
        # successfully retrieved.

        with self.subTest(msg='test if AnalogIn, AnalogOut and Trigger objects were retrieved'):
            self.assertIsNot((ain, aout, trig), (None, None, None), 'Analog objects: ain, aout, trig ')

    def test_digital_objects(self):
        # Verify through open_context() function if the analog objects Digital and Digital Trigger were
        # successfully retrieved.
        with self.subTest(msg='test if Digital and Digital Trigger objects were retrieved'):
            self.assertIsNot((dig, d_trig), (None, None), 'Digital objects: dig, d_trig ')

    def test_trigger_object(self):
        #  Verifies if the Trigger object was successfully retrieved

        with self.subTest(msg='test if the Trigger object was retrieved'):
            self.assertIsNot(trig, 0, 'Trigger object')

    def test_frequency(self):
        # Verifies if a frequency sent on aout channels is the same on ain channels, for different values of the ADC
        # and DAC sample rates. Frequencies are compared in compare_in_out_frequency().

        frequency_test = [compare_in_out_frequency(libm2k.ANALOG_IN_CHANNEL_1, ain, aout, trig),
                          compare_in_out_frequency(libm2k.ANALOG_IN_CHANNEL_2, ain, aout, trig)]
        for i in range(2):
            with self.subTest(
                    msg='Loop through all availabe sampling rates for ain and aout and test some frequency values on ch' + str(
                        i)):
                self.assertEqual(all(frequency_test[i]), True,
                                 'in and out frequencies do not correspond on channel' + str(i))

    def test_amplitude(self):
        # Verifies that all the elements of a vector that holds the amplitude coefficients are greater than 0.9. The
        # vector is returned by test_amplitude()

        out0_buffer_samples, out1_buffer_samples, ch0_sample_ratio, ch1_sample_ratio, in0_buffer_samples, in1_buffer_samples = set_samplerates_for_shapetest(
            ain, aout)
        buffer0 = shape_gen(out0_buffer_samples)
        ref_shape_buf0 = ref_shape_gen(in0_buffer_samples)
        buffer1 = shape_gen(out1_buffer_samples)
        ref_shape_buf1 = ref_shape_gen(in1_buffer_samples)
        amp_coeff_ch0 = test_amplitude(buffer0[0], ref_shape_buf0[0], in0_buffer_samples, ain, aout,
                                       libm2k.ANALOG_IN_CHANNEL_1, trig)
        amp_coeff_ch1 = test_amplitude(buffer1[0], ref_shape_buf1[0], in1_buffer_samples, ain, aout,
                                       libm2k.ANALOG_IN_CHANNEL_2, trig)
        amplitude_coefficients = (amp_coeff_ch0, amp_coeff_ch1)
        for i in range(2):
            with self.subTest(msg='Test different signal amplitudes on ch ' + str(i)):
                self.assertGreater(amplitude_coefficients[i], (0.9, 0.9), 'amplitude on channel' + str(i))

    def test_offset(self):
        # Verifies that all the elements of a vector that holds the offset coefficients are greater than 0.9. The
        # vector is returned by test_offset()

        out0_buffer_samples, out1_buffer_samples, ch0_sample_ratio, ch1_sample_ratio, in0_buffer_samples, in1_buffer_samples = set_samplerates_for_shapetest(
            ain, aout)
        buffer0 = shape_gen(out0_buffer_samples)
        offset_ch0 = test_offset(buffer0[0], in0_buffer_samples, ain, aout, trig, libm2k.ANALOG_IN_CHANNEL_1)
        offset_ch1 = test_offset(buffer0[0], in1_buffer_samples, ain, aout, trig, libm2k.ANALOG_IN_CHANNEL_2)
        offset_coefficients = (offset_ch0, offset_ch1)
        for i in range(2):
            with self.subTest(msg='Test different signal offsets on ch ' + str(i)):
                self.assertGreater(offset_coefficients[i], 0.9, 'offset on channel' + str(i))

    def test_build_exceptions(self):
        # Test if an exception is thrown when an invalid channel number is used
        # We are intentionally trying to extract the name of an invalid channel
        # If exceptions are build then an invalid channel number should rise an "ValueError: no such channel" exception
        with self.subTest(msg='Test if exceptions are built by rising ValueError exception'):
            self.assertRaises(ValueError, ain.getChannelName, 2)
