#!/usr/bin/env python
'''

@author: Claude BIDEAU
@date: 5 may. 2014
@contact: claude.bideau@gmail.com
@copyright: GPL

'''

from PySide import QtCore, QtGui


__all__ = ['LedStatus']

class LedStatus(QtGui.QLabel):
    
    __STATE__ ={ True: "red-led-on-th.png", False : "red-led-off-th.png" }
    def __init__(self, parent=None):
        super(LedStatus, self).__init__(parent)

        self.__led__= {}
        for k,v in self.__STATE__.items():
            self.__led__[k] = QtGui.QIcon(v)
            
        self.__build__()
        self.updateState(False)


    def __build__(self):
        self.setEnabled(False)
        self.setAlignment(QtCore.Qt.AlignCenter)
        self.setSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        self.setAutoFillBackground(True)
        self.setMaximumSize(16, 16)

    def updateState(self,value=True):
        pixmap = self.__led__[value].pixmap(QtCore.QSize(16, 16), QtGui.QIcon.Normal, QtGui.QIcon.On)
        self.setPixmap(pixmap)
        self.setEnabled(not pixmap.isNull())        
        