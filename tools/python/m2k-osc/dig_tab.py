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
from ctx_tab import *
from ain_tab import *
from aout_tab import *
from ps_tab import *

LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)

class dig_tab_frame(tk.Frame):
    def __init__(self,parent,dig,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.dig=dig
        self.create_singlewidgets()
        self.channel_widgets()
    def create_singlewidgets(self):
        dig=self.dig
        in_samplerate_label=tk.Label(self,text="Sample rate(in)",anchor='w',width=18,font=SMALL_FONT)
        in_samplerate_label.grid(row=1,column=0)
        out_samplerate_label=tk.Label(self,text="Sample rate(out)",anchor='w',width=18,font=SMALL_FONT)
        out_samplerate_label.grid(row=2,column=0)
        kernelbuff_label=tk.Label(self,text="Kernel Buffers count",anchor='w',width=18,font=SMALL_FONT)
        kernelbuff_label.grid(row=3,column=0)
        self.in_samplerate=tk.StringVar()
        self.in_samplerate.set(str(dig.getSampleRateIn()))
        self.out_samplerate=tk.StringVar()
        self.out_samplerate.set(str(dig.getSampleRateOut()))
        self.kernelbuff_count=tk.StringVar()
        self.in_samplerate_entry=tk.Entry(self,textvariable=self.in_samplerate,width=15,font=SMALL_FONT)
        self.in_samplerate_entry.grid(row=1,column=1,columnspan=3)
        self.out_samplerate_entry=tk.Entry(self,textvariable=self.out_samplerate,width=15,font=SMALL_FONT)
        self.out_samplerate_entry.grid(row=2,column=1,columnspan=3)
        self.kernelbuff_entry=tk.Entry(self,textvariable=self.kernelbuff_count,width=15,font=SMALL_FONT)
        self.kernelbuff_entry.grid(row=3,column=1,columnspan=3)
        in_samplerate_submit=tk.Button(self,text="Set",width=5,command=self.set_insamplerate,font=SMALL_FONT)
        in_samplerate_submit.grid(row=1,column=5)
        out_samplerate_submit=tk.Button(self,text="Set",width=5,command=self.set_outsamplerate,font=SMALL_FONT)
        out_samplerate_submit.grid(row=2,column=5)
        kernelbuff_submit=tk.Button(self,text="Set",width=5,command=self.set_kernelbuffcount,font=SMALL_FONT)
        kernelbuff_submit.grid(row=3,column=5)
        self.cyclic_var=tk.IntVar()
        cyclic_check=tk.Checkbutton(self,text="Enable",width=15,anchor='e',font=SMALL_FONT,variable=self.cyclic_var,command=self.enable_cyclic)
        cyclic_check.grid(row=2,column=6)
        direction_label=tk.Label(self,text="Direction",anchor='w',width=15,font=SMALL_FONT)
        direction_label.grid(row=4,column=1)
        enable_label=tk.Label(self,text="Output Mode",anchor='w',width=15,font=SMALL_FONT)
        enable_label.grid(row=4,column=2)
        enable_label=tk.Label(self,text="Raw Value",anchor='w',width=15,font=SMALL_FONT)
        enable_label.grid(row=4,column=3)
        enable_label=tk.Label(self,text="Cyclic mode",anchor='w',width=15,font=SMALL_FONT)
        enable_label.grid(row=1,column=6)
        button_set=tk.Button(self,text="Set",width=5,command=self.set_values,font=SMALL_FONT)
        button_set.grid(row=20,column=5)
        button_get=tk.Button(self,text="Get",width=5,command=self.get_values,font=SMALL_FONT)
        button_get.grid(row=19,column=5)
        load_label=tk.Label(self,text="Load CSV file and",font=SMALL_FONT,anchor='w',width=17)
        load_label.grid(row=5,column=5,columnspan=2)
        load_label=tk.Label(self,text="push samples to",font=SMALL_FONT,anchor='w',width=17)
        load_label.grid(row=6,column=5,columnspan=2)
        load_label=tk.Label(self,text="all digital channels :",font=SMALL_FONT,anchor='w',width=17)
        load_label.grid(row=7,column=5,columnspan=2)
        load_button=tk.Button(self,text="Load&Push",font=SMALL_FONT,anchor='w',command=self.push_data)
        load_button.grid(row=8,column=6)
    def channel_widgets(self):
        def set_direction(direction,*parags):
            self.set_chndirection()
        def set_outmode(outmode,*parags):
            self.set_chnoutmode()
        self.direction=[]
        self.outmode=[]
        self.raw_value=[]
        self.outmode_opt=[]
        self.rawvalue_entry=[]
        for i in range(16):
            self.direction.append(tk.StringVar())
            self.outmode.append(tk.StringVar())
            self.raw_value.append(tk.StringVar())
            self.outmode_opt.append(tk.OptionMenu(self,self.outmode[i],"OpenDrain","PushPull"))
            self.rawvalue_entry.append(tk.Entry(self))
        for i in range(16):
            label=tk.Label(self,text="Ch."+str(i),anchor='w',width=15,font=SMALL_FONT)
            label.grid(row=5+i,column=0)
            if self.dig.getDirection(i)==1:
                self.direction[i].set("Out")
            else:
                self.direction[i].set("In")
            self.direction[i].trace('w',lambda *parags:set_direction(self.direction[i],*parags))
            direction_opt=tk.OptionMenu(self,self.direction[i],"In","Out")
            direction_opt.grid(row=5+i,column=1,sticky='w')
            if self.dig.getOutputMode(i)==0:
                self.outmode[i].set("OpenDrain")
            else:
                self.outmode[i].set("PushPull")
            self.outmode[i].trace('w',lambda *parags:set_outmode(self.outmode,*parags))
            self.outmode_opt[i]=tk.OptionMenu(self,self.outmode[i],"OpenDrain","PushPull")
            self.outmode_opt[i].grid(row=5+i,column=2,sticky='w')
            if self.dig.getDirection(i)==0:
                self.outmode_opt[i].config(state='disabled')
            else:
                self.outmode_opt[i].config(state='normal')
            self.raw_value[i].set(str(self.dig.getValueRaw(i)))
            self.rawvalue_entry[i]=tk.Entry(self,textvariable=self.raw_value[i],width=13,font=SMALL_FONT)
            self.rawvalue_entry[i].grid(row=5+i,column=3,sticky='w')
    def enable_cyclic(self):
        if self.cyclic_var.get()==1:
            self.dig.setCyclic(True)
        else:
            self.dig.setCyclic(False)
    def get_values(self):
        dig=self.dig
        for i in range(16):
            self.raw_value[i].set(str(dig.getValueRaw(i)))
    def push_data(self):
        for i in range(16):
            self.dig.setOutputMode(i,1)
            self.dig.setDirection(i,1)
            self.dig.enableChannel(i,True)
        buff=self.read_csv(0)
        print(buff)
        self.dig.push(buff)

    def read_csv(self,column):
        num_data=[]
        self.filename=filedialog.askopenfilename(title="Select file")
        f=open(self.filename,"rt")
        reader=csv.reader(f,delimiter=',')
        for row in reader:
            entry=row[column]
            try:
                num_data.append(int(entry))
            except ValueError:
                print("Cannot convert {} to a int ... skipping".format(entry))
        return num_data
    
    def set_values(self):
        dig=self.dig
        for i in range(16):
            dig.setValueRaw(i,int(self.rawvalue_entry[i].get()))
            self.raw_value[i].set(str(dig.getValueRaw(i)))
                            
    def set_insamplerate(self):
        self.dig.setSampleRateIn(float(self.in_samplerate_entry.get()))
        self.in_samplerate.set(str(self.dig.getSampleRateIn()))
    def set_outsamplerate(self):
        self.dig.setSampleRateOut(float(self.out_samplerate_entry.get()))
        self.out_samplerate.set(str(self.dig.getSampleRateOut()))
    def set_kernelbuffcount(self):
        self.dig.setKernelBuffersCountIn(int(self.kernelbuff_entry.get()))
    def set_chndirection(self):
        dig=self.dig
        for i in range(16):
            if self.direction[i].get() == 'Out':
                dig.setDirection(i,1)
                self.outmode_opt[i].config(state="normal")
            else:
                dig.setDirection(i,0)
                self.outmode_opt[i].config(state="disabled")

    def set_chnoutmode(self):
        dig=self.dig
        for i in range(16):
            if self.outmode[i].get() == 'OpenDrain':
                dig.setOutputMode(i,0)
                print(dig.getOutputMode(i))
            else:
                dig.setOutputMode(i,1)
                print(dig.getOutputMode(i))

