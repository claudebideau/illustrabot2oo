#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PySide import QtCore, QtGui
import array, re
import ParamDialog


__all__ = ['ElementWidget', 'LedStatus']

LOW = 0
HIGH = 1
CLOCKWISE     = LOW
ANTICLOCKWISE = HIGH


class LedStatus(QtGui.QLabel):
    
    __STATE__ ={ True: "red-led-on-th.png", False : "red-led-off-th.png" }
    def __init__(self, parent=None):
        super(LedStatus, self).__init__(parent)

        self.__led__= {}
        for k,v in self.__STATE__.items():
            self.__led__[k] = QtGui.QIcon(v)
            
        self.__build__()
        self.updateState(True)

    def __build__(self):
        self.setEnabled(False)
        self.setAlignment(QtCore.Qt.AlignCenter)
        self.setSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        self.setAutoFillBackground(True)
        self.setMaximumSize(16, 16)

    def updateState(self,value=True):
        pixmap = self.__led__[value].pixmap(QtCore.QSize(16, 16), QtGui.QIcon.Normal, QtGui.QIcon.On)
        self.setPixmap(pixmap)
        # print "value= ",value, " / ",self.__led__[value] , " / not pixmap.isNull() = ", not pixmap.isNull()
        self.setEnabled(not pixmap.isNull())

        

class LedColor(QtGui.QLabel):
    
    __STATE__ ={ 'blue': "icone/ledblue.png", 'lightblue': "icone/ledlightblue.png",
                 'green': "icone/ledgreen.png", 'lightgreen': "icone/ledlightgreen.png",
                 'red': "icone/ledred.png", 'orange': "icone/ledorange.png", 'purple': "icone/ledpurple.png" , 'yellow': "icone/ledyellow.png"}
    def __init__(self, parent=None):
        super(LedColor, self).__init__(parent)

        self.__led__= {}
        for k,v in self.__STATE__.items():
            self.__led__[k] = QtGui.QIcon(v)
            
        self.__build__()
        self.updateState()

    def __build__(self):
        self.setEnabled(False)
        self.setAlignment(QtCore.Qt.AlignCenter)
        self.setSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        self.setAutoFillBackground(True)
        self.setMaximumSize(16, 16)

    def updateState(self,value='red'):
        pixmap = self.__led__[value].pixmap(QtCore.QSize(16, 16), QtGui.QIcon.Normal, QtGui.QIcon.On)
        self.setPixmap(pixmap)
        self.setEnabled(not pixmap.isNull())
        
        
