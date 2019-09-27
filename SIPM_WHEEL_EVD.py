#!bin/python

import os

# get this directory
current_dir = os.getcwd()

# get top majorana directory
top_dir = os.environ.get('MAJORANADIR')
if top_dir is None:
    print('Could not find environment variable \'MAJORANADIR\'. Please point \'MAJORANADIR\' to the top directory.')
    exit()

print('')
print('Running event display...')
print('')

command = 'root -l \'./cpb348_evd/EventDisplay.C(\"'+top_dir+'\")\''
os.system(command)
