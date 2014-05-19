#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys, os
from PySide import QtGui, QtCore
import Element
import traceback

class Elements(QtGui.QWidget):
    
    def __init__(self, rpc, env=[None, None, None], parent=None):
        super(Elements, self).__init__(parent)
        self.env      = env
        self.status   = env[1]
        self.progress = env[2]

        try:
            self.setrpc(rpc)
        except Exception, e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
            pass

        self.__timer__ = QtCore.QTimer(self)
        self.__timer__.timeout.connect(lambda :self.do_refresh(True))
        self.setTimer(2)
        self.__timerSpinBox__.setValue(2)
            
    def setrpc(self,rpc):
        try:
            self.__rpc__ = rpc
            self.__mth__ = "xml.element.info"
            self.__info_mth__ = getattr(self.__rpc__,self.__mth__)
            self.__grid__ = QtGui.QGridLayout()

            self.__grid__.addWidget(self.__info__(),0,0)
            self.__grid__.setRowStretch(4, 1)
            self.setLayout(self.__grid__)
            
            if self.status != None:
                self.status.updateState(True)
            

        except Exception, e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
            if self.status != None:
                self.status.updateState(False)

            pass
        # self.itemExpanded.connect(self.treeExpanded)
        # self.itemChanged.connect(self.updateLevel)

    def __info__(self):
        groupBox = QtGui.QGroupBox()
        groupBox.setFlat(False)
        elementLayout = QtGui.QVBoxLayout()

        res = self.__info_mth__()
        self.__elements__= []
        
        elementLayout.addWidget(self.__autoRefreshButton__(), 0, 0 )
        i=1
        for elementname in res:
            print 'elementname=',elementname
            element = Element.ElementWidget(rpc=self.__rpc__,name=elementname,env=self.env,parent=self)
            self.__elements__.append(element)
            elementLayout.addWidget(element, i, 0 )
            i+=1
        elementLayout.addStretch(1)
        groupBox.setLayout(elementLayout)

        return groupBox

    def __autoRefreshButton__(self):
        groupBox = QtGui.QGroupBox()
        groupBox.setFlat(True)
        checkLayout = QtGui.QHBoxLayout()
        self.__checkBox__ = QtGui.QCheckBox("&AutoRefresh")
        self.__checkBox__.toggled.connect(lambda:self.do_refresh(True))
        checkLayout.addWidget(self.__checkBox__)
        self.__timerSpinBox__ = QtGui.QSpinBox()
        self.__timerSpinBox__.setRange(1, 10)
        self.__timerSpinBox__.setSingleStep(1)
        self.__timerSpinBox__.valueChanged[int].connect(self.setTimer)
        checkLayout.addWidget(self.__timerSpinBox__)
        sc = QtGui.QLabel("second(s)")
        checkLayout.addWidget(sc)

        groupBox.setLayout(checkLayout)

        return groupBox        
        
    def do_refresh(self,auto=False):
        if self.__checkBox__.isChecked():
            for elt in self.__elements__:
                elt.do_refresh(auto)
        return

    def setTimer(self,value):
        print "timer=",value
        self.__timer__.stop()
        self.__timer__.start(value*1000)
        return
        
if __name__ == '__main__':


    import xmlrpclib
    proxy = xmlrpclib.ServerProxy("http://172.25.195.58:8000")
    
    app = QtGui.QApplication(sys.argv)


    viewTx = Elements(rpc=proxy)
    viewTx.show()
    
    sys.exit(app.exec_())