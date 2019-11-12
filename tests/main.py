import libm2k
import unittest
import HtmlTestRunner
import logging
import sys
from m2k_analog_test import AnalogTests
from m2k_powersupply_test import PowerSupplyTests
#from m2k_trigger_test import TriggerTests
#from m2k_digital_test import DigitalTests


from open_context_and_files import ctx, results_dir, open_context, calibrate, create_dir


logger = logging.getLogger()
logger.level = logging.DEBUG
logger.addHandler(logging.StreamHandler(sys.stdout))


m2k_test_suite=unittest.TestSuite()
m2k_test_suite.addTest(AnalogTests())
m2k_test_suite.addTest(PowerSupplyTests())
#m2k_test_suite.addTest(TriggerTests())
#m2k_test_suite.addTest(DigitalTests())

if __name__ =="__main__":
    """Main file where tests for all segments are called. The test classes are organized in a test suite.
"""
    
    result= unittest.main(testRunner=HtmlTestRunner.HTMLTestRunner(output=str(results_dir), report_title="ADALM2000 libm2k test results",report_name='M2K_test_results',open_in_browser=True, combine_reports=True))

    m2k_test_suite.run(result)
libm2k.contextClose(ctx, True)