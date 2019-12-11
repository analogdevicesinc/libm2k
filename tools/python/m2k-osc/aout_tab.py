import numpy as np
import tkinter as tk
from tkinter import ttk
from tkinter import filedialog
import matplotlib.figure as  fg
import libm2k
import ctx_tab
import libm2k_tab
import csv

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
    def __init__(self,parent,aout,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.aout=aout
        self.outstream_ch0=False
        self.outstream_ch1=False
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

        aout_label=tk.Label(self,text="Push data to analog output channels.",font=SMALL_FONT)
        aout_label.grid(row=0,column=1,columnspan=2)
        aout_ch_label=tk.Label(self,text="Enable output channels:",anchor='w',width='20',font=SMALL_FONT)
        aout_ch_label.grid(row=2,column=0)
        self.aout_ch0_var=tk.IntVar()
        self.aout_ch1_var=tk.IntVar()
        aout_ch0_check=tk.Checkbutton(self,text="Channel 1",variable=self.aout_ch0_var,command=self.enable_ch0,font=SMALL_FONT)
        aout_ch0_check.grid(row=2,column=1)
        aout_ch1_check=tk.Checkbutton(self,text="Channel 2",variable=self.aout_ch1_var,command=self.enable_ch1,font=SMALL_FONT)
        aout_ch1_check.grid(row=2,column=2)
        aout_samplesbuff_label=tk.Label(self,text="Buffer size",anchor='w',width='20',font=SMALL_FONT)
        aout_samplesbuff_label.grid(row=17,column=0)
        self.num_of_samples_ch0=tk.StringVar()
        self.num_of_samples_ch0.set("1000")
        self.num_of_samples_entrych0=tk.Entry(self,textvariable=self.num_of_samples_ch0,width=15,font=SMALL_FONT)
        self.num_of_samples_entrych0.grid(row=17,column=1)
        self.num_of_samples_ch1=tk.StringVar()
        self.num_of_samples_ch1.set("1000")
        self.num_of_samples_entrych1=tk.Entry(self,textvariable=self.num_of_samples_ch1,width=15,font=SMALL_FONT)
        self.num_of_samples_entrych1.grid(row=17,column=2)
        num_of_samples_submit=tk.Button(self,text="Set",command=self.set_numofsamples,font=SMALL_FONT)
        num_of_samples_submit.grid(row=17,column=3)
        aout_samplerate_label=tk.Label(self,text="Sample rates",anchor='w',width=20,font=SMALL_FONT)
        aout_samplerate_label.grid(row=3,column=0)
        self.out_samplerate_ch0=tk.StringVar()
        self.out_samplerate_ch0.set(str(int(self.aout.getSampleRate(0))))
        self.out_samplerate_ch1=tk.StringVar()
        self.out_samplerate_ch1.set(str(int(self.aout.getSampleRate(1))))
        self.out_samplerate_ch0.trace('w',lambda *parags: change_samplerate_ch0(self.out_samplerate_ch0, *parags))
        out_options_ch0 = tk.OptionMenu(self,self.out_samplerate_ch0,"750","7500","75000","750000","7500000","75000000")
        out_options_ch0.grid(row=3,column=1)
        self.out_samplerate_ch1.trace('w',lambda *parags: change_samplerate_ch1(self.out_samplerate_ch1, *parags))
        out_options_ch1 = tk.OptionMenu(self,self.out_samplerate_ch1,"750","7500","75000","750000","7500000","75000000")
        out_options_ch1.grid(row=3,column=2)
        out_calibscale_label=tk.Label(self,text="Calibration Scale",anchor='w',width=20,font=SMALL_FONT)
        out_calibscale_label.grid(row=4,column=0)
        self.out_calibscale_ch0=tk.StringVar()
        self.out_calibscale_ch1=tk.StringVar()
        self.out_calibscale_ch0.set(str(self.aout.getCalibscale(0)))
        self.out_calibscale_ch1.set(str(self.aout.getCalibscale(1)))
        self.calibscale_ch0_entry=tk.Entry(self,textvariable=self.out_calibscale_ch0,width=15,font=SMALL_FONT)
        self.calibscale_ch0_entry.grid(row=4,column=1)
        self.calibscale_ch1_entry=tk.Entry(self,textvariable=self.out_calibscale_ch1,width=15,font=SMALL_FONT)
        self.calibscale_ch1_entry.grid(row=4,column=2)
        out_setcalibscale=tk.Button(self,text="Set",command=self.set_calibscale,font=SMALL_FONT)
        out_setcalibscale.grid(row=4,column=3)
        oversampling_label=tk.Label(self,text="Oversampling Ratio",anchor='w',width=20,font=SMALL_FONT)
        oversampling_label.grid(row=6,column=0)
        self.oversampling_ch0=tk.StringVar()
        self.oversampling_ch1=tk.StringVar()
        self.oversampling_ch0.set(str(self.aout.getOversamplingRatio(0)))
        self.oversampling_ch1.set(str(self.aout.getOversamplingRatio(1)))
        self.oversampling_ch0_entry=tk.Entry(self,textvariable=self.oversampling_ch0,width=15,font=SMALL_FONT)
        self.oversampling_ch0_entry.grid(row=6,column=1)
        self.oversampling_ch1_entry=tk.Entry(self,textvariable=self.oversampling_ch1,width=15,font=SMALL_FONT)
        self.oversampling_ch1_entry.grid(row=6,column=2)
        out_setoversampling=tk.Button(self,text="Set",command=self.set_oversampling,font=SMALL_FONT)
        out_setoversampling.grid(row=6,column=3)
        out_filtercomp_label=tk.Label(self,text="Filter Compensation",anchor='w',width=20,font=SMALL_FONT)
        out_filtercomp_label.grid(row=9,column=0)
        self.filtercomp_ch0=tk.StringVar()
        self.filtercomp_ch0.set(str(self.aout.getFilterCompensation(self.aout.getSampleRate(0))))
        self.filtercomp_ch1=tk.StringVar()
        self.filtercomp_ch1.set(str(self.aout.getFilterCompensation(self.aout.getSampleRate(1))))
        filtercomp_ch0label=tk.Label(self,textvariable=self.filtercomp_ch0,width=20,font=SMALL_FONT)
        filtercomp_ch0label.grid(row=9,column=1)
        filtercomp_ch1label=tk.Label(self,textvariable=self.filtercomp_ch1,width=20,font=SMALL_FONT)
        filtercomp_ch1label.grid(row=9,column=2)
        scalfact_label=tk.Label(self,text="Scaling Factor",anchor='w',width=20,font=SMALL_FONT)
        scalfact_label.grid(row=10,column=0)
        self.scalfact_ch0=tk.StringVar()
        self.scalfact_ch0.set(str(self.aout.getScalingFactor(0)))
        self.scalfact_ch1=tk.StringVar()
        self.scalfact_ch1.set(str(self.aout.getScalingFactor(1)))
        scalfact_ch0label=tk.Label(self,textvariable=self.scalfact_ch0,width=20,font=SMALL_FONT)
        scalfact_ch0label.grid(row=10,column=1)
        scalfact_ch1label=tk.Label(self,textvariable=self.scalfact_ch1,width=20,font=SMALL_FONT)
        scalfact_ch1label.grid(row=10,column=2)
        scalfact_get=tk.Button(self,text="Get",command=self.scal_fact,font=SMALL_FONT)
        scalfact_get.grid(row=10,column=3)
        out_cyclic_label=tk.Label(self,text="Enable cyclic mode",anchor='w',width=20,font=SMALL_FONT)
        out_cyclic_label.grid(row=12,column=0)
        self.cyclic_ch0_var=tk.IntVar()
        self.cyclic_ch1_var=tk.IntVar()
        cyclic_ch0_check=tk.Checkbutton(self,text="Set Cyclic",variable=self.cyclic_ch0_var,command=self.set_cyclic,font=SMALL_FONT)
        cyclic_ch0_check.grid(row=12,column=1)
        cyclic_ch1_check=tk.Checkbutton(self,text="Set Cyclic",variable=self.cyclic_ch1_var,command=self.set_cyclic,font=SMALL_FONT)
        cyclic_ch1_check.grid(row=12,column=2)
        waveform_label=tk.Label(self,text="Select waveform",anchor='w',width=20,font=SMALL_FONT)
        waveform_label.grid(row=13,column=0)
        self.func_ch0 = tk.StringVar()
        self.func_ch0.set("Sine Wave")
        self.func_ch0.trace('w',lambda *parags: change_waveform_ch0(self.func_ch0, *parags))
        options_ch0 = tk.OptionMenu(self,self.func_ch0,"Sine Wave","Square Wave","Triangular Wave","Custom CSV")
        options_ch0.grid(row=13,column=1)
        self.func_ch1 = tk.StringVar()
        self.func_ch1.set("Sine Wave")
        self.func_ch1.trace('w',lambda *parags: change_waveform_ch1(self.func_ch1, *parags))
        options_ch1 = tk.OptionMenu(self,self.func_ch1,"Sine Wave","Square Wave","Triangular Wave","Custom CSV")
        options_ch1.grid(row=13,column=2)
        self.amin_ch0= tk.StringVar()
        self.amin_ch0.set("0")
        self.amax_ch0 = tk.StringVar()
        self.amax_ch0.set("1")
        self.amin_ch1= tk.StringVar()
        self.amin_ch1.set("0")
        self.amax_ch1 = tk.StringVar()
        self.amax_ch1.set("1")
        min_ampl_label = tk.Label(self,text= "Min. amplitude:",anchor='w',width=20,font=SMALL_FONT)
        min_ampl_label.grid(row=14,column=0)
        self.min_ampl_entry_ch0 = tk.Entry(self,textvariable=self.amin_ch0,width=15,font=SMALL_FONT)
        self.min_ampl_entry_ch0.grid(row=14,column=1)
        self.min_ampl_entry_ch1 = tk.Entry(self,textvariable=self.amin_ch1,width=15,font=SMALL_FONT)
        self.min_ampl_entry_ch1.grid(row=14,column=2)
        max_ampl_label = tk.Label(self,text="Max. amplitude",anchor='w',width=20,font=SMALL_FONT)
        max_ampl_label.grid(row=15,column=0)
        self.max_ampl_entry_ch0 = tk.Entry(self,textvariable=self.amax_ch0,width=15,font=SMALL_FONT)
        self.max_ampl_entry_ch0.grid(row=15,column=1)
        self.max_ampl_entry_ch1 = tk.Entry(self,textvariable=self.amax_ch1,width=15,font=SMALL_FONT)
        self.max_ampl_entry_ch1.grid(row=15,column=2)
        change_amplitude = tk.Button(self,text="Set",command=self.change_ampl,font=SMALL_FONT)
        change_amplitude.grid(row=14,column=3,rowspan=2)
        push_label=tk.Label(self,text="Push Buffer",anchor='w',width=20,font=SMALL_FONT) 
        push_label.grid(row=19,column=0)
        ch0_push=tk.Button(self,text="Push Ch1",command=self.push_ch0,font=SMALL_FONT)
        ch0_push.grid(row=19,column=1)
        ch1_push=tk.Button(self,text="Push Ch2",command=self.push_ch1,font=SMALL_FONT)
        ch1_push.grid(row=19,column=2)
        stop_label=tk.Label(self,text="Stop Pushing",anchor='w',width=20,font=SMALL_FONT)
        stop_label.grid(row=20,column=0)
        ch0_stop=tk.Button(self,text="Stop Ch1",command=self.stop_ch0,font=SMALL_FONT)
        ch0_stop.grid(row=20,column=1)
        ch1_stop=tk.Button(self,text="Stop Ch2",command=self.stop_ch1,font=SMALL_FONT)
        ch1_stop.grid(row=20,column=2)

    def change_ampl(self):
        self.amax_ch0.set(self.max_ampl_entry_ch0.get())
        self.amax_ch1.set(self.max_ampl_entry_ch1.get())
        self.amin_ch0.set(self.min_ampl_entry_ch0.get())
        self.amin_ch1.set(self.min_ampl_entry_ch1.get())

    def change_sampleratech0(self):
        aout=self.aout
        desired_samplerate=float(self.out_samplerate_ch0.get())
        aout.setSampleRate(0,desired_samplerate)
        self.filtercomp_ch0.set(str(aout.getFilterCompensation(aout.getSampleRate(0))))

    def change_sampleratech1(self):
        aout=self.aout
        desired_samplerate=float(self.out_samplerate_ch1.get())
        aout.setSampleRate(1,desired_samplerate)
        self.filtercomp_ch1.set(str(aout.getFilterCompensation(aout.getSampleRate(1))))

    def enable_ch0(self):
        aout=self.aout
        if self.aout_ch0_var.get()==1:
            aout.enableChannel(0,True)
        else:
            aout.enableChannel(0,False)

    def enable_ch1(self):
        aout=self.aout
        if self.aout_ch1_var.get()==1:
            aout.enableChannel(1,True)
        else:
            aout.enableChannel(1,False)

    def outputch0_function(self):
        aout=self.aout
        #define displayed waveform
        if self.func_ch0.get()=='Sine Wave':
            def_buffer=gen_sine(float(aout.getSampleRate(0)),int(self.num_of_samples_ch0.get()),float(self.amin_ch0.get()),float(self.amax_ch0.get()))
        elif self.func_ch0.get()=='Square Wave':
            def_buffer=gen_square(int(self.num_of_samples_ch0.get()),float(self.amin_ch0.get()),float(self.amax_ch0.get()))
        elif self.func_ch0.get()=='Triangular Wave':
            def_buffer=gen_triangle(int(self.num_of_samples_ch0.get()),float(self.amin_ch0.get()),float(self.amax_ch0.get()))
        else:
            def_buffer=self.read_csv(0)
        if self.outstream_ch0 is True:
            #when the analog output is enabled, data buffer is pushed
            aout.push(0,def_buffer)

    def outputch1_function(self):
        aout=self.aout
        #define displayed waveform
        if self.func_ch1.get()=='Sine Wave':
            def_buffer=gen_sine(float(aout.getSampleRate(1)),int(self.num_of_samples_ch1.get()),float(self.amin_ch1.get()),float(self.amax_ch1.get()))
        elif self.func_ch1.get()=='Square Wave':
            def_buffer=gen_square(int(self.num_of_samples_ch1.get()),float(self.amin_ch1.get()),float(self.amax_ch1.get()))
        elif self.func_ch1.get()=='Triangular Wave':
            def_buffer=gen_triangle(int(self.num_of_samples_ch1.get()),float(self.amin_ch1.get()),float(self.amax_ch1.get()))
        else:
            def_buffer=self.read_csv(0)
        if self.outstream_ch1 is True:
            #when the analog output is enabled, data buffer is pushed
            aout.push(1,def_buffer)

    def push_ch0(self):
        self.outstream_ch0=True
        self.outputch0_function()

    def push_ch1(self):
        self.outstream_ch1=True
        self.outputch1_function()

    def read_csv(self,column):
        num_data=[]
        self.filename=filedialog.askopenfilename(title="Select file")
        f= open(self.filename,"rt")
        reader=csv.reader(f,delimiter=",")
        for row in reader:
            entry=row[column]
            try:
                num_data.append(float(entry))
            except ValueError:
                print("Cannot convert {} to a float ... skipping".format(entry))
        return num_data

    def scal_fact(self):
        self.scalfact_ch0.set(str(self.aout.getScalingFactor(0)))
        self.scalfact_ch1.set(str(self.aout.getScalingFactor(1)))

    def set_cyclic(self):
        aout=self.aout
        if self.cyclic_ch0_var.get()==1:
            aout.setCyclic(0,True)
        else:
            aout.setCyclic(0,False)
        if self.cyclic_ch1_var.get()==1:
            aout.setCyclic(1,True)
        else:
            aout.setCyclic(1,False)

    def set_calibscale(self):
        aout=self.aout
        if self.calibscale_ch0_entry.get() is not None:
            aout.setCalibscale(0,float(self.calibscale_ch0_entry.get()))
        if self.calibscale_ch1_entry.get() is not None:
            aout.setCalibscale(1,float(self.calibscale_ch1_entry.get()))

    def set_numofsamples(self):
        self.num_of_samples_ch0.set(self.num_of_samples_entrych0.get())
        self.num_of_samples_ch1.set(self.num_of_samples_entrych1.get())

    def set_oversampling(self):
        aout=self.aout
        aout.setOversamplingRatio(0,float(self.oversampling_ch0.get()))
        aout.setOversamplingRatio(1,float(self.oversampling_ch1.get()))

    def stop_ch0(self):
        self.outstream_ch0=False
        self.aout.stop(0)

    def stop_ch1(self):
        self.outstream_ch1=False
        self.aout.stop(1)
