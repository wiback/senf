# -*- python -*-

import sys, glob, os.path, datetime, pwd, time
sys.path.append('senfscons')
import SENFSCons

###########################################################################

# Load utilities and setup libraries
SENFSCons.UseBoost()
SENFSCons.UseSTLPort()
env = SENFSCons.MakeEnvironment()

# Configure build
env.Append(
   CPPPATH = [ '#' ],
   LIBS = [ 'iberty', '$BOOSTREGEXLIB' ],
   DOXY_XREF_TYPES = [ 'bug', 'fixme', 'todo', 'idea' ],
   DOXY_HTML_XSL = '#/doclib/html-munge.xsl',
   ENV = { 'TODAY' : str(datetime.date.today()),
           'REVISION' : os.popen("svnversion").read().strip(),
           'LOGNAME' : os.environ['LOGNAME']
           },
)

Export('env')

# Build modules (that is, instruct to build ... the build happens later)
SConscript(glob.glob("*/SConscript"))

SENFSCons.StandardTargets(env)
SENFSCons.GlobalTargets(env)
SENFSCons.Doxygen(env)

SENFSCons.DoxyXRef(env,
                   HTML_HEADER = '#/doclib/doxy-header-overview.html',
                   HTML_FOOTER = '#/doclib/doxy-footer.html')

def updateRevision(target, source, env):
    rev = env['ENV']['REVISION']
    if ':' in rev:
        print
        print "Working copy not clean. Run 'svn update'"
        print
        return 1
    if 'M' in rev:
        print
        print "Working copy contains local changes. Commit first"
        print
        return 1
    if 'S' in rev or 'M' in rev:
        rev = rev[:-1]
    changelog = file('debian/changelog.template').read() % {
        'rev': rev,
        'user': pwd.getpwuid(os.getuid()).pw_gecos.split(',')[0].strip(),
        'date': time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime()) }
    file('debian/changelog','w').write(changelog)

env.AlwaysBuild(
    env.Alias('deb', [], [ updateRevision,
                           "dpkg-buildpackage -us -uc -rfakeroot" ]))

# Create Doxyfile.local if not cleaning and the file does not exist
# otherwise doxygen will barf on this non-existent file
if not env.GetOption('clean') and not os.path.exists("Doxyfile.local"):
    Execute(Touch("Doxyfile.local"))
