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

import time
import os

results_dir = None
results_file = None
csv = None


def create_dir(timestamp):
    # Creates a new directory where all the plot files will be saved
    # Arguments:
    #   timestamp -- Time and date when the program was run

    path = os.getcwd()
    results_dir = "results" + str(timestamp)
    path = os.path.join(path, results_dir)
    os.mkdir(path)

    return results_dir


def open_files_and_dirs():
    global timestamp
    timestamp = time.strftime("_%Y-%m-%d_%H-%M-%S")  # create timestamp
    results_dir = create_dir(timestamp)  # create new directory
    results_file = open(str(results_dir) + '/results' + timestamp + '.txt',
                        'w')  # create new file where the computed values can be found
    csv = str(results_dir) + '/csv_'
    results_file.write("\nADALM2000 Libm2k Tests- Result Values \n\n")
    return results_file, results_dir, csv


results_file, results_dir, csv = open_files_and_dirs()
global timestamp
results_file_path = str(results_dir) + '/results_' + timestamp + '.txt'
