#!/usr/bin/env python

############################################################################
##
############################################################################

from PySide import QtCore, QtGui
from PySide.QtCore import QSettings

import time
import sys, os.path
import traceback

import illusooUi
import xmlrpclib

 

class MainButton(QtGui.QVBoxLayout):
    
    LIST_BUTTON = [ ( "Boardrst",      'boardrstAction', True),
                    ( "Shutdown",      'boardShutdownAction', True),
                    ( "stop APPLI",      'stopAppliAction', True),
                    ( "calibrate",      'calibrateAction', True),
                ]
                
    def __init__(self, env=[None, None, None], tstLbl=None, parent=None):
        super(MainButton, self).__init__(parent)
        self.parent = parent
        self.tstLbl= tstLbl
        self.__rpc__ = None

        self.env      = env
        self.settings = env[0]
        self.status   = env[1]
        self.progress = env[2]
        
        self.addWidget(self.status )
        self.__button__ = []
        self.__setting__()
        for i in range(len(self.LIST_BUTTON)):
            param=self.LIST_BUTTON[i]
            button = QtGui.QPushButton(param[0])
            self.__button__.append(button)
            self.addWidget(button)
            button.clicked.connect(getattr(self,param[1]))
            button.setEnabled(param[2])
    
        #insert spacer at last item
        self.insertStretch(len(self.LIST_BUTTON)+1)

    def __setting__(self):
        self.tstDir=''
        if self.settings != None:
            self.__mem_info__ = []
            self.settings[1].lock()
            self.settings[0].beginGroup("MainButton")
        
            self.tstDir = self.settings[0].value("tstDir", '')
            print 'open test folder "%s"'%self.tstDir

            self.settings[0].endGroup()          
            self.settings[1].unlock()
   
            if self.tstLbl != None:
                self.tstLbl.setText('"%s" is currently opened'%self.tstDir)

    def setrpc(self,rpc):
        self.__rpc__ = rpc


    def getTstDir(self):
        root='c:/'
        if os.path.isdir(self.tstDir):
            root = self.tstDir
        flags = QtGui.QFileDialog.DontResolveSymlinks | QtGui.QFileDialog.ShowDirsOnly
        d = directory = QtGui.QFileDialog.getExistingDirectory(None,
                                                               u"Open Directory",
                                                               unicode(root), flags)
            
        #filename, _ = QtGui.QFileDialog.getOpenFileName(self, 'Open file', root, "Map Files (*.map)")
        if len(directory) >0:
            self.tstDir = directory
        if self.tstLbl != None:
            self.tstLbl.setText('"%s" is currently opened'%self.tstDir)

    def boardShutdownAction(self,item=None):
        self.__boardInitAction__(0,"Confirm Board Shutdown","Are you sure you want to shutdown the board ?")
        return

        
    def boardrstAction(self,item=None):
        self.__boardInitAction__(6,"Confirm Board Reset","Are you sure you want to reset the board ?")
        return

    def __boardInitAction__(self,value=6, msg1= "Confirm Board Reset",msg2="Are you sure you want to reset the board ?"):
        print '__boardInitAction__', value
        initAct= getattr(self.__rpc__,'xml.os.init')
        button = QtGui.QMessageBox.question(self.__button__[0], msg1, msg2, QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
        if button == QtGui.QMessageBox.Yes:
            try:
                initAct(*[value])
                if self.status != None: self.status.updateState(True)
            except Exception,e:
                exc_type, exc_obj, exc_tb = sys.exc_info()
                fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
                sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
                if self.status != None: self.status.updateState(False)
                pass
        return
        
    def stopAppliAction(self,item=None):
        button = QtGui.QMessageBox.question(self.__button__[1], "Confirm to stop appli ",
                    "Are you sure you want to stop appli  ?" ,
                    QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
        if button == QtGui.QMessageBox.Yes:
            try:
                self.__rpc__.system.shutdown("end client")
            except Exception,e:
                exc_type, exc_obj, exc_tb = sys.exc_info()
                fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
                sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
                if self.status != None: self.status.updateState(False)
                pass
        return

    def calibrateAction(self,item=None):
        calibrateAct= getattr(self.__rpc__,'xml.orientation.calibrate')
        button = QtGui.QMessageBox.question(self.__button__[1], "Confirm calibrate ",
                    "Are you sure you want to calibrate ?" ,
                    QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
        if button == QtGui.QMessageBox.Yes:
            try:
                calibrateAct()
            except Exception,e:
                exc_type, exc_obj, exc_tb = sys.exc_info()
                fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
                sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
                if self.status != None: self.status.updateState(False)
                pass
        return

    def close(self):
        
        if self.settings!=None:
            self.settings[1].lock()
        
            self.settings[0].beginGroup("MainButton")
        
            self.settings[0].setValue("tstDir", self.tstDir)

            self.settings[0].endGroup()      
            self.settings[1].unlock()

       
        
class CentralWidget(QtGui.QWidget):

    LIST_TAB = [
               ('Elements', illusooUi.Elements) ,
               ('RtTrace', illusooUi.RttraceBuffers) ,
               ('Arm', illusooUi.Arm) ,
           ]

    
    def __init__(self, parent=None, settings=None):
        super(CentralWidget, self).__init__(parent)
        
        self.parent = parent
        self.__rpc__ = None

        self.__layout__ = QtGui.QGridLayout()

        self.settings = settings
        self.status= illusooUi.IconStatus()
        self.status.updateState(True) 

        self.progress = QtGui.QProgressBar()
        
        self.__env__= [ self.settings, self.status, self.progress]

        self.progress.setMinimum(0)

        self.currentTestLabel = QtGui.QLabel('no file open')

        self.statusLayout = QtGui.QGridLayout()
        self.statusLayout.addWidget(self.status, 0, 0)
        self.statusLayout.addWidget(self.progress, 0, 1)
        self.statusLayout.addWidget(self.currentTestLabel, 0, 2)

        self.__button__ = MainButton(env=self.__env__,tstLbl=self.currentTestLabel)

        self.tabdialog = illusooUi.TabWidget(listTab=self.LIST_TAB,rpc=None, env = self.__env__)

        self.__build_layout__()

    def __build_layout__(self):
        i=0
        self.__layout__.addWidget(self.tabdialog, i, 0);
        self.__layout__.addLayout(self.__button__, i, 1); i+=1
        # console
        
        # self.__layout__.addLayout(self.consoleLayout, i, 0); i+=1

        # statusBar and progress 
        self.__layout__.addLayout(self.statusLayout, i, 0); i+=1
        
        self.setLayout(self.__layout__)



    def setrpc(self,rpc):
        self.__button__.setrpc(rpc)
        self.tabdialog.setrpc(rpc)

    def closeEvent(self,event):
        # print self.__class__.__name__ + '.closeEvent()'
        self.tabdialog.close()
        self.__button__.close()
        event.accept()
        

class MainWindow(QtGui.QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.setWindowTitle("supervisor Interface")
        self.__mutex__ = QtCore.QMutex()

        self.__log__()
        self.__setting__()
        self.createActions()
        self.createMenus()
        self.__rpc__ = None
        self.centralWidget = CentralWidget(None, (self.settings, self.__mutex__) )

        self.statusBar().showMessage("Ready")


    def __setting__(self):
        print "get setting"
        #self.settings = QSettings(QSettings.IniFormat,QSettings.SystemScope, '__ILLUSTRABOT2OO', '__settings')
        self.settings = QSettings(QSettings.IniFormat,QSettings.UserScope, '__ILLUSTRABOT2OO', '__settings')
        self.settings.setFallbacksEnabled(False)    # File only, not registry or or.
        
        self.__mutex__.lock()
        self.settings.beginGroup("Main")
        self.resize(self.settings.value("size", QtCore.QSize(1024, 850)))
        self.move(self.settings.value("pos", QtCore.QPoint(50, 50)))
        self.ip = self.settings.value("ip",  r'172.25.195.58')
        print "previous IP=",self.ip
        
        self.settings.endGroup()
        self.__mutex__.unlock()


    def __log__(self):
        self.windowlog = {}


    def rpc(self):
        # Try to connect to a board
        print self.ip
        text, ok = QtGui.QInputDialog.getText(self, self.tr("Board IP"),
                                              self.tr("IP address:"), QtGui.QLineEdit.Normal,
                                              self.ip)

        if ok and not (len(text) ==0):
            try:
                self.__rpc__ = xmlrpclib.ServerProxy("http://%s:8000"%str(text))
                self.statusBar().showMessage("Connected...")
                self.centralWidget.setrpc(self.__rpc__) 
                self.__get_level__()
                self.setCentralWidget(self.centralWidget)
                self.ip= str(text)
            except Exception,e:
                self.statusBar().showMessage("Failed to connect to %s"%text)
                sys.stderr.write( str(e))
                traceback.print_exc(file=sys.stderr)
                pass
            

        return


    def about(self):
        QtGui.QMessageBox.about(self, "About supervisor Debugger ",
                """The <b>supervisor debugger</b> is used to help to analyze .....<br>
                designer  : claude bideau<br>
                Copyright @ LPGL<br>
                """)

    def createActions(self):
        self.rpcAct = QtGui.QAction("&Connect Board...", self, shortcut="Alt+C",
                triggered=self.rpc)


        self.exitAct = QtGui.QAction("&Exit", self, shortcut="Alt+F4",
                triggered=self.close)

        self.aboutAct = QtGui.QAction("&About", self, triggered=self.about)


    def __get_level__(self):
        res = self.__rpc__.xml.trace.level.get()
        self.__level__ = getattr(self,'__level__',None)
        if self.__level__== None:
            self.__level__ ={}
            for k,v in res.items():
                self.__level__[k]= QtGui.QAction("%s"%k, self)
                self.__level__[k].toggled.connect(self.setLevel)
                #~ print k,v
                self.__level__[k].setCheckable(True)
                self.__level__[k].setChecked(v==1)
                self.traceMenu.addAction(self.__level__[k])
        else:
            for k,v in res.items():
                if k not in self.__level__:
                    self.__level__[k]= QtGui.QAction("%s"%k, self)
                    self.__level__[k].toggled.connect(self.setLevel)
                    #~ print k,v
                    self.__level__[k].setCheckable(True)
                    self.traceMenu.addAction(self.__level__[k])
                self.__level__[k].setChecked(v==1)
                    

        self.traceMenu.setEnabled(1)
        #~ self.traceMenu.addAction()

    def setLevel(self,item=None):
        for level,v in self.__level__.items():
            if v.isChecked():
                self.__rpc__.xml.trace.level.set(*[level,1])
            else:
                self.__rpc__.xml.trace.level.set(*[level,0])
        return

    def createMenus(self):
        self.mainMenu = self.menuBar().addMenu("&Main")
        self.mainMenu.addAction(self.rpcAct)
        self.mainMenu.addAction(self.exitAct)

        self.traceMenu = self.menuBar().addMenu("&TraceLevel")
        self.traceMenu.setDisabled(1)
        #~ self.helpMenu.addAction(self.aboutAct)

        self.helpMenu = self.menuBar().addMenu("&Help")
        self.helpMenu.addAction(self.aboutAct)

    def closeEvent(self, event):

        # Write window size and position to config file
        self.settings.beginGroup("Main")
        self.settings.setValue("size", self.size())
        self.settings.setValue("pos", self.pos())
        self.settings.setValue("ip", self.ip)
        self.settings.endGroup()      

        self.centralWidget.close()
    
        for n,w in self.windowlog.items():
            # print 'close request to ',n
            w.close()
        
        event.accept()
        
if __name__ == "__main__":

    app = QtGui.QApplication(sys.argv)


    mainWin = MainWindow()

    mainWin.show()
    sys.exit(app.exec_())
