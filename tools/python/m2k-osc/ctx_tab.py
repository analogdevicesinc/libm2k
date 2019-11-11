import matplotlib
import numpy as np
import tkinter as tk
from tkinter import ttk
import libm2k

LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)
global ctx,ain,aout,dig,ps
ctx = None
ain = None
aout = None
dig = None
ps= None

           
class ctx_tab_frame(tk.Frame):

    def __init__(self,parent,app,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs) 
        self.parent=parent
        self.app=app
        self.create_widgets()
        self.options_widgets()
    
    def create_widgets(self):
        ctx_label= tk.Label(self, text="Set up the M2K context.")
        ctx_label.grid(row=0,column=0)
        global option_var
        option_var=tk.IntVar()
        ctx_open_label=tk.Label(self,text="Open context:")
        ctx_open_label.grid(row=1,column=0)
        ctx_option1=tk.Radiobutton(self,text="IP Address",variable=option_var,value=1,command= self.selection)
        ctx_option1.grid(row=2,column=0)
        ctx_option2=tk.Radiobutton(self,text="USB Location",variable=option_var,value=2,command= self.selection)
        ctx_option2.grid(row=3,column=0)
        ctx_option3=tk.Radiobutton(self,text="One device only",variable=option_var,value=3,command=self.selection)
        ctx_option3.grid(row=4,column=0)

        global ctx_message,ctx_serialno,ctx_name
        ctx_serialno=tk.StringVar()
        ctx_message=tk.StringVar()
        ctx_name=tk.StringVar()
        ctx_message.set("Click OPEN to connect to the M2K learning module")
        ctx_message_box = tk.Label(self,textvariable = ctx_message)
        ctx_message_box.grid(row=6,column=1)
        ctx_serialno_box=tk.Label(self,textvariable=ctx_serialno)
        ctx_serialno_box.grid(row=7,column=1)
        ctx_calibrate_adc_button = tk.Button(self,text=" Calibrate ADCs ",command=self.calibrate_adc)
        ctx_calibrate_adc_button.grid(row=10,column=0)
        ctx_calibrate_dac_button = tk.Button(self,text=" Calibrate DACs ",command=self.calibrate_dac)
        ctx_calibrate_dac_button.grid(row=12,column=0)
        ctx_reset_calib_button = tk.Button(self,text ="Undo Calibration",command=self.reset_calib)
        ctx_reset_calib_button.grid(row=14,column=0)
        ctx_init_button = tk.Button(self,text ="Initialize Module",command=self.initialize)
        ctx_init_button.grid(row=7,column=0)
        ctx_resetinit_button = tk.Button(self,text ="Undo Initialization",command=self.reset_init)
        ctx_resetinit_button.grid(row=8,column=0)

        global dacA_calib_offset,dacA_calib_gain,dacB_calib_offset,dacB_calib_gain
        dacA_calib_offset=tk.StringVar()
        dacA_calib_gain=tk.StringVar()
        dacB_calib_offset=tk.StringVar()
        dacB_calib_gain=tk.StringVar()
        global ch1_calib_offset,ch1_calib_gain,ch2_calib_offset,ch2_calib_gain
        ch1_calib_offset=tk.StringVar()
        ch1_calib_gain=tk.StringVar()
        ch2_calib_offset=tk.StringVar()
        ch2_calib_gain=tk.StringVar()
        calib_offset_label = tk.Label(self, text ="Calibration Offset")
        calib_offset_label.grid(row=9,column=3)
        ch1_label=tk.Label(self,text="CH.1")
        ch1_label.grid(row=10,column=2) 
        ch2_label=tk.Label(self,text="CH.2")
        ch2_label.grid(row=11,column=2)
        ch1_calib_offset_value = tk.Label(self,textvariable = ch1_calib_offset)
        ch1_calib_offset_value.grid(row=10,column=3)
        ch2_calib_offset_value = tk.Label(self,textvariable = ch2_calib_offset)
        ch2_calib_offset_value.grid(row=11,column=3)
        ch1_calib_gain_value = tk.Label(self,textvariable =ch1_calib_gain)
        ch1_calib_gain_value.grid(row=10,column=5)
        ch2_calib_gain_value = tk.Label(self,textvariable = ch2_calib_gain)
        ch2_calib_gain_value.grid(row=11,column=5)

        daca_label=tk.Label(self,text="DAC A")
        daca_label.grid(row=12,column=2)
        dacb_label=tk.Label(self,text="DAC B")
        dacb_label.grid(row=13,column=2)
        dacA_calib_offset_value = tk.Label(self,textvariable = dacA_calib_offset)
        dacA_calib_offset_value.grid(row=12,column=3)
        dacB_calib_offset_value = tk.Label(self,textvariable = dacB_calib_offset)
        dacB_calib_offset_value.grid(row=13,column=3)
        calib_gain_label = tk.Label(self, text ="Calibration Gain")
        calib_gain_label.grid(row=9,column=5)
        dacA_calib_gain_value = tk.Label(self,textvariable = dacA_calib_gain)
        dacA_calib_gain_value.grid(row=12,column=5)
        dacB_calib_gain_value = tk.Label(self,textvariable = dacB_calib_gain)
        dacB_calib_gain_value.grid(row=13,column=5)
        global led_var
        led_var=tk.IntVar()
        ctx_set_led_check = tk.Checkbutton(self,text = "Set Led ON", variable=led_var,command =self.set_led)
        ctx_set_led_check.grid(row=15,column=0)

        ctx_close_check=tk.Button(self,text="Close Context",command=self.close_context)
        ctx_close_check.grid(row=16,column=0)

    def open_context(self):
        global ctx
        if ctx is None:
            ctx = libm2k.m2kOpen()
        else:
            print("M2K already connected")
            return
       
        if ctx is None:
            ctx_message.set("No module connected!")
        else:
            global ain,aout,dig

            ain=ctx.getAnalogIn()
            if ain is not None:
                self.app.open_ain_tab() 

            aout=ctx.getAnalogOut()
            if aout is not None:
                self.app.open_aout_tab()
 
            #dig=ctx.getDigital()
            #ps=ctx.getPowerSupply()
            #trig=ain.getTrigger()
            ctx_message.set("Context created.Connected to device with serial no.:")
            ctx_serialno.set(str(ctx.getSerialNumber()))
 
    def calibrate_adc(self):
        ctx.calibrateADC()
        ch1_calib_offset.set(str(ctx.getAdcCalibrationOffset(0)))
        ch1_calib_gain.set(str(ctx.getAdcCalibrationGain(0)))
        ch2_calib_offset.set(str(ctx.getAdcCalibrationOffset(1)))
        ch2_calib_gain.set(str(ctx.getAdcCalibrationGain(1)))

        print("ADC Calibrated")
    def calibrate_dac(self):
        ctx.calibrateDAC()
        dacA_calib_offset.set(str(ctx.getDacACalibrationOffset()))
        dacA_calib_gain.set(str(ctx.getDacACalibrationGain()))
        dacB_calib_offset.set(str(ctx.getDacBCalibrationOffset()))
        dacB_calib_gain.set(str(ctx.getDacBCalibrationGain()))
        print("DAC Calibrated")
    def close_context(self):
        libm2k.contextClose(ctx)
        self.app.remove_ain_tab()
        self.app.remove_aout_tab()
        ch1_calib_offset.set(" ")
        ch1_calib_gain.set(" ")
        ch2_calib_offset.set(" ")
        ch2_calib_gain.set(" ")
        dacA_calib_offset.set(" ")
        dacA_calib_gain.set(" ")
        dacB_calib_offset.set(" ")
        dacB_calib_gain.set(" ")
        ctx_serialno.set(" ")
        ctx_message.set("Context Closed")
        print("closed")
    def initialize(self):
        ctx.init()
        print("initialization done")
                
    def reset_calib(self):
        ctx.resetCalibration()
        ch1_calib_offset.set(" ")
        ch1_calib_gain.set(" ")
        ch2_calib_offset.set(" ")
        ch2_calib_gain.set(" ")
        dacA_calib_offset.set(" ")
        dacA_calib_gain.set(" ")
        dacB_calib_offset.set(" ")
        dacB_calib_gain.set(" ")
    def reset_init(self):
        ctx.deinitialize()
        print("initialization undone")
    def set_led(self):
        if led_var.get()==1:
            ctx.setLed(True)
        else:
            ctx.setLed(False)
    def selection(self):
        if option_var.get()==1:
            print("option1")
            self.hide_widgets_op1()
            self.opt1_label.grid()
            self.ip_entry.grid()
            self.ip_submit.grid()
        elif option_var.get()==2:
            print("option2")
            self.hide_widgets_op2()
            self.opt2_label.grid()
            self.opt2_dropdown.grid()
            self.opt2_submit.grid()       
        else:
            print("option3")
            self.hide_widgets_op3()
            self.opt3_button.grid()

    def options_widgets(self):
        self.opt1_label=tk.Label(self,text="Enter the IP Address for your remote device:")
        self.opt1_label.grid(row=1,column=1)
        self.opt1_label.grid_remove()
        self.ip_entry=tk.Entry(self)
        self.ip_entry.grid(row=2,column=1)
        self.ip_entry.grid_remove()
        self.ip_submit=tk.Button(self,text="OK",command=self.open_ip)
        self.ip_submit.grid(row=2,column=2)
        self.ip_submit.grid_remove()
        self.opt2_label=tk.Label(self,text="Select USB location for desired device:")
        self.opt2_label.grid(row=1,column=1)
        self.opt2_label.grid_remove()
        usb_options=libm2k.getAllContexts()
        global trace_var
        trace_var=tk.StringVar()
        trace_var.set(usb_options[0])
        self.opt2_dropdown=tk.OptionMenu(self,trace_var,tuple(usb_options))
        self.opt2_dropdown.grid(row=2,column=1)
        self.opt2_dropdown.grid_remove()
        self.opt2_submit=tk.Button(self,text="OK",command=self.open_usb)
        self.opt2_submit.grid(row=2,column=2)
        self.opt2_submit.grid_remove()
        self.opt3_button=tk.Button(self,text="Open Context",command=self.open_context)
        self.opt3_button.grid(row=1,column=1)
        self.opt3_button.grid_remove()
    def hide_widgets_op1(self):
        self.opt2_label.grid_remove()
        self.opt2_dropdown.grid_remove()
        self.opt2_submit.grid_remove()
        self.opt3_button.grid_remove()
    def hide_widgets_op2(self):
        self.opt1_label.grid_remove()
        self.ip_entry.grid_remove()
        self.ip_submit.grid_remove()
        self.opt3_button.grid_remove()
    def hide_widgets_op3(self):
        self.opt1_label.grid_remove()
        self.ip_entry.grid_remove()
        self.ip_submit.grid_remove()
        self.opt2_label.grid_remove()
        self.opt2_dropdown.grid_remove()
        self.opt2_submit.grid_remove()
    def open_usb(self):
        print("open usb")
        global ctx
        uri=trace_var.get()
        ctx=libm2k.m2kOpen(uri)
        if ctx is None:
            ctx_message.set("No module connected!")
        else:
            global ain,aout,dig
            ain=ctx.getAnalogIn()
            aout=ctx.getAnalogOut()
            dig=ctx.getDigital()
            #ps=ctx.getPowerSupply()
            #trig=ain.getTrigger()
            ctx_message.set("Context created.Connected to device with serial no.:")
            ctx_serialno.set(str(ctx.getSerialNumber()))


    def open_ip(self):
        print("open ip")
        global ctx
        uri=ip_entry.get()
        ctx=libm2k.m2kOpen(uri)
        if ctx is None:
            ctx_message.set("No module connected!")
        else:
            global ain,aout,dig
            ain=ctx.getAnalogIn()
            aout=ctx.getAnalogOut()
            dig=ctx.getDigital()
            #ps=ctx.getPowerSupply()
            #trig=ain.getTrigger()
            ctx_message.set("Context created.Connected to device with serial no.:")
            ctx_serialno.set(str(ctx.getSerialNumber()))



            

       
