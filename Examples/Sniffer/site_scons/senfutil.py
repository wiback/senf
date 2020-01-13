#
# Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
#                    Network Research Group (NET)
#                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
#                    Contact: http://wiback.org
#
# This file is part of the SENF code tree.
# It is licensed under the 3-clause BSD License (aka New BSD License).
# See LICENSE.txt in the top level directory for details or visit
# https://opensource.org/licenses/BSD-3-Clause
#


def findsenf(sysdirs = ('/usr/local/lib/senf','/usr/lib/senf'),
             subdirs = ('', 'senf', 'Senf', 'SENF')):
    import os, itertools

    def ancestors(d):
        p = d.split(os.path.sep)
        return (os.path.join(p[0]+os.path.sep,*p[1:i]) for i in range(len(p),0,-1))

    for d in itertools.chain(ancestors(os.getcwd()),sysdirs):
        for s in subdirs:
            py = os.path.join(d,s,'site_scons/senfutil.py')
            if os.path.exists(py) and not os.path.samefile(py,__file__): return py

    raise ImportError, 'senfutil not found'

# replace module with real senfutil
import sys, os
__file__ = findsenf()
sys.path.append(os.path.dirname(__file__))
del sys, os, findsenf
execfile(__file__, locals(), globals())
