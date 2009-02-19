# -*- python -*-

import sys, glob, os.path, datetime, pwd, time, fnmatch, string
sys.path.append(Dir('#/senfscons').abspath)
sys.path.append(Dir('#/doclib').abspath)
import SENFSCons

###########################################################################

# This hack is needed for SCons V 0.96.1 compatibility. In current SCons versions
# we can just use 'env.AlwaysBuild(env.Alias(target), [], action)'
def PhonyTarget(env, target, action, sources=[]):
    env.AlwaysBuild(env.Command(target + '.phony', [ 'SConstruct' ] + sources, env.Action(action)))
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
    url = None
    for line in os.popen("svn info"):
        elts=line.split(':',1)
        if elts[0] == 'URL':
            url = elts[1].strip()
    version = None
    if '/tags/' in url:
        version = url.rsplit('/',1)[-1].split('_',1)[0]
        if version[0] not in string.digits:
            version = None
    if version is None:
        version = '1:0r%s' % rev
    changelog = file('debian/changelog.template').read() % {
        'version': version,
        'rev': rev,
        'user': pwd.getpwuid(os.getuid()).pw_gecos.split(',')[0].strip(),
        'date': time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime()) }
    file('debian/changelog','w').write(changelog)

def nonemptyFile(f):
    try: return os.stat(f).st_size > 0
    except OSError: return False

def checkLocalConf(target, source, env):
    if [ True for f in env['LOCAL_CONFIG_FILES'] if nonemptyFile(f) ]:
        print
        print "You have made local modifications to one of the following local configuration"
        print "files:"
        for f in env['LOCAL_CONFIG_FILES']:
            print "    ",f
        print
        print "Building a debian package would remove those files."
        print
        print "To continue, remove the offending file(s) and try again. Alternatively,"
        print "build a source package using 'scons debsrc' and may then build debian"
        print "binary packages from this source-package without disrupting your local"
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

