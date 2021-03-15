#!/usr/bin/env python3

# Copyright 2018 Tony Karnigen

import os,sys
import subprocess
import re

def make_vars(cmd='make -pn', origin=['environment', 'makefile']):
    '''
    Generate the (key,value) dict of all variables defined in make process
    cmd
        - "make -pnB"
        - this command returns all variables defined in Makefile to stdout
        - tested only with gnu make
    origin
        - who defined variable
        - 'automatic', 'environment', 'default', "'override'", 'makefile'
        - = None returns all origins
    '''
    p = subprocess.getoutput(cmd)
    # even with bad status variables are set
#    st, p = subprocess.getstatusoutput(cmd)
#    if st != 0:
#        print("Error {} in cmd: {}".format(st,cmd))
#        return None    # user must check this value

    M={}
    re_var = re.compile(r"^#\s*Variables\b")  # start of variable segment
    re_varend = re.compile(r"^#\s*variable")  # end of variables
    s=None                                    # state of parser
    mname=None
    for line in p.splitlines():
        if s is None and re_var.search(line):
            s='var'
        elif s == 'var':
            line=line.strip()
            if re_varend.search(line):     # last line of variable block
                s='end'
                break
            if line.startswith("#"):       # type of variable
                q=line.split()
                mname = q[1]
            elif mname is not None:
                if origin is not None and mname not in origin:
                    continue
                if mname not in M:
                    # print("Creating {}".format(mname))
                    M[mname]={}
                q = line.split(maxsplit=2)   # key =|:= value
                if q:
                    if len(q)<=2: M[mname][q[0]]=''
                    else:         M[mname][q[0]]=q[2]
                else:
                    M[mname] = ''
                mname=None
    return M

def make_expand(M, val):
    '''
    expand all $(var) variables in val
    '''
    if M is None: return val
    rev = re.compile(r"\$\((\w+)\)")
    s=val
    p1=p2=0
    cnt=0
    while True:
        m=rev.search(s, p2)   # position p2 search starts - default=0
        if m is None: break
        # print(m.group(0))
        k = m.group(1)
        p1,p2 = m.span(0)
        # print(k,p1,p2)

        v = None # new value
        # search for key in multikey dictinary
        for o in M:
            if k in M[o]:
                v = M[o][k]
                break

        if v is not None:
            s = s[:p1] + v + s[p2:]
            p2=p1  # at position p1 it may be new replacement $(...)
        # on None - no replacement but search starts from position p2

        cnt+=1
        if cnt > 1000:
            print("Error maybe loop detected cnt: {}".format(cnt))
            sys.exit(1)
    # print("cnt=",cnt)
    return s
