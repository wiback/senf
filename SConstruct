import sys, glob
sys.path.append('satscons')
import SatSCons

###########################################################################

SatSCons.UseBoost();
SatSCons.UseSTLPort();
SatSCons.UseDoxygen();
env = SatSCons.MakeEnvironment();

env.Append(
   CPPPATH = [ '#..', '#' ]
)

Export('env')

SConscript(glob.glob("*/SConscript"))

SatSCons.StandardTargets(env)
SatSCons.GlobalTargets(env)
