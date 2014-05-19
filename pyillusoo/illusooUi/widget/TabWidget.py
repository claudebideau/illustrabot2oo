#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
date: Created on 2013/02/04
author: bideau1
"""
from PySide import QtCore, QtGui, QtXml


class TabWidget(QtGui.QWidget):
    def __init__(self,  name="Tab Dialog", listTab=[], parent=None):
        super(TabWidget, self).__init__(parent)
        self.name = name
        self.tabWidget = QtGui.QTabWidget()
        # build tab
        self.tab=[]
        self.reftab={}
        self.tabname=[k for k,v in listTab]
        for k,v in listTab:
            widget= v(parent)
            widget.display()
            self.tabWidget.addTab(widget , k)
            self.tab.append((k,widget))
            self.reftab[k] = widget

        self.__buttonbox__()
        #layout
        mainLayout = QtGui.QVBoxLayout()
        mainLayout.addWidget(self.tabWidget)
        mainLayout.addWidget(self.InfoButton)
        self.setLayout(mainLayout)

        self.setWindowTitle(self.name)
        
    def __buttonbox__(self):
        self.InfoButton = QtGui.QPushButton("&Info")
        self.InfoButton.show()
        self.InfoButton.clicked.connect(self.information)

    def information(self):
        print self.getValues()

    def setValues(self,attr):
        return
    
    def getValues(self):
        return [ (k,v.getValues()) for k,v in self.tab]
            

    def fromXml(self, doc):
        self.doc = doc.documentElement()
        # loop on children tree
        n = self.doc.firstChild()
        while not n.isNull():
            e =n.toElement()
            if not e.isNull():
                #~ print self.__class__.__name__, 'fromXml', self.doc.tagName()+'.'+e.tagName(), self.tabname ,
                if e.tagName() in self.tabname:
                    #~ print 'match'
                    self.reftab[e.tagName()].fromXml(e)
                #~ else:
                    #~ print 'not match'
            n = n.nextSibling()            
        return

    def toXml(self, doc,root):
        for k,v in self.tab:
            #~ print 'toXml', k, type(v)
            v.toXml(doc,root)
        return


    def clear(self):
        for v in self.reftab.values():
            v.clear()
        return