prepare      Create all source files not part of the repository
all_tests    Build and run unit tests for all modules
all_docs     Build documentation for all modules
all          Build everything
install_all  Install SENF into $PREFIX
deb          Build debian source and binary package
debsrc       Build debian source package
debbin       Build debian binary package
linklint     Check links of doxygen documentation with 'linklint'
fixlinks     Fix broken links in doxygen documentation
valgrind     Run all tests under valgrind/memcheck
""")

if os.environ.get('debian_build'):
    rev = os.popen("dpkg-parsechangelog | awk '/^Version:/{print $2}'").read().strip()
else:
    rev = 'r' + os.popen("svnversion").read().strip().lower()

logname = os.environ.get('LOGNAME')
if not logname:
    logname = pwd.getpwuid(os.getuid()).pw_name

def dpkgIgnoredFilesOpts(target, source, env, for_signature):
    return [ '-I%s' % os.path.split(f)[1] for f in env.subst('$DPKG_IGNORED_FILES').split() ]

# Options used to debug inlining:
#
# INLINE_OPTS = [ '-finline-limit=20000', '--param','large-function-growth=10000',
#                 '--param', 'large-function-insns=10000', '--param','inline-unit-growth=10000',
#                 '-fvisibility-inlines-hidden', '-fno-inline-functions', '-Winline' ]
#
# BEWARE: You need lots of ram to compile with these settings (approx 1G)
#

INLINE_OPTS = [ '-finline-limit=5000' ]

env.Append(
   CPPPATH = [ '#/include' ],
   CXXFLAGS = [ '-Wall', '-Woverloaded-virtual', '-Wno-long-long' ] + INLINE_OPTS,
   LIBS = [ 'rt', '$BOOSTREGEXLIB', '$BOOSTIOSTREAMSLIB', '$BOOSTSIGNALSLIB',
            '$BOOSTFSLIB' ],
   TEST_EXTRA_LIBS = [ ],
   DOXY_XREF_TYPES = [ 'bug', 'fixme', 'todo', 'idea' ],
   DOXY_HTML_XSL = '#/doclib/html-munge.xsl',
   ENV = { 'TODAY' : str(datetime.date.today()),
           'REVISION' : rev,
           'LOGNAME' : logname, # needed by the debian build scripts
           'CONCURRENCY_LEVEL' : env.GetOption('num_jobs') or "1",
           'SCONS' : 1,
           'PATH' : os.environ.get('PATH'),
	   'TEXINPUTS' : os.environ.get('TEXINPUTS',':'),
         },
   LOCAL_CONFIG_FILES = [ 'Doxyfile.local', 'SConfig', 'local_config.hh' ],
   DPKG_IGNORED_FILES = [ '$LOCAL_CONFIG_FILES', '.svn', '_templates' ],
   DPKG_IGNORED_FILES_OPTS = dpkgIgnoredFilesOpts,
   CLEAN_PATTERNS = [ '*~', '#*#', '*.pyc', 'semantic.cache', '.sconsign', '.sconsign.dblite' ],
   BUILDPACKAGE_COMMAND = "dpkg-buildpackage -us -uc -rfakeroot $DPKG_IGNORED_FILES_OPTS",
   TOP_INCLUDES = [ 'Packets', 'PPI', 'Scheduler', 'Socket', 'Utils', 'Console',
                    'config.hh', 'local_config.hh' ],
)

def parseLogOption(value):
    stream, area, level = ( x.strip() for x in value.strip().split('|') )
    if stream  : stream = ''.join('(%s)' % x for x in stream.split('::') )
    else       : stream = '(_)'
    if area : area = ''.join( '(%s)' % x for x in area.split('::') )
    else    : area = '(_)'
    return '(( %s,%s,%s ))' % (stream,area,level)

def expandLogOption(target, source, env, for_signature):
    return ' '.join( parseLogOption(x) for x in env.subst('$LOGLEVELS').split() )

if env.subst('$LOGLEVELS'):
    env.Append( expandLogOption=expandLogOption )
    env.Append( CPPDEFINES = { 'SENF_LOG_CONF': '$expandLogOption' } )

env.SetDefault(
       LIBSENF = "senf"
)

Export('env')

# Create Doxyfile.local otherwise doxygen will barf on this non-existent file
# Create it even when cleaning, to silence the doxygen builder warnings
if not os.path.exists("Doxyfile.local"):
    Execute(Touch("Doxyfile.local"))

# Create local_config.h
if not env.GetOption('clean') and not os.path.exists("local_config.hh"):
    Execute(Touch("local_config.hh"))

###########################################################################
# Define build targets

# Before defining any targets, check wether this is the first build in
# pristine directory tree. If so, call 'scons prepare' so the dependencies
# created later are correct

if not env.GetOption('clean') and not os.path.exists(".prepare-stamp") \
   and not os.environ.get("SCONS") and COMMAND_LINE_TARGETS != [ 'prepare' ]:
    env.Execute([ "scons prepare" ])

env.Clean('all', '.prepare-stamp')

# Not nice, but until we get to fixing the dependency jungle
# concerning generated sources ...
scripts = []
dependencies = []

SConscript(glob.glob("*/SConscript"))

SENFSCons.StandardTargets(env)
SENFSCons.GlobalTargets(env)
SENFSCons.Doxygen(env)
SENFSCons.DoxyXRef(env,
                   HTML_HEADER = '#/doclib/doxy-header.html',
                   HTML_FOOTER = '#/doclib/doxy-footer.html')

SENFSCons.InstallIncludeFiles(env, [ 'config.hh' ])

# Build combined library 'libsenf'
libsenf = env.Library(env.subst("$LIBSENF$LIBADDSUFFIX"), env['ALLOBJECTS'])
env.Default(libsenf)
env.Clean('all', libsenf)
env.Alias('default', libsenf)

env.Alias('install_all', env.Install('$LIBINSTALLDIR', libsenf))

if env.GetOption('clean'):
    env.Clean('all', [ os.path.join(path,f)
                       for path, subdirs, files in os.walk('.')
                       for pattern in env['CLEAN_PATTERNS']
                       for f in fnmatch.filter(files,pattern) ])

PhonyTarget(env, 'deb', [
    checkLocalConf,
    updateRevision,
    "$BUILDPACKAGE_COMMAND",
    "fakeroot ./debian/rules debclean"
])

PhonyTarget(env, 'debsrc', [
    updateRevision,
    "$BUILDPACKAGE_COMMAND -S",
])

PhonyTarget(env, 'debbin', [
    checkLocalConf,
    updateRevision,
    "$BUILDPACKAGE_COMMAND -b",
    "fakeroot ./debian/rules debclean"
])

PhonyTarget(env, 'linklint', [
    'rm -rf linklint',
    'linklint -doc linklint -limit 99999999 `find -type d -name html -printf "/%P/@ "`',
    '[ ! -r linklint/errorX.html ] || python doclib/linklint_addnames.py <linklint/errorX.html >linklint/errorX.html.new',
    '[ ! -r linklint/errorX.html.new ] || mv linklint/errorX.html.new linklint/errorX.html',
    '[ ! -r linklint/errorAX.html ] || python doclib/linklint_addnames.py <linklint/errorAX.html >linklint/errorAX.html.new',
    '[ ! -r linklint/errorAX.html.new ] || mv linklint/errorAX.html.new linklint/errorAX.html',
    'echo -e "\\nLokal link check results: linklint/index.html\\nRemote link check results: linklint/urlindex.html\\n"',
])

PhonyTarget(env, 'fixlinks', [
    'python doclib/fix-links.py -v -s .svn -s linklint -s debian linklint/errorX.txt linklint/errorAX.txt',
])

PhonyTarget(env, 'prepare', [])

PhonyTarget(env, 'valgrind', [
    'find -name .test.bin | while read test; do echo; echo "Running $$test"; echo; valgrind --tool=memcheck --error-exitcode=99 --suppressions=valgrind.sup $$test $BOOSTTESTARGS; [ $$? -ne 99 ] || exit 1; done'
    ], [ 'all_tests' ])

env.Clean('all', env.Dir('linklint'))

env.Clean('all','.prepare-stamp')
if not env.GetOption('clean') and not os.path.exists(".prepare-stamp"):
    Execute(Touch(".prepare-stamp"))
