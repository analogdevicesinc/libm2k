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
fig=fg.Figure(figsize=(7,5))
fig_subplot=fig.add_subplot(111)

def get_samples(ain):
    global read_samples,stop_thread
    while not stop_thread:
        global samples_queuech0,samples_queuech1
        samples_queuech0.append(ain.getSamples(int(read_samples.get()))[0])
        samples_queuech1.append(ain.getSamples(int(read_samples.get()))[1])
class plot_tab_frame(tk.Frame):
    def __init__(self,parent,ain,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.ain=ain
        self.instream=False
        self.create_widgets()
        self.create_canvas()
    def create_widgets(self):
        ain_ch_label=tk.Label(self,text="Enable input channels",anchor='w',width=20,font=SMALL_FONT)
        ain_ch_label.grid(row=1,column=0)
        self.ain_ch0_var=tk.IntVar()
        self.ain_ch1_var=tk.IntVar()
        ain_ch0_check=tk.Checkbutton(self,text="Channel 1",width=15,variable=self.ain_ch0_var,command=self.enable_ch0,font=SMALL_FONT)
        ain_ch0_check.grid(row=1,column=1)
        ain_ch1_check=tk.Checkbutton(self,text="Channel 2",width=15,variable=self.ain_ch1_var,command=self.enable_ch1,font=SMALL_FONT)
        ain_ch1_check.grid(row=1,column=2)
        ain_samples_label=tk.Label(self,text="Buffer size",justify="left",anchor='w',width=20,font=SMALL_FONT)
        ain_samples_label.grid(row=2,column=0)
        global read_samples
        read_samples=tk.StringVar()
        read_samples.set("10000")
        self.ain_samples_entry=tk.Entry(self,width=15,textvariable=read_samples,font=SMALL_FONT)
        self.ain_samples_entry.grid(row=2,column=1)
        ain_samples_submit=tk.Button(self,text="Set",width=5,command=self.update_readsamples,font=SMALL_FONT)
        ain_samples_submit.grid(row=2,column=2)
        ain_startch0=tk.Button(self,text="Start",width=5,command=self.start_in,font=SMALL_FONT)
        ain_startch0.grid(row=2,column=3)
        ain_stopch0=tk.Button(self,text="Stop",width=5,command=self.stop_in,font=SMALL_FONT)
        ain_stopch0.grid(row=3,column=3)
        refresh_label=tk.Label(self,text="Refresh time for plot in ms :",font=SMALL_FONT)
        refresh_label.grid(row=3,column=0)
        self.refresh_time=tk.StringVar()
        self.refresh_time.set("25")
        self.refreshtime_entry=tk.Entry(self,textvariable=self.refresh_time,width=15,font=SMALL_FONT)
        self.refreshtime_entry.grid(row=3,column=1)
        set_refresh=tk.Button(self,text="Set",command=self.set_refreshtime,width=5,font=SMALL_FONT)
        set_refresh.grid(row=3,column=2)
    def create_canvas(self):
        x=[1,2,3]
        y_ch0=[5,4,7]
        y_ch1=[8,6,5]
        self.linech0,=fig_subplot.plot(x,y_ch0)
        self.linech1,=fig_subplot.plot(x,y_ch1)
        self.canvas=FigureCanvasTkAgg(fig,master=self)
        self.canvas.draw()
        self.canvas.get_tk_widget().grid(row=11,column=0,columnspan=6)
    def enable_ch0(self):
        ain=self.ain
        if self.ain_ch0_var.get() ==1:
            ain.enableChannel(0,True)
        else:
            ain.enableChannel(0,False)
    def enable_ch1(self):
        ain=self.ain
        if self.ain_ch1_var.get() ==1:
            ain.enableChannel(1,True)
        else:
            ain.enableChannel(1,False)
    def plot(self):
        global samples_queuech0,samples_queuech1
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
        ax.autoscale()
        ax.relim()
        if self.instream is True:
            ax.set_xlabel("Sample Count")
            ax.set_ylabel("Gathered Data")
        else:
            ax.set_xlabel("Previous Sample Count")
            ax.set_ylabel("Previous Gathered Data")
        self.canvas.draw()
    def set_refreshtime(self):
        self.refresh_time.set(self.refreshtime_entry.get())

    def start_in(self):
        self.instream=True
        self.start_thread(self.ain)
        self.update_plot()
    def start_thread(self,ain):
        global stop_thread
        stop_thread=False
        get_samples_thread=threading.Thread(target=get_samples,args=(ain,))
        get_samples_thread.setDaemon(True)
        get_samples_thread.start()
    def stop_in(self):
        global stop_thread
        stop_thread=True
        self.instream=False
    def update_plot(self):
        self.plot()
        if self.instream is True:
            self.after(int(self.refresh_time.get()),self.update_plot)
    def update_readsamples(self):
        global read_samples
        read_samples.set(self.ain_samples_entry.get())
    
   

