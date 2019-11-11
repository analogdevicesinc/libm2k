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
from dig_tab import *
from ps_tab import *

LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)

#style = ttk.Style()
#style.theme_create("MyStyle",parent="alt",settings={"TNotebook":{"configure":{"tabmargins":[2,5,2,0]}},"TNotebook.Tab":{"configure":{"padding":[100,100]},}})
#style.theme_use("MyStyle")

class m2k_osc_app(tk.Frame):
    def __init__(self,parent,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        tab_parent = ttk.Notebook()
        self.tab_parent = tab_parent
        tab_ctx = ctx_tab_frame(tab_parent, self)
        tab_parent.add(tab_ctx,text='Context' )

        tab_parent.grid(row=0,column=0)
        tab_parent.grid_columnconfigure(0,weight=1)
        tab_parent.grid_rowconfigure(0,weight=1)

        """
        tab_dig = dig_tab_frame(tab_parent)
        tab_ps =ps_tab_frame(tab_parent)
        tab_parent.add(tab_dig,text='Digital I/O')
        tab_parent.add(tab_ps,text='Power Supply' )
        """
               
    def open_ain_tab(self):
        self.tab_ain = ain_tab_frame(self.tab_parent)
        self.tab_parent.add(self.tab_ain,text='Analog In')
    def open_aout_tab(self):
        self.tab_aout = aout_tab_frame(self.tab_parent)
        self.tab_parent.add(self.tab_aout,text='Analog Out')

    def remove_ain_tab(self):
        self.tab_parent.forget(self.tab_ain)
    def remove_aout_tab(self):
        self.tab_parent.forget(self.tab_aout)
  

def main():
    root = tk.Tk()
    root.title("M2K Osc")
    root.geometry("800x850")
    #m2k_osc_app(root).place(side="top", fill="both", expand=True)
    m2k_osc_app(root).grid(row=0,column=0) 
    root.mainloop()



if  __name__ == "__main__":
    main()


