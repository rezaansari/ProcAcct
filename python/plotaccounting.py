#!/usr/bin/env python
# -*- coding: ISO-8859-1 -*-


import sys

import pyfits
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import glob
import os
import math
from scipy import spatial


class Log:
    def __init__(self):
        pass

Logs = []

if __name__ == '__main__':

    if len(sys.argv) < 3:
        print 'plotaccounting.py <input_logfile> <output_imagefile>'
        exit()

    logfile = sys.argv[1]
    imagefile = sys.argv[2]

    last = Log()

    with open (logfile) as f:
        lines = f.readlines()
        first = True
        keys = None
        for line in lines:
            line = line.strip()
            line = line.replace ('/', '_')
            line = line.replace ('%', '_')
            tokens = line.split()

            if first:
                first = False
                tokens.remove('#')
                tokens.remove('(_)')
                keys = tokens
            elif tokens[0] == '#':
                continue
            else:
                tokens.remove('(_)')
                log = Log()
                for i, value in enumerate(tokens):
                    try:
                        value = float(value)
                    except:
                        pass

                    # print i,value

                    key = keys[i]
                    setattr(last, key, value)
                    setattr(log, key, value)

                Logs.append(log)

    fig, plots = plt.subplots(4, 4)

    for log in Logs:
        for i, key in enumerate(keys):
            if i == 15:
                continue

            x = i/4
            y = i % 4
            pl = plots[x][y]
            pl.set_title(key)
            pl.plot(log.log_time, getattr(log, key), 'g.')

    for i, key in enumerate(keys):
        if i == 15:
            continue
        x = i/4
        y = i % 4
        pl = plots[x][y]
        pl.grid()

    # plt.grid()
    # plt.show()

    plt.rcParams.update({'font.size': 6})
    plt.subplots_adjust(wspace=0.5, hspace=0.5)

    plt.savefig(imagefile, dpi=300)

