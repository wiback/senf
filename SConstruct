# -*- python -*-

import sys, glob, os.path, datetime, pwd, time, fnmatch
sys.path.append('senfscons')
import SENFSCons

###########################################################################

# This hack is needed for SCons V 0.96.1 compatibility. In current SCons versions
# we can just use 'env.AlwaysBuild(env.Alias(target), [], action)'
def PhonyTarget(env, target, action):
    env.AlwaysBuild(env.Command(target + '.phony', 'SConstruct', env.Action(action)))
    env.Alias(target, target + '.phony')

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

def nonemptyFile(f):
    try: return os.stat(f).st_size > 0
    except OSError: return False

def checkLocalConf(target, source, env):
    if [ True for f in env['CONFIG_FILES'] if nonemptyFile(f) ]:
        print
        print "You have made local modifications to 'SConfig' and/or 'Doxyfile.local'."
        print "Building a debian package would remove those files."
        print
        print "To continue, remove the offending file(s) and try again. Alternatively,"
        print "build a source package using 'scons debsrc' and may then build debian"
        print "binary packages from this source-package without disrupting your print local"
        print "configuration."
        print
        return 1

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
linklint     Check links of doxygen documentation with 'linklint'
""")

if os.environ.get('debian_build'):
    rev = os.popen("dpkg-parsechangelog | awk '/^Version:/{print $2}'").read().strip()
else:
    rev = 'r' + os.popen("svnversion").read().strip().lower()

logname = os.environ.get('LOGNAME')
if not logname:
    logname = pwd.getpwuid(os.getuid()).pw_name

def configFilesOpts(target, source, env, for_signature):
    return [ '-I%s' % os.path.split(f)[1] for f in env['CONFIG_FILES'] ]

env.Append(
   CPPPATH = [ '#/include' ],
   LIBS = [ 'iberty', '$BOOSTREGEXLIB' ],
   DOXY_XREF_TYPES = [ 'bug', 'fixme', 'todo', 'idea' ],
   DOXY_HTML_XSL = '#/doclib/html-munge.xsl',
   ENV = { 'TODAY' : str(datetime.date.today()),
           'REVISION' : rev,
           'LOGNAME' : logname, # needed by the debian build scripts
           'CONCURRENCY_LEVEL' : env.GetOption('num_jobs') or "1"
           },
   CONFIG_FILES = [ 'Doxyfile.local', 'SConfig', 'local_config.hh' ],
   CONFIG_FILES_OPTS = configFilesOpts,
   CLEAN_PATTERNS = [ '*.pyc', 'semantic.cache', '.sconsign', '.sconsign.dblite' ],
   BUILDPACKAGE_COMMAND = "dpkg-buildpackage -us -uc -rfakeroot -I.svn $CONFIG_FILES_OPTS",
   TOP_INCLUDES = [ 'Packets', 'PPI', 'Scheduler', 'Socket', 'Utils',
                    'config.hh', 'local_config.hh' ]
)

Export('env')

# Create Doxyfile.local if not cleaning and the file does not exist
# otherwise doxygen will barf on this non-existent file
if not env.GetOption('clean') and not os.path.exists("Doxyfile.local"):
    Execute(Touch("Doxyfile.local"))

# Create local_config.h
if not env.GetOption('clean') and not os.path.exists("local_config.hh"):
    Execute(Touch("local_config.hh"))

###########################################################################
# Define build targets

SConscript(glob.glob("*/SConscript"))

SENFSCons.StandardTargets(env)
SENFSCons.GlobalTargets(env)
SENFSCons.Doxygen(env)
SENFSCons.DoxyXRef(env,
                   HTML_HEADER = '#/doclib/doxy-header-overview.html',
                   HTML_FOOTER = '#/doclib/doxy-footer.html')

SENFSCons.InstallIncludeFiles(env, [ 'config.hh' ])

# Build combined library 'libsenf'
libsenf = env.Library(
    SENFSCons.LibPath('senf'),
    Flatten([ env.File(SENFSCons.LibPath(lib)).sources for lib in env['ALLLIBS'] ]))
env.Default(libsenf)
env.Clean('all', 'libsenf.a')
env.Alias('all', 'libsenf.a')

env.Alias('install_all', env.Install('$LIBINSTALLDIR', libsenf))

env.Clean('all', [ os.path.join(path,f)
                   for path, subdirs, files in os.walk('.')
                   for pattern in env['CLEAN_PATTERNS']
                   for f in fnmatch.filter(files,pattern) ])

PhonyTarget(env, 'deb', [
    checkLocalConf,
    updateRevision,
    "$BUILDPACKAGE_COMMAND",
])

PhonyTarget(env, 'debsrc', [
    updateRevision,
    "$BUILDPACKAGE_COMMAND -S",
])

PhonyTarget(env, 'debbin', [
    checkLocalConf,
    updateRevision,
    "$BUILDPACKAGE_COMMAND -nc",
])

PhonyTarget(env, 'linklint', [
    'rm -rf linklint',
    'linklint -doc linklint -net -limit 99999999 `find -type d -name html -printf "/%P/@ "`',
    '[ ! -r linklint/errorX.html ] || python linklint_addnames.py <linklint/errorX.html >linklint/errorX.html.new',
    '[ ! -r linklint/errorX.html.new ] || mv linklint/errorX.html.new linklint/errorX.html',
    '[ ! -r linklint/errorAX.html ] || python linklint_addnames.py <linklint/errorAX.html >linklint/errorAX.html.new',
    '[ ! -r linklint/errorAX.html.new ] || mv linklint/errorAX.html.new linklint/errorAX.html',
    'echo -e "\\nLokal link check results: linklint/index.html\\nRemote link check results: linklint/urlindex.html\\n"',
])

PhonyTarget(env, 'fixlinks', [
    'python doclib/fix-links.py -v -s .svn -s linklint -s debian linklint/errorX.txt linklint/errorAX.txt',
])

env.Clean('all', env.Dir('linklint'))
