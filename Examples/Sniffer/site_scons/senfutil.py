import sys, os

def findsenf(sysdirs = ('/usr/local/lib/senf','/usr/lib/senf'),
             subdirs = ('', 'senf', 'Senf', 'SENF')):
    from os.path import join, sep, exists, normpath
    from itertools import starmap, product, chain

    def joinpaths(*p): return starmap(join, product(*p))
    def existing(l):   return (e for e in l if exists(e))
    def ancestors(d): p = d.split(sep); return (join(p[0]+sep,*p[1:i]) for i in range(len(p),0,-1))
    def butfirst(l):
        i = iter(l); i.next()
        for e in i : yield e

    try: return normpath(existing(butfirst(joinpaths(
                    chain(ancestors(os.getcwd()),sysdirs),
                    subdirs,('site_scons/senfutil.py',)))).next())
    except StopIteration: raise ImportError, 'senfutil not found'

__file__ = findsenf(); del findsenf
sys.path.append(os.path.dirname(__file__))
execfile(__file__, locals(), globals())
