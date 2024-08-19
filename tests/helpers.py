
from pandas import DataFrame
import matplotlib.pyplot as plt



def get_result_files(gen_reports):
    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []
        dir_name = []
        csv_path = []

    return file, dir_name, csv_path


def save_data_to_csv(csv_vals, csv_file):
    df = DataFrame(csv_vals)
    df.to_csv(csv_file)
    return


def plot_to_file(title, data, dir_name, filename, xlabel=None, ylabel=None, data1=None, data_marked=None):
    # Saves the plots in a separate folder
    # Arguments:
    #    title  -- Title of the plot
    #    data  -- Data to be plotted
    #    filename  -- Name of the file with the plot
    # Keyword Arguments:
    #    xlabel  -- Label of x-Axis (default: {None})
    #    ylabel  -- Label of y-Axis(default: {None})
    #    data1  --  Data that should be plotted on the same plot(default: {None})
    #    data_marked -- Data that represents specific points on the plot(default: {None})
    # plot the signals in a separate folder
    plt.title(title)
    if xlabel is not None:  # if xlabel and ylabel are not specified there will be default values
        plt.xlabel(xlabel)
    else:
        plt.xlabel('Samples')
    if ylabel is not None:
        plt.ylabel(ylabel)
    else:
        plt.ylabel('Voltage [V]')
    plt.grid(visible=True)
    plt.plot(data)  # if a second set of data must be printed (for ch0 and ch1 phase difference in this case)
    if data1 is not None:
        plt.plot(data1)
    if data_marked is not None:
        plt.plot(data_marked, data[data_marked], 'xr')        
    plt.savefig(dir_name + "/" + filename)
    plt.close()
    return