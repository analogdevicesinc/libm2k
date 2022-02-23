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
