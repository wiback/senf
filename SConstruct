# -*- python -*-

import sys, glob, os.path, datetime, pwd, time, fnmatch
sys.path.append('senfscons')
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
    changelog = file('debian/changelog.template').read() % {
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

def getLibDepends(script):
    # OUCH ...
    return os.popen("perl -0777 -n -e '$,=\" \"; print $1=~m/'\"'\"'([^'\"'\"']*)'\"'\"'/g if /LIBS\s*=\s*\[([^\]]*)\]/' %s" % script).read().split()

# Original topological sort code written by Ofer Faigon
# (www.bitformation.com) and used with permission
def topological_sort(items, partial_order):
    """Perform topological sort.
       items is a list of items to be sorted.
       partial_order is a list of pairs. If pair (a,b) is in it, it means
       that item a should appear before item b.
       Returns a list of the items in one of the possible orders, or None
       if partial_order contains a loop.
    """
    def add_node(graph, node):
        if not graph.has_key(node):
            graph[node] = [0] 
    def add_arc(graph, fromnode, tonode):
        graph[fromnode].append(tonode)
        graph[tonode][0] = graph[tonode][0] + 1
    graph = {}
    for v in items:
        add_node(graph, v)
    for a,b in partial_order:
        add_arc(graph, a, b)
    roots = [node for (node,nodeinfo) in graph.items() if nodeinfo[0] == 0]
    while len(roots) != 0:
        root = roots.pop()
        yield root
        for child in graph[root][1:]:
            graph[child][0] = graph[child][0] - 1
            if graph[child][0] == 0:
                roots.append(child)
        del graph[root]
    if len(graph.items()) != 0:
        raise RuntimeError, "Loop detected in partial_order"

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

def configFilesOpts(target, source, env, for_signature):
    return [ '-I%s' % os.path.split(f)[1] for f in env['LOCAL_CONFIG_FILES'] ]

env.Append(
   CPPPATH = [ '#/include' ],
   LIBS = [ 'iberty', '$BOOSTREGEXLIB', '$BOOSTFSLIB' ],
   DOXY_XREF_TYPES = [ 'bug', 'fixme', 'todo', 'idea' ],
   DOXY_HTML_XSL = '#/doclib/html-munge.xsl',
   ENV = { 'TODAY' : str(datetime.date.today()),
           'REVISION' : rev,
           'LOGNAME' : logname, # needed by the debian build scripts
           'CONCURRENCY_LEVEL' : env.GetOption('num_jobs') or "1",
           'SCONS' : 1,
         },
   LOCAL_CONFIG_FILES = [ 'Doxyfile.local', 'SConfig', 'local_config.hh' ],
   CONFIG_FILES_OPTS = configFilesOpts,
   CLEAN_PATTERNS = [ '*.pyc', 'semantic.cache', '.sconsign', '.sconsign.dblite' ],
   BUILDPACKAGE_COMMAND = "dpkg-buildpackage -us -uc -rfakeroot -I.svn $CONFIG_FILES_OPTS",
   TOP_INCLUDES = [ 'Packets', 'PPI', 'Scheduler', 'Socket', 'Utils',
                    'config.hh', 'local_config.hh' ]
)

Export('env')

# Create Doxyfile.local otherwise doxygen will barf on this non-existent file
if not env.GetOption('clean') and not os.path.exists("Doxyfile.local"):
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

for script in glob.glob("*/SConscript"):
    depends = getLibDepends(script)
    script = script.split('/',1)[0]
    scripts.append(script)
    dependencies += [ (dep, script) for dep in depends ]

for subdir in topological_sort(scripts, dependencies):
    SConscript(os.path.join(subdir, "SConscript"))
    
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
env.Alias('default', 'libsenf.a')

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
