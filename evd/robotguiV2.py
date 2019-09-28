
# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\logno\Desktop\LOGAN\Research\Python\robotguiV2.ui'
#
# Created by: PyQt4 UI code generator 5.11.3
# Written By: Logan Norman
#
# WARNING! All changes made in this file will be lost!

from PyQt4.QtGui import QApplication, QMainWindow, QMenu, QVBoxLayout, QSizePolicy, QMessageBox, QWidget, QPushButton, QSlider, QRadioButton
from PyQt4.QtGui import (QApplication, QCheckBox, QGridLayout, QGroupBox,
                             QMenu, QPushButton, QRadioButton, QVBoxLayout, QWidget, QSlider)
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import pyqtSignal

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

import serial
#ArduinoSerial = serial.Serial('COM6',115200) @hunter

import math
import time
import threading

global x1, x2, x3, x4, myUI
x1,x2,x3,x4,myUI=0,0,0,0,None

# option for input steering file
doSteering = True
steeringSteps = list()

# Stepper motor and disk information
disk_r   = 0.5*100 #cm
center   = (0,0)
#ball_inc = 0.2 #inches / rev
ball_inc = 0.508 # cm / rev

'''
@hunter
creating encoder emulator 
'''
import Emulator as emu
emulator = emu.Emulator()

