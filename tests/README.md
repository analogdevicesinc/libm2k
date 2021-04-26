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
    ```python main.py noreports```
 Ex: ```main.py A_AnalogTests.test_1_analog_objects nofiles```
