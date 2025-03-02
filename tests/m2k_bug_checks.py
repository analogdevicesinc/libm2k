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
import libm2k
import logger
from repeat_test import repeat
from bug_checks_functions import test_dac_artifact
from open_context import ain, aout, trig


class E_BugChecks(unittest.TestCase):
    # Test class for specific bugs/ issues

    @classmethod
    def setUpClass(self):
        # wait for user to make appropriate connections
        global wait_for_input_a
        wait_for_input_a = True
        log = logger.myLogger()
        log.info("\nTest suite to check for known bugs/ issues\n")

    def test_dac_artifact(self):
        log = logger.myLogger()
        log.info("\nSearch for DAC artifact on both channels\n")
        log.info("Connections:\n"
                 "W1 ====> 1+\n"
                 "W2 ====> 2+\n"
                 "GND ===> 1-\n")
        for i in range(2):
            dac_artifact = test_dac_artifact(ain, aout, trig, i)
            with self.subTest(msg='Test if a spike appears on channel' + str(i)):
                self.assertEqual(dac_artifact, 0, 'spike on channel' + str(i))
