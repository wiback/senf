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
   DOXYFILES = [ '#/doclib/doxy-header.html', '#/doclib/doxy-footer.html',
                 '#/doclib/Doxyfile.global' ]
)

Export('env')

SConscript(glob.glob("*/SConscript"))

SENFSCons.StandardTargets(env)
SENFSCons.GlobalTargets(env)
SENFSCons.Doxygen(env)

if not os.path.exists("Doxyfile.local") : Execute(Touch("Doxyfile.local"))