class Ui_Dialog(QtCore.QObject):
    updatePosition = pyqtSignal()
    moveIt = pyqtSignal()
    stepperThread = None 
    currentStep = 0
    stepping = False

    def __init__(self):
        QtCore.QObject.__init__(self)
        self.myCanvas = None
        self.position = {'x':0.0, 'y':0.0}
        self.target = None
        self.updatePosition.connect(self.update)
        self.moveIt.connect(self.doMove)

    def getCanvas(self):
        return self.myCanvas

    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(903, 506)

        self.makeManualSteer()
        self.makeLabels()
        self.makeManualInput()
        
        self.figure = plt.figure() # @hunter figsize=(350,350)
        self.myCanvas = PlotCanvas(Dialog, width=3.5, height=3.5)
        self.myCanvas.move(40,60)
        
        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

        self.connectThem()

    def makeManualSteer(self):
        self.butClockwise = QtGui.QPushButton(Dialog)
        self.butClockwise.setGeometry(QtCore.QRect(400, 50, 130, 50))
        self.butClockwise.setObjectName("butClockwise")
        self.butCounterbutClockwise = QtGui.QPushButton(Dialog)
        self.butCounterbutClockwise.setGeometry(QtCore.QRect(540, 50, 130, 50))
        self.butCounterbutClockwise.setObjectName("butCounterbutClockwise")
        self.butForward = QtGui.QPushButton(Dialog)
        self.butForward.setGeometry(QtCore.QRect(400, 140, 130, 50))
        self.butForward.setObjectName("butForward")
        self.butBackward = QtGui.QPushButton(Dialog)
        self.butBackward.setGeometry(QtCore.QRect(540, 140, 130, 50))
        self.butBackward.setObjectName("butBackward")
        self.butSetPosition = QtGui.QPushButton(Dialog)
        self.butSetPosition.setGeometry(QtCore.QRect(540, 220, 130, 50))
        self.butSetPosition.setObjectName("butSetPosition")
        self.butZero = QtGui.QPushButton(Dialog)
        self.butZero.setGeometry(QtCore.QRect(155, 420, 120, 50))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.butZero.setFont(font)
        self.butZero.setObjectName("butZero")
        self.butQuit = QtGui.QPushButton(Dialog)
        self.butQuit.setGeometry(QtCore.QRect(450, 420, 120, 50))
        self.butQuit.setObjectName("butQuit")

        self.stepsForRotation = QtGui.QPlainTextEdit(Dialog)
        self.stepsForRotation.setGeometry(QtCore.QRect(700, 50, 120, 50))
        self.stepsForRotation.setObjectName("stepsForRotation")
        self.stepsForLinear = QtGui.QPlainTextEdit(Dialog)
        self.stepsForLinear.setGeometry(QtCore.QRect(700, 140, 120, 50))
        self.stepsForLinear.setObjectName("stepsForLinear")

        self.positionInputX = QtGui.QPlainTextEdit(Dialog)
        self.positionInputX.setGeometry(QtCore.QRect(420, 220, 110, 25))
        self.positionInputX.setObjectName("positionInputX")
        self.positionInputY = QtGui.QPlainTextEdit(Dialog)
        self.positionInputY.setGeometry(QtCore.QRect(420, 250, 110, 25))
        self.positionInputY.setObjectName("positionInputY")

    def makeLabels(self):
        self.label = QtGui.QLabel(Dialog)
        self.label.setGeometry(QtCore.QRect(480, 20, 121, 31))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(False)
        font.setWeight(50)
        self.label.setFont(font)
        self.label.setObjectName("label")
        self.label_2 = QtGui.QLabel(Dialog)
        self.label_2.setGeometry(QtCore.QRect(495, 110, 91, 31))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(False)
        font.setWeight(50)
        self.label_2.setFont(font)
        self.label_2.setObjectName("label_2")
        self.label_3 = QtGui.QLabel(Dialog)
        self.label_3.setGeometry(QtCore.QRect(720, 20, 81, 31))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(False)
        font.setWeight(50)
        self.label_3.setFont(font)
        self.label_3.setObjectName("label_3")
        self.label_4 = QtGui.QLabel(Dialog)
        self.label_4.setGeometry(QtCore.QRect(720, 110, 81, 31))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(False)
        font.setWeight(50)
        self.label_4.setFont(font)
        self.label_4.setObjectName("label_4")
        self.label_5 = QtGui.QLabel(Dialog)
        #self.label_5.setGeometry(QtCore.QRect(485, 330, 100, 31))
        #font = QtGui.QFont()
        #font.setPointSize(20)
        #font.setBold(False)
        #font.setWeight(50)
        #self.label_5.setFont(font)
        #self.label_5.setObjectName("label_5")
        self.label_6 = QtGui.QLabel(Dialog)
        self.label_6.setGeometry(QtCore.QRect(400, 220, 20, 20))
        self.label_6.setObjectName("label_6")
        self.label_7 = QtGui.QLabel(Dialog)
        self.label_7.setGeometry(QtCore.QRect(400, 250, 20, 20))
        self.label_7.setObjectName("label_7")
        self.label_8 = QtGui.QLabel(Dialog)
        self.label_8.setGeometry(QtCore.QRect(650, 450, 120, 20))
        self.label_8.setObjectName("label_8")
        self.currentPositionLabel = QtGui.QLabel(Dialog)
        self.currentPositionLabel.setGeometry(QtCore.QRect(770, 450, 120, 20))
        self.currentPositionLabel.setObjectName("currentPositionLabel")

    def makeManualInput(self):
        self.butRunStepper = QtGui.QPushButton(Dialog)
        self.butRunStepper.setGeometry(QtCore.QRect(400, 300, 130, 50))
        self.butRunStepper.setObjectName("butRunStepper")
        self.butResetStepper = QtGui.QPushButton(Dialog)
        self.butResetStepper.setGeometry(QtCore.QRect(540, 300, 130, 50))
        self.butResetStepper.setObjectName("butResetStepper")

    def connectThem(self):
        self.butClockwise.clicked.connect(self.doClockwise)
        self.butCounterbutClockwise.clicked.connect(self.doCounterClockwise)
        self.butForward.clicked.connect(self.doForward)
        self.butZero.clicked.connect(self.doZero)
        self.butBackward.clicked.connect(self.doBackward)
        self.butQuit.clicked.connect(self.doQuit)
        self.butSetPosition.clicked.connect(self.doSetPosition)
        self.butRunStepper.clicked.connect(self.doStartStepper)
        self.butResetStepper.clicked.connect(self.doResetStepper)

    def retranslateUi(self, Dialog):
        _translate = QtCore.QCoreApplication.translate
        Dialog.setWindowTitle(_translate("Dialog", "Dialog"))
        self.butClockwise.setText(_translate("Dialog", "Clockwise"))
        self.butCounterbutClockwise.setText(_translate("Dialog", "CounterClockwise"))
        self.butForward.setText(_translate("Dialog", "Forward"))
        self.butBackward.setText(_translate("Dialog", "Backward"))
        self.label.setText(_translate("Dialog", "Rotation"))
        self.label_2.setText(_translate("Dialog", "Linear"))
        self.label_3.setText(_translate("Dialog", "Steps"))
        self.label_4.setText(_translate("Dialog", "Steps"))
        #self.label_5.setText(_translate("Dialog", "Position"))
        self.label_6.setText(_translate("Dialog", "X :"))
        self.label_7.setText(_translate("Dialog", "Y :"))
        self.butZero.setText(_translate("Dialog", "zero"))
        self.stepsForRotation.setPlainText(_translate("Dialog", "0"))
        self.stepsForLinear.setPlainText('0')
        self.butQuit.setText(_translate("Dialog", "quit"))
        self.positionInputX.setPlainText('0')
        self.positionInputY.setPlainText('0')
        self.butSetPosition.setText(_translate("Dialog", "Set position"))
        self.label_8.setText(_translate("Dialog", "Current position: "))
        temp = '('+str(self.position['x'])+','+str(self.position['y'])+')'
        self.currentPositionLabel.setText(_translate("Dialog", temp))
        self.butRunStepper.setText(_translate("Dialog", "Start Stepper"))
        self.butResetStepper.setText(_translate("Dialog", "Reset Stepper"))
        
    def update(self):
        _translate = QtCore.QCoreApplication.translate
        temp = "({:.2f}, {:.2f})".format(self.position['x'], self.position['y'])
        self.currentPositionLabel.setText(_translate("Dialog", temp))
    
    def doResetStepper(self):
        self.currentStep = 0
    
    def doStartStepper(self):
        _translate = QtCore.QCoreApplication.translate
        if not self.stepping:
            self.butRunStepper.setText(_translate("Dialog", "Stop Stepper"))
            self.stepping = True
            self.stepperThread = threading.Thread(target=self.doStep)
            self.stepperThread.daemon = True
            self.stepperThread.start()
        else:
            self.stepping = False
            self.butRunStepper.setText(_translate("Dialog", "Start Stepper"))

    def doStep(self):
        while self.stepping and self.currentStep < len(steeringSteps):
            x,y = steeringSteps[self.currentStep]
            self.target = {'x':x, 'y':y}
            self.moveIt.emit()
            time.sleep(2) # we need time to get there
            self.currentStep += 1

    def doQuit(self):
        QtCore.QCoreApplication.instance().quit()
    
    def doSetPosition(self):
        '''
        Set the position manually.
        '''
        x,y = float(self.positionInputX.toPlainText()), float(self.positionInputY.toPlainText())
        self.target = {'x':x, 'y':y}
        self.moveIt.emit()

    def doMove(self):
        currentX, currentY = self.position['x'], self.position['y']
        targetX, targetY   = self.target['x'], self.target['y']

        # compute the angle
        import numpy as np
        currentAngle = np.angle(currentX + currentY * 1j, deg = True)
        targetAngle  = np.angle(targetX + targetY * 1j, deg = True)
        
        deltaTheta = targetAngle - currentAngle
        deltaDist  = (targetX**2+targetY**2)**0.5 - (currentX**2+currentY**2)**0.5

        # how many steps?
        angleSteps = deltaTheta
        distSteps  = 360. * deltaDist / ball_inc

        print("Moving to X = {} Y = {} in {} angle steps and {} linear steps".format(targetX, targetY, angleSteps, distSteps))
        _translate = QtCore.QCoreApplication.translate
        self.stepsForRotation.setPlainText(_translate("Dialog", str(abs(angleSteps))))
        self.stepsForLinear.setPlainText(_translate("Dialog", str(abs(distSteps))))
        print deltaDist, deltaTheta
        if angleSteps < 0:
            self.doCounterClockwise()
        else:
            self.doClockwise()
        if distSteps < 0:
            self.doBackward()
        else:
            self.doForward()
        self.position = {'x':targetX, 'y':targetY}
        self.update()
    
    def doClockwise(self):
        print("Clockwise")
        self.doSerialWrite('a', self.stepsForRotation, sleep=0.01)
                
    def doCounterClockwise(self):
        print("CounterClockwise")
        self.doSerialWrite('b', self.stepsForRotation, sleep=0.1)
        
    def doBackward(self):
        print("Backward")
        self.doSerialWrite('e', self.stepsForLinear, sleep=0.1)

    def doForward(self):
        print("Forward")
        self.doSerialWrite('f', self.stepsForLinear, sleep=0.1)
        
    def doZero(self):
        global x1, x2, x3, x4
        print("zero")
        var = 'e'
        ArduinoSerial.write(var.encode())
        time.sleep(5)
        x1,x2,x3,x4=0,0,0,0
        myCanvas.plot(x1, x2, x3, x4)

    def doSerialWrite(self, var, textEdit, sleep=0.1):
        #ArduinoSerial.write(var.encode()) @hunter
        emulator.setData(var, int(float(textEdit.toPlainText()))) # @hunter in case we get 0.0
        time.sleep(sleep)
        #c= str(textEdit.toPlainText())  @hunter
        #ArduinoSerial.write(c.encode()) @hunter

