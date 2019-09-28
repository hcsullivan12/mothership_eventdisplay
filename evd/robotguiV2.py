
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
stepping = False

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
    stepperThread = None 
    currentStep = 0

    def __init__(self):
        QtCore.QObject.__init__(self)
        self.myCanvas = None
        self.position = {'x':0.0, 'y':0.0}
        self.updatePosition.connect(self.Update)

    def getCanvas(self):
        return self.myCanvas

    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(903, 506)
        self.clockwiset = QtGui.QPushButton(Dialog)
        self.clockwiset.setGeometry(QtCore.QRect(400, 60, 120, 50))
        self.clockwiset.setObjectName("clockwiset")
        self.clockwiseb = QtGui.QPushButton(Dialog)
        self.clockwiseb.setGeometry(QtCore.QRect(400, 110, 120, 50))
        self.clockwiseb.setObjectName("clockwiseb")
        self.counterclockwiseb = QtGui.QPushButton(Dialog)
        self.counterclockwiseb.setGeometry(QtCore.QRect(550, 110, 120, 50))
        self.counterclockwiseb.setObjectName("counterclockwiseb")
        self.counterclockwiset = QtGui.QPushButton(Dialog)
        self.counterclockwiset.setGeometry(QtCore.QRect(550, 60, 120, 50))
        self.counterclockwiset.setObjectName("counterclockwiset")
        self.forwardb = QtGui.QPushButton(Dialog)
        self.forwardb.setGeometry(QtCore.QRect(400, 270, 120, 50))
        self.forwardb.setObjectName("forwardb")
        self.backwardb = QtGui.QPushButton(Dialog)
        self.backwardb.setGeometry(QtCore.QRect(550, 270, 120, 50))
        self.backwardb.setObjectName("backwardb")
        self.forwardt = QtGui.QPushButton(Dialog)
        self.forwardt.setGeometry(QtCore.QRect(400, 220, 120, 50))
        self.forwardt.setObjectName("forwardt")
        self.backwardt = QtGui.QPushButton(Dialog)
        self.backwardt.setGeometry(QtCore.QRect(550, 220, 120, 50))
        self.backwardt.setObjectName("backwardt")
        self.stepsForRotation = QtGui.QPlainTextEdit(Dialog)
        self.stepsForRotation.setGeometry(QtCore.QRect(700, 60, 120, 50))
        self.stepsForRotation.setObjectName("stepsForRotation")
        self.stepsForLinear = QtGui.QPlainTextEdit(Dialog)
        self.stepsForLinear.setGeometry(QtCore.QRect(700, 220, 120, 50))
        self.stepsForLinear.setObjectName("stepsForLinear")
        #self.inputPosition = QtGui.QPlainTextEdit(Dialog)
        #self.inputPosition.setGeometry(QtCore.QRect(700, 325, 120, 50))
        #self.inputPosition.setObjectName("stepsForLinear")
        self.label = QtGui.QLabel(Dialog)
        self.label.setGeometry(QtCore.QRect(475, 20, 121, 31))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(False)
        font.setWeight(50)
        self.label.setFont(font)
        self.label.setObjectName("label")
        self.label_2 = QtGui.QLabel(Dialog)
        self.label_2.setGeometry(QtCore.QRect(490, 180, 91, 31))
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
        self.label_4.setGeometry(QtCore.QRect(720, 180, 81, 31))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(False)
        font.setWeight(50)
        self.label_4.setFont(font)
        self.label_4.setObjectName("label_4")
        self.zero = QtGui.QPushButton(Dialog)
        self.zero.setGeometry(QtCore.QRect(155, 420, 120, 50))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.zero.setFont(font)
        self.zero.setObjectName("zero")
        self.quit = QtGui.QPushButton(Dialog)
        self.quit.setGeometry(QtCore.QRect(450, 420, 120, 50))
        self.quit.setObjectName("quit")
        self.label_5 = QtGui.QLabel(Dialog)
        self.label_5.setGeometry(QtCore.QRect(485, 330, 100, 31))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(False)
        font.setWeight(50)
        self.label_5.setFont(font)
        self.label_5.setObjectName("label_5")
        self.label_6 = QtGui.QLabel(Dialog)
        self.label_6.setGeometry(QtCore.QRect(395, 370, 10, 20))
        self.label_6.setObjectName("label_6")
        self.label_7 = QtGui.QLabel(Dialog)
        self.label_7.setGeometry(QtCore.QRect(540, 370, 10, 20))
        self.label_7.setObjectName("label_7")
        self.positionInputX = QtGui.QPlainTextEdit(Dialog)
        self.positionInputX.setGeometry(QtCore.QRect(410, 370, 120, 25))
        self.positionInputX.setObjectName("positionInputX")
        self.positionInputY = QtGui.QPlainTextEdit(Dialog)
        self.positionInputY.setGeometry(QtCore.QRect(555, 370, 120, 25))
        self.positionInputY.setObjectName("positionInputY")
        self.setPosition = QtGui.QPushButton(Dialog)
        self.setPosition.setGeometry(QtCore.QRect(700, 360, 120, 50))
        self.setPosition.setObjectName("setPosition")
        self.label_8 = QtGui.QLabel(Dialog)
        self.label_8.setGeometry(QtCore.QRect(650, 450, 120, 20))
        self.label_8.setObjectName("label_8")
        self.currentPositionLabel = QtGui.QLabel(Dialog)
        self.currentPositionLabel.setGeometry(QtCore.QRect(770, 450, 120, 20))
        self.currentPositionLabel.setObjectName("currentPositionLabel")
        self.runStepper = QtGui.QPushButton(Dialog)
        self.runStepper.setGeometry(QtCore.QRect(700, 300, 120, 50))
        self.runStepper.setObjectName("runStepper")
        
        self.figure = plt.figure() # @hunter figsize=(350,350)
        self.myCanvas = PlotCanvas(Dialog, width=3.5, height=3.5)
        self.myCanvas.move(40,60)
        
        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

        # connecting buttons
        self.clockwiseb.clicked.connect(self.Clockwiseb)
        self.counterclockwiseb.clicked.connect(self.Counterclockwiseb)
        self.clockwiset.clicked.connect(self.Clockwiset)
        self.counterclockwiset.clicked.connect(self.Counterclockwiset)
        self.forwardb.clicked.connect(self.Forwardb)
        self.forwardt.clicked.connect(self.Forwardt)
        self.zero.clicked.connect(self.Zero)
        self.backwardb.clicked.connect(self.Backwardb)
        self.backwardt.clicked.connect(self.Backwardt)
        self.quit.clicked.connect(self.Quit)
        self.setPosition.clicked.connect(self.SetPosition)
        self.runStepper.clicked.connect(self.StartStepper)

    def retranslateUi(self, Dialog):
        _translate = QtCore.QCoreApplication.translate
        Dialog.setWindowTitle(_translate("Dialog", "Dialog"))
        self.clockwiset.setText(_translate("Dialog", "Top \n" "Clockwise"))
        self.clockwiseb.setText(_translate("Dialog", "Bottom \n" "Clockwise"))
        self.counterclockwiseb.setText(_translate("Dialog", "Bottom \n" "CounterClockwise"))
        self.counterclockwiset.setText(_translate("Dialog", "Top \n" "CounterClockwise"))
        self.forwardb.setText(_translate("Dialog", "Bottom \n" "Forward"))
        self.backwardb.setText(_translate("Dialog", "Bottom \n" "Backward"))
        self.forwardt.setText(_translate("Dialog", "Top \n" "Forward"))
        self.backwardt.setText(_translate("Dialog", "Top \n" "Backward"))
        self.label.setText(_translate("Dialog", "Rotation"))
        self.label_2.setText(_translate("Dialog", "Linear"))
        self.label_3.setText(_translate("Dialog", "Steps"))
        self.label_4.setText(_translate("Dialog", "Steps"))
        self.label_5.setText(_translate("Dialog", "Position"))
        self.label_6.setText(_translate("Dialog", "X: "))
        self.label_7.setText(_translate("Dialog", "Y: "))
        self.zero.setText(_translate("Dialog", "Zero"))
        self.stepsForRotation.setPlainText(_translate("Dialog", "0"))
        self.stepsForLinear.setPlainText('0')
        self.quit.setText(_translate("Dialog", "Quit"))
        self.positionInputX.setPlainText('0')
        self.positionInputY.setPlainText('0')
        self.setPosition.setText(_translate("Dialog", "Set position"))
        self.label_8.setText(_translate("Dialog", "Current position: "))
        temp = '('+str(self.position['x'])+','+str(self.position['y'])+')'
        self.currentPositionLabel.setText(_translate("Dialog", temp))
        self.runStepper.setText(_translate("Dialog", "Start Stepper"))
        
    def Update(self):
        _translate = QtCore.QCoreApplication.translate
        temp = "({:.2f}, {:.2f})".format(self.position['x'], self.position['y'])
        self.currentPositionLabel.setText(_translate("Dialog", temp))
    
    def StartStepper(self):
        _translate = QtCore.QCoreApplication.translate
        if not stepping:
            self.runStepper.setText(_translate("Dialog", "Stop Stepper"))
            stepping = True
            self.stepperThread = threading.Thread(target=self.doStep)
            self.stepperThread.daemon = True
            self.stepperThread.start()
        else:
            self.stepperThread.stop()
            stepping = False
            self.runStepper.setText(_translate("Dialog", "Start Stepper"))

    def doStep(self):
        while currentStep < len(steeringSteps):
            currentStep += 1


        
        

    def Quit(self):
        QtCore.QCoreApplication.instance().quit()
    
    def SetPosition(self):
        '''
        Set the position manually.
        '''
        currentX, currentY = self.position['x'], self.position['y']
        targetX, targetY = float(self.positionInputX.toPlainText()), float(self.positionInputY.toPlainText())

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
        if angleSteps < 0:
            self.Clockwiseb()
        else:
            self.Counterclockwiseb()
        if distSteps < 0:
            self.Backwardb()
        else:
            self.Forwardb()
        self.position = {'x':targetX, 'y':targetY}
        self.Update()
    
    def Clockwiseb(self):
        print("Clockwise")
        self.SerialWrite('a', self.stepsForRotation, sleep=0.01)
        
    def Clockwiset(self):
        print("Clockwise")
        self.SerialWrite('c', self.stepsForRotation, sleep=0.01)
                
    def Counterclockwiseb(self):
        print("CounterClockwise")
        self.SerialWrite('b', self.stepsForRotation, sleep=0.1)
        
    def Counterclockwiset(self):
        print("CounterClockwise")
        self.SerialWrite('d', self.stepsForRotation, sleep=0.1)
        
    def Backwardb(self):
        print("Backward")
        self.SerialWrite('e', self.stepsForLinear, sleep=0.1)
        
    def Backwardt(self):
        print("Backward")
        self.SerialWrite('g', self.stepsForLinear, sleep=0.1)

    def Forwardb(self):
        print("Forward")
        self.SerialWrite('f', self.stepsForLinear, sleep=0.1)
        
    def Forwardt(self):
        print("Forward")
        self.SerialWrite('h', self.stepsForLinear, sleep=0.1)
        
    def Zero(self):
        global x1, x2, x3, x4
        print("Zero")
        var = 'e'
        ArduinoSerial.write(var.encode())
        time.sleep(5)
        x1,x2,x3,x4=0,0,0,0
        myCanvas.plot(x1, x2, x3, x4)

    def SerialWrite(self, var, textEdit, sleep=0.1):
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
    with open('evd/steering.txt') as f:
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

