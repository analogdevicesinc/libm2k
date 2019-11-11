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
global outstream_ch0,outstream_ch1
outstream_ch0=False
outstream_ch1=False


LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)
def gen_sine(sample_rate,num_of_samples,a_min,a_max):
    A = (a_max-a_min)/2
    C = (a_min+a_max)/2
    t = np.linspace(-np.pi,np.pi,num_of_samples)
    x = (C+A)*np.sin(t)
    return x

#create buffer for square waveform
def gen_square(num_of_samples,a_min,a_max):
    x=[]
    for i in range(int(num_of_samples/2)):
        x.append(float(a_min))
    for i in range(int(num_of_samples/2)):
        x.append(float(a_max))
    return x

#create buffer for triangle waveform
def gen_triangle(num_of_samples,a_min,a_max):
    x=np.abs(np.linspace(a_min,a_max,num_of_samples))
    return x



class aout_tab_frame(tk.Frame):
    def __init__(self,parent,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.create_widgets()
    def __del__(self):
        print("DESTROYED");
    def create_widgets(self):
        def change_samplerate_ch0(out_samplerate_ch0,*parags):
            self.change_sampleratech0()
        def change_samplerate_ch1(out_samplerate_ch1,*parags):
            self.change_sampleratech1()
        def change_waveform_ch0(func_ch0,*parags):
            self.outputch0_function()
        def change_waveform_ch1(func_ch1,*parags):
            self.outputch1_function()
        aout_label=tk.Label(self,text="Push data to analog output channels.")
        aout_label.grid(row=0,column=0)
        aout_ch_label=tk.Label(self,text="Enable output channels:")
        aout_ch_label.grid(row=1,column=1)
        global aout_ch0_var,aout_ch1_var
        aout_ch0_var=tk.IntVar()
        aout_ch1_var=tk.IntVar()
        aout_ch0_check=tk.Checkbutton(self,text="Channel 1",variable=aout_ch0_var,command=self.enable_ch0)
        aout_ch0_check.grid(row=2,column=1)
        aout_ch1_check=tk.Checkbutton(self,text="Channel 2",variable=aout_ch1_var,command=self.enable_ch1)
        aout_ch1_check.grid(row=3,column=1)
        aout_samplesbuff_label=tk.Label(self,text="Sample count for data buffer")
        aout_samplesbuff_label.grid(row=1,column=2)
        global num_of_samples
        num_of_samples=tk.StringVar()
        num_of_samples.set("1000")
        self.num_of_samples_entry=tk.Entry(self)
        self.num_of_samples_entry.grid(row=2,column=2)
        num_of_samples_submit=tk.Button(self,text="OK",command=self.set_numofsamples)
        num_of_samples_submit.grid(row=2,column=3)
        aout_samplerate_label=tk.Label(self,text="Sample rates")
        aout_samplerate_label.grid(row=5,column=0)
        aout_ch0_label = tk.Label(self,text="Set-up for Channel 1")
        aout_ch0_label.grid(row=4,column=1)
        aout_ch1_label=tk.Label(self,text="Set-up for Channel 2")
        aout_ch1_label.grid(row=4,column=2)
        global out_samplerate_ch0,out_samplerate_ch1
        out_samplerate_ch0=tk.StringVar()
        out_samplerate_ch1=tk.StringVar()
        out_samplerate_ch0.trace('w',lambda *parags: change_samplerate_ch0(out_samplerate_ch0, *parags))
        out_options_ch0 = tk.OptionMenu(self,out_samplerate_ch0,"750","7500","75000","750000","7500000","75000000")
        out_options_ch0.grid(row=5,column=1)
        out_samplerate_ch1.trace('w',lambda *parags: change_samplerate_ch1(out_samplerate_ch1, *parags))
        out_options_ch1 = tk.OptionMenu(self,out_samplerate_ch1,"750","7500","75000","750000","7500000","75000000")
        out_options_ch1.grid(row=5,column=2)
        out_cyclic_label=tk.Label(self,text="Enable cyclic mode")
        out_cyclic_label.grid(row=6,column=0)
        global cyclic_ch0_var,cyclic_ch1_var
        cyclic_ch0_var=tk.IntVar()
        cyclic_ch1_var=tk.IntVar()
        cyclic_ch0_check=tk.Checkbutton(self,text="Set Cyclic",variable=cyclic_ch0_var,command=self.set_cyclic)
        cyclic_ch0_check.grid(row=6,column=1)
        cyclic_ch1_check=tk.Checkbutton(self,text="Set Cyclic",variable=cyclic_ch1_var,command=self.set_cyclic)
        cyclic_ch1_check.grid(row=6,column=2)
        waveform_label=tk.Label(self,text="Select waveform")
        waveform_label.grid(row=7,column=0)
        global func_ch0,func_ch1
        func_ch0 = tk.StringVar()
        func_ch0.trace('w',lambda *parags: change_waveform_ch0(func_ch0, *parags))
        options_ch0 = tk.OptionMenu(self,func_ch0,"Sine Wave","Square Wave","Triangular Wave")
        options_ch0.grid(row=7,column=1)
        func_ch1 = tk.StringVar()
        func_ch1.trace('w',lambda *parags: change_waveform_ch1(func_ch1, *parags))
        options_ch1 = tk.OptionMenu(self,func_ch1,"Sine Wave","Square Wave","Triangular Wave")
        options_ch1.grid(row=7,column=2)
        global amin_ch0,amin_ch1, amax_ch1,amax_ch0
        amin_ch0= tk.StringVar()
        amin_ch0.set("0")
        amax_ch0 = tk.StringVar()
        amax_ch0.set("1")
        amin_ch1= tk.StringVar()
        amin_ch1.set("0")
        amax_ch1 = tk.StringVar()
        amax_ch1.set("1")
        min_ampl_label = tk.Label(self,text= "Min. amplitude:")
        min_ampl_label.grid(row=8,column=0)
        self.min_ampl_entry_ch0 = tk.Entry(self)
        self.min_ampl_entry_ch0.grid(row=8,column=1)
        self.min_ampl_entry_ch1 = tk.Entry(self)
        self.min_ampl_entry_ch1.grid(row=8,column=2)
        max_ampl_label = tk.Label(self,text="Max. amplitude")
        max_ampl_label.grid(row=9,column=0)
        self.max_ampl_entry_ch0 = tk.Entry(self)
        self.max_ampl_entry_ch0.grid(row=9,column=1)
        self.max_ampl_entry_ch1 = tk.Entry(self)
        self.max_ampl_entry_ch1.grid(row=9,column=2)
        change_amplitude_min = tk.Button(self,text="OK",command=self.change_ampl_min)
        change_amplitude_min.grid(row=8,column=3)
        change_amplitude_max = tk.Button(self,text="OK",command=self.change_ampl_max)
        change_amplitude_max.grid(row=9,column=3)
        push_label=tk.Label(self,text="Push Data to channels") 
        push_label.grid(row=10,column=0)
        ch0_push=tk.Button(self,text="Push Ch1",command=self.push_ch0)
        ch0_push.grid(row=10,column=1)
        ch1_push=tk.Button(self,text="Push Ch2",command=self.push_ch1)
        ch1_push.grid(row=10,column=2)
        stop_label=tk.Label(self,text="Stop Pushing") 
        stop_label.grid(row=11,column=0)
        ch0_stop=tk.Button(self,text="Stop Ch1",command=self.stop_ch0)
        ch0_stop.grid(row=11,column=1)
        ch1_stop=tk.Button(self,text="Stop Ch2",command=self.stop_ch1)
        ch1_stop.grid(row=11,column=2)


    def change_ampl_max(self):
        amax_ch0.set(self.max_ampl_entry_ch0.get())
        amax_ch1.set(self.max_ampl_entry_ch1.get())
    def change_ampl_min(self):
        amin_ch0.set(self.min_ampl_entry_ch0.get())
        amin_ch1.set(self.min_ampl_entry_ch1.get())
    def change_sampleratech0(self):
        desired_samplerate=float(out_samplerate_ch0.get())
        current_samplerate=aout.getSampleRate(0)
        aout.setOversamplingRatio(0,int(current_samplerate/desired_samplerate))
    def change_sampleratech1(self):
        desired_samplerate=float(out_samplerate_ch1.get())
        current_samplerate=aout.getSampleRate(1)
        aout.setOversamplingRatio(1,int(current_samplerate/desired_samplerate))

    def enable_ch0(self):
        self.get_aout()
        if aout_ch0_var.get() ==1:
            aout.enableChannel(0,True)
        else:
            aout.enableChannel(0,False)
    def enable_ch1(self):
        self.get_aout()
        if aout_ch1_var.get() ==1:
            aout.enableChannel(1,True)
        else:
            aout.enableChannel(1,False)
    def get_aout(self):
        global aout
        if ctx_tab.aout is not None:
            aout=ctx_tab.aout
    def outputch0_function(self):
        global outstream_ch0
        #define displayed waveform
        if func_ch0.get() == 'Sine Wave':
            def_buffer = gen_sine(float(aout.getOversamplingRatio(0)),int(num_of_samples.get()),float(amin_ch0.get()),float(amax_ch0.get()))
        elif func_ch0.get() == 'Square Wave':
            def_buffer = gen_square(int(num_of_samples.get()),float(amin_ch0.get()),float(amax_ch0.get()))
        else:
            def_buffer = gen_triangle(int(num_of_samples.get()),float(amin_ch0.get()),float(amax_ch0.get()))

        if outstream_ch0 is True:
            #when the analog output is enabled, data buffer is pushed
            aout.push(0,def_buffer)
    def outputch1_function(self):
        global outstream_ch1
        #define displayed waveform
        if func_ch1.get() == 'Sine Wave':
            def_buffer = gen_sine(float(aout.getOversamplingRatio(1)),int(num_of_samples.get()),float(amin_ch1.get()),float(amax_ch1.get()))
        elif func_ch1.get() == 'Square Wave':
            def_buffer = gen_square(int(num_of_samples.get()),float(amin_ch1.get()),float(amax_ch1.get()))
        else:
            def_buffer = gen_triangle(int(num_of_samples.get()),float(amin_ch1.get()),float(amax_ch1.get()))

        if outstream_ch1 is True:
            #when the analog output is enabled, data buffer is pushed
            aout.push(1,def_buffer)
    def push_ch0(self):
        global outstream_ch0
        outstream_ch0=True
        self.outputch0_function()
    def push_ch1(self):
        global outstream_ch1
        outstream_ch1=True
        self.outputch1_function()

    def set_cyclic(self):
        if cyclic_ch0_var.get()==1:
            aout.setCyclic(0,True)
        else:
            aout.setCyclic(0,False)
        if cyclic_ch1_var.get()==1:
            aout.setCyclic(1,True)
        else:
            aout.setCyclic(1,False)
    def set_numofsamples(self):
        global num_of_samples
        num_of_samples.set(self.num_of_samples_entry.get())
    def stop_ch0(self):
        global outstream_ch0
        outstream_ch0=False
        aout.stop(0)
    def stop_ch1(self):
        global outstream_ch1
        outstream_ch1=False
        aout.stop(1)


 