class PlotCanvas(FigureCanvas):

    def __init__(self, parent=None, width=3.2, height=3.2, dpi=100):
        
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
 
        FigureCanvas.__init__(self, fig)
        self.setParent(parent)
    
        FigureCanvas.setSizePolicy(self,
                                   QSizePolicy.Expanding,
                                   QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
        self.plot(x1, x2, x3, x4)
 
    def plot(self, x1, x2, x3, x4):
        '''
        Plotting information from arduino.
        xi: encoder counts, translates to degrees
        Units will be in cm
        Disk diameter ~ 100 cm
        '''
        
        ax = self.figure.add_subplot(111)
        ax.cla()
        circle = plt.Circle((0, 0), disk_r, color='b', fill=False)
        ax.set_xlim((-disk_r, disk_r))
        ax.set_ylim((-disk_r, disk_r))
        ax.add_artist(circle)
        
        def limits(ang):
            xh, xl = center[0] + disk_r * math.cos(ang), center[0] - disk_r * math.cos(ang)
            yh, yl = center[1] + disk_r * math.sin(ang), center[1] - disk_r * math.sin(ang)
            return [xl,xh],[yl,yh]

        linang1 = int(x1);
        linang1 = linang1 * math.pi / 180.;
        xlim,ylim = limits(linang1)
        ax.plot([xlim[0],xlim[1]], [ylim[0],ylim[1]], 'r')
        
        #linang2 = int(x2);                                 @hunter
        #linang2 = linang2 * math.pi / 180.;                @hunter
        #xlim,ylim = limits(linang2)                        @hunter
        #ax.plot([xlim[0],xlim[1]], [ylim[0],ylim[1]], 'b') @hunter
        
        dist1 = ball_inc * int(x3) / 360.;
        xl = center[0] + dist1 * math.cos(linang1);
        yl = center[1] + dist1 * math.sin(linang1);
        ax.plot(xl,yl, 'r*')
        
        #dist2 = ball_inc * int(x4) / 360.;          @hunter
        #xl = center[0] + dist2 * math.cos(linang2); @hunter
        #yl = center[1] + dist2 * math.sin(linang2); @hunter
        #ax.plot(xl,yl, 'b*')                        @hunter
        self.draw()
        return xl,yl
        
if doSteering:
    with open('steering.txt') as f:
        while True:
            lineVec = f.readline().split()
            if len(lineVec) < 1:
                break
            x,y = float(lineVec[0]),float(lineVec[1])
            steeringSteps.append([x,y])

#EncSerial = serial.Serial('COM5', 115200) @hunter
def reading():
    global myUI
    while True:
        #raw_reading = EncSerial.readline()                  @hunter
        #DATASPLIT= raw_reading.decode('utf-8').split(' , ') @hunter
        #x1 = DATASPLIT[0] @hunter
        #x2 = DATASPLIT[2] @hunter
        #x3 = DATASPLIT[1] @hunter
        #x4 = DATASPLIT[3] @hunter
        x1,x2,x3,x4 = emulator.getData()
        if myUI is not None:
            if myUI.getCanvas() is not None:
                x,y = myUI.getCanvas().plot(x1, x2, x3, x4)
                myUI.position['x'] = x
                myUI.position['y'] = y
                myUI.updatePosition.emit()

reading_thread = threading.Thread(target=reading) # @hunter , daemon=True)
reading_thread.daemon = True
reading_thread.start()

if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    Dialog = QtGui.QDialog()
    myUI = Ui_Dialog()
    myUI.setupUi(Dialog)
    Dialog.show()
    sys.exit(app.exec_())

