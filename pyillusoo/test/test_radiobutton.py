#!/usr/bin/env python


from PySide import QtCore, QtGui



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
        

class ElementWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        super(ElementWidget, self).__init__(parent)
        grid = QtGui.QGridLayout()

        #horizontal layout
        # 1) name 
        # 2) direction
        # 3) sensor
        # 4) number of step
        # 5) button step
        grid.addWidget(self.__label__("element"), 0, 0)
        grid.addWidget(self.__directionButton__(), 0, 1)
        grid.addWidget(self.__sensorIcon__(), 0, 2)
        grid.addWidget(self.__stepNumber__(), 0, 3)
        grid.addWidget(self.__stepAction__(), 0, 4)
        self.setLayout(grid)

        self.setWindowTitle("Group Box")
        self.resize(480, 100)
        
        
    def __label__(self,name):
        groupBox = QtGui.QGroupBox("element")
        groupBox.setFlat(True)
        labelLayout = QtGui.QVBoxLayout()
       
        label = QtGui.QLabel(self)
        label.setAlignment(QtCore.Qt.AlignJustify | QtCore.Qt.AlignLeft)        
        label.setText(name)
        labelLayout.addWidget(label)
        
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

    def __sensorIcon__(self):
        groupBox = QtGui.QGroupBox("sensor")
        groupBox.setFlat(True)
        ledLayout = QtGui.QVBoxLayout()
        self.__led__ = LedStatus(self)
        ledLayout.addWidget(self.__led__)
        groupBox.setLayout(ledLayout)
        
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
        buttonLayout = QtGui.QVBoxLayout()
        self.__stepButton__ = QtGui.QPushButton("&Do Step(s)")
        self.__refreshButton__ = QtGui.QPushButton("&Refresh")
        self.__stepButton__.clicked.connect(self.do_step)
        self.__refreshButton__.clicked.connect(self.do_refresh)
        buttonLayout.addWidget(self.__stepButton__)
        buttonLayout.addWidget(self.__refreshButton__)
        groupBox.setLayout(buttonLayout)
        
        return groupBox       

    def do_step(self):
        # get number of step
        print self.__slider__.value()

    def do_refresh(self):
        # get number of step
        print "refresh"
        
        
class Window(QtGui.QWidget):
    def __init__(self, parent=None):
        super(Window, self).__init__(parent)

        grid = QtGui.QGridLayout()
        
        grid.addWidget(self.__radioButton__(), 0, 0)

        # grid.addWidget(self.createFirstExclusiveGroup(), 0, 0)
        grid.addWidget(self.createSecondExclusiveGroup(), 1, 0)
        grid.addWidget(self.createNonExclusiveGroup(), 0, 1)
        grid.addWidget(self.createPushButtonGroup(), 1, 1)
        self.setLayout(grid)

        self.setWindowTitle("Group Box")
        self.resize(480, 320)

    def __radioButton__(self):
        groupBox = QtGui.QGroupBox("Test Buttons")
        groupBox.setFlat(True)

        radioLayout = QtGui.QVBoxLayout()
        self.__rbox__ = {}
        self.__dump__ = {}
        first=True
        for name in ['clockwise','anticlockwise']:
            radiobutton = QtGui.QRadioButton(name)
            radioLayout.addWidget(radiobutton)
            if first: radiobutton.toggle()
            first=False
            self.__rbox__[name] = radiobutton
            # self.__rbox__[name].clicked.connect(self.show_hide)
        groupBox.setLayout(radioLayout)

        return groupBox
        
    def createFirstExclusiveGroup(self):
        groupBox = QtGui.QGroupBox("Exclusive Radio Buttons")

        radio1 = QtGui.QRadioButton("&Radio button 1")
        radio2 = QtGui.QRadioButton("R&adio button 2")
        radio3 = QtGui.QRadioButton("Ra&dio button 3")

        radio1.setChecked(True)

        vbox = QtGui.QVBoxLayout()
        vbox.addWidget(radio1)
        vbox.addWidget(radio2)
        vbox.addWidget(radio3)
        vbox.addStretch(1)
        groupBox.setLayout(vbox)

        return groupBox

    def createSecondExclusiveGroup(self):
        groupBox = QtGui.QGroupBox("E&xclusive Radio Buttons")
        groupBox.setCheckable(True)
        groupBox.setChecked(False)

        radio1 = QtGui.QRadioButton("Rad&io button 1")
        radio2 = QtGui.QRadioButton("Radi&o button 2")
        radio3 = QtGui.QRadioButton("Radio &button 3")
        radio1.setChecked(True)
        checkBox = QtGui.QCheckBox("Ind&ependent checkbox")
        checkBox.setChecked(True)

        vbox = QtGui.QVBoxLayout()
        vbox.addWidget(radio1)
        vbox.addWidget(radio2)
        vbox.addWidget(radio3)
        vbox.addWidget(checkBox)
        vbox.addStretch(1)
        groupBox.setLayout(vbox)

        return groupBox

    def createNonExclusiveGroup(self):
        groupBox = QtGui.QGroupBox("Non-Exclusive Checkboxes")
        groupBox.setFlat(True)

        checkBox1 = QtGui.QCheckBox("&Checkbox 1")
        checkBox2 = QtGui.QCheckBox("C&heckbox 2")
        checkBox2.setChecked(True)
        tristateBox = QtGui.QCheckBox("Tri-&state button")
        tristateBox.setTristate(True)
        tristateBox.setCheckState(QtCore.Qt.PartiallyChecked)

        vbox = QtGui.QVBoxLayout()
        vbox.addWidget(checkBox1)
        vbox.addWidget(checkBox2)
        vbox.addWidget(tristateBox)
        vbox.addStretch(1)
        groupBox.setLayout(vbox)

        return groupBox

    def createPushButtonGroup(self):
        groupBox = QtGui.QGroupBox("&Push Buttons")
        groupBox.setCheckable(True)
        groupBox.setChecked(True)

        pushButton = QtGui.QPushButton("&Normal Button")
        toggleButton = QtGui.QPushButton("&Toggle Button")
        toggleButton.setCheckable(True)
        toggleButton.setChecked(True)
        flatButton = QtGui.QPushButton("&Flat Button")
        flatButton.setFlat(True)

        popupButton = QtGui.QPushButton("Pop&up Button")
        menu = QtGui.QMenu(self)
        menu.addAction("&First Item")
        menu.addAction("&Second Item")
        menu.addAction("&Third Item")
        menu.addAction("F&ourth Item")
        popupButton.setMenu(menu)

        newAction = menu.addAction("Submenu")
        subMenu = QtGui.QMenu("Popup Submenu", self)
        subMenu.addAction("Item 1")
        subMenu.addAction("Item 2")
        subMenu.addAction("Item 3")
        newAction.setMenu(subMenu)

        vbox = QtGui.QVBoxLayout()
        vbox.addWidget(pushButton)
        vbox.addWidget(toggleButton)
        vbox.addWidget(flatButton)
        vbox.addWidget(popupButton)
        vbox.addStretch(1)
        groupBox.setLayout(vbox)

        return groupBox


if __name__ == '__main__':

    import sys

    app = QtGui.QApplication(sys.argv)
    clock = ElementWidget()
    clock.show()
    sys.exit(app.exec_())