#!/usr/bin/env python
'''

@author: Claude BIDEAU
@date: 2 may. 2014
@contact: claude.bideau@gmail.com

'''
import sys
from PySide import QtCore, QtGui


__all__ = ['OutputWindow', 'logWindow']


class OutputWindow(QtGui.QTextEdit):
    def write(self, txt):
        self.insertPlainText(txt)
        QtCore.QCoreApplication.instance().processEvents()
        

class logWindow(QtGui.QMainWindow):
    def __init__(self, name, param,parent=None):
        super(logWindow, self).__init__(parent=parent)
        self.name = name
        print 'Create windows %s'%self.name
        self.setWindowTitle(name)
        self.resize(512, 512)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        
        self.log = OutputWindow()
        if param[1] != None:
            self.log.setTextColor(param[1])
        if param[0] == sys.stdout:
            sys.stdout = self.log
        elif param[0] == sys.stderr:
            sys.stderr = self.log
        self.log.show()

    def closeEvent(self, event):
        if self.maybeSave():
            event.accept()
        else:
            event.ignore()
        self.log.close()
        return True

            
    def maybeSave(self):
        #~ return True
        print 'maybeSave %s'%self.name
        ret = QtGui.QMessageBox.warning(self, self.tr(self.name),
                self.tr("Do you want to save your log '%s'?"%self.name),
                QtGui.QMessageBox.Yes | QtGui.QMessageBox.Default,
                QtGui.QMessageBox.No)
        if ret == QtGui.QMessageBox.Yes:
            return self.save()
        elif ret == QtGui.QMessageBox.Cancel:
            return False

        return True

    def save(self):

        logv= self.log.toPlainText()
        print 'Save %s'%self.name
        fd=open("%s.log"%self.name,'wb')
        fd.write(logv.encode('utf8'))
        fd.close()
        return True
