#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@date: Created on 2013/02/04
@author: bideau1
"""
from PySide import QtGui
from LabelModule import LabelModule


class GenericLayout(QtGui.QWidget):

    XML_ELT_NAME='generic'

    def __init__(self, parent=None, widgetlist=[]):
        super(GenericLayout, self).__init__(parent)
        self.widgetlist=widgetlist
        self.widgetname=[]
        self.widget={}


    def display(self):
        self.layout = QtGui.QGridLayout()
        i=0
        for widget in self.widgetlist:
            
            if type(widget)== LabelModule:
                self.widgetname.append(widget.name)
                widget.display()
                self.layout.addWidget(widget.label, i, 0)
                self.layout.addWidget(widget.module, i, 1)
                self.widget[widget.name]=widget
            else:
                w = widget()
                self.widgetname.append(w.name)
                w.display()
                self.layout.addWidget(w, i, 1)
                self.widget[w.name]=w

            i+=1
        self.setLayout(self.layout)
    

    def clear(self):
        """ clear all instanciated widget """
        for v in self.widget.values():
            v.clear()
        return

    def setValues(self, attr=[]):
        """ set (name,state) of the checkBox 
        @param attr: list of tuple (chkboxname,value) """
        for k,v in attr:
            self.widget[k].setValues(v)
        return

 
    def getValues(self):
        """ @return list of tuple (chkboxname, value) """
        return [self.widget[k].getValues() for k in self.widgetname]

    def fromXml(self, elt):
        if elt.tagName() == self.XML_ELT_NAME:
            for name in self.widgetname:
                if type(self.widget[name])== LabelModule:
                    self.widget[name].setValues(elt.attribute(name))
                else:
                    self.widget[name].fromXml(elt)
        return

    def toXml(self, doc,root):
        elt=doc.createElement(self.XML_ELT_NAME)
        for name in self.widgetname:
            if type(self.widget[name])== LabelModule:
                k,v=self.widget[name].getValues()
                #~ print k,v
                elt.setAttribute(k,v)
            else:
                self.widget[name].toXml(doc,elt)
        root.appendChild(elt)
        return elt

