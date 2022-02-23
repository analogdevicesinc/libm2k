import numpy as np
import math


# create multiple signal shapes and reference shapes


# signals that will be sent to output buffer
def shape_gen(n):
    # Generates different signal shapes that will be sent to the output
    # Arguments:
    #    n  -- Number of samples in the output buffer
    # Returns:
    #    shape -- list that contains arrays corresponding to each signal shape generated

    shape = [[]]
    # generate sine wave
    sine = np.sin(np.linspace(-np.pi, np.pi, n))
    # generate square wave
    square = np.append(np.linspace(-1, -1, int(n / 2)), np.linspace(1, 1, int(n / 2)))

    # generate triangle
    triangle = np.append(np.linspace(-1, 1, int(n / 2)), np.linspace(1, -1, int(n / 2)))

    # generate rising ramp
    rising_ramp = np.linspace(-1, 1, n)

    # generate falling ramp
    falling_ramp = np.linspace(1, -1, n)

    # shape and reference shape buffers
    shape = [sine, square, triangle, rising_ramp, falling_ramp]

    return shape


def ref_shape_gen(n):
    # Generates reference shapes which will be compared with the input collected through ain
    # Arguments:
    #    n -- Number of samples in the reference buffer
    # Returns:
    #    ref_shape -- list that contains arrays corresponding to each reference  shape generated

    ref_shape = [[]]
    ref_sine = np.sin(np.linspace(-np.pi, np.pi, n))
    ref_square = np.append(np.linspace(-1, -1, int(n / 2)), np.linspace(1, 1, math.ceil(n / 2)))

    ref_triangle = np.append(np.linspace(-1, 1, int(n / 2)), np.linspace(1, -1, math.ceil(n / 2)))
    ref_rising_ramp = np.linspace(-1, 1, n)
    ref_falling_ramp = np.linspace(1, -1, n)
    ref_shape = [ref_sine, ref_square, ref_triangle, ref_rising_ramp, ref_falling_ramp]
    return ref_shape


def shape_name():
    # Returns:
    #    shape_name-- Array that holds the names of the generated signal shapes in string form

    shape_name = ['Sine', 'Square', 'Triangle', 'Rising_ramp', 'Falling_ramp']
    return shape_name
