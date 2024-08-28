
import numpy as np
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


def plot_to_file(title, data, dir_name, filename, xlabel=None, x_lim = None, ylabel=None, y_lim = None, data1=None, x_data = None, data_marked=None):
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
    # if xlabel and ylabel are not specified there will be default values
    if xlabel is not None:  
        plt.xlabel(xlabel)
    else:
        plt.xlabel('Samples')
    if ylabel is not None:
        plt.ylabel(ylabel)
    else:
        plt.ylabel('Voltage [V]')
    plt.grid(visible=True)
    # if x_data is not None, the plot will be displayed with the specified x_data
    if x_data is not None:
        plt.plot(x_data, data)
    else:
        plt.plot(data)
    # if a second set of data must be printed (for ch0 and ch1 phase difference in this case)
    if data1 is not None:
        if x_data is not None:
            plt.plot(x_data, data1)
        else:
            plt.plot(data1)
    # Optional configurations
    if x_lim is not None:
        plt.xlim(*x_lim)
    if y_lim is not None:
        plt.ylim(*y_lim)
    if data_marked is not None:
        plt.plot(data_marked, data[data_marked], 'xr')        
    plt.savefig(dir_name + "/" + filename)
    plt.close()
    return

def plot_to_file_multiline(
    title,
    datasets,
    dir_name,
    filename,
    xlabel="Samples", ylabel="Voltage [V]",
    xlim = None,  ylim = None,
):
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.grid(visible=True)

    for data in datasets:
        xdata, ydata, fmt = data
        if xdata is not None:
            if "marker" in fmt:
                # Mark scattered points
                plt.plot(xdata, ydata[xdata], linestyle="None", **fmt)
            else:
                plt.plot(xdata, ydata, **fmt)
        else:
            plt.plot(ydata, **fmt)
    
    if xlim is not None:
        plt.xlim(*xlim)
    if ylim is not None:
        plt.ylim(*ylim)
    plt.legend()

    plt.savefig(f"{dir_name}/{filename}")
    plt.close()
    return


def get_time_format(samples, sample_rate):
    x_time = np.linspace(0, samples/sample_rate, samples)

    if x_time[-1] < 1e-6:
        x_time *= 1e9
        x_label = "Time [ns]"
    elif x_time[-1] < 1e-3:
        x_time *= 1e6
        x_label = "Time [us]"
    elif x_time[-1] < 1:
        x_time *= 1e3
        x_label = "Time [ms]"
    else:
        x_label = "Time [s]"
    return x_time, x_label


def get_sample_rate_display_format(sample_rate):
    if sample_rate < 1e3:
        return f"{sample_rate:.2f} Hz"
    if sample_rate < 1e6:
        return f"{sample_rate/1e3:.2f} KHz"
    if sample_rate < 1e9:
        return f"{sample_rate/1e6:.2f} MHz"
    return f"{sample_rate/1e9:.2f} GHz"
