import unittest
import libm2k
from digital_functions import dig_reset, set_digital_trigger, check_digital_channels_state, check_digital_output, \
    check_digital_trigger, check_open_drain_mode
from digital_functions import test_digital_cyclic_buffer
from open_context import dig, d_trig
import logger
from repeat_test import repeat

class D_DigitalTests(unittest.TestCase):
    """Class where are defined tests for the digital segment
    """

    @classmethod
    def setUpClass(self):
        log = logger.myLogger()
        log.info("Digital Segment")

    def test_digital_objects(self):
        with self.subTest(msg='test if Digital and Digital Trigger objects were retrieved'):
            self.assertIsNot((dig, d_trig), (None, None), 'Digital objects: dig, d_trig ')

    def test_state_digital_channels(self):
        for i in range(16):
            result = check_digital_channels_state(dig, i)
            for j in range(len(result)):
                with self.subTest(i):
                    self.assertEqual(result[j], 1, "Channel: " + str(i))

    def test_digital_output_channels(self):
        return_data = check_digital_output(dig)
        for i in range(16):
            with self.subTest(i):
                self.assertEqual(return_data[i], 1, "Channel: " + str(i))

    def test_trig_conditions(self):
        for i in range(16):
            trig_test = check_digital_trigger(i, dig, d_trig)
            for j in range(len(trig_test)):
                with self.subTest(j):
                    self.assertEqual(trig_test[j], 1, "Digital channel: "+ str(i))

    def test_cyclic_buffer(self):
        for i in range(16):
            with self.subTest(i):
                self.assertEqual(test_digital_cyclic_buffer(dig, d_trig, i), 0, "Channel: " + str(i))

