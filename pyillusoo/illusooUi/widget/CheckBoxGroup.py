#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@date: Created on 2013/02/04
@author: bideau1
"""
from PySide import QtGui

__all__=['CheckBoxGroup']

class CheckBoxGroup(QtGui.QGroupBox):
    """ Build group of checkBox """
    
    def __init__(self, names=[]):
        """constructor 
        @param chkBoxNames: checkbox list names 
        """
        super(CheckBoxGroup, self).__init__()
        self.chkButtonNames = names
        self.cbox = {}
    
    def display(self):
        # build horizontal layout
        self.layout = QtGui.QHBoxLayout()
        for name in self.chkButtonNames:
            box = QtGui.QCheckBox(name)
            self.layout.addWidget(box)
            self.cbox[name] = box
        self.setLayout(self.layout)

        return

    def clear(self):
        """ disable all checked checkbox """
        for k in self.cbox:
            if k.isChecked(): 
                k.click()
        return

    def setValues(self, attr=[]):
        """ set (name,state) of the checkBox 
        @param attr: list of tuple (chkboxname,value) """
#        print 'checkboxgroup attribute = ',attr
        if len(self.cbox)==0:        self.display()

        for k,v in attr:
            if k in self.chkButtonNames:
                cb = self.cbox[k]
                if v == False and cb.isChecked():
                    cb.click()
                elif v == True and not cb.isChecked():
                    cb.click()
        return

 
    def getValues(self):
        """ @return list of tuple (chkboxname, value) """
        return [(name, self.cbox[name].isChecked()) for name in self.chkButtonNames]

    def fromXml(self, child):
#        print 'fromXml',elt.tagName()
        attr = [(name, bool(int(child.attribute(name)))) for name in self.chkButtonNames]
        self.setValues(attr)

    def toXml(self, elt):
        for k, v in self.getValues():
            elt.setAttribute(k, v)
        return elt

class CheckBoxGroupMerge(CheckBoxGroup):
   
    
    def __init__(self, names=[],mergeAttrName=''):
        """constructor 
        @param chkBoxNames: checkbox list names 
        """
        super(CheckBoxGroupMerge, self).__init__(names=names)
        self.mergeAttrName=mergeAttrName

    def setValues(self,attr=[]):
        if len(self.cbox)==0:        self.display()        
        for k,v in attr:
            if k == self.mergeAttrName:
                super(CheckBoxGroupMerge,self).setValues([(kname, (kname in v)) for kname in self.chkButtonNames])
        

    def getValues(self):
        """ @return list of tuple (chkboxname, value) """
        return [(self.mergeAttrName,''.join([ k for k, v in super(CheckBoxGroupMerge,self).getValues() if v]) )]

    def fromXml(self, child):
#        print 'fromXml',elt.tagName()
        #only one attribute
        self.setValues([(self.mergeAttrName, child.attribute(self.mergeAttrName))])

    def toXml(self, elt):
        elt.setAttribute(self.mergeAttrName, self.getValues())
        return elt        
