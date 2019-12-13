## Test suite for ADALM2000 based on libm2k library

The tests are launched when running [main.py](main.py).
In the current working directory is created a results directory which contains:
* HTML test report
* plots
* .csv files with data used to plot the signals
* .txt file with results used in the tests



To build the documentation is necessary to install pdoc3 python package: 

    pip3 install pdoc3
    pdoc --html ./

In order to use the testing scripts, some packages are required:
* unittest
* html-testRunner
* numpy
* matplotlib
* scipy
* pandas 
* pdoc3 

To run a single test from one of the test classes open cmd in the working directory and run:
    ```main.py TestClass.test_name```
 Ex: ```main.py A_AnalogTests.test_1_analog_objects```
        
