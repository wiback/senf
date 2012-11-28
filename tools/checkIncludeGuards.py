#!/usr/bin/python

import sys, re, os

srcPath = os.path.join( os.path.normpath( os.path.join(os.path.dirname( sys.argv[0]), '..')), 'senf')

r = 0

for (path, dirs, files) in os.walk(srcPath, followlinks=True):
    for f in files:
        if not f.endswith('.hh'):
            continue
        fp = os.path.join(path,f)
        header = open(fp).read()
        m = re.match('.*?#ifndef HH_SENF_(\w*)_', header, re.DOTALL)
        if not m:
#            sys.stderr.write( '%s:1: include guard not found\n' % fp)
#            r = 1
            continue
        ig = fp[fp.find('senf/')+5:-3].replace('/', '_').replace('.', '_')
        line = len(header[0:header.find('#ifndef HH_SENF_')].split('\n'))
        if ig != m.groups()[0]:
            sys.stderr.write( '%s:%d: wrong include guard: "HH_SENF_%s_" != "HH_SENF_%s_"\n' % 
                              (fp, line, m.groups()[0], ig) )
            r = 1
        if header.find('#define HH_SENF_%s_ 1' % m.groups()[0]) == -1:
            sys.stderr.write( '%s:%d: wrong include guard define\n' % (fp, line+1) )
            r = 1
            
    
sys.exit(r)
         