#!/usr/bin/env python

from PySide import QtCore, QtGui
import sys, time
import math

class armParamCl(QtGui.QWidget):
    
    latitudeChanged  = QtCore.Signal((int))
    longitudeChanged = QtCore.Signal((int))
    radiusChanged    = QtCore.Signal((int))
    allChanged       = QtCore.Signal((int),(int),(int))
    
    def __init__(self, cvalue=[45,0, 0.5], crange=[-180,180], parent=None):
        super(armParamCl, self).__init__(parent)
        self._cvalue= cvalue
        self._crange= crange
        self.__layout__()

        
    def __layout__(self):
        longLabel    = QtGui.QLabel("arm longitude (in degre): ")
        longLabel2   = QtGui.QLabel("scaled value : ")
        latLabel     = QtGui.QLabel("arm latitude  (in degre): ")
        latLabel2    = QtGui.QLabel("scaled value: ")
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

        self._buttonUpdate = QtGui.QPushButton("&Update")

        
        self._latValueEdit.textChanged[unicode].connect(self.__latDegre2scaled__)
        self._longValueEdit.textChanged[unicode].connect(self.__longDegre2scaled__)
        self._radiusValueEdit.textChanged[unicode].connect(self.__radius2scaled__)
        self._buttonUpdate.clicked.connect(self.__scaledAll__)

        
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
        mainLayout.addWidget(self._buttonUpdate,   4, 0)

        self.setLayout(mainLayout)

    def __scaledAll__(self):
        self.allChanged.emit(self.__longitudeValue,self.__latitudeValue,self.__radiusValue)
        
    def __latDegre2scaled__(self,value):
        if len(value)==0: return
        self.__latitudeValue = self.__degre2scaled__(self._latValueEdit2,value)
        self.latitudeChanged.emit(self.__latitudeValue)
        return

    def __longDegre2scaled__(self,value):
        if len(value)==0: return
        self.__longitudeValue = self.__degre2scaled__(self._longValueEdit2,value)
        self.longitudeChanged.emit(self.__degre2scaled__(self._longValueEdit2,value))
        return
        
    def __degre2scaled__(self, obj, value):
        if value == '-':
            value=0
        v = int(value)
        v *=2**15
        v /= 90
        obj.setText("%d"%v)
        return v

    def __radius2scaled__(self, value):
        if len(value)==0: return
        v = float(value)
        if v > 1.0:
            v = 1.0
        elif v < 0.0:
            v = 0.0
        v *=2**15
        self._radiusValueEdit2.setText("%5.0f"%v)
        self.__radiusValue = v
        self.radiusChanged.emit(v)
        return

    def getScaledValue(self):
        return ( int(self._latValueEdit2.text()), int(self._longValueEdit2.text()) , int(self._radiusValueEdit2.text()) )

    def getNormValue(self):
        return ( int(self._latValueEdit.text()), int(self._longValueEdit.text()) , float(self._radiusValueEdit.text()) )

TIMELINE_STATE2NAME= {QtCore.QTimeLine.NotRunning : 'Not running', QtCore.QTimeLine.Paused : 'Paused', QtCore.QTimeLine.Running : 'Running' }

class RobotPart(QtGui.QGraphicsItem):
    def __init__(self, parent=None):
        super(RobotPart, self).__init__(parent)

        self.color = QtGui.QColor(QtCore.Qt.lightGray)
        self.pixmap = None
        self.dragOver = False

        self.setAcceptDrops(True)

    def dragEnterEvent(self, event):
        if event.mimeData().hasColor() or \
          (isinstance(self, RobotHead) and event.mimeData().hasImage()):
            event.setAccepted(True)
            self.dragOver = True
            self.update()
        else:
            event.setAccepted(False)

    def dragLeaveEvent(self, event):
        self.dragOver = False
        self.update()
 
    def dropEvent(self, event):
        self.dragOver = False
        if event.mimeData().hasColor():
            self.color = QtGui.QColor(event.mimeData().colorData())
        elif event.mimeData().hasImage():
            self.pixmap = QtGui.QPixmap(event.mimeData().imageData())

        self.update()

class RobotBase(RobotPart):
    def boundingRect(self):
        return QtCore.QRectF(-30, -20, 20, 200)

    def paint(self, painter, option, widget=None):
        gradient = QtGui.QRadialGradient(-3, -3, 10)
        gradient.setColorAt(1, QtCore.Qt.yellow)

        painter.setBrush(QtGui.QBrush(gradient))    
        painter.drawRoundedRect(-10, -10, 50, 50, 25, 25, QtCore.Qt.RelativeSize)

        
