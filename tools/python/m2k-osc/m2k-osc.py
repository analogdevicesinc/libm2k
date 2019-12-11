import tkinter as tk
from tkinter import ttk
import libm2k
from libm2k_tab import *
from ctx_tab import ctx_tab_frame
from ain_tab import ain_tab_frame
from aout_tab import aout_tab_frame
from dig_tab import dig_tab_frame
from ps_tab import ps_tab_frame
from plot_tab import plot_tab_frame
from digplot_tab import digplot_tab_frame
LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)

class m2k_osc_app(tk.Frame):
    def __init__(self,parent,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        tab_parent = ttk.Notebook()
        self.tab_parent = tab_parent
        tab_libm2k = libm2k_tab_frame(tab_parent, self)
        tab_parent.add(tab_libm2k,text='LibM2K')

        tab_parent.grid(row=0,column=0)
        tab_parent.grid_columnconfigure(0,weight=1)
        tab_parent.grid_rowconfigure(0,weight=1)

    def open_ctx_tab(self,ctx,ain,aout):
        self.tab_ctx=ctx_tab_frame(self.tab_parent,ctx,ain,aout)
        self.tab_parent.add(self.tab_ctx,text='Context')
    def open_ain_tab(self, ain):
        self.tab_ain = ain_tab_frame(self.tab_parent, ain)
        self.tab_parent.add(self.tab_ain,text='Analog In')
    def open_aout_tab(self,aout):
        self.tab_aout = aout_tab_frame(self.tab_parent,aout)
        self.tab_parent.add(self.tab_aout,text='Analog Out')
    def open_plot_tab(self,ain):
        self.tab_plot=plot_tab_frame(self.tab_parent,ain)
        self.tab_parent.add(self.tab_plot,text='Analog Plot')
    def open_ps_tab(self,ps):
        self.tab_ps=ps_tab_frame(self.tab_parent,ps)
        self.tab_parent.add(self.tab_ps,text='Power Supply')
    def open_dig_tab(self,dig):
        self.tab_dig=dig_tab_frame(self.tab_parent,dig)
        self.tab_parent.add(self.tab_dig,text="Digital I/O")
    def open_digplot_tab(self,dig):
        self.tab_digplot=digplot_tab_frame(self.tab_parent,dig)
        self.tab_parent.add(self.tab_digplot,text="Digital Plot")
    def remove_ctx_tab(self):
        self.tab_parent.forget(self.tab_ctx)
    def remove_ain_tab(self):
        self.tab_parent.forget(self.tab_ain)
    def remove_aout_tab(self):
        self.tab_parent.forget(self.tab_aout)
    def remove_plot_tab(self):
        self.tab_parent.forget(self.tab_plot)
    def remove_ps_tab(self):
        self.tab_parent.forget(self.tab_ps)
    def remove_dig_tab(self):
        self.tab_parent.forget(self.tab_dig)
    def remove_digplot_tab(self):
        self.tab_parent.forget(self.tab_digplot)

def main():
    root = tk.Tk()
    root.title("M2K Osc")
    root.geometry("745x615")
    m2k_osc_app(root).grid(row=0,column=0) 
    root.mainloop()



if  __name__ == "__main__":
    main()


