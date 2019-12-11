import matplotlib
import numpy as np
import tkinter as tk
from tkinter import ttk
import matplotlib.figure as fg
import libm2k
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from matplotlib import style
import threading
import libm2k_tab

samples_queue=[]
for i in range(16):
    samples_queue.append([])

LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)

fig=fg.Figure(figsize=(6,5))
fig_subplot=fig.add_subplot(111)

def get_samples(dig):
    global read_samples, samples_queue
    data=dig.getSamples(int(read_samples.get()))
    for i in range(16):
        samples_queue[i]=[]
    for val in data:
        v=(format(val, "016b"))
        v=v[::-1]
        for i in range(16):
            samples_queue[i].append(int(v[i]))

class digplot_tab_frame(tk.Frame):
    def __init__(self,parent,dig,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.dig=dig
        self.instream=False
        self.colours=["#a8b8d6","#ffa500","#51a663","#ff6652","#513371","#c15600","#f9a8c4","#0d6785","#bada55","#babaff","#003366","#ff919c","#b4127c","#69d7ac","#ffd700","#4c4c4c"]
        self.create_widgets()
        self.create_canvas()

    def create_widgets(self):
        enable_label=tk.Label(self,text="Enable channels",anchor='w',width=15,font=SMALL_FONT)
        enable_label.grid(row=0,column=0)
        self.enable_var=[]
        for i in range(16):
            self.enable_var.append(tk.IntVar())
            self.enable_check=tk.Checkbutton(self,text="Ch."+str(i),variable=self.enable_var[i],command=self.enable_channels(i),anchor='w',width=5,activebackground=self.colours[i],font=SMALL_FONT)
            self.enable_check.grid(row=i+1,column=0)
        start_button = tk.Button(self,text="Start",command=self.start_in,font=SMALL_FONT)
        start_button.grid(row=0,column=4)
        stop_button=tk.Button(self,text="Stop",command=self.stop_in,font=SMALL_FONT)
        stop_button.grid(row=1,column=4)
        dig_samples_label=tk.Label(self,text="Buffer size",justify="left",anchor='w',width=20,font=SMALL_FONT)
        dig_samples_label.grid(row=0,column=1)
        global read_samples
        read_samples=tk.StringVar()
        read_samples.set("100")
        self.dig_samples_entry=tk.Entry(self,width=15,textvariable=read_samples,font=SMALL_FONT)
        self.dig_samples_entry.grid(row=0,column=2)
        dig_samples_submit=tk.Button(self,text="Set",width=5,command=self.update_readsamples,font=SMALL_FONT)
        dig_samples_submit.grid(row=0,column=3)
        refresh_label=tk.Label(self,text="Refresh time for plot in ms :",font=SMALL_FONT)
        refresh_label.grid(row=1,column=1)
        self.refresh_time=tk.StringVar()
        self.refresh_time.set("25")
        self.refreshtime_entry=tk.Entry(self,textvariable=self.refresh_time,width=15,font=SMALL_FONT)
        self.refreshtime_entry.grid(row=1,column=2)
        set_refresh=tk.Button(self,text="Set",command=self.set_refreshtime,width=5,font=SMALL_FONT)
        set_refresh.grid(row=1,column=3)

    def set_refreshtime(self):
        self.refresh_time.set(self.refreshtime_entry.get())

    def enable_channels(self,i):
        dig=self.dig
        if self.enable_var[i].get() == 1:
            dig.enableChannel(i,True)
        else:
            dig.enableChannel(i,False)

    def create_canvas(self):
        x=[1,2,3]
        y=[5,4,7]
        self.line_list=[]
        for i in range(16):
            self.line=fig_subplot.plot(x,y,color=self.colours[i])
        self.line_list=fig_subplot.lines
        self.canvas=FigureCanvasTkAgg(fig,master=self)
        self.canvas.draw()
        self.canvas.get_tk_widget().grid(row=2,column=1,columnspan=5,rowspan=15)

    def update_readsamples(self):
        global read_samples
        read_samples.set(self.dig_samples_entry.get())

    def plot(self):
        global samples_queue
        y=[]
        if self.instream is True:
            get_samples(self.dig)
        for i in range(16):
            if samples_queue[i]:
                y.append([])
                for val in samples_queue[i]:
                    y[i].append(val+(i*2))
                if self.line_list[i]:
                    current_line=self.line_list[i]
                    if self.enable_var[i].get()==1:
                        current_line.set_data(range(len(y[i])),y[i])
                    else:
                        current_line.set_data(range(int(read_samples.get())),np.zeros(int(read_samples.get())))
        ax=self.canvas.figure.axes[0]
        ax.set_xlim(0,int(read_samples.get()))
        ax.set_ylim(0,35)
        ax.set_xlabel("Sample Count")
        ax.set_ylabel("Gathered Data")
        self.canvas.draw()

    def start_in(self):
        self.instream=True
        self.update_plot()

    def stop_in(self):
        self.instream=False
        self.dig.flushBufferIn()
        self.dig.stopBufferOut()

    def update_plot(self):
        self.plot()
        if self.instream is True:
            self.after(int(self.refresh_time.get()),self.update_plot)
