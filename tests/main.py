
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


m2k_test_suite=unittest.TestSuite()
m2k_test_suite.addTest(A_AnalogTests())
m2k_test_suite.addTest(B_TriggerTests())
m2k_test_suite.addTest(C_PowerSupplyTests())
#m2k_test_suite.addTest(DigitalTests())

if __name__ =="__main__":
    """Main file where tests for all segments are called. The test classes are organized in a test suite.
    To stop from running a specific test class comment the m2k_test_suite.addTest() line and the line that imports the test class in the main file (like in case of Digital Tests)
"""
    
    result= unittest.main(testRunner=HtmlTestRunner.HTMLTestRunner(output=str(results_dir), report_title="ADALM2000 libm2k test results",report_name='M2K_test_results',open_in_browser=True, combine_reports=True))

    m2k_test_suite.run(result)
libm2k.contextClose(ctx, True)