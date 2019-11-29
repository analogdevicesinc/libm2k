
import libm2k
import unittest
import HtmlTestRunner
import logging
import sys
from m2k_analog_test import A_AnalogTests
from m2k_powersupply_test import C_PowerSupplyTests
from m2k_trigger_test import B_TriggerTests
#from m2k_digital_test import DigitalTests


from open_context_and_files import ctx, results_dir, open_context, calibrate, create_dir


logger = logging.getLogger()
logger.level = logging.DEBUG
logger.addHandler(logging.StreamHandler(sys.stdout))




def run_test_suite():
    """ Test suite that contains all available tests.
    When run it will create a HTML report of the tests, along with plot files and csv files with results
    """
    m2k_test_suite=unittest.TestSuite()
    m2k_test_suite.addTest(A_AnalogTests())
    m2k_test_suite.addTest(B_TriggerTests())
    m2k_test_suite.addTest(C_PowerSupplyTests())
    #m2k_test_suite.addTest(DigitalTests())
    result= unittest.main(testRunner=HtmlTestRunner.HTMLTestRunner(output=str(results_dir), report_title="ADALM2000 libm2k test results",report_name='M2K_test_results',open_in_browser=True, combine_reports=True))
    m2k_test_suite.run(result)
    return

def run_specific_tests_from_A_AnalogTests():
    """Test suite for A_AnalogTests where the used can choose the desired test
        Available Analog segment tests: test_amplitude, test_analog_trigger_ch0, test_analog_trigger_ch1, test_cyclic_buffer,test_frequency
                                        test_offset, test_oversampling_ratio, test_phase_difference_between_channels_in_degrees,
                                        test_phase_difference_between_channels_in_samples, test_shapes_ch0, test_shapes_ch1, test_voltmeter
    """
    suite = unittest.TestSuite()
    suite.addTest(A_AnalogTests("test_1_analog_objects")) #test necessary to open the context and retrieve Analog Objects
    suite.addTest(A_AnalogTests("test_2_calibration")) #test necessary to calibrate
    suite.addTest(A_AnalogTests("add_test_name_here")) #add the name of the test between " "
    #you can add many tests from the available ones using  the line above as example
    runner = unittest.TextTestRunner()
    runner.run(suite)
    return

def run_specific_tests_from_B_TriggerTests():
    """Test suite for B_TriggerTests where the used can choose the desired test
        Available Trigger tests: test_trigger_jitter_ch0, test_trigger_jitter_ch1
    """
    suite = unittest.TestSuite()
    suite.addTest(B_TriggerTests("test_1_trigger_object")) #test necessary to retrieve Trigger object
    suite.addTest(B_TriggerTests("add_test_name_here")) #add the name of the test between " "
    #you can add many tests from the available ones using the line above as example
    runner = unittest.TextTestRunner()
    runner.run(suite)
    return

def run_specific_tests_from_C_PowerSupplyTests():
    """Test suite for C_Power_SupplyTests where the used can choose the desired test
        Available Power Supply tests: test_negative_power_supply, test_positive_power_supply
    """
    suite = unittest.TestSuite()
    suite.addTest(C_PowerSupplyTests("test_1_power_supply_object")) #test necessary to retrieve the power supply object
    suite.addTest(C_PowerSupplyTests("add_test_name_here")) #add the name of the test between " "
    #you can add manytests from the available ones using  the line above as example

    runner = unittest.TextTestRunner()
    runner.run(suite)
    return



if __name__ =="__main__":
    """Main file where tests for all segments are called. The test classes are organized in a test suite.
    To run specific tests, comment run_test_suite() line and uncomment run_specific_tests_from...()  line(s)

    """
    run_test_suite()

    #run_specific_tests_from_A_AnalogTests()
    #run_specific_tests_from_B_TriggerTests()
    #run_specific_tests_from_C_PowerSupplyTests()



libm2k.contextClose(ctx, True)