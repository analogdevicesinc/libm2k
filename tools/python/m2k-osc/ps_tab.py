import matplotlib
import numpy as np
import tkinter as tk
from tkinter import ttk
import matplotlib.figure as  fg
import libm2k
from matplotlib import style
import threading
import time
LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)

class ps_tab_frame(tk.Frame): 
    def __init__(self,parent,ps,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.ps=ps
        self.create_widgets()
    def create_widgets(self):
        psch_label=tk.Label(self,text="Enable pwr spl channels:",font=SMALL_FONT)
        psch_label.grid(row=3,column=0)
        self.psch0_var=tk.IntVar()
        self.psch1_var=tk.IntVar()
        psch0_check=tk.Checkbutton(self,text="Positive PwrSpl",width=25,variable=self.psch0_var,command=self.enable_ch0,font=SMALL_FONT)
        psch0_check.grid(row=3,column=1)
        psch1_check=tk.Checkbutton(self,text="Negative PwrSpl",width=25,variable=self.psch1_var,command=self.enable_ch1,font=SMALL_FONT)
        psch1_check.grid(row=3,column=2)
        ps_warning=tk.Label(self,text="Power supply values must not be greater than +/- 5V",font=SMALL_FONT)
        ps_warning.grid(row=0,column=0,columnspan=2)
        ps_currentvalue=tk.Label(self,text="Current pwr spl value",font=SMALL_FONT)
        ps_currentvalue.grid(row=4,column=0)
        ps_newvalue=tk.Label(self,text="New  pwr spl value",font=SMALL_FONT)
        ps_newvalue.grid(row=5,column=0)
        self.psvalue_ch0=tk.StringVar()
        self.psvalue_ch0.set(str(self.ps.readChannel(0)))
        self.psvalue_ch1=tk.StringVar()
        self.psvalue_ch1.set(str(self.ps.readChannel(1)))
        psvalue_ch0label=tk.Label(self,textvariable=self.psvalue_ch0,font=SMALL_FONT)
        psvalue_ch1label=tk.Label(self,textvariable=self.psvalue_ch1,font=SMALL_FONT)
        psvalue_ch0label.grid(row=4,column=1)
        psvalue_ch1label.grid(row=4,column=2)
        self.psvalue_ch0_entry=tk.Entry(self,width=25,font=SMALL_FONT)
        self.psvalue_ch1_entry=tk.Entry(self,width=25,font=SMALL_FONT)
        self.psvalue_ch0_entry.grid(row=5,column=1)
        self.psvalue_ch1_entry.grid(row=5,column=2) 
        ch0_push=tk.Button(self,text="Push Ch1",command=self.push_ch0,font=SMALL_FONT)
        ch0_push.grid(row=6,column=1)
        ch1_push=tk.Button(self,text="Push Ch2",command=self.push_ch1,font=SMALL_FONT)
        ch1_push.grid(row=6,column=2)
        ch0_read=tk.Button(self,text="Read Ch1",command=self.read_ch0,font=SMALL_FONT)
        ch0_read.grid(row=7,column=1)
        ch1_read=tk.Button(self,text="Read Ch2",command=self.read_ch1,font=SMALL_FONT)
        ch1_read.grid(row=7,column=2)

    def enable_ch0(self):
        ps=self.ps
        if self.psch0_var.get() ==1:
            ps.enableChannel(0,True)
        else:
            ps.enableChannel(0,False)
    def enable_ch1(self):
        ps=self.ps
        if self.psch1_var.get() ==1:
            ps.enableChannel(1,True)
        else:
            ps.enableChannel(1,False)
    def push_ch0(self):
        ps=self.ps
        voltage=float(self.psvalue_ch0_entry.get())
        ps.pushChannel(0,voltage)
        self.psvalue_ch0.set(str(self.ps.readChannel(0)))
    def push_ch1(self):
        ps=self.ps
        voltage=float(self.psvalue_ch1_entry.get())
        ps.pushChannel(1,voltage)
        self.psvalue_ch1.set(str(self.ps.readChannel(1)))
    def read_ch0(self):
        self.psvalue_ch0.set(str(self.ps.readChannel(0)))
    def read_ch1(self):
        self.psvalue_ch1.set(str(self.ps.readChannel(1)))
