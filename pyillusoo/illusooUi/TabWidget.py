#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
date: Created on 2013/02/04
author: bideau1
"""
from PySide import QtGui


class TabWidget(QtGui.QWidget):
    def __init__(self,  name="Dialog", listTab=[], rpc=None, env=[None, None, None], parent=None):
        super(TabWidget, self).__init__(parent)
        self.__rpc__     = rpc
        self.parent      = parent
        self.env         = env
        self.status      = env[1]
        self.progress    = env[2]
        
        self.name        = name
        self.__listTab__ = listTab
        self.tab=[]
        self.tabWidget = QtGui.QTabWidget()
    
    def setrpc(self,rpc):
        self.__rpc__ =rpc
        self.build()
        #~ self.clear()
        

    def build(self):
        # build tab
        for i in range(self.tabWidget.count()):
            self.tabWidget.removeTab(0)
        self.tab=[]
        self.reftab={}
        self.tabname=[k for k,v in self.__listTab__]
        for k,v in self.__listTab__:
            widget= v(parent=self.parent,rpc=self.__rpc__, env=self.env)
            self.tabWidget.addTab(widget , k)
            self.tab.append((k,widget))
            self.reftab[k] = widget
        #layout
        mainLayout = QtGui.QVBoxLayout()
        mainLayout.addWidget(self.tabWidget)
        self.setLayout(mainLayout)

        self.setWindowTitle(self.name)
        #~ self.setEnable()

    def clear(self):
        for v in self.reftab.values():
            v.clear()
        return

    def closeEvent(self,event):
        # print self.__class__.__name__ + '.closeEvent()'
    
        for k,w in self.tab:
            w.close()


