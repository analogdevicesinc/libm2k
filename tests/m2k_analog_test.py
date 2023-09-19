import sys
import unittest
import libm2k

from shapefile import shape_gen, ref_shape_gen, shape_name
from analog_functions import test_amplitude, test_shape, phase_diff_ch0_ch1, test_offset, test_analog_trigger, \
    test_voltmeter_functionality, test_kernel_buffers, test_buffer_transition_glitch
from analog_functions import noncyclic_buffer_test, set_samplerates_for_shapetest, set_trig_for_cyclicbuffer_test, \
    test_calibration
from analog_functions import compare_in_out_frequency, test_oversampling_ratio, channels_diff_in_samples, test_timeout, \
    cyclic_buffer_test
import reset_def_values as reset
from open_context import ctx, ain, aout, trig, create_dir
from create_files import results_dir, csv, results_file
import logger
from repeat_test import repeat


class A_AnalogTests(unittest.TestCase):
    # Class Where are defined all test methods for AnalogIn, AnalogOut, AnalogTrigger

    @classmethod
    def setUpClass(self):
        # wait for user to make appropriate connections
        global wait_for_input_a
        wait_for_input_a = True
        log = logger.myLogger()
        log.info("\nAnalogical Segment\n"
                 "Connections:\n"
                 "W1 ====> 1+\n"
                 "W2 ====> 2+\n"
                 "GND ===> 1-\n")

    def test_1_analog_objects(self):
        # Verify through open_context() function if the analog objects AnalogIn, AnalogOut and Trigger were 
        # successfully retrieved.

        with self.subTest(msg='test if AnalogIn, AnalogOut and Trigger objects were retrieved'):
            self.assertIsNot((ain, aout, trig), (None, None, None), 'Analog objects: ain, aout, trig ')

    def test_2_calibration(self):
        # Verify trough calibrate(ctx) function if the ADC and the DAC were calibrated.

        calibration = test_calibration(ctx)
        with self.subTest(msg='Test if ADC and DAC were succesfully calibrated'):
            self.assertEqual(calibration, (True, True), 'Calibration failed')

    def test_kernel_buffers(self):
        # Verifies if the kernel buffer count can be set without throwing runtime error (busy retry works)
        test_err = test_kernel_buffers(ain, trig, 4)
        with self.subTest(
            msg='Set kernel buffers count on AIN without raising an error '):
            self.assertEqual(test_err, False, 'Error occured')

    def test_shapes_ch0(self):
        # Verifies that all the elements of a correlation vector  returned by test_shape() are greater than 0.85. A
        # correlation coefficient greater 0.7 indicates that there is a strong positive linear relationship between
        # two signals.

        out0_buffer_samples, out1_buffer_samples, ch0_sample_ratio, ch1_sample_ratio, in0_buffer_samples, in1_buffer_samples = set_samplerates_for_shapetest(
            ain, aout)
        shapename = shape_name()  # names of generated signals
        buffer0 = shape_gen(out0_buffer_samples)
        ref_shape_buf0 = ref_shape_gen(in0_buffer_samples)

        # correlation coefficient vector for signals acquired on channel0
        # correlation coef. >0.7 there is a strong positive linear relationship between the 2 signals
        corr_shape_vect0, phase_diff_vect0 = test_shape(libm2k.ANALOG_IN_CHANNEL_1, buffer0, ref_shape_buf0, ain, aout,
                                                        trig, ch0_sample_ratio, shapename)

        for i in range(len(corr_shape_vect0)):
            with self.subTest(msg='Is sent ' + str(shapename[i]) + ' signal shape on aout ch0 and received on ain ch0'):
                self.assertGreater(corr_shape_vect0[i], 0.85, shapename[i])

    def test_shapes_ch1(self):
        # Verifies that all the elements of a correlation vector returned by test_shape()  are greater than 0.85.

        out0_buffer_samples, out1_buffer_samples, ch0_sample_ratio, ch1_sample_ratio, in0_buffer_samples, in1_buffer_samples = set_samplerates_for_shapetest(
            ain, aout)
        shapename = shape_name()  # names of generated signals
        buffer1 = shape_gen(out1_buffer_samples)
        ref_shape_buf1 = ref_shape_gen(in1_buffer_samples)
        corr_shape_vect1, phase_diff_vect1 = test_shape(libm2k.ANALOG_IN_CHANNEL_2, buffer1, ref_shape_buf1, ain, aout,
                                                        trig, ch1_sample_ratio, shapename)
        for i in range(len(corr_shape_vect1)):
            with self.subTest(msg='Is sent ' + str(shapename[i]) + ' signal shape on aout ch1 and received on ain ch1'):
                self.assertGreater(corr_shape_vect1[i], 0.85, shapename[i])

    def test_phase_difference_between_channels_in_degrees(self):
        # Verifies if a signal sent simultaneously on both analog channels is the same on both receive channels and
        # there is no phase difference between them. The phase difference value is returned by phase_diff_ch0_ch1().

        phase_diff_between_channels, adc_sr = phase_diff_ch0_ch1(aout, ain, trig)
        for i in range(len(adc_sr)):
            with self.subTest('ADC SampleRate' + str(adc_sr[i])):
                self.assertEqual(int(phase_diff_between_channels[i]), 0,
                                 'not in phase, ADC SR:' + str(phase_diff_between_channels[i]))

    @unittest.skip("Test needs further investigation.")
    def test_phase_difference_between_channels_in_samples(self):

        samples_diff0, adc_sr, dac_osr, freq = channels_diff_in_samples(trig, libm2k.ANALOG_IN_CHANNEL_1, aout, ain)
        samples_diff1, adc_sr, dac_osr, freq = channels_diff_in_samples(trig, libm2k.ANALOG_IN_CHANNEL_2, aout, ain)
        for i in range(len(adc_sr)):
            with self.subTest(msg='ADC SR:' + str(adc_sr[i]) + '; trig src: Ch0'):
                self.assertEqual(sum(samples_diff0[i]), 0,
                                 '\ndiff: ' + str(samples_diff0[i]) + '\ndac_osr: ' + str
                                 (dac_osr) + '\nfreq: ' + str(freq))
        for i in range(len(adc_sr)):
            with self.subTest(msg='ADC SR:' + str(adc_sr[i]) + '; trig src: Ch1'):
                self.assertEqual(sum(samples_diff1[i]), 0,
                                 '\ndiff: ' + str(samples_diff1[i]) + '\ndac_osr: ' + str(
                                     dac_osr) + '\nfreq: ' + str(freq))

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

    def test_analog_trigger_ch0(self):
        # Verifies the analog trigger conditions on analog channel 0. The trigger conditions are set and verified in
        # test_analog_trigger()

        trig_test0, condition_name = test_analog_trigger(libm2k.ANALOG_IN_CHANNEL_1, trig, aout, ain)
        reset.trigger(trig)
        for i in range(len(trig_test0)):
            with self.subTest(
                    msg='Test the analog trigger on ch 0 with ' + str(condition_name[i]) + ' trigger condition'):
                self.assertEqual(trig_test0[i], 1, condition_name[i])

    def test_analog_trigger_ch1(self):
        # Verifies the analog trigger conditions on analog channel 1

        trig_test1, condition_name = test_analog_trigger(libm2k.ANALOG_IN_CHANNEL_2, trig, aout, ain)
        reset.trigger(trig)
        for i in range(len(trig_test1)):
            with self.subTest(
                    msg='Test the analog trigger on ch 1 with ' + str(condition_name[i]) + ' trigger condition'):
                self.assertEqual(trig_test1[i], 1, condition_name[i])

    def test_voltmeter(self):
        # Verifies voltmeter functionality on analog channels.

        voltmeter_ch0 = test_voltmeter_functionality(libm2k.ANALOG_IN_CHANNEL_1, ain, aout, ctx)
        voltmeter_ch1 = test_voltmeter_functionality(libm2k.ANALOG_IN_CHANNEL_2, ain, aout, ctx)
        voltmeter_test = (voltmeter_ch0, voltmeter_ch1)
        for i in range(2):
            with self.subTest(
                    msg='Send a constant buffer through aout ch ' + str(i) + ' and receive it on ain ch ' + str(i)):
                self.assertEqual(all(voltmeter_test[i]), True, 'voltmeter channel' + str(i))

    def test_cyclic_buffer(self):
        # Verifies that multiple buffers are received when cyclic buffer is set to true.
        # cyclic_buffer_test() returns 1 if the operation was successful

        cyclic_buffer_test_ch1 = cyclic_buffer_test(aout, ain, libm2k.ANALOG_IN_CHANNEL_1, trig)
        cyclic_buffer_test_ch2 = cyclic_buffer_test(aout, ain, libm2k.ANALOG_IN_CHANNEL_2, trig)
        cyclic_buffer = (cyclic_buffer_test_ch1, cyclic_buffer_test_ch2)

        for i in range(2):
            with self.subTest(
                    msg='Test if multiple buffers are received when cyclic buffer is set to true on ch ' + str(i)):
                self.assertEqual(cyclic_buffer[i], 1, "channel" + str(i))

    def test_noncyclic_buffer(self):
        # Verifies if a single buffer is received when cyclic buffer is set to false.
        # cyclic_buffer_test() returns 1 if the operation was successful

        non_cyclic_buffer_test_ch1 = noncyclic_buffer_test(aout, ain, libm2k.ANALOG_IN_CHANNEL_1, trig, ctx)
        non_cyclic_buffer_test_ch2 = noncyclic_buffer_test(aout, ain, libm2k.ANALOG_IN_CHANNEL_2, trig, ctx)
        non_cyclic_buffer = (non_cyclic_buffer_test_ch1, non_cyclic_buffer_test_ch2)

        for i in range(2):
            with self.subTest(
                    msg='Test if a single buffer is received when cyclic buffer is set to false on ch ' + str(i)):
                self.assertEqual(non_cyclic_buffer[i], 1, "channel" + str(i))

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

    def test_oversampling_ratio(self):
        # Verifies the effect of setting the oversampling ratio parameter for analog input

        osr_test = (test_oversampling_ratio(libm2k.ANALOG_IN_CHANNEL_1, ain, aout, trig),
                    test_oversampling_ratio(libm2k.ANALOG_IN_CHANNEL_2, ain, aout, trig))
        for i in range(2):
            with self.subTest(msg='Test different oversampling ratio values for ain on ch' + str(i)):
                self.assertEqual(osr_test[i], 1, 'oversampling on channel' + str(i))

    def test_timeout(self):
        # Verifies if there is data acquisition if trigger is reset after timeout occurs

        for i in range(2):
            data = False
            offset, average, t_occ = test_timeout(ctx, ain, aout, trig, i, results_dir, results_file, csv)
            if (offset - 0.1 * offset) <= average <= (offset + 0.1 * offset):
                data = True

            if t_occ:
                with self.subTest(msg='Timeout occurred'):
                    self.assertEqual(data, True, 'Data was not acquired correctly after timeout')
            else:
                with self.subTest(msg='No timeout'):
                    self.assertEqual(data, True, 'Data was not acquired correctly')

    @unittest.skipIf(ctx.getFirmwareVersion() < 'v0.32', 'This is a known bug in previous firmware implementations which is fixed in v0.32')
    def test_buffer_transition_glitch(self):
        # Pushing a new cyclic buffer should output the value of the raw attr. In previous firmware versions, the new buffer would start 
        # with the last sample from previous buffer which lead to a glitch in the output signal. This test verifies that the glitch is not present anymore.
        
        for channel in [libm2k.ANALOG_IN_CHANNEL_1, libm2k.ANALOG_IN_CHANNEL_2]:
            for waveform in ['dc', 'sine']:
                num_glitches = test_buffer_transition_glitch(channel, ain, aout, trig, waveform)
                
                with self.subTest(msg='Test buffer transition glitch: ' + waveform + ' on ch' + str(channel)):
                    self.assertEqual(num_glitches, 0, 'Found ' + str(num_glitches) + ' glitches on channel ' + str(channel))