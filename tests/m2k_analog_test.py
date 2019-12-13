import sys
import unittest
import libm2k
from shapefile import shape_gen, ref_shape_gen, shape_name
from analog_functions import  test_amplitude, test_shape, phase_diff_ch0_ch1,  test_offset, test_analog_trigger, test_voltmeter_functionality
from analog_functions import cyclic_buffer_test, set_samplerates_for_shapetest, set_trig_for_cyclicbuffer_test
from analog_functions import compare_in_out_frequency, test_oversampling_ratio, channels_diff_in_samples
import reset_def_values as reset
from open_context_and_files import ctx, ain, aout, trig, results_dir, results_file, csv_path, calibration, create_dir
import logging




class A_AnalogTests(unittest.TestCase):
    """Class Where are defined all test methods for AnalogIn, AnalogOut, AnalogTrigger
    
    """
    @classmethod
    def setUpClass(self):
        #print on the terminal some info 
        logging.getLogger().info("Analogical Segment\n")
        logging.getLogger().info("Connections:")
        logging.getLogger().info("W1 ====> 1+")
        logging.getLogger().info("W2 ====> 2+")
        logging.getLogger().info("GND ===> 1-")
        logging.getLogger().info("GND ===> 2-\n")
  
    def test_1_analog_objects(self):
        """Verify through open_context() function if the analog objects AnalogIn, AnalogOut and Trigger were successfully retrieved.
    """
        with self.subTest(msg='test if AnalogIn, AnalogOut and Trigger objects were retrieved'):
            self.assertIsNot((ain,aout, trig),(0,0,0),'Analog objects: ain, aout, trig ')

    def test_2_calibration(self):

        """Verify trough calibrate(ctx) function if the ADC and the DAC were calibrated.
        """
        with self.subTest(msg='Test if ADC and DAC were succesfully calibrated'):
            self.assertEqual(calibration,(True,True),'Calibration failed')


    def test_shapes_ch0(self):
        """Verifies that all the elements of a correlation vector  returned by test_shape() are greater than 0.85.
        A correlation coefficient greater 0.7 indicates that there is a strong positive linear relationship between two signals.
        """
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
        out0_buffer_samples, out1_buffer_samples, ch0_sample_ratio, ch1_sample_ratio, in0_buffer_samples, in1_buffer_samples=set_samplerates_for_shapetest(ain, aout)
        shapename=shape_name()#names of generated signals
        buffer0=shape_gen(out0_buffer_samples)
        ref_shape_buf0=ref_shape_gen(in0_buffer_samples)

        #coorrelation coefficient vector for signals acquired on channel0 
        #correlation coeff >0.7 there is a strong positive linear relationship between the 2 signals
        corr_shape_vect0, phase_diff_vect0=test_shape(libm2k.ANALOG_IN_CHANNEL_1,buffer0,ref_shape_buf0,ain,aout,trig,ch0_sample_ratio, shapename, results_dir, results_file, csv_path)

        for i in range(len(corr_shape_vect0)):
            with self.subTest(msg='Is sent '+str(shapename[i])+' signal shape on aout ch0 and received on ain ch0'):
                self.assertGreater(corr_shape_vect0[i], 0.85, shapename[i] )


    def test_shapes_ch1(self):
        """Verifies that all the elements of a correlation vector returned by test_shape()  are greater than 0.85.
        """
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
        out0_buffer_samples, out1_buffer_samples, ch0_sample_ratio, ch1_sample_ratio, in0_buffer_samples, in1_buffer_samples=set_samplerates_for_shapetest(ain, aout)
        shapename=shape_name()#names of generated signals
        buffer1=shape_gen(out1_buffer_samples)
        ref_shape_buf1=ref_shape_gen(in1_buffer_samples)
        corr_shape_vect1, phase_diff_vect1=test_shape(libm2k.ANALOG_IN_CHANNEL_2,buffer1,ref_shape_buf1,ain,aout,trig,ch1_sample_ratio,shapename,results_dir, results_file, csv_path)
        for i in range(len(corr_shape_vect1)):
            with self.subTest(msg='Is sent '+str(shapename[i])+' signal shape on aout ch1 and received on ain ch1'):
                self.assertGreater(corr_shape_vect1[i], 0.85, shapename[i] )



    def test_phase_difference_between_channels_in_degrees(self):
        """Verifies if a signal sent simultaneously on both analog channels is the same on both receive channels and there is no phase difference between them.
        The phase difference value is returned by phase_diff_ch0_ch1().
        """
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)

        phase_diff_between_channels,adc_sr=phase_diff_ch0_ch1( aout,ain,trig, results_dir, results_file,csv_path)
        for i in range(len(adc_sr)):
            with self.subTest('ADC SampleRate'+str(adc_sr[i]) ):
                self.assertEqual(int(phase_diff_between_channels[i]),0,'not in phase, ADC SR:'+str(phase_diff_between_channels[i]))
        

    def test_phase_difference_between_channels_in_samples(self):
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
        samples_diff0, adc_sr, dac_osr, freq=channels_diff_in_samples(trig, libm2k.ANALOG_IN_CHANNEL_1,aout, ain, results_file, csv_path)
        samples_diff1, adc_sr, dac_osr, freq=channels_diff_in_samples(trig, libm2k.ANALOG_IN_CHANNEL_2,aout, ain, results_file, csv_path)
        for i in range(len(adc_sr)):
            with self.subTest(msg='ADC SR:' +str(adc_sr[i]) +'; trig src: Ch0' ):
                self.assertEqual(sum(samples_diff0[i]),0,'\ndiff: '+str(samples_diff0[i])+'\ndac_osr: '+str(dac_osr)+'\nfreq: '+str(freq))
        for i in range(len(adc_sr)):
            with self.subTest(msg='ADC SR:' +str(adc_sr[i]) +'; trig src: Ch1' ):
                self.assertEqual(sum(samples_diff1[i]),0,'\ndiff: '+str(samples_diff1[i])+'\ndac_osr: '+str(dac_osr)+'\nfreq: '+str(freq))

    def test_amplitude(self):
        """Verifies that all the elements of a vector that holds the amplitude coefficients are greater than 0.9. The vector is returned by test_amplitude()
        """
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
        out0_buffer_samples, out1_buffer_samples, ch0_sample_ratio, ch1_sample_ratio, in0_buffer_samples, in1_buffer_samples=set_samplerates_for_shapetest(ain, aout)
        buffer0=shape_gen(out0_buffer_samples)
        ref_shape_buf0=ref_shape_gen(in0_buffer_samples)
        buffer1=shape_gen(out1_buffer_samples)
        ref_shape_buf1=ref_shape_gen(in1_buffer_samples)
        amp_coeff_ch0=test_amplitude(buffer0[0],ref_shape_buf0[0],in0_buffer_samples, ain, aout,libm2k.ANALOG_IN_CHANNEL_1, trig,results_dir, results_file,csv_path)
        amp_coeff_ch1=test_amplitude(buffer1[0],ref_shape_buf1[0], in1_buffer_samples, ain, aout, libm2k.ANALOG_IN_CHANNEL_2, trig,results_dir, results_file,csv_path )
        amplitude_coefficients=(amp_coeff_ch0,amp_coeff_ch1)
        for i in range(2):
            with self.subTest( msg='Test different signal amplitudes on ch '+ str(i)):
                self.assertGreater(amplitude_coefficients[i], (0.9, 0.9), 'amplitude on channel'+str(i) )

    def test_offset(self):
        """Verifies that all the elements of a vector that holds the offset coefficients are greater than 0.9. The vector is returned by test_offset()
        """
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
        out0_buffer_samples, out1_buffer_samples, ch0_sample_ratio, ch1_sample_ratio, in0_buffer_samples, in1_buffer_samples=set_samplerates_for_shapetest(ain, aout)
        buffer0=shape_gen(out0_buffer_samples)
        offset_ch0=test_offset(buffer0[0],in0_buffer_samples,ain, aout,trig,libm2k.ANALOG_IN_CHANNEL_1,results_dir, results_file, csv_path)
        offset_ch1=test_offset(buffer0[0],in1_buffer_samples,ain, aout,trig,libm2k.ANALOG_IN_CHANNEL_2, results_dir, results_file, csv_path)
        offset_coefficients=(offset_ch0, offset_ch1)
        for i in range(2):
            with self.subTest( msg='Test different signal offsets on ch ' +str(i)):
                self.assertGreater(offset_coefficients[i], 0.9, 'offset on channel'+str(i) )

    def test_analog_trigger_ch0(self):
        """Verifies the analog trigger conditions on analog channel 0. The trigger conditions are set and verified in test_analog_trigger()
        """
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
        trig_test0, condition_name=test_analog_trigger(libm2k.ANALOG_IN_CHANNEL_1,trig,aout,ain, results_dir, results_file, csv_path)
        reset.trigger(trig)
        for i in range(len(trig_test0)):
            with self.subTest(  msg='Test the analog trigger on ch 0 with ' +str(condition_name[i])+' trigger condition'):
                self.assertEqual(trig_test0[i], 1, condition_name[i])


    def test_analog_trigger_ch1(self):
        """Verifies the analog trigger conditions on analog channel 1
        """
        #analog trigger
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
        trig_test1, condition_name=test_analog_trigger(libm2k.ANALOG_IN_CHANNEL_2,trig,aout,ain, results_dir, results_file, csv_path)
        reset.trigger(trig)
        for i in range(len(trig_test1)):
            with self.subTest( msg='Test the analog trigger on ch 1 with ' +str(condition_name[i])+' trigger condition'):
                self.assertEqual(trig_test1[i], 1,condition_name[i])

    def test_voltmeter(self):
        """Verifies voltmeter functionality on analog channels. 
        """
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
         #VOLTMETER
        voltmeter_ch0=test_voltmeter_functionality(libm2k.ANALOG_IN_CHANNEL_1, ain, aout,ctx, results_file)
        voltmeter_ch1=test_voltmeter_functionality(libm2k.ANALOG_IN_CHANNEL_2, ain, aout,ctx, results_file)
        voltmeter_test=(voltmeter_ch0, voltmeter_ch1)
        for i in range(2):
            with self.subTest(msg='Send a constant buffer through aout ch '+str(i)+ ' and receive it on ain ch '+str(i)):
                self.assertEqual(all(voltmeter_test[i]), True, 'voltmeter channel'+str(i) )

    def test_cyclic_buffer(self):
        """Verifies if a single buffer is received when cyclic buffer is set to false.
        cyclic_buffer_test() returns 1 if the operation was successful
        """
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)

        cyclic_buffer_test_ch1 = cyclic_buffer_test(aout, ain, libm2k.ANALOG_IN_CHANNEL_1, trig, results_dir, results_file, csv_path,ctx)
        cyclic_buffer_test_ch2 = cyclic_buffer_test(aout, ain, libm2k.ANALOG_IN_CHANNEL_2, trig, results_dir, results_file, csv_path,ctx)
        cyclic_buffer=(cyclic_buffer_test_ch1, cyclic_buffer_test_ch2)
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)

        for i in range(2):
            with self.subTest(msg='Test if a single buffer is received when cyclic buffer is set to false on ch '+str(i)):
                self.assertEqual(cyclic_buffer[i], 1, "channel"+str(i))

    def test_frequency(self):
        """Verifies if a frequency sent on aout channels is the same on ain channels, for different values of the ADC and DAC sample rates.
        Frequencies are compared in compare_in_out_frequency().
        """

        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
        frequency_test=[compare_in_out_frequency(libm2k.ANALOG_IN_CHANNEL_1, ain, aout, trig, results_file),compare_in_out_frequency(libm2k.ANALOG_IN_CHANNEL_2, ain, aout, trig, results_file)]
        for i in range(2):
            with self.subTest(msg='Loop through all availabe sampling rates for ain and aout and test some frequency values on ch'+str(i)):
                self.assertEqual(all(frequency_test[i]), True, 'in and out frequencies do not correspond on channel'+str(i) )
    def test_oversampling_ratio(self):
        """Verifies the effect of setting the oversampling ratio parameter for analog input
        """
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)

        osr_test=(test_oversampling_ratio(libm2k.ANALOG_IN_CHANNEL_1,ain,aout,trig, results_file, csv_path),test_oversampling_ratio(libm2k.ANALOG_IN_CHANNEL_2,ain,aout,trig, results_file, csv_path))
        for i in range(2):
            with self.subTest(msg='Test different oversampling ratio values for ain on ch'+str(i)):
                self.assertEqual(osr_test[i], 1, 'oversampling on channel'+str(i) )
   









