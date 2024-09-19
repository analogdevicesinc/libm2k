#
# Copyright (c) 2024 Analog Devices Inc.
#
# This file is part of libm2k
# (see http://www.github.com/analogdevicesinc/libm2k).
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

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
