# -*- python -*-

import sys, glob
sys.path.append('satscons')
import SatSCons

###########################################################################

SatSCons.UseBoost()
SatSCons.UseSTLPort()
SatSCons.UseDoxygen()
env = SatSCons.MakeEnvironment()
env.Tool('Dia2Png', [ 'satscons' ])

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
