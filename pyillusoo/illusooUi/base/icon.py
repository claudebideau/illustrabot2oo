#!/usr/bin/env python
'''

@author: Claude BIDEAU
@date: 18 nov. 2013
@contact: claude.bideau@gmail.com
@copyright: GPL

'''

from PySide import QtCore, QtGui


__all__ = ['IconStatus']




class IconStatus(QtGui.QLabel):
    
    __STATE__ ={ True: "s_okay.png", False : "s_error.png" }
    def __init__(self, parent=None):
        super(IconStatus, self).__init__(parent)

        self.__icon__= {}
        for k,v in self.__STATE__.items():
            self.__icon__[k] = QtGui.QIcon(v)
            
        self.__build__()
        self.updateState(True)


    def __build__(self):
        self.setEnabled(False)
        self.setAlignment(QtCore.Qt.AlignCenter)
        #~ self.statusWidget.setFrameShape(QtGui.QFrame.Box)
        self.setSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        #~ self.statusWidget.setBackgroundRole(QtGui.QPalette.Base)
        self.setAutoFillBackground(True)
        self.setMaximumSize(16, 16)

    def updateState(self,value=True):
        #~ print "icon = ",self.__STATE__[value]
        pixmap = self.__icon__[value].pixmap(QtCore.QSize(16, 16), QtGui.QIcon.Normal, QtGui.QIcon.On)
        self.setPixmap(pixmap)
        self.setEnabled(not pixmap.isNull())        
        