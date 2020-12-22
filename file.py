#! /usr/bin/env/ python3

import os

GB1 = 1024**2-1 #GB1

with open("1gb.txt", "wb") as fout:
    out.write(os.urandom(GB1))
