# -*- python -*-

import sys, glob, os.path
sys.path.append('senfscons')
import SatSCons

###########################################################################

SatSCons.UseBoost()
SatSCons.UseSTLPort()
env = SatSCons.MakeEnvironment()

env.Append(
   CPPPATH = [ '#' ],
   LIBS = [ 'iberty' ],
   DOXYFILES = [ '#/doclib/doxy-header.html', '#/doclib/doxy-footer.html',
                 '#/doclib/Doxyfile.global' ]
)

Export('env')

SConscript(glob.glob("*/SConscript"))

SatSCons.StandardTargets(env)
SatSCons.GlobalTargets(env)
SatSCons.Doxygen(env)

if not os.path.exists("Doxyfile.local") : Execute(Touch("Doxyfile.local"))
