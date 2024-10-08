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
from open_context import ain, aout, ctx, trig
import reset_def_values as reset
import sys
import libm2k
from trig_functions import *  # trigger_jitter
import logger
from repeat_test import repeat


class B_TriggerTests(unittest.TestCase):
    #  Class Where are defined all test methods for Trigger

    @classmethod
    def setUpClass(self):
        # print on the terminal some info
        log = logger.myLogger()
        log.info("\n\n Trigger\n")

    def test_1_trigger_object(self):
        #  Verifies if the Trigger object was successfully retrieved

        with self.subTest(msg='test if the Trigger object was retrieved'):
            self.assertIsNot(trig, 0, 'Trigger object')

    def test_trigger_jitter_ch1(self):
        reset.analog_in(ain)
        reset.analog_out(aout)
        reset.trigger(trig)
        buffers = 100
        sig = ['square', 'sine']
        trig_cond = [libm2k.RISING_EDGE_ANALOG, libm2k.FALLING_EDGE_ANALOG]
        for s in range(len(sig)):
            for c in trig_cond:
                trigs_counted, adc_sr, nr_samples, dac_sr = trigger_jitter(buffers, trig, libm2k.ANALOG_IN_CHANNEL_2,
                                                                           sig[s], c, ain, aout)
                for i in range(len(trigs_counted)):
                    for j in range(len(trigs_counted[i])):
                        with self.subTest('Triggers in ' + str(buffers) + ' buffers;' + sig[s] + ' ;trig:' + str(c)):
                            self.assertEqual(trigs_counted[i][j], buffers,
                                             'ADC sample rate ' + str(adc_sr[i]) + ' Frequency:' + str(
                                                 dac_sr / nr_samples[j]))

    def test_trigger_jitter_ch0(self):
        buffers = 100
        sig = ['square', 'sine']
        trig_cond = [libm2k.RISING_EDGE_ANALOG, libm2k.FALLING_EDGE_ANALOG]
        for s in range(len(sig)):
            for c in trig_cond:
                trigs_counted, adc_sr, nr_samples, dac_sr = trigger_jitter(buffers, trig, libm2k.ANALOG_IN_CHANNEL_1,
                                                                           sig[s], c, ain, aout)
                for i in range(len(trigs_counted)):
                    for j in range(len(trigs_counted[i])):
                        with self.subTest('Triggers in ' + str(buffers) + ' buffers; ' + sig[s] + ' ;trig:' + str(c)):
                            self.assertEqual(trigs_counted[i][j], buffers,
                                             'ADC sample rate ' + str(adc_sr[i]) + ' Frequency:' + str(
                                                 dac_sr / nr_samples[j]))
