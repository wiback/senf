# -*- python -*-

import sys, glob, os.path, datetime, pwd, time
sys.path.append('senfscons')
import SENFSCons

###########################################################################

def updateRevision(target, source, env):
    rev = env['ENV']['REVISION'][1:]
    if ':' in rev:
        print
        print "Working copy not clean. Run 'svn update'"
        print
        return 1
    if 'm' in rev and not ARGUMENTS.get('force_deb'):
        print
        print "Working copy contains local changes. Commit first"
        print
        return 1
    if 's' in rev:
        rev = rev[:-1]
    if 'm' in rev:
        rev = rev[:-1]
    changelog = file('debian/changelog.template').read() % {
        'rev': rev,
        'user': pwd.getpwuid(os.getuid()).pw_gecos.split(',')[0].strip(),
        'date': time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime()) }
    file('debian/changelog','w').write(changelog)


###########################################################################
# Load utilities and setup libraries and configure build

SENFSCons.UseBoost()
SENFSCons.UseSTLPort()
env = SENFSCons.MakeEnvironment()

env.Help("""
Additional top-level build targets:

all_tests    Build and run unit tests for all modules
all_docs     Build documentation for all modules
all          Build everything
install_all  Install SENF into $PREFIX
deb          Build debian source and binary package
debsrc       Build debian source package
debbin       Build debian binary package
""")

if os.environ.get('debian_build'):
    rev = os.popen("dpkg-parsechangelog | awk '/^Version:/{print $2}'").read().strip()
else:
    rev = 'r' + os.popen("svnversion").read().strip().lower()

env.Append(
   CPPPATH = [ '#' ],
   LIBS = [ 'iberty', '$BOOSTREGEXLIB' ],
   DOXY_XREF_TYPES = [ 'bug', 'fixme', 'todo', 'idea' ],
   DOXY_HTML_XSL = '#/doclib/html-munge.xsl',
   ENV = { 'TODAY' : str(datetime.date.today()),
           'REVISION' : rev,
           'LOGNAME' : os.environ['LOGNAME'], # needed by the debian build scripts
           'CONCURRENCY_LEVEL' : env.GetOption('num_jobs') or "1"
           },
)

Export('env')

# Create Doxyfile.local if not cleaning and the file does not exist
# otherwise doxygen will barf on this non-existent file
if not env.GetOption('clean') and not os.path.exists("Doxyfile.local"):
    Execute(Touch("Doxyfile.local"))

###########################################################################
# Define build targets

SConscript(glob.glob("*/SConscript"))

SENFSCons.StandardTargets(env)
SENFSCons.GlobalTargets(env)
SENFSCons.Doxygen(env)
SENFSCons.DoxyXRef(env,
                   HTML_HEADER = '#/doclib/doxy-header-overview.html',
                   HTML_FOOTER = '#/doclib/doxy-footer.html')

# Build combined library 'libsenf'
libsenf = env.Library(
    SENFSCons.LibPath('senf'),
    Flatten([ env.File(SENFSCons.LibPath(lib)).sources for lib in env['ALLLIBS'] ]))
env.Default(libsenf)
env.Alias('install_all', env.Install('$LIBINSTALLDIR', libsenf))

env.AlwaysBuild(
    env.Alias('deb', [], [ updateRevision,
                           "dpkg-buildpackage -us -uc -rfakeroot -I.svn" ]))

env.AlwaysBuild(
    env.Alias('debsrc', [], [ updateRevision,
                              "dpkg-buildpackage -us -uc -rfakeroot -S -I.svn" ]))

env.AlwaysBuild(
    env.Alias('debbin', [], [ updateRevision,
                              "dpkg-buildpackage -us -uc -rfakeroot -nc" ]))
