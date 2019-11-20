#!/bin/python3

import subprocess as subp
import shlex
import time 
import datetime
import os

BUF_TO_FILL="/home/apashnin/src/ed-reader/buffer_to_fill"
BUF_TO_COPY="/home/apashnin/src/ed-reader/buffer_to_copy"
ROOT_PATH="/eos/project-a/amstest/www/"
SCROT_CMD = "scrot ./ss_current.png"

# Read the configuration

scrot_key_q = 100
n_files = 3

print("CONFIG: q = " + str(scrot_key_q))
print("CONFIG: n = " + str(n_files))

subp.Popen(['rm', '-rf', BUF_TO_COPY]) #delete temp
subp.Popen(['rm', '-rf', BUF_TO_FILL]) #delete local

# Read the screen and save to a file

fns = []
while True:
    ts = int(datetime.datetime.now(datetime.timezone.utc).strftime("%s"))
    print("ts = ", ts)

    # Check if the local destination directory exist
    subp.Popen(['mkdir', '-p', BUF_TO_FILL])

    # Take a screenshot and copy the file
    
    fn = '/ss_{0}.png'.format(ts)
    fns.append(fn)
    file_path = BUF_TO_FILL + fn 
    cmd = "{0} -q{2} -e 'mv $f {1}'".format(SCROT_CMD, file_path, scrot_key_q)
    print(cmd)
    cmd_seq = shlex.split(cmd)
    p = subp.Popen(cmd_seq)
    pout, perr = p.communicate()

    # TODO: handle the errors!
    # print(pout)
    print(perr)

    # Take a pause. TODO: make a timer interrupt to be precise (1 file per second)
    time.sleep(1)

    # Check number of existing files at DEST_PATH. Skip if too many.
    # It's up to the WebServer to clean up.
    n_files_copied_str = subp.check_output('ls -l ' + BUF_TO_FILL + ' | wc -l', shell=True)
    n_files_copied = int(n_files_copied_str) - 1 # substract -1 for the 'total' line of 'ls' output
    print("n = ", n_files_copied)

    if (n_files_copied >= n_files):
        
        print("Moving to temp: " + BUF_TO_FILL + ' -> ' + BUF_TO_COPY)
        p = subp.Popen(['rm', '-rf', BUF_TO_COPY]) #delete temp
        pout, perr = p.communicate()
        print(pout)
        print(perr)
        p = subp.Popen(['mv', BUF_TO_FILL, BUF_TO_COPY]) #move to temp
        pout, perr = p.communicate()
        print(pout)
        print(perr)

        
        p = subp.Popen(['mkdir', '-p', BUF_TO_FILL]) #re-create local
        pout, perr = p.communicate()
        print(pout)
        print(perr)

        # check if destination exists
        dest_path = ROOT_PATH + '/buffer_copied'
        if(os.path.exists(dest_path)):
            # print("Destination path " + dest_path + " exists. Do nothing.")
            print("Destination path exists. Deleting...")
            p = subp.Popen(['rm', '-rf', dest_path]) #delete temp
            pout, perr = p.communicate()
            print(pout)
            print(perr)
            
            # continue

        cmd = "mv " + BUF_TO_COPY + ' ' + dest_path
        print("Moving to dest: " + cmd)
        cmd_seq = shlex.split(cmd)
        p = subp.Popen(cmd_seq)
        pout, perr = p.communicate()
        print(pout)
        print(perr)

        f = open(dest_path + '/content.json', 'a')
        f.write("test record")
        f.close()



    



