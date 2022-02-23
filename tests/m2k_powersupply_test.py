import unittest
import sys
from open_context import ain, aout, ctx, ps
import reset_def_values as reset
from ps_functions import ps_test_negative, ps_test_positive, config_for_ps_test
import logger
import main
from repeat_test import repeat

class C_PowerSupplyTests(unittest.TestCase):
    #  Class Where are defined all test methods for Positive PowerSupply and Negative PowerSupply

    @classmethod
    def setUpClass(self):
        # wait for user to make appropriate connections
        # print on the terminal some info

        log = logger.myLogger()
        log.info("Power Supply\n"
                 "Connections:\n"
                 "V+ ====> 1+\n"
                 "V- ====> 2+\n"
                 "GND ===> 1-\n"
                 "GND ===> 2-\n"
                 "If needed, press enter, when you've made the connections.")

        if main.wait_for_input:
            input()

    def test_1_power_supply_object(self):
        # Verifies if the Power Supply object was successfully retrieved from the context

        reset.analog_in(ain)
        config_for_ps_test(ps, ain)
        with self.subTest(msg='Test if the Power Supply object was retrieved'):
            self.assertIsNotNone(ps, 'Power supply object was not retrieved')

    def test_positive_power_supply(self):
        # Verifies the Positive Power supply through ps_test_positive().

        pos_supply = ps_test_positive(ps, ain)
        with self.subTest(msg='Test the positive Power Supply '):
            self.assertEqual(all(pos_supply), 1, 'positive power supply')

    def test_negative_power_supply(self):
        #  Verifies the Negative Power supply through ps_test_negative().

        neg_supply = ps_test_negative(ps, ain)
        with self.subTest(msg='Test the negative  Power Supply'):
            self.assertEqual(all(neg_supply), 1, 'negative power supply')
