import unittest
import libm2k
from digital_functions import dig_reset, set_digital_trigger,ch_0_7_digital_output,ch_8_16_digital_output, check_digital_trigger, check_open_drain_mode
from digital_functions import test_digital_cyclic_buffer
from open_context import dig, d_trig
import logger
class DigitalTests(unittest.TestCase):
    """Class where are defined tests for the digital segment
    """
    @classmethod
    def setUpClass(self):
        log = logger.myLogger()
        log.info("Digital Segment")

    # #test signal shapes on  ain ch0
    def test_input_output_digital_channels(self):
        buf=[255,127,63,31,15,7,3,1,0 ] # to test the channels
        ch_8_16_input=ch_0_7_digital_output(dig,buf)
        ch_0_7_input=ch_8_16_digital_output(dig,buf)
        for i in range(8):
            with self.subTest(i):
                self.assertEqual(all(ch_0_7_input[i]),all(ch_8_16_input[i+8]))

    def test_trig_conditions(self):
        trig_test=check_digital_trigger(1,dig, d_trig)
        for i in range(len(trig_test)):
            with self.subTest(i):
                self.assertEqual(trig_test[i],1)

    def test_cyclic_buffer(self):
        test_digital_cyclic_buffer(dig, d_trig, 1)



