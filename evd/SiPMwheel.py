# -*- coding: utf-8 -*-
"""
Created on Wed Mar 13 12:29:04 2019

@author: Logan Norman
"""

import datetime
import time
import serial
import threading


EncSerial = serial.Serial('COM5', 115200)
def reading():
    while True:
        global x1, x2, x3, x4
        x1=0; x2=0; x3=0; x4=0
        raw_reading = EncSerial.readline()
        DATASPLIT= raw_reading.decode('utf-8').split(' , ')
        x1 = DATASPLIT[0]
        #x2 = DATASPLIT[2]
        x3 = DATASPLIT[1]
        #x4 = DATASPLIT[3]
        f = open("EncData", "a+")
        t = datetime.datetime.now()
        f.write("%s , 0 , %s , 0 , %s\n" %(x1, x3, t))
        f.close

reading_thread = threading.Thread(target=reading) # @hunter for python 2.7
reading_thread.daemon = True
reading_thread.start()

motorserial = serial.Serial('COM6', 115200)
for i in range(2):
    var = 'a'
    motorserial.write(var.encode())
    time.sleep(.1)
    c1= str(500)
    motorserial.write(c1.encode())
    time.sleep(10)
    x1=0; x2=0; x3=0; x4=0
    
for i in range(20):
    for i in range(10):
        var = 'b'
        motorserial.write(var.encode())
        time.sleep(.1)
        c1= str(20)
        motorserial.write(c1.encode())
        time.sleep(10)
        
    var = 'f'
    motorserial.write(var.encode())
    time.sleep(.1)
    c2= str(500)
    motorserial.write(c2.encode())
    time.sleep(10)
    
    for i in range(10):
        var = 'a'
        motorserial.write(var.encode())
        time.sleep(.1)
        c1= str(20)
        motorserial.write(c1.encode())
        time.sleep(10)
        
    var = 'f'
    motorserial.write(var.encode())
    time.sleep(.1)
    c2= str(500)
    motorserial.write(c2.encode())
    time.sleep(10)

motorserial.close()

        
        