class RobotArmElt(RobotPart):
    def boundingRect(self,size=100):
        return QtCore.QRectF(-5, -5, size, 10)

    def paint(self, painter, option, widget=None):
        gradient = QtGui.QRadialGradient(-3, -3, 10)
        gradient.setColorAt(1, QtCore.Qt.darkYellow)
        painter.setBrush(QtGui.QBrush(gradient))    
        painter.drawRoundedRect(self.boundingRect(), 50, 50,
                QtCore.Qt.RelativeSize)
        painter.drawEllipse(-5, -5, 10, 10)

class RobotUp(RobotPart):

    stopped = QtCore.Signal((str))
    
    def __init__(self):
        super(RobotUp, self).__init__()

        self.baseItem          = RobotBase(self)
        self.upperRightArmItem = RobotArmElt(self.baseItem)
        self.lowerRightArmItem = RobotArmElt(self.upperRightArmItem)

        self.timeline = QtCore.QTimeLine(1000)
        self.timeline.finished.connect(self.stop)
        settings = [
        #             item               position   
        #                                 x    y    
            ( self.baseItem,              0,  -18  ),
            ( self.upperRightArmItem,    15,   5  ),
        ]

        self.animations = []
        for item, pos_x, pos_y  in settings: 
            item.setPos(pos_x,pos_y)
        self._oldAngle= 90
        self.__requestedAngle= [90]
        self.applyAngle()
        
    @QtCore.Slot(int)
    def setAngle(self,arm1):
        self.__requestedAngle.append((arm1))
        self.applyAngle()


    @QtCore.Slot()
    def applyAngle(self):
        state=self.timeline.state()
        # print 'state = %s'%(TIMELINE_STATE2NAME[state])
        if self.timeline.state()== QtCore.QTimeLine.NotRunning:
            # self.animations = []
            if len( self.__requestedAngle ) > 0:
                angle = self.__requestedAngle.pop(0)
                # print 'old=',self._oldAngle, ' new=',angle
                
                settings = [
                #             item                          rotation at
                #                                          time 0  /  1
                    ( self.baseItem,                            0,        0  ),
                    ( self.upperRightArmItem,     -self._oldAngle,   -angle ),
                ]        
                for item, rotation1, rotation2 in settings: 
                    animation = QtGui.QGraphicsItemAnimation()
                    animation.setItem(item)
                    animation.setTimeLine(self.timeline)
                    animation.setRotationAt(0, rotation1)
                    animation.setRotationAt(1, rotation2)
                    self.animations.append(animation)
                self._oldAngle =  angle
                self.animations[0].setScaleAt(1, 1.05, 1.05)

                # print 'start'
                self.start()
        return


    def start(self):
        self.timeline.setLoopCount(1)
        self.timeline.setDuration(2000)        
        self.timeline.start()
        return

    def stop(self):
        # print "finish / state = %s"%TIMELINE_STATE2NAME[self.timeline.state()]
        self.timeline.stop()
        # self.stopped.emit('stopped')
        if len(self.__requestedAngle) > 0:
            self.applyAngle()
        return

    def boundingRect(self):
        return QtCore.QRectF()

    def paint(self, painter, option, widget=None):
        pass
        
class Robot(RobotPart):

    stopped = QtCore.Signal((str))
    
    def __init__(self):
        super(Robot, self).__init__()

        self.baseItem          = RobotBase(self)
        self.upperRightArmItem = RobotArmElt(self.baseItem)
        self.lowerRightArmItem = RobotArmElt(self.upperRightArmItem)

        self.timeline = QtCore.QTimeLine(1000)
        self.timeline.finished.connect(self.stop)
        settings = [
        #             item               position 
        #                                 x    y  
            ( self.baseItem,              0,  -18 ),
            ( self.upperRightArmItem,    15,  -10 ),
            ( self.lowerRightArmItem,    100,   0 ),

        ]

        self.animations = []
        for item, pos_x, pos_y in settings: 
            item.setPos(pos_x,pos_y)
        self._oldAngle=[45,45]
        self.__requestedAngle=[(45,45)]
        # self.setAngle(self.__requestedAngle[0],self.__requestedAngle[1])
        self.applyAngle()
        
    @QtCore.Slot(int, int)
    def setAngle(self,arm1,arm2):
        self.__requestedAngle.append((arm1,arm2))
        # print 'Robot.setAngle',arm1,arm2
        self.applyAngle()

    @QtCore.Slot()
    def applyAngle(self):
        state=self.timeline.state()
        # print 'state = %s'%(TIMELINE_STATE2NAME[state])
        if self.timeline.state()== QtCore.QTimeLine.NotRunning:
            # self.animations = []
            if len( self.__requestedAngle ) > 0:
                angle = self.__requestedAngle.pop(0)
                # print 'old=',self._oldAngle, ' new=',angle
                
                settings = [
                #             item                    rotation at
                #                                     time 0  /  1
                    ( self.baseItem,                           0,           0 ),
                    ( self.upperRightArmItem, -self._oldAngle[0],   -angle[0] ),
                    ( self.lowerRightArmItem,  self._oldAngle[1],    angle[1] ),
                ]        
                for item, rotation1, rotation2 in settings: 
                    animation = QtGui.QGraphicsItemAnimation()
                    animation.setItem(item)
                    animation.setTimeLine(self.timeline)
                    animation.setRotationAt(0, rotation1)
                    animation.setRotationAt(1, rotation2)
                    self.animations.append(animation)
                self._oldAngle =  angle
                self.animations[0].setScaleAt(1, 1.05, 1.05)

                # print 'start'
                self.start()
        return


    def start(self):
        self.timeline.setLoopCount(1)
        self.timeline.setDuration(2000)        
        self.timeline.start()
        return

    def stop(self):
        # print "finish / state = %s"%TIMELINE_STATE2NAME[self.timeline.state()]
        self.timeline.stop()
        # self.stopped.emit('stopped')
        if len(self.__requestedAngle) > 0:
            self.applyAngle()
        return

    def boundingRect(self):
        return QtCore.QRectF()

    def paint(self, painter, option, widget=None):
        pass