class ElementWidget(QtGui.QWidget):

    LED_LEVEL=1
    STATE2COLOR={0:'red', 1 :'orange', 2: 'green' }
    SPEED_TABLE =[('M0',1),('M1',2),('M2',4)]
    CHECK_BOX_VALUE = { False : QtCore.Qt.CheckState.Unchecked, True :QtCore.Qt.CheckState.Checked }

    def __init__(self, rpc, name, env=[None, None, None], parent=None):
        super(ElementWidget, self).__init__(parent)

        self.__name__ = name
        self.__rpc__  = rpc
        self.env      = env
        self.status   = env[1]
        self.progress = env[2]
        self.NBWORD =4

        self.__info_mth__ = "xml.element.info"
        self.__step_mth__ = "xml.element.step"
        self.__cal_mth__ = "xml.element.calibrate"
        self.__speed_mth__ = "xml.element.speed"
        self.__pos_reset_mth__ = "xml.element.pos.reset"
        self.__pos_min_mth__ = "xml.element.pos.min"
        self.__pos_max_mth__ = "xml.element.pos.max"
        self.__info_act__  = getattr(self.__rpc__,self.__info_mth__)
        self.__step_act__  = getattr(self.__rpc__,self.__step_mth__)
        self.__cal_act__  = getattr(self.__rpc__,self.__cal_mth__)
        self.__speed_act__  = getattr(self.__rpc__,self.__speed_mth__)
        self.__pos_reset_act__  = getattr(self.__rpc__,self.__pos_reset_mth__)
        self.__pos_min_act__  = getattr(self.__rpc__,self.__pos_min_mth__)
        self.__pos_max_act__  = getattr(self.__rpc__,self.__pos_max_mth__)
        self.__display__()
        self.do_refresh()
      
        
    def __display__(self):
        grid = QtGui.QGridLayout()

        #horizontal layout
        # 1) name 
        # 2) direction
        # 3) current position
        # 4) sensor
        # 5) number of step
        # 6) button step
        i=0
        grid.addWidget(self.__label__(self.__name__), 0, i); i+=1
        grid.addWidget(self.__directionButton__(), 0, i); i+=1
        grid.addWidget(self.__speedBox__(),   0, i); i+=1
        grid.addWidget(self.__currentPos__(), 0, i); i+=1
        grid.addWidget(self.__sensorIcon__(), 0, i); i+=1
        grid.addWidget(self.__stepNumber__(), 0, i); i+=1
        grid.addWidget(self.__stepAction__(), 0, i); i+=1
        self.setLayout(grid)
        
        
    def __label__(self,name):
        groupBox = QtGui.QGroupBox("name")
        groupBox.setFlat(True)
        labelLayout = QtGui.QVBoxLayout()
       
        label = QtGui.QLabel(self)
        label.setAlignment(QtCore.Qt.AlignJustify | QtCore.Qt.AlignLeft)        
        label.setText(name)
        labelLayout.addWidget(label)
        self.__ledState__ = LedColor(self)
        labelLayout.addWidget(self.__ledState__)

        
        groupBox.setLayout(labelLayout)
        
        return groupBox

    def __directionButton__(self):
        groupBox = QtGui.QGroupBox("Direction")
        groupBox.setFlat(True)
        radioLayout = QtGui.QVBoxLayout()
        self.__dirbox__ = {}
        first=True
        for name in ['clockwise','anticlockwise']:
            radiobutton = QtGui.QRadioButton(name)
            radioLayout.addWidget(radiobutton)
            if first: radiobutton.toggle()
            first=False
            self.__dirbox__[name] = radiobutton
        groupBox.setLayout(radioLayout)

        return groupBox


    def __speedBox__(self):
        groupBox = QtGui.QGroupBox("speed")
        groupBox.setFlat(True)
        checkLayout = QtGui.QVBoxLayout()
        self.__speedbox__ = {}
        for name,mask in self.SPEED_TABLE:
            checkbox = QtGui.QCheckBox(name)
            checkLayout.addWidget(checkbox)
            self.__speedbox__[name] = checkbox
            self.__speedbox__[name].stateChanged.connect(self.__updateSpeed__)
        groupBox.setLayout(checkLayout)

        return groupBox

    def __refreshSpeed__(self):
        res = self.__speed_act__(*[self.__name__])
        for name,mask in self.SPEED_TABLE:
            v= ((res['mask'] & mask) == 0)
            if v:
                self.__speedbox__[name].hide()
            else:
                self.__speedbox__[name].show()
        for name,mask in self.SPEED_TABLE:
            v = ((res['speed'] & mask) != 0)
            # if self.__speedbox__[name].isChecked() != v:
            self.__speedbox__[name].setCheckState(self.CHECK_BOX_VALUE[v])
        return

    
    def __updateSpeed__(self):
        value = 0
        for name,mask in self.SPEED_TABLE:
            if self.__speedbox__[name].isChecked():
                value += mask
        # apply new speed
        res = self.__speed_act__(*[self.__name__, value])
        # self.__refreshSpeed__()
        
                
        return 
    
    def __sensorIcon__(self):
        groupBox = QtGui.QGroupBox("sensor")
        groupBox.setFlat(True)
        ledLayout = QtGui.QVBoxLayout()
        self.__led__ = LedStatus(self)
        ledLayout.addWidget(self.__led__)
        groupBox.setLayout(ledLayout)
        
        return groupBox     
        
    def __currentPos__(self):
        groupBox = QtGui.QGroupBox("position")
        groupBox.setFlat(True)
        posLayout = QtGui.QVBoxLayout()
        self.__pos__ = QtGui.QLabel(self)
        self.__pos__.setAlignment(QtCore.Qt.AlignJustify | QtCore.Qt.AlignLeft)        
        self.__pos__.setText("0")
        self.__razButton__ = QtGui.QPushButton("&Reset")
        self.__razButton__.clicked.connect(self.do_raz)
        posLayout.addWidget(self.__pos__)        
        posLayout.addWidget(self.__razButton__)
        posLayout.addStretch(1)
        groupBox.setLayout(posLayout)

        return groupBox     
        
    def __stepNumber__(self):
        groupBox = QtGui.QGroupBox("number of step")
        groupBox.setFlat(True)
        numberLayout = QtGui.QVBoxLayout()
        self.__slider__ = QtGui.QSlider(QtCore.Qt.Horizontal)
        self.__slider__.setMinimum(1)
        self.__slider__.setMaximum(100)
        numberLayout.addWidget(self.__slider__)
        self.__valueSpinBox__ = QtGui.QSpinBox()
        self.__valueSpinBox__.setRange(1, 100)
        self.__valueSpinBox__.setSingleStep(1)
        numberLayout.addWidget(self.__valueSpinBox__)
        self.__slider__.valueChanged[int].connect(self.__valueSpinBox__.setValue)
        self.__valueSpinBox__.valueChanged[int].connect(self.__slider__.setValue)

        groupBox.setLayout(numberLayout)
        
        return groupBox  

    def __stepAction__(self):
        groupBox = QtGui.QGroupBox("Execute Step")
        groupBox.setFlat(True)
        buttonLayout = QtGui.QGridLayout()
        self.__stepButton__ = QtGui.QPushButton("&Do Step(s)")
        self.__refreshButton__ = QtGui.QPushButton("&Refresh")
        self.__calibrateButton__ = QtGui.QPushButton("&Calibrate")
        self.__stopCalibrateButton__ = QtGui.QPushButton("&Stop Calibrate")
        self.__paramButton__ = QtGui.QPushButton("&Update Field(s)")
        # self.__minButton__ = QtGui.QPushButton("&Min")
        # self.__maxButton__ = QtGui.QPushButton("M&ax")
        # associated function
        self.__stepButton__.clicked.connect(self.do_step)
        self.__refreshButton__.clicked.connect(self.do_refresh)
        self.__calibrateButton__.clicked.connect(self.do_calibrate)
        self.__stopCalibrateButton__.clicked.connect(self.do_stopCalibrate)
        # self.__minButton__.clicked.connect(self.do_min)
        # self.__maxButton__.clicked.connect(self.do_max)
        self.__paramButton__.clicked.connect(self.do_update)
        buttonLayout.addWidget(self.__stepButton__,0,0)
        buttonLayout.addWidget(self.__refreshButton__,1,0)
        buttonLayout.addWidget(self.__calibrateButton__,0,1)
        buttonLayout.addWidget(self.__stopCalibrateButton__,1,1)
        # buttonLayout.addWidget(self.__minButton__,0,2)
        # buttonLayout.addWidget(self.__maxButton__,1,2)
        buttonLayout.addWidget(self.__paramButton__,0,2)
        groupBox.setLayout(buttonLayout)
        
        return groupBox       

    def do_step(self):
        # get number of step
        # print self.__slider__.value()
        if self.__dirbox__['clockwise'].isChecked():
            dir = CLOCKWISE
        else:
            dir = ANTICLOCKWISE
        res = self.__step_act__(*[self.__name__, dir ,self.__slider__.value() ])
        self.do_refresh()
        # print res

    def __update_pos__(self,value):
        self.__pos__.setText("%d"%value)
        return

    def do_refresh(self,auto=False):
        # get number of step
        res = self.__info_act__(*[self.__name__])

        # print "res sensor =", res
        self.__ledState__.updateState(self.STATE2COLOR[res['state']] )
        self.__led__.updateState(res['sensor'] == self.LED_LEVEL )
        if not auto:
            if res['dir'] ==CLOCKWISE:
                self.__dirbox__['clockwise'].toggle()
            else:
                self.__dirbox__['anticlockwise'].toggle()
        else:
            sys.stdout.write(".")
        self.__update_pos__(res['current'])
        self.__refreshSpeed__()
            
    def do_calibrate(self):
        # get number of step
        # print self.__slider__.value()
        res = self.__cal_act__(*[self.__name__, 'start' ])
        self.do_refresh()
        # print res

    def do_stopCalibrate(self):
        # get number of step
        # print self.__slider__.value()
        res = self.__cal_act__(*[self.__name__, 'stop' ])
        self.do_refresh()
        
    def do_raz(self):
        res = self.__pos_reset_act__(*[self.__name__ ])
        self.do_refresh()

    # def do_min(self):
        # res = self.__info_act__(*[self.__name__])
        
        # i, ok = QtGui.QInputDialog.getInteger(self,"Fill Min value", "min value:", res['min'])
        # if ok:
            # res = self.__pos_min_act__(*[self.__name__, i ])
        # return

    # def do_max(self):
        # res = self.__info_act__(*[self.__name__])
        # i, ok = QtGui.QInputDialog.getInteger(self,"Fill Max value", "max value:", res['max'])
        # if ok:
            # res = self.__pos_max_act__(*[self.__name__, i ])
        # return

    def do_update(self):
        res = self.__info_act__(*[self.__name__])
        # print res
        param = { 'Minimum': ( [res['min'],res['minAngle']], [-180,180] ), 'Maximum': ( [res['max'],res['maxAngle']], [-180,180] )}
        tabdialog = ParamDialog.ParamDialog(param)
        res, ok =  tabdialog.exec_()
        if ok:
            for k,v,d in res:
                if k == 'Minimum':
                    res = self.__pos_min_act__(*[self.__name__, v, d ])
                elif k == 'Maximum':
                    res = self.__pos_max_act__(*[self.__name__, v, d ])
                else:
                    raise Exception( "update : not supported for '%s'"%k)

        return
        