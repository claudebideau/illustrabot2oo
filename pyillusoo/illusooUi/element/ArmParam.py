#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys, os
from PySide import QtGui, QtCore
import traceback
import math

__all__=['armParamCl']

class armParamCl(QtGui.QWidget):
    
    def __init__(self, cvalue=[-100,90, 0.5], crange=[-180,180], parent=None):
        super(armParamCl, self).__init__(parent)
        self._cvalue= cvalue
        self._crange= crange
        self.__layout__()

        
    def __layout__(self):
        latLabel     = QtGui.QLabel("arm latitude  (in degre): ")
        latLabel2    = QtGui.QLabel("scaled value: ")
        longLabel    = QtGui.QLabel("arm longitude (in degre): ")
        longLabel2   = QtGui.QLabel("scaled value : ")
        radiusLabel  = QtGui.QLabel("radius (0-1) : ")
        radiusLabel2 = QtGui.QLabel("scaled value : ")

        descInfo    = QtGui.QLabel("pi/2 or 90 degre <-> scaled Q2.15 ")
        
        self._latValueEdit = QtGui.QLineEdit("")
        self._latValueEdit.setInputMask("#nnn")
        self._latValueEdit2 = QtGui.QLineEdit("")
        self._latValueEdit2.setInputMask("#nnnnnnn")
        self._latValueEdit2.setEnabled(False)

        self._longValueEdit = QtGui.QLineEdit("")
        self._longValueEdit.setInputMask("#nnn")
        self._longValueEdit2 = QtGui.QLineEdit("")
        self._longValueEdit2.setInputMask("#nnnnnnn")
        self._longValueEdit2.setEnabled(False)

        self._radiusValueEdit = QtGui.QLineEdit("")
        self._radiusValueEdit.setInputMask("n.nnnnn")
        self._radiusValueEdit2 = QtGui.QLineEdit("")
        self._radiusValueEdit2.setInputMask("nnnnnn")
        self._radiusValueEdit2.setEnabled(False)

        
        self._latValueEdit.textChanged[unicode].connect(self.__latDegre2scaled__)
        self._longValueEdit.textChanged[unicode].connect(self.__longDegre2scaled__)
        self._radiusValueEdit.textChanged[unicode].connect(self.__radius2scaled__)

        self._latValueEdit.setText("%d"%self._cvalue[0])
        self._longValueEdit.setText("%d"%self._cvalue[1])
        self._radiusValueEdit.setText("%1.5f"%self._cvalue[2])

        mainLayout = QtGui.QGridLayout()
        mainLayout.setHorizontalSpacing(3)
        mainLayout.setVerticalSpacing(4)        
        mainLayout.addWidget(latLabel,  0, 0)
        mainLayout.addWidget(latLabel2,  0, 2)
        mainLayout.addWidget(self._latValueEdit,  0, 1)
        mainLayout.addWidget(self._latValueEdit2,  0, 3)
        mainLayout.addWidget(longLabel, 1, 0)
        mainLayout.addWidget(longLabel2, 1, 2)
        mainLayout.addWidget(self._longValueEdit,   1, 1)
        mainLayout.addWidget(self._longValueEdit2,  1, 3)
        mainLayout.addWidget(radiusLabel, 2, 0)
        mainLayout.addWidget(self._radiusValueEdit,   2, 1)
        mainLayout.addWidget(radiusLabel2, 2, 2)
        mainLayout.addWidget(self._radiusValueEdit2,   2, 3)
        mainLayout.addWidget(descInfo,   3, 0)
 
        self.setLayout(mainLayout)

    def __latDegre2scaled__(self,value):
        self.__degre2scaled__(self._latValueEdit2,value)
        return

    def __longDegre2scaled__(self,value):
        self.__degre2scaled__(self._longValueEdit2,value)
        return
        
    def __degre2scaled__(self, obj, value):
        if value == '-':
            value=0
        v = int(value)
        v *=2**15
        v /= 90
        obj.setText("%d"%v)
        return

    # def __latDegre2norm__(self,value):
        # self.__degre2norm__(self._latValueEdit,value)
        # return

    # def __longDegre2norm__(self,value):
        # self.__degre2norm__(self._longValueEdit,value)
        # return
        

    # def __degre2norm__(self, obj, value):
        # v = int(value)
        # v *= 90
        # v /= 2**15
        # obj.setText("%d"%v)
        # return
        
    def __radius2scaled__(self, value):
        v = float(value)
        v *=2**15
        self._radiusValueEdit2.setText("%5.0f"%v)
        return

    # def __radius2norm__(self, value):
        # v = float(value)
        # v /=2**15
        # self._radiusValueEdit.setText("%1.5f"%v)
        # return

    def getScaledValue(self):
        return ( int(self._latValueEdit2.text()), int(self._longValueEdit2.text()) , int(self._radiusValueEdit2.text()) )

    def getNormValue(self):
        return ( int(self._latValueEdit.text()), int(self._longValueEdit.text()) , float(self._radiusValueEdit.text()) )

        
if __name__ == '__main__':

   
    app = QtGui.QApplication(sys.argv)


    param = armParamCl()
    param.show()
    
    sys.exit(app.exec_())