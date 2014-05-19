#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@date: Created on 2013/02/04
@author: bideau1
"""
from PySide import QtGui

__all__=['RadioButtonGroup']

class RadioButtonGroup(QtGui.QGroupBox):
    """ Build group of checkBox """
    
    def __init__(self, names=[]):
        """constructor 
        @param chkBoxNames: checkbox list names 
        """
        super(RadioButtonGroup, self).__init__()
        self.names = names
        self.rbox = {}
    
    def display(self):
        # build horizontal layout
        first=True
        self.layout = QtGui.QHBoxLayout()
        for name in self.names:
            radiobutton = QtGui.QRadioButton(name)
            self.layout.addWidget(radiobutton)
            if first: radiobutton.toggle()
            first=False
            self.rbox[name] = radiobutton
        self.setLayout(self.layout)

        return

    def clear(self):
        """ disable all checked checkbox """
        for k,v in self.rbox.items():
            #~ print type(v)
            if v.isChecked(): 
                v.click()
        return

    def setValues(self, name):
        """ set (name,state) of the checkBox 
        @param name: name of the active one """
        if len(self.rbox)==0: self.display()
        if name in self.names:
            b = self.rbox[name]
            if not b.isChecked():
                b.click()
        return

 
    def getValues(self):
        """ @return active radiobutton name """
        for name in self.names:
            if self.rbox[name].isChecked():
                return name
        # default first one
        return self.names[0]
