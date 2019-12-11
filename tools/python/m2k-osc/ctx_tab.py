import tkinter as tk
from tkinter import ttk
import libm2k
import libm2k_tab
LARGE_FONT = ("Verdana",12)
SMALL_FONT = ("Vedana",10)

           
class ctx_tab_frame(tk.Frame):

    def __init__(self,parent,ctx,ain,aout,*args,**kwargs):
        tk.Frame.__init__(self,parent,*args,**kwargs)
        self.ctx=ctx
        self.ain=ain
        self.aout=aout
        self.create_widgets()
        
    
    def create_widgets(self):
        ctx_location=tk.Label(self,text="Context at "+self.ctx.getUri(),anchor='w',font=SMALL_FONT)
        ctx_location.grid(row=1,column=0,columnspan=2)
        ctx_description=tk.Label(self,text="Context description "+self.ctx.getContextDescription(),anchor='w',font=SMALL_FONT)
        ctx_description.grid(row=2,column=0,columnspan=6)
        ctx_calibrate_adc_button = tk.Button(self,text=" Calib ADCs ",width=15,command=self.calibrate_adc,font=SMALL_FONT)
        ctx_calibrate_adc_button.grid(row=10,column=0)
        ctx_calibrate_dac_button = tk.Button(self,text=" Calib DACs ",width=15,command=self.calibrate_dac,font=SMALL_FONT)
        ctx_calibrate_dac_button.grid(row=12,column=0)
        ctx_reset_calib_button = tk.Button(self,text ="Reset Calib",width=15,command=self.reset_calib,font=SMALL_FONT)
        ctx_reset_calib_button.grid(row=14,column=0)
        ctx_setvalues=tk.Button(self,text="Set",width=5,command=self.set_values,font=SMALL_FONT)
        ctx_setvalues.grid(row=10,column=4,rowspan=4)
        ctx_init_button = tk.Button(self,text ="Init",width=15,command=self.initialize,font=SMALL_FONT)
        ctx_init_button.grid(row=7,column=0)
        ctx_resetinit_button = tk.Button(self,text ="Deninit",width=15,command=self.reset_init,font=SMALL_FONT)
        ctx_resetinit_button.grid(row=8,column=0)

        self.dacA_calib_offset=tk.StringVar()
        self.dacA_calib_gain=tk.StringVar()
        self.dacB_calib_offset=tk.StringVar()
        self.dacB_calib_gain=tk.StringVar()
        self.ch1_calib_offset=tk.StringVar()
        self.ch1_calib_gain=tk.StringVar()
        self.ch2_calib_offset=tk.StringVar()
        self.ch2_calib_gain=tk.StringVar()
        calib_offset_label = tk.Label(self, text ="Calibration Offset",font=SMALL_FONT)
        calib_offset_label.grid(row=9,column=3)
        ch1_label=tk.Label(self,text="CH.1",anchor='e',width=15,font=SMALL_FONT)
        ch1_label.grid(row=10,column=1)
        ch2_label=tk.Label(self,text="CH.2",anchor='e',width=15,font=SMALL_FONT)
        ch2_label.grid(row=11,column=1)
        self.ch1_calib_offset_value = tk.Entry(self,textvariable = self.ch1_calib_offset)
        self.ch1_calib_offset_value.grid(row=10,column=3)
        self.ch2_calib_offset_value = tk.Entry(self,textvariable = self.ch2_calib_offset)
        self.ch2_calib_offset_value.grid(row=11,column=3)
        self.ch1_calib_gain_value = tk.Entry(self,textvariable =self.ch1_calib_gain)
        self.ch1_calib_gain_value.grid(row=10,column=2)
        self.ch2_calib_gain_value = tk.Entry(self,textvariable = self.ch2_calib_gain)
        self.ch2_calib_gain_value.grid(row=11,column=2) 
        daca_label=tk.Label(self,text="DAC A",anchor='e',width=15,font=SMALL_FONT)
        daca_label.grid(row=12,column=1)
        dacb_label=tk.Label(self,text="DAC B",anchor='e',width=15,font=SMALL_FONT)
        dacb_label.grid(row=13,column=1)
        self.dacA_calib_offset_value = tk.Label(self,textvariable = self.dacA_calib_offset)
        self.dacA_calib_offset_value.grid(row=12,column=3)
        self.dacB_calib_offset_value = tk.Label(self,textvariable = self.dacB_calib_offset)
        self.dacB_calib_offset_value.grid(row=13,column=3)
        calib_gain_label = tk.Label(self, text ="Calibration Gain",font=SMALL_FONT)
        calib_gain_label.grid(row=9,column=2)
        self.dacA_calib_gain_value = tk.Entry(self,textvariable = self.dacA_calib_gain)
        self.dacA_calib_gain_value.grid(row=12,column=2)
        self.dacB_calib_gain_value = tk.Entry(self,textvariable = self.dacB_calib_gain)
        self.dacB_calib_gain_value.grid(row=13,column=2)
        self.led_var=tk.IntVar()
        ctx_set_led_check = tk.Checkbutton(self,text = "Set Led ON",width=15, variable=self.led_var,command =self.set_led,font=SMALL_FONT)
        ctx_set_led_check.grid(row=7,column=1)

    def calibrate_adc(self):
        ctx=self.ctx
        ctx.calibrateADC()
        self.ch1_calib_offset.set(str(ctx.getAdcCalibrationOffset(0)))
        self.ch1_calib_gain.set(str(ctx.getAdcCalibrationGain(0)))
        self.ch2_calib_offset.set(str(ctx.getAdcCalibrationOffset(1)))
        self.ch2_calib_gain.set(str(ctx.getAdcCalibrationGain(1)))

        print("ADCs Calibrated")
    def calibrate_dac(self):
        ctx=self.ctx
        ctx.calibrateDAC()
        self.dacA_calib_offset.set(str(ctx.getDacACalibrationOffset()))
        self.dacA_calib_gain.set(str(ctx.getDacACalibrationGain()))
        self.dacB_calib_offset.set(str(ctx.getDacBCalibrationOffset()))
        self.dacB_calib_gain.set(str(ctx.getDacBCalibrationGain()))
        print("DACs Calibrated")

    def initialize(self):
        ctx=self.ctx
        ctx.init()
        print("init done")
                
    def reset_calib(self):
        print("calib reset")
        ctx=self.ctx
        ctx.resetCalibration()
        self.ch1_calib_offset.set(" ")
        self.ch1_calib_gain.set(" ")
        self.ch2_calib_offset.set(" ")
        self.ch2_calib_gain.set(" ")
        self.dacA_calib_offset.set(" ")
        self.dacA_calib_gain.set(" ")
        self.dacB_calib_offset.set(" ")
        self.dacB_calib_gain.set(" ")
    def reset_init(self):
        print("deinit")
        self.ctx.deinitialize()
    def set_led(self):
        if self.led_var.get()==1:
            self.ctx.setLed(True)
        else:
            self.ctx.setLed(False)
    def set_values(self):
        ain=self.ain
        aout=self.aout
        if self.ch1_calib_gain_value.get()is None:
            return
        else:   
            ain.setAdcCalibGain(0,float(self.ch1_calib_gain.get()))
        if self.ch2_calib_gain_value.get()is None:
            return
        else:
            ain.setAdcCalibGain(1,float(self.ch2_calib_gain.get()))
        if self.ch1_calib_offset_value.get()is None:
            return
        else:
             ain.setAdcCalibOffset(0,int(self.ch1_calib_offset.get()))
        if self.ch2_calib_offset_value.get()is None:
            return
        else:
            ain.setAdcCalibOffset(1,int(self.ch2_calib_offset.get()))
        if self.dacA_calib_gain_value.get()is None:
            return
        else:   
            aout.setDacCalibVlsb(0,float(self.dacA_calib_gain.get()))
        if self.dacB_calib_gain_value.get()is None:
            return
        else:
            aout.setDacCalibVlsb(1,float(self.dacB_calib_gain.get()))
    
