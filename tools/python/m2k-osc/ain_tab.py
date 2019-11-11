import matplotlib
import numpy as np
import tkinter as tk
from tkinter import ttk
import matplotlib.figure as  fg
import libm2k
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from matplotlib import style
import threading
import time
import ctx_tab
import aout_tab
LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)

samples_queuech0=[]
samples_queuech1=[]
fig=fg.Figure(figsize=(8,6))
fig_subplot=fig.add_subplot(111)

def get_samples():
    global read_samples
    while 1:
        global samples_queuech0,samples_queuech1
        samples_queuech0.append(ain.getSamples(int(read_samples.get()))[0])
        samples_queuech1.append(ain.getSamples(int(read_samples.get()))[1])

class ain_tab_frame(tk.Frame):
    def __init__(self,parent,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.create_widgets()
        self.create_canvas()

    def create_widgets(self):
        def change_samplerate(in_samplerate,*parags):
            self.change_sample_rate()
        ain_label=tk.Label(self,text="Observe analog input channels.")
        ain_label.grid(row=0,column=0)
        ain_ch_label=tk.Label(self,text="Enable input channels:")
        ain_ch_label.grid(row=1,column=1)
        ain_kernelbuff_label=tk.Label(self,text="Set Kernel Buffers Count")
        ain_kernelbuff_label.grid(row=1,column=2)
        global kernelbuff_var
        kernelbuff_var=tk.StringVar()
        self.kernelbuff_entry=tk.Entry(self)#,textvariable=kernelbuff_var)
        self.kernelbuff_entry.grid(row=2,column=2)
        kernelbuff_submit=tk.Button(self,text="OK",command=self.set_kernelbuff)
        kernelbuff_submit.grid(row=2,column=3)
        global ain_ch0_var,ain_ch1_var
        ain_ch0_var=tk.IntVar()
        ain_ch1_var=tk.IntVar()
        ain_ch0_check=tk.Checkbutton(self,text="Channel 1",variable=ain_ch0_var,command=self.enable_ch0)
        ain_ch0_check.grid(row=2,column=1)
        ain_ch1_check=tk.Checkbutton(self,text="Channel 2",variable=ain_ch1_var,command=self.enable_ch1)
        ain_ch1_check.grid(row=3,column=1)
        ain_samplerate_label=tk.Label(self,text="Sample rates")
        ain_samplerate_label.grid(row=4,column=0)
        global in_samplerate
        in_samplerate=tk.StringVar()
        in_samplerate.trace('w',lambda *parags: change_samplerate(in_samplerate, *parags))
        samplerate_options = tk.OptionMenu(self, in_samplerate,"1000","10000","100000","1000000","10000000","100000000")
        samplerate_options.grid(row=4,column=1)
        ain_samples_label=tk.Label(self,text="Samples to read")
        ain_samples_label.grid(row=5,column=0)
        global read_samples
        read_samples=tk.StringVar()
        read_samples.set("1000")
        self.ain_samples_entry=tk.Entry(self,textvariable=read_samples)
        self.ain_samples_entry.grid(row=5,column=1)
        ain_samples_submit=tk.Button(self,text="OK",command=self.update_readsamples)
        ain_samples_submit.grid(row=5,column=2)
        ain_ch0_label = tk.Label(self,text="Set-up for Channel 1")
        ain_ch0_label.grid(row=6,column=1)
        ain_ch1_label=tk.Label(self,text="Set-up for Channel 2")
        ain_ch1_label.grid(row=6,column=2)
        ain_range_label=tk.Label(self,text="Range")
        ain_range_label.grid(row=7,column=0)
        global ain_range_ch0_var,ain_range_ch1_var
        ain_range_ch0_var= tk.IntVar()
        ain_range_ch0_check=tk.Checkbutton(self,text ="Set High Range",variable=ain_range_ch0_var,command=self.set_range_ch0)
        ain_range_ch0_check.grid(row=7,column=1)
        ain_range_ch1_var= tk.IntVar()
        ain_range_ch1_check=tk.Checkbutton(self,text ="Set High Range",variable=ain_range_ch1_var,command=self.set_range_ch1)
        ain_range_ch1_check.grid(row=7,column=2)
        ain_rangelimits_label=tk.Label(self,text="Range Limits")
        ain_rangelimits_label.grid(row=8,column=0)
        global rangelimits_ch0,rangelimits_ch1
        rangelimits_ch0=tk.StringVar()
        rangelimits_ch1=tk.StringVar()
        ain_rangelimits_ch0=tk.Label(self,textvariable=rangelimits_ch0)
        ain_rangelimits_ch0.grid(row=8,column=1)
        ain_rangelimits_ch1=tk.Label(self,textvariable=rangelimits_ch1)
        ain_rangelimits_ch1.grid(row=8,column=2)
        ain_startch0=tk.Button(self,text="Read",command=self.start_in)
        ain_startch0.grid(row=9,column=1)
        ain_stopch0=tk.Button(self,text="Stop",command=self.stop_in)
        ain_stopch0.grid(row=9,column=2)

    def change_sample_rate(self):
        desired_samplerate=float(in_samplerate.get())
        current_samplerate=ain.getSampleRate()
        ain.setOversamplingRatio(int(current_samplerate/desired_samplerate))
        print(ain.getOversamplingRatio())
    def create_canvas(self):
        x=[1,2,3]
        y_ch0=[5,4,7]
        y_ch1=[8,6,5]
        self.linech0,=fig_subplot.plot(x,y_ch0)
        self.linech1,=fig_subplot.plot(x,y_ch1)
        self.canvas=FigureCanvasTkAgg(fig,master=self)
        self.canvas.draw()
        self.canvas.get_tk_widget().grid(row=11,column=0,columnspan=4)
    def enable_ch0(self):
        self.get_ain()
        if ain_ch0_var.get() ==1:
            ain.enableChannel(0,True)
            print(ain.getRange(0))
        else:
            ain.enableChannel(0,False)
    def enable_ch1(self):
        self.get_ain()
        if ain_ch1_var.get() ==1:
            ain.enableChannel(1,True)
        else:
            ain.enableChannel(1,False)
    def get_ain(self):
        global ain
        if ctx_tab.ain is not None:
            ain=ctx_tab.ain
            self.start_thread()
    def plot(self):
        global instream,samples_queuech0,samples_queuech1
        y_ch0=[]
        y_ch1=[]
        if samples_queuech0:
           y_ch0=samples_queuech0.pop()
        if samples_queuech1:
            y_ch1=samples_queuech1.pop()
        x=range(len(y_ch0))
        self.linech0.set_data(x,y_ch0)
        self.linech1.set_data(x,y_ch1)
        ax=self.canvas.figure.axes[0]
        if float(aout_tab.amax_ch0.get()) >= float(aout_tab.amax_ch1.get()):
            y_max=float(aout_tab.amax_ch0.get())+1
        else:
            y_max=float(aout_tab.amax_ch1.get())+1
        if float(aout_tab.amin_ch0.get())>=float(aout_tab.amin_ch1.get()):
            y_min=float(aout_tab.amin_ch1.get())-1
        else:
            y_min=float(aout_tab.amin_ch0.get())-1

        ax.set_xlim(0,len(x))
        ax.set_ylim(y_min,y_max)
        if instream is True:
            ax.set_xlabel("Sample Count")
            ax.set_ylabel("Gathered Data")
        else:
            ax.set_xlabel("Previous Sample Count")
            ax.set_ylabel("Previous Gathered Data")
        self.canvas.draw()
    def set_kernelbuff(self):
        count=int(self.kernelbuff_entry.get())
        ain.setKernelBuffersCount(count)
    def set_range_ch0(self):
        if ain.isChannelEnabled(0) is True:
            if ain_range_ch0_var.get()==1:
                ain.setRange(0,1)
            else:
                ain.setRange(0,0)
            rangelimits_ch0.set(str(ain.getRangeLimits(ain.getRange(0))))
    def set_range_ch1(self):
        if ain.isChannelEnabled(1) is True:
            if ain_range_ch1_var.get()==1:
                ain.setRange(1,1)
            else:
                ain.setRange(1,0)
            rangelimits_ch1.set(str(ain.getRangeLimits(ain.getRange(1))))
    def start_in(self):
        global instream
        instream=True
        self.update_plot()
    def start_thread(self):
        get_samples_thread=threading.Thread(None,get_samples,"get_samples")
        get_samples_thread.setDaemon(True)
        get_samples_thread.start()
    def stop_in(self):
        global instream
        instream=False
    def update_plot(self):
        self.plot()
        if instream is True:
            self.after(10,self.update_plot)
    def update_readsamples(self):
        global read_samples
        read_samples.set(self.ain_samples_entry.get())
        print(read_samples.get())
        print(self.ain_samples_entry.get())


