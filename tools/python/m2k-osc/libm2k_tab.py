import libm2k
import tkinter as tk
from tkinter import ttk

LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)

global ctx
ctx=None

class libm2k_tab_frame(tk.Frame):
    def __init__(self,parent,app,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.parent=parent
        self.app=app
        self.create_widgets()
    def create_widgets(self):
        self.options_widgets()
        self.option_var=tk.IntVar()
        self.option_var.set(3)
        self.ip_entry.config(state="disabled")
        self.opt2_dropdown.config(state="disabled")
        ctx_open_label=tk.Label(self,text="Select method:",font=SMALL_FONT)
        ctx_open_label.grid(row=1,column=0)
        ctx_option1=tk.Radiobutton(self,text="URI ",justify="left",variable=self.option_var,value=1,command= self.selection,font=SMALL_FONT)
        ctx_option1.grid(row=2,column=0)
        ctx_option2=tk.Radiobutton(self,text="USB",justify="left",variable=self.option_var,value=2,command=self.selection,font=SMALL_FONT) 
        ctx_option2.grid(row=4,column=0)
        ctx_option3=tk.Radiobutton(self,text="None",justify="left",variable=self.option_var,value=3,command=self.selection,font=SMALL_FONT)
        ctx_option3.grid(row=6,column=0)
        self.ctx_message=tk.StringVar()
        self.ctx_message.set("Open a context ...")
        libm2k_message=tk.Label(self,textvariable=self.ctx_message,justify="left",font=SMALL_FONT)
        libm2k_message.grid(row=8,column=0,columnspan=3)
        self.ctx_serial=tk.StringVar()
        libm2k_serial=tk.Label(self,textvariable=self.ctx_serial,justify="left",font=SMALL_FONT)
        libm2k_serial.grid(row=9,column=1,columnspan=2)
    def selection(self):
        if self.option_var.get()==1:
            print("option1")
            self.opt2_dropdown.config(state="disabled")
            self.ip_entry.config(state="normal")
            self.ctx_message.set("")
        elif self.option_var.get()==2:
            print("option2")
            self.ip_entry.config(state="disabled")
            self.opt2_dropdown.config(state="normal")
            self.usb_options=libm2k.getAllContexts()
            if self.usb_options:
                self.trace_var.set(self.usb_options[0])
                self.ctx_message.set("")
            else:
                self.ctx_message.set("No device found at any USB port")
        else:
            print("option3")
            self.ip_entry.config(state="disabled")
            self.opt2_dropdown.config(state="disabled")
            self.ctx_message.set("")

      

    def options_widgets(self):
        self.opt1_label=tk.Label(self,text="URI for remote device:",justify="left",font=SMALL_FONT)
        self.opt1_label.grid(row=1,column=1)
        self.ip_entry=tk.Entry(self)
        self.ip_entry.grid(row=2,column=1,rowspan=2)
        self.opt2_label=tk.Label(self,text="USB location of device:",justify="left",font=SMALL_FONT)
        self.trace_var=tk.StringVar()
        self.usb_options=libm2k.getAllContexts()
        self.opt2_dropdown=tk.OptionMenu(self,self.trace_var,tuple(self.usb_options))
        self.opt2_dropdown.grid(row=5,column=1,rowspan=2)
        self.opt2_label.grid(row=4,column=1)
        self.open_button=tk.Button(self,text="Open Context",command=self.open_context,justify="left",font=SMALL_FONT)
        self.open_button.grid(row=2,column=2)
        self.close_button=tk.Button(self,text="Close Context",command=self.close_context,justify="left",font=SMALL_FONT)
        self.close_button.grid(row=4,column=2) 
    def open_context(self):
        global ctx
        if self.option_var.get()==2:
            if ctx is None:
                ctx = libm2k.m2kOpen(self.trace_var.get())
            else:
                self.ctx_message.set("M2K already connected")
                return
        elif self.option_var.get()==1:
            if ctx is None:
                uri='ip:'+self.ip_entry.get()
                ctx = libm2k.m2kOpen(uri)
            else:
                self.ctx_message.set("M2K already connected")
                return
        else:
            if ctx is None:
                ctx = libm2k.m2kOpen()
            else:
                self.ctx_message.set("M2K already connected")
                return

                   
        if ctx is None:
            self.ctx_message.set("No module connected!")
        else:
            ain=ctx.getAnalogIn()
            aout=ctx.getAnalogOut()
            self.ctx_message.set("Context for device with serial no:")
            self.ctx_serial.set(ctx.getSerialNumber())
            self.app.open_ctx_tab(ctx,ain,aout)
            if ain is not None:
                self.app.open_ain_tab(ain)
            if aout is not None:
                self.app.open_aout_tab(aout)
            if ain is not None:
                self.app.open_plot_tab(ain)
            dig=ctx.getDigital()
            if dig is not None:
                self.app.open_dig_tab(dig)
                self.app.open_digplot_tab(dig)
            ps=ctx.getPowerSupply()
            if ps is not None:
                self.app.open_ps_tab(ps)
    def close_context(self):
        global ctx
        self.ctx_message.set(" ")
        self.ctx_serial.set(" ")
        self.app.remove_ain_tab()
        self.app.remove_ctx_tab()
        self.app.remove_aout_tab()
        self.app.remove_ps_tab()
        self.app.remove_dig_tab()
        self.app.remove_digplot_tab()
        self.app.remove_plot_tab()
        libm2k.contextClose(ctx)
        ctx=None

