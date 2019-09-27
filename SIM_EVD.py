#!/bin/python
import os

# get this directory
current_dir = os.getcwd()

# get top majorana directory
top_dir = os.environ.get('MAJORANADIR')
if top_dir is None:
    print('Could not find environment variable \'MAJORANADIR\'. Please point \'MAJORANADIR\' to the top directory.')
    exit()

print('')
print('Running G4 sim...')
print('')

# start a g4 session
g4_cmd = 'gnome-terminal -- /bin/sh -c \'cd '+top_dir+'/build; ./simulate -c ../config/MthShip_SimConfiguration.ini -E ON; echo "Press Enter to exit simulation..."; read var\''
os.system(g4_cmd)


print('')
print('Opening event display...')
print('')

command = 'root -l \'./mthship_evd/EventDisplay.C(\"'+top_dir+'\")\''
os.system(command)
