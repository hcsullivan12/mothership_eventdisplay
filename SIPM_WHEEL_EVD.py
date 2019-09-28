#!bin/python

import os
from subprocess import Popen

# get this directory
current_dir = os.getcwd()
evd_dir = os.path.join(current_dir, 'evd')
guipath = os.path.join(evd_dir, 'robotguiV2.py')

print('')
print('Opening robot gui...')
print('')
command = 'python '+str(guipath)
proc = Popen([command], shell=True, stdin=None, stdout=None, stderr=None, close_fds=True)

print('')
print('Running event display...')
print('')

command = 'root -l \'./evd/EventDisplay.C(\"'+evd_dir+'\")\''
os.system(command)