class RobotScene( QtGui.QGraphicsScene):

    angleChangedFront = QtCore.Signal((int),(int))
    angleChangedUp  = QtCore.Signal((int))
    
    def __init__(self,  parent=None):
        super(RobotScene, self).__init__(parent)
        self.robotFront = Robot()
        self.robotFront.scale(1.2, 1.2)
        self.robotFront.setPos(100, -20)
        self.addItem(self.robotFront)

        self.robotUp = RobotUp()
        self.robotUp.setPos(-100, -20)

        self.addItem(self.robotUp)
        self.angleChangedFront.connect(self.robotFront.setAngle)
        self.angleChangedUp.connect(self.robotUp.setAngle)
        self._value = [0,0,0]

    @QtCore.Slot(int)
    def setLongitude(self,long):
        self._value[0] = long
        self._applyAll()
        return

    @QtCore.Slot(int)
    def setLatitude(self,lat):
        self._value[1] = lat
        self._applyAll()
        return
        
    @QtCore.Slot(int)
    def setRadius(self,rad):
        self._value[2] = rad
        self._applyAll()
        return

    @QtCore.Slot(int,int,int)
    def setAll(self,long,lat,rad):
        self._value = [long,lat,rad]
        self._applyAll()
        # print long,lat,rad
        # value = self.calcFrontAngle(lat,rad)
        # norm_long = (90*long /2**15) + 90
        # value = [ (90*v /2**15) for v in long,lat]
        
        # self.angleChangedFront.emit(value[0],value[1])
        # self.angleChangedUp.emit(norm_long)
        return

    def _applyAll(self):
        value = self.calcFrontAngle(self._value[1],self._value[2])
        norm_long = (90*self._value[0] /2**15) + 90
        # value = [ (90*v /2**15) for v in long,lat]
        
        self.angleChangedFront.emit(value[0],value[1])
        self.angleChangedUp.emit(norm_long)
        return
        
    def calcFrontAngle(self,lat,rad):
        int_lat = 90.0*lat /2**15
        radius_value= float(rad)/(2**15)
        # print 'calcFrontAngle input: lat = %2.4f / rad = %d => %2.4f '%(int_lat,rad, radius_value)
        radivalue = math.acos(radius_value)
        alpha = math.degrees(radivalue)
        beta = 2*alpha
        # print 'calcFrontAngle value=',radivalue, alpha, int_lat+alpha, beta
        return int_lat+alpha,beta
        


if __name__== '__main__':

    app = QtGui.QApplication(sys.argv)

    class MainWindow(QtGui.QMainWindow):
        def __init__(self):
            QtGui.QMainWindow.__init__(self)
            layout = QtGui.QVBoxLayout()
            self.param = armParamCl()
            layout.addWidget(self.param)

            scene = RobotScene()
            scene.setSceneRect(QtCore.QRectF(-100, -300, 400, 400))
            self.scene = scene
            self.view = QtGui.QGraphicsView(self.scene)
            self.param.latitudeChanged.connect(self.scene.setLatitude)
            self.param.longitudeChanged.connect(self.scene.setLongitude)
            self.param.radiusChanged.connect(self.scene.setRadius)
            self.param.allChanged.connect(self.scene.setAll)
            layout.addWidget(self.view)
            self.widget = QtGui.QWidget()
            self.widget.setLayout(layout)
            self.setCentralWidget(self.widget)
            self.setWindowTitle("my scene")    

    mainWindow = MainWindow()
    mainWindow.setGeometry(100, 100, 800, 500)
    mainWindow.show()

    
    sys.exit(app.exec_()) 