#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PySide import QtCore, QtGui

__all__ = ['ParamDialog', 'minMaxTabCl']

class ParamDialog(QtGui.QDialog):
    def __init__(self, param={} , parent=None):
        super(TabDialog, self).__init__(parent)

        self._tabWidget = QtGui.QTabWidget()
        self._elt = {}
        
        for k,v in param.items():
            self._elt[k] = minMaxTabCl(k,v[0], v[1])
            self._tabWidget.addTab(self._elt[k], k)

        buttonBox = QtGui.QDialogButtonBox(QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel)

        buttonBox.accepted.connect(self.accept)
        buttonBox.rejected.connect(self.reject)

        mainLayout = QtGui.QVBoxLayout()
        mainLayout.addWidget(self._tabWidget)
        mainLayout.addWidget(buttonBox)
        self.setLayout(mainLayout)

        self.setWindowTitle("Tab Dialog")

    def getValue(self):
        return  [v.getValue() for k,v in self._elt.items()]

    def exec_(self):
        res= super(TabDialog, self).exec_()
        return self.getValue(), res

        
class minMaxTabCl(QtGui.QWidget):
    
    def __init__(self, tname, cvalue=[-2000,90], crange=[-180,180], parent=None):
        super(minMaxTabCl, self).__init__(parent)
        self._tname = tname
        self._cvalue= cvalue
        self._crange= crange

        stepLabel = QtGui.QLabel("%s step:"%self._tname)
        angleLabel = QtGui.QLabel("%s angle:"%self._tname)

        self._stepValueEdit = QtGui.QLineEdit("")
        self._stepValueEdit.setInputMask("#nnnnnn")
        self._stepValueEdit.setText("%d"%self._cvalue[0])

        self._angleSpinEdit = QtGui.QSpinBox()
        self._angleSpinEdit.setRange(self._crange[0], self._crange[1])
        
        self._angleSliderEdit = QtGui.QSlider(QtCore.Qt.Horizontal)
        self._angleSliderEdit.setMinimum(self._crange[0])
        self._angleSliderEdit.setMaximum(self._crange[1])
       

        self._angleSliderEdit.valueChanged[int].connect(self._angleSpinEdit.setValue)
        self._angleSpinEdit.valueChanged[int].connect(self._angleSliderEdit.setValue)
        self._angleSliderEdit.setValue(self._cvalue[1])
        
        mainLayout = QtGui.QVBoxLayout()
        mainLayout.addWidget(stepLabel)
        mainLayout.addWidget(self._stepValueEdit)
        mainLayout.addWidget(angleLabel)
        mainLayout.addWidget(self._angleSpinEdit)
        mainLayout.addWidget(self._angleSliderEdit)

        mainLayout.addStretch(1)
        self.setLayout(mainLayout)

    def getValue(self):
        return ( self._tname , int(self._stepValueEdit.text()), self._angleSpinEdit.value() )


if __name__ == '__main__':

    import sys

    app = QtGui.QApplication(sys.argv)

    param = { 'Minimum': ( [-2000,0], [-180,180] ), 'Maximum': ( [2000,135], [-180,180] )}
    tabdialog = ParamDialog(param)
    sys.exit(tabdialog.exec_())
