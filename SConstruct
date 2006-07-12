import sys, os, os.path, glob
satscons = os.getcwd()
while satscons!='/' and not os.path.isdir(os.path.join(satscons,"satscons")): satscons = os.path.dirname(satscons)
if satscons=='/': print "satscons directory not found"; Exit(1)
satscons = os.path.join(satscons,"satscons")
sys.path.append(satscons)
import SatSCons

###########################################################################

SatSCons.UseBoost();
SatSCons.UseSTLPort();
SatSCons.UseDoxygen();
env = SatSCons.MakeEnvironment();

env.Append(
   CPPPATH = [ '#..', '#' ]
)

Export('env satscons')

SConscript(glob.glob("*/SConscript"))

SatSCons.StandardTargets(env)
