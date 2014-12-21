#!/usr/bin/python
#Author Ha Shao <hashao@china.com>
#2000.8.30

import fileinput
import string

for line in fileinput.input():
    if(fileinput.filename() == "400b5.txt"):
    	total = 171894734
    elif (fileinput.filename() == "400gb.txt"):
        total = 45376453

    s = string.split(line)
    print "%s,\t%f" % (s[1], (float(s[2]))/ total * 100)
