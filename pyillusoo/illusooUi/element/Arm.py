#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys, os
from PySide import QtGui, QtCore
import traceback
import math
import ArmDraw

__all__=['Arm']
        
class Arm(QtGui.QWidget):
    
    def __init__(self, rpc, env=[None, None, None], parent=None):
        super(Arm, self).__init__(parent)
        self.env      = env
        self.status   = env[1]
        self.progress = env[2]
        self.__rpc__ = rpc

        self.__info_mth__ = "xml.arm.info"        
        self.__set_mth__ = "xml.arm.set"
        self.__info_act__  = getattr(self.__rpc__,self.__info_mth__)
        self.__set_act__  = getattr(self.__rpc__,self.__set_mth__)        
        try:
            self.setrpc(rpc)
        except Exception, e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
            pass
        self.__layout__()

            
    def setrpc(self,rpc):
        try:
            self.__rpc__ = rpc

        except Exception, e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))

            pass

    def __layout__(self):
        layout = QtGui.QVBoxLayout()
        self.param = ArmDraw.armParamCl()
        layout.addWidget(self.param)

        scene = ArmDraw.RobotScene()
        scene.setSceneRect(QtCore.QRectF(-100, -300, 400, 400))
        self.scene = scene
        self.view = QtGui.QGraphicsView(self.scene)
        self.param.latitudeChanged.connect(self.scene.setLatitude)
        self.param.longitudeChanged.connect(self.scene.setLongitude)
        self.param.radiusChanged.connect(self.scene.setRadius)
        self.param.allChanged.connect(self.scene.setAll)
        self.param.allChanged.connect(self.sendParam)
        layout.addWidget(self.view)
        self.setLayout(layout)

    @QtCore.Slot(int,int,int)
    def sendParam(self,long,lat,rad):
        print [long, lat,rad ]
        res = self.__set_act__(*[long, lat,rad ])
        return
        
    # def __info__(self):
        # groupBox = QtGui.QGroupBox()
        # groupBox.setFlat(False)
        # elementLayout = QtGui.QVBoxLayout()

        # res = self.__info_mth__()
        # self.__Arm__= []
        
        # elementLayout.addWidget(self.__autoRefreshButton__(), 0, 0 )
        # i=1
        # for elementname in res:
            # print 'elementname=',elementname
            # element = Element.ElementWidget(rpc=self.__rpc__,name=elementname,env=self.env,parent=self)
            # self.__Arm__.append(element)
            # elementLayout.addWidget(element, i, 0 )
            # i+=1
        # elementLayout.addStretch(1)
        # groupBox.setLayout(elementLayout)

        # return groupBox

    # def __autoRefreshButton__(self):
        # groupBox = QtGui.QGroupBox()
        # groupBox.setFlat(True)
        # checkLayout = QtGui.QHBoxLayout()
        # self.__ledBox__ = QtGui.QCheckBox("&Toggle-Led level")
        # self.__ledBox__.toggled.connect(lambda:self.changeLedLevel())
        # checkLayout.addWidget(self.__ledBox__)
        # self.__checkBox__ = QtGui.QCheckBox("&Auto-Refresh")
        # self.__checkBox__.toggled.connect(lambda:self.do_refresh(True))
        # checkLayout.addWidget(self.__checkBox__)

        # self.__timerSpinBox__ = QtGui.QSpinBox()
        # self.__timerSpinBox__.setRange(1, 10)
        # self.__timerSpinBox__.setSingleStep(1)
        # self.__timerSpinBox__.valueChanged[int].connect(self.setTimer)
        # checkLayout.addWidget(self.__timerSpinBox__)
        # sc = QtGui.QLabel("second(s)")
        # checkLayout.addWidget(sc)

        # groupBox.setLayout(checkLayout)

        # return groupBox        
        
    # def do_refresh(self,auto=False, force=False):
        # if self.__checkBox__.isChecked() or force==True:
            # for elt in self.__Arm__:
                # elt.do_refresh(auto)
        # return

    # def changeLedLevel(self):
        # if self.__ledBox__.isChecked():
            # Element.ElementWidget.LED_LEVEL=0
        # else:
            # Element.ElementWidget.LED_LEVEL=1
        # self.do_refresh(force=True)
        # return
        
        
    # def setTimer(self,value):
        # print "timer=",value
        # self.__timer__.stop()
        # self.__timer__.start(value*1000)
        # return
        
if __name__ == '__main__':


    import xmlrpclib
    proxy = xmlrpclib.ServerProxy("http://172.25.195.58:8000")
    
    app = QtGui.QApplication(sys.argv)


    viewTx = Arm(rpc=proxy)
    viewTx.show()
    
    sys.exit(app.exec_())