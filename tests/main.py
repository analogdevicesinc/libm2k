import libm2k
import unittest
import HtmlTestRunner
import sys
import os
import shutil
import ps_functions
import analog_functions
import trig_functions
import digital_functions
import bug_checks_functions
from open_context import ctx
from create_files import results_dir, results_file, results_file_path
from m2k_analog_test import *
from m2k_powersupply_test import *
from m2k_trigger_test import *
from m2k_digital_test import *
from m2k_emulator_test import *
# from m2k_bug_checks import *
global gen_reports, wait_for_input
gen_reports = True
wait_for_input = False


def no_reports():
    if len(sys.argv) > 1 and "nofiles" in sys.argv:
        global gen_reports
        gen_reports = False
        ps_functions.gen_reports = False
        trig_functions.gen_reports = False
        analog_functions.gen_reports = False
        digital_functions.gen_reports = False
        bug_checks_functions.gen_reports = False

def wait_():
    global wait_for_input
    if len(sys.argv) == 1 or sys.argv[1] == "nofiles" or (len(sys.argv) > 3 and "C_PowerSupplyTests" in sys.argv):
        wait_for_input = True
    else:
        wait_for_input = False


no_reports()
wait_()
if __name__ == "__main__":
    #  Main file where tests for all segments are called. The test classes are organized in a test suite.
    #  To run specific tests, specify the class and the test(s) when running this script.
    #  To run a specific class of tests, indicate said class(es) when running this script.
    #  If you do not require reports to be generated, add "nofiles" when running this script.
    #  For instructions, run: --> main.py -h
    #                       --> main.py --help

    if len(sys.argv) > 1 and (sys.argv[1] == "--help" or sys.argv[1] == "-h"):
        print("\n====== Instructions ======\n")
        print("To run all tests run: main.py\n ")
        print("To run a specific class of tests run: main.py TestClass \n")
        print("To run a specific test run: main.py TestClass.TestName \n")
        print("To skip generating reports add <<no_reports>> when executing this script\n")
        print("List of classes and tests: \n")
        print("\n ===== class A_AnalogTests ===== \n")
        print(" ===== tests ====== \n")
        print("test_1_analog_objects\n"
              "test_2_calibration\n"
              "test_kernel_buffers"
              "test_amplitude\n"
              "test_analog_trigger_ch0\n"
              "test_analog_trigger_ch1\n"
              "test_cyclic_buffer\n"
              "test_frequency\n"
              "test_offset\n"
              "test_oversampling_ratio\n"
              "test_phase_difference_between_channels_in_degrees\n"
              "test_phase_difference_between_channels_in_samples\n"
              "test_shapes_ch0\n"
              "test_shapes_ch1\n"
              "test_voltmeter\n"
              "test_buffer_transition_glitch\n")
        print("\n ===== class B_TriggerTests ===== \n")
        print(" ===== tests ====== \n")
        print("test_1_trigger_object\n"
              "test_trigger_jitter_ch0\n"
              "test_trigger_jitter_ch1\n")
        print("\n ===== class C_PowerSupplyTests ===== \n")
        print(" ===== tests ====== \n")
        print("test_1_power_supply_object\n")
        print("test_negative_power_supply\n")
        print("test_positive_power_supply\n")
        print("\n ===== class D_DigitalTests ===== \n")
        print(" ===== tests ====== \n")
        print("test_digital_objects\n")
        print("test_state_digital_channels\n")
        print("test_digital_output_channels\n")
        print("test_trig_conditions\n")
        print("test_cyclic_buffer\n")
        print("test_kernel_buffers\n")
        print("test_pattern_generator_pulse\n")

        exit()
    elif len(sys.argv) > 1 and "nofiles" in sys.argv:
        print("\n Reports will not be generated \n")
        for i in range(len(sys.argv)):
            if sys.argv[i] == "nofiles":
                sys.argv.pop(i)
                break

        dir_path = os.path.dirname(os.path.realpath(results_file_path))
        if os.path.isdir(dir_path):
            shutil.rmtree(dir_path)
        unittest.main(testRunner=unittest.TextTestRunner(stream=sys.stderr), exit=False)

        libm2k.contextClose(ctx)
        exit()
    else:

        unittest.main(testRunner=HtmlTestRunner.HTMLTestRunner(output=str(results_dir),
                                                               report_title="ADALM2000 libm2k test results",
                                                               report_name='M2K_test_results', open_in_browser=True,
                                                               combine_reports=True))
        libm2k.contextClose(ctx)
        exit()
