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

Export('env')

SConscript(glob.glob("*/SConscript"))

SENFSCons.StandardTargets(env)
SENFSCons.GlobalTargets(env)
SENFSCons.Doxygen(env)
SENFSCons.DoxyXRef(env, env.Alias('all_docs')[0].sources,
                   HTML_HEADER = '#/doclib/doxy-header-overview.html',
                   HTML_FOOTER = '#/doclib/doxy-footer.html')

if not env.GetOption('clean') and not os.path.exists("Doxyfile.local"):
    Execute(Touch("Doxyfile.local"))
