#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
k = input()
f = open("{}".format(k) , "r")
p = f.read().replace("口","・" )
f.close()
os.remove(k)
g = open("{}".format(k),"w")
g.write(p)

g.flush()
g.close()
