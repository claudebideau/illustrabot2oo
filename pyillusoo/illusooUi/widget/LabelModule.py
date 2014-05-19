#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@date: Created on 2013/02/04
@author: bideau1
"""
from PySide import QtGui
from RadioButtonGroup import *
from CheckBoxGroup import *

__all__=['LabelModule']


    
class LabelModule(object):
    def __init__(self, name, label, module, param=None):
        self.name = name
        self.labelInfo = label
        self.param = param
        self.moduletype = module 
    
    def display(self):
        self.label = QtGui.QLabel(self.labelInfo)
        self.module = self.moduletype(self.param)
        if getattr(self.module,'display',None) != None:
            self.module.display()
        
    def __getattr__(self, attrb):
#        print '__getattribute__ ',name
        if attrb in ['info']:
            if type(self.module) in [QtGui.QCheckBox]:
                return (self.name , self.module.isChecked())
            if type(self.module) in [QtGui.QLineEdit, RadioButtonGroup]:
                return (self.name , self.module.text())
            if type(self.module) in [QtGui.QTextEdit]:
                return (self.name , self.module.toPlainText())
            return (self.name , type(self.module))
        if attrb in self.__dict__:
            return self.__dict__[attrb]
        raise AttributeError()
    
    def clear(self):
        "disable module"
        if type(self.module) in [QtGui.QCheckBox]:
            if self.module.isChecked(): 
                self.module.click()
            return
        if type(self.module) in [QtGui.QLineEdit, QtGui.QTextEdit]:
            self.module.setText('')
            return
        if type(self.module) in [RadioButtonGroup,CheckBoxGroup]:
            self.module.clear()
        return
    
    def setValues(self, attr):
        if type(self.module) in [QtGui.QCheckBox]:
            attr=bool(int(attr))
            if (attr and not self.module.isChecked()) or (not attr and self.module.isChecked()):
                self.module.click()
            return
        if type(self.module) in [QtGui.QLineEdit, QtGui.QTextEdit]:
            self.module.setText(attr)
            return
        if type(self.module) in [RadioButtonGroup, CheckBoxGroup]:
            self.module.setValues(attr)
        return

    def getValues(self):
        """ @return list of tuple (chkboxname, value) """
        if type(self.module) in [QtGui.QCheckBox]:
            return (self.name , self.module.isChecked())
        if type(self.module) in [QtGui.QLineEdit]:
            return (self.name , self.module.text())
        if type(self.module) in [QtGui.QTextEdit]:
            return (self.name , self.module.toPlainText())
        if type(self.module) in [RadioButtonGroup, CheckBoxGroup]:
            return (self.name , self.module.getValues())
        return (self.name , type(self.module))


    def fromXml(self, elt):
        if type(self.module) in [QtGui.QCheckBox]:
            self.setValues(bool(int(elt.attribute(self.name))))
            return
        if type(self.module) in [QtGui.QLineEdit]:
            self.setValues(elt.attribute(self.name))
            return
        if type(self.module) in [QtGui.QTextEdit]:
            self.setValues(elt.attribute(self.name))
            return 
        if type(self.module) in [RadioButtonGroup]:
            self.module.fromXml(elt)
            return 
        if type(self.module) in [CheckBoxGroup]:
            self.module.setValues(elt.attribute(self.name))
            return 

        return

    def toXml(self, elt):
        for k, v in self.getValues():
            elt.setAttribute(k, v)
        return elt
    
   