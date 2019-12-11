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
import libm2k_tab
LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)

samples_queuech0=[]
samples_queuech1=[]
fig=fg.Figure(figsize=(8,6))
fig_subplot=fig.add_subplot(111)

class ain_tab_frame(tk.Frame):
    def __init__(self,parent,ain,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.ain=ain
        self.create_widgets()


    def create_widgets(self):
        def change_samplerate(in_samplerate,*parags):
            self.change_sample_rate()
        ain_ch1_label=tk.Label(self,text="Set-up channel 1",font=SMALL_FONT)
        ain_ch2_label=tk.Label(self,text="Set-up channel 2",font=SMALL_FONT)
        ain_ch1_label.grid(row=7,column=1)
        ain_ch2_label.grid(row=7,column=2)
        ain_kernelbuff_label=tk.Label(self,text="Set Kernel Buffers Count",anchor='w',width=20,font=SMALL_FONT)
        ain_kernelbuff_label.grid(row=1,column=0)
        self.kernelbuff_var=tk.StringVar()
        self.kernelbuff_entry=tk.Entry(self,width=15)
        self.kernelbuff_entry.grid(row=1,column=1)
        kernelbuff_submit=tk.Button(self,text="Set",width=5,command=self.set_kernelbuff,font=SMALL_FONT)
        kernelbuff_submit.grid(row=1,column=3)
        ain_samplerate_label=tk.Label(self,text="Sample rates",anchor='w',width='20',font=SMALL_FONT)
        ain_samplerate_label.grid(row=3,column=0)
        self.in_samplerate=tk.StringVar()
        self.in_samplerate.set(str(int(self.ain.getSampleRate())))
        self.in_samplerate.trace('w',lambda *parags: change_samplerate(self.in_samplerate, *parags))
        self.samplerate_options = tk.OptionMenu(self,self.in_samplerate,"1000","10000","100000","1000000","10000000","100000000")
        self.samplerate_options.grid(row=3,column=1)
        ain_oversampling_label=tk.Label(self,text="Oversampling Ratio",anchor='w',width=20,font=SMALL_FONT)
        ain_oversampling_label.grid(row=4,column=0)
        self.oversampling_ratio=tk.StringVar()
        self.oversampling_ratio.set(str(self.ain.getOversamplingRatio()))
        self.ain_oversampling_entry=tk.Entry(self,width=15,textvariable=self.oversampling_ratio)
        self.ain_oversampling_entry.grid(row=4,column=1)
        ain_oversampling_set=tk.Button(self,text="Set",width=5,command=self.set_oversampling,font=SMALL_FONT)
        ain_oversampling_set.grid(row=4,column=3)
        ain_filtercomp_label=tk.Label(self,text="Filter Compensation",justify="left",anchor='w',width=20,font=SMALL_FONT)
        ain_filtercomp_label.grid(row=6,column=0)
        self.filter_comp=tk.StringVar()
        self.filter_comp.set(str(self.ain.getFilterCompensation(self.ain.getSampleRate())))
        ain_filtercomp=tk.Label(self,textvariable=self.filter_comp,justify="left",font=SMALL_FONT)
        ain_filtercomp.grid(row=6,column=1)
        self.calibscale_ch0=tk.StringVar()
        self.calibscale_ch0.set(str(self.ain.getCalibscale(0)))
        self.calibscale_ch1=tk.StringVar()
        self.calibscale_ch1.set(str(self.ain.getCalibscale(1)))
        self.vertoffset_ch0=tk.StringVar()
        self.vertoffset_ch1=tk.StringVar()
        self.hystrange_ch0=tk.StringVar()
        self.hystrange_ch1=tk.StringVar()
        self.vertoffset_ch1.set(str(self.ain.getVerticalOffset(1)))
        self.vertoffset_ch0.set(str(self.ain.getVerticalOffset(0)))
        self.hystrange_ch0.set(str(self.ain.getHysteresisRange(0)))
        self.hystrange_ch1.set(str(self.ain.getHysteresisRange(1)))
        self.sclfact_ch0=tk.StringVar()
        self.sclfact_ch1=tk.StringVar()
        self.sclfact_ch0.set(str(self.ain.getScalingFactor(0)))
        self.sclfact_ch1.set(str(self.ain.getScalingFactor(1)))
        self.calibscale_ch0_entry=tk.Entry(self,textvariable=self.calibscale_ch0,width=15)
        self.calibscale_ch1_entry=tk.Entry(self,textvariable=self.calibscale_ch1,width=15)
        self.calibscale_ch0_entry.grid(row=8,column=1)
        self.calibscale_ch1_entry.grid(row=8,column=2)   
        self.vertoffset_ch0_entry=tk.Entry(self,textvariable=self.vertoffset_ch0,width=15)
        self.vertoffset_ch1_entry=tk.Entry(self,textvariable=self.vertoffset_ch1,width=15)
        self.vertoffset_ch0_entry.grid(row=9,column=1)
        self.vertoffset_ch1_entry.grid(row=9,column=2)
        values_submit=tk.Button(self,text="Set",command=self.set_values,width=5,font=SMALL_FONT)
        values_submit.grid(row=8,column=3,rowspan=2)
        values_get=tk.Button(self,text="Get",command=self.get_values,width=5,font=SMALL_FONT)
        values_get.grid(row=10,column=3,rowspan=2)
        calibscale_label=tk.Label(self,text="Calibration Scale",justify="left",anchor='w',width=20,font=SMALL_FONT)
        calibscale_label.grid(row=8,column=0)
        vertoffset_label=tk.Label(self,text="Vertical Offset",justify="left",anchor='w',width=20,font=SMALL_FONT)
        vertoffset_label.grid(row=9,column=0)
        ain_range_label=tk.Label(self,text="Set Range",justify="left",anchor='w',width=20,font=SMALL_FONT)
        ain_range_label.grid(row=12,column=0)
        hystrange_label=tk.Label(self,text="Hysteresis Range",justify="left",anchor='w',width=20,font=SMALL_FONT)
        hystrange_label.grid(row=10,column=0)
        sclfact_label=tk.Label(self,text="Scaling Factor",justify="left",anchor='w',width=20,font=SMALL_FONT)
        sclfact_label.grid(row=11,column=0)
        hystrangech0_box=tk.Label(self,textvariable=self.hystrange_ch0)
        hystrangech0_box.grid(row=10,column=1)
        hystrangech1_box=tk.Label(self,textvariable=self.hystrange_ch1)
        hystrangech1_box.grid(row=10,column=2)
        sclfactch0_box=tk.Label(self,textvariable=self.sclfact_ch0)
        sclfactch0_box.grid(row=11,column=1)
        sclfactch1_box=tk.Label(self,textvariable=self.sclfact_ch1)
        sclfactch1_box.grid(row=11,column=2)

        self.ain_range_ch0_var= tk.IntVar()
        ain_range_ch0_check=tk.Checkbutton(self,text ="Set High Range",variable=self.ain_range_ch0_var,command=self.set_range_ch0,font=SMALL_FONT)
        ain_range_ch0_check.grid(row=12,column=1)
        self.ain_range_ch1_var= tk.IntVar()
        ain_range_ch1_check=tk.Checkbutton(self,text ="Set High Range",variable=self.ain_range_ch1_var,command=self.set_range_ch1,font=SMALL_FONT)
        ain_range_ch1_check.grid(row=12,column=2)
        ain_rangelimits_label=tk.Label(self,text="Range Limits",justify="left",anchor='w',width=20,font=SMALL_FONT)
        ain_rangelimits_label.grid(row=13,column=0)
        self.rangelimits_ch0=tk.StringVar()
        self.rangelimits_ch1=tk.StringVar()
        self.rangelimits_ch0.set(str(self.ain.getRangeLimits(self.ain.getRange(0))))
        self.rangelimits_ch1.set(str(self.ain.getRangeLimits(self.ain.getRange(1))))
        ain_rangelimits_ch0=tk.Label(self,textvariable=self.rangelimits_ch0,font=SMALL_FONT)
        ain_rangelimits_ch0.grid(row=13,column=1)
        ain_rangelimits_ch1=tk.Label(self,textvariable=self.rangelimits_ch1,font=SMALL_FONT)
        ain_rangelimits_ch1.grid(row=13,column=2)

    def change_sample_rate(self):
        ain=self.ain
        desired_samplerate=float(self.in_samplerate.get())
        ain.setSampleRate(desired_samplerate)
        self.filter_comp.set(str(ain.getFilterCompensation(ain.getSampleRate())))
    def get_values(self):
        ain=self.ain
        self.calibscale_ch0.set(str(ain.getCalibscale(0)))
        self.calibscale_ch1.set(str(ain.getCalibscale(1)))
        self.vertoffset_ch0.set(str(ain.getVerticalOffset(0)))
        self.vertoffset_ch1.set(str(ain.getVerticalOffset(1)))
        self.rangelimits_ch0.set(str(ain.getRangeLimits(ain.getRange(0))))
        self.rangelimits_ch1.set(str(ain.getRangeLimits(ain.getRange(1))))
        self.hystrange_ch0.set(str(ain.getHysteresisRange(0)))
        self.hystrange_ch1.set(str(ain.getHysteresisRange(1)))
        self.sclfact_ch0.set(str(ain.getScalingFactor(0)))
        self.sclfact_ch1.set(str(ain.getScalingFactor(1)))
    def set_kernelbuff(self):
        ain=self.ain
        count=int(self.kernelbuff_entry.get())
        ain.setKernelBuffersCount(count)
    def set_oversampling(self):
        ain=self.ain
        ain.setOversamplingRatio(int(self.oversampling_ratio.get()))
    def set_range_ch0(self):
        ain=self.ain
        if self.ain_range_ch0_var.get()==1:
            ain.setRange(0,1)
        else:
            ain.setRange(0,0)
        self.rangelimits_ch0.set(str(ain.getRangeLimits(ain.getRange(0))))
    def set_range_ch1(self):
        ain=self.ain
        if self.ain_range_ch1_var.get()==1:
            ain.setRange(1,1)
        else:
            ain.setRange(1,0)
        self.rangelimits_ch1.set(str(ain.getRangeLimits(ain.getRange(1))))
    def set_values(self):
        ain=self.ain
        if self.calibscale_ch0_entry.get() is not None:
            ain.setCalibscale(0,float(self.calibscale_ch0_entry.get()))
        if self.calibscale_ch1_entry.get() is not None:
            ain.setCalibscale(1,float(self.calibscale_ch1_entry.get()))
        if self.vertoffset_ch0_entry.get() is not None:
            ain.setVerticalOffset(0,float(self.vertoffset_ch0_entry.get()))
        if self.vertoffset_ch1_entry.get() is not None:
            ain.setVerticalOffset(1,float(self.vertoffset_ch1_entry.get()))
        print("set")
    
