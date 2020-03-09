#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -*- Python -*-
import sys
import time
sys.path.append(".")
import RTC
import OpenRTM_aist

import tkinter
from tkinter import messagebox
from tkinter import Frame
from tkinter import Radiobutton
from tkinter import StringVar

mod_spec = ["implementation_id", "SetMode", 
            "type_name", "SetMode", 
            "description", "", 
            "version", "1.0", 
            "vendor", "MasutaniLab", 
            "category", "Generic", 
            "activity_type", "DataFlowComponent", 
            "max_instance", "1", 
            "language", "Python", 
            "lang_type""SCRIPT",
            ""]

class RadioButton(Frame):
  def __init__(self, title="", master=None):
    Frame.__init__(self, master)
    self.new = False
    self.var = StringVar()
    self.var.set(0)
    Radiobutton(self, 
                indicatoron = False,
                width = 20,
                text = 'ON', 
                variable = self.var, 
                value = 'on', 
                command = self.buttonCommand
                ).pack()
    Radiobutton(self, 
                indicatoron = False,
                width = 20,
                text = 'OFF', 
                variable = self.var, 
                value = 'off', 
                command = self.buttonCommand
                ).pack()
    self.master.title(title)
    self.master.lift()
    self.master.attributes('-topmost', True)
    self.master.focus_set()
    self.master.protocol('WM_DELETE_WINDOW', self.closeButton)
    self.pack()

  def closeButton(self):
    tkMessageBox.showerror('Error', "閉じるボタンは使わないでください")

  def buttonCommand(self):
    self.new = True

  def buttonNew(self):
    if (self.new):
      self.new = False
      return True
    return False

  def change(self):
    self.change = True
  def get(self):
    return self.var.get()

rb = RadioButton(title = "SetMode")


class SetMode(OpenRTM_aist.DataFlowComponentBase):
  def __init__(self, manager):
    OpenRTM_aist.DataFlowComponentBase.__init__(self, manager)
    self._d_mode = RTC.TimedString(RTC.Time(0,0), "")
    self._modeOut = OpenRTM_aist.OutPort("mode", self._d_mode)
    self.log = OpenRTM_aist.Manager.instance().getLogbuf("SetMode")
    return   

  def onInitialize(self):
    self.log.RTC_INFO("onInitialize()")
    self.addOutPort("mode",self._modeOut)
    return RTC.RTC_OK

  def onActivated(self, ec_id):
    self.log.RTC_INFO("onActivated()")
    return RTC.RTC_OK

  def onDeactivated(self, ec_id):
    self.log.RTC_INFO("onDeactivated()")
    return RTC.RTC_OK

  def onExecute(self, ec_id):
    try:
      if (rb.buttonNew()):
        self._d_mode.data = rb.get()
        self.log.RTC_INFO("mode: " + str(self._d_mode.data))
        self._modeOut.write()
    except Exception as e:
      self.log.RTC_ERROR("Exception cought in onExecute(): " + str(e))
    return RTC.RTC_OK

  def onShutdown(self, ec_id):
    self.log.RTC_INFO("onShutdown()")
    rb.quit()
    return RTC.RTC_OK

def SetModeInit(manager):
  profile = OpenRTM_aist.Properties(defaults_str=mod_spec)
  manager.registerFactory(profile,
                          SetMode,
                          OpenRTM_aist.Delete)

def MyModuleInit(manager):
  SetModeInit(manager)
  # Create a component
  comp = manager.createComponent("SetMode")

def main():
  # Initialize manager
  mgr = OpenRTM_aist.Manager.init(sys.argv)

  # Set module initialization proceduer
  # This procedure will be invoked in activateManager() function.
  mgr.setModuleInitProc(MyModuleInit)

  # Activate manager and register to naming service
  mgr.activateManager()

  # run the manager in blocking mode
  # runManager(False) is the default
  #mgr.runManager()

  # If you want to run the manager in non-blocking mode, do like this
  mgr.runManager(True)
  rb.mainloop()

if __name__ == "__main__":
  main()
