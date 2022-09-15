## Test suite for ADALM2000 based on libm2k library

The tests are launched when running [main.py](main.py).
In the current working directory, a results directory is created. This contains:
* HTML test report
* plots
* .csv files with data used to plot the signals
* .txt file with results used in the tests


Before running the test suite, make sure all required Python packages are installed: 
```pip install -r requirements.txt```

To run a single test from one of the test classes open cmd in the working directory and run:
    ```main.py TestClass.test_name```
 Ex: ```main.py A_AnalogTests.test_1_analog_objects```


To run tests without generating the report files, add the "nofiles" option when running main.py .
    ```python main.py nofiles```
 Ex: ```main.py A_AnalogTests.test_1_analog_objects nofiles```


To run the test suite for the ADALM 2000 emulator, first make sure to have installed and running the 
emulator aplication [iio-emu](https://github.com/analogdevicesinc/iio-emu).
After that run from [main.py](main.py) the emulator tests with a specific local uri. 
Ex: ```main.py --uri ip:127.0.0.1 E_EmulatorTests```

To repeat a test, add the @repeat(times) decorator before the test to be repeated.
 
 Ex: <br>
```    @repeat(50)``` <br>
```    def test_2_calibration(self):```