# -*- python -*-

import sys, glob, os.path
sys.path.append('senfscons')
import SENFSCons

###########################################################################

SENFSCons.UseBoost()
SENFSCons.UseSTLPort()
env = SENFSCons.MakeEnvironment()

env.Append(
   CPPPATH = [ '#' ],
   LIBS = [ 'iberty' ],
   DOXY_XREF_TYPES = [ 'bug', 'fixme', 'todo', 'idea' ],
)

import datetime

svninfo = dict(
    [ tuple(map(lambda y:y.strip(),x.split(":",1)))
      for x in os.popen("svn info").read().split("\n")
      if ':' in x ] )
svninfo['commited'] = not(os.popen("svn status").read())

env.Append(
    ENV = { 'TODAY' : datetime.date.today(),
            'REVISION' : svninfo['Revision'] + (not(svninfo['commited']) and " + local changes" or "") }
)

Export('env')

SConscript(glob.glob("*/SConscript"))

SENFSCons.StandardTargets(env)
SENFSCons.GlobalTargets(env)
SENFSCons.Doxygen(env)
SENFSCons.DoxyXRef(env, 
                   HTML_HEADER = '#/doclib/doxy-header-overview.html',
                   HTML_FOOTER = '#/doclib/doxy-footer.html')

if not env.GetOption('clean') and not os.path.exists("Doxyfile.local"):
    Execute(Touch("Doxyfile.local"))
