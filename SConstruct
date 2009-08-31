# -*- python -*-

import sys, glob, os.path, fnmatch
import SENFSCons, senfutil

# Fix for SCons 0.97 compatibility
try:
    BoolVariable
except NameError:
    BoolVariable = BoolOption

###########################################################################
# Load utilities and setup libraries and configure build

env = Environment()

# Load all the local SCons tools
env.Tool('Doxygen')
env.Tool('Doxygen')
env.Tool('Dia2Png')
env.Tool('PkgDraw')
env.Tool('InstallSubdir')
env.Tool('CopyToDir')
env.Tool('Boost')
env.Tool('CombinedObject')
env.Tool('PhonyTarget')
env.Tool('InstallDir')

env.Help("""
Additional top-level build targets:

prepare      Create all target files not part of the repository
default      Build all default targets (like calling scons with no arguments)
examples     Build all examples
all_tests    Build and run unit tests for all modules
all_docs     Build documentation for all modules
all          Build everything
install_all  Install SENF into $$PREFIX
deb          Build debian source and binary package
debsrc       Build debian source package
debbin       Build debian binary package
linklint     Check links of doxygen documentation with 'linklint'
fixlinks     Fix broken links in doxygen documentation
valgrind     Run all tests under valgrind/memcheck
""")

env.Replace(
   PKGDRAW                = 'doclib/pkgdraw',
)

env.Append(
   ENV                    = { 'PATH' : os.environ.get('PATH') },
   CLEAN_PATTERNS         = [ '*~', '#*#', '*.pyc', 'semantic.cache', '.sconsign*' ],

   CPPPATH                = [ '#' ],
   LOCALLIBDIR            = '#',
   LIBPATH                = [ '$LOCALLIBDIR' ],
   LIBS                   = [ '$LIBSENF$LIBADDSUFFIX', 'rt', '$BOOSTREGEXLIB', 
                              '$BOOSTIOSTREAMSLIB', '$BOOSTSIGNALSLIB', '$BOOSTFSLIB' ], 
   TEST_EXTRA_LIBS        = [  ],

   PREFIX                 = '#/dist',
   LIBINSTALLDIR          = '$PREFIX${syslayout and "/lib" or ""}',
   BININSTALLDIR          = '$PREFIX${syslayout and "/bin" or ""}',
   INCLUDEINSTALLDIR      = '$PREFIX${syslayout and "/include" or ""}',
   CONFINSTALLDIR         = '${syslayout and "$LIBINSTALLDIR/senf" or "$PREFIX"}',
   OBJINSTALLDIR          = '$CONFINSTALLDIR',
   DOCINSTALLDIR          = '$PREFIX${syslayout and "/share/doc/senf" or "/manual"}',
   SCONSINSTALLDIR        = '$CONFINSTALLDIR/site_scons',

   CPP_INCLUDE_EXTENSIONS = [ '.h', '.hh', '.ih', '.mpp', '.cci', '.ct', '.cti' ],
   CPP_EXCLUDE_EXTENSIONS = [ '.test.hh' ],

   # These options are insane. Only useful for inline debugging. Need at least 1G free RAM
   INLINE_OPTS_DEBUG      = [ '-finline-limit=20000', '-fvisibility-inlines-hidden', 
                              '-fno-inline-functions', '-Winline' 
                              '--param','large-function-growth=10000',
                              '--param', 'large-function-insns=10000', 
                              '--param','inline-unit-growth=10000' ],
   INLINE_OPTS_NORMAL     = [ '-finline-limit=5000' ],
   INLINE_OPTS            = [ '$INLINE_OPTS_NORMAL' ],
   CXXFLAGS               = [ '-Wall', '-Woverloaded-virtual', '-Wno-long-long', '$INLINE_OPTS',
                              '$CXXFLAGS_' ],
   CXXFLAGS_              = senfutil.BuildTypeOptions('CXXFLAGS'),
   CXXFLAGS_final         = [ '-O3' ],
   CXXFLAGS_normal        = [ '-O0', '-g' ],
   CXXFLAGS_debug         = [ '$CXXFLAGS_normal' ],

   CPPDEFINES             = [ '$expandLogOption', '$CPPDEFINES_' ],
   expandLogOption        = senfutil.expandLogOption,
   CPPDEFINES_            = senfutil.BuildTypeOptions('CPPDEFINES'),
   CPPDEFINES_final       = [ ],
   CPPDEFINES_normal      = [ 'SENF_DEBUG' ],
   CPPDEFINES_debug       = [ '$CPPDEFINES_normal' ],

   LINKFLAGS              = [ '-rdynamic', '$LINKFLAGS_' ],
   LINKFLAGS_             = senfutil.BuildTypeOptions('LINKFLAGS'),
   LINKFLAGS_final        = [ ],
   LINKFLAGS_normal       = [ '-Wl,-S' ],
   LINKFLAGS_debug        = [ '-g' ],
)

env.SetDefault(
    LIBSENF   = "senf",
    final     = False,
    debug     = False,
    syslayout = False
)

# Set variables from command line
senfutil.parseArguments(
    env,
    BoolVariable('final', 'Build final (optimized) build', False),
    BoolVariable('debug', 'Link in debug symbols', False),
    BoolVariable('syslayout', 'Install in to system layout directories (lib/, include/ etc)', False),
)

Export('env')

# Create Doxyfile.local otherwise doxygen will barf on this non-existent file
# Create it even when cleaning, to silence the doxygen builder warnings
if not os.path.exists("Doxyfile.local"):
    Execute(Touch("Doxyfile.local"))

if not env.GetOption('clean') and not os.path.exists(".prepare-stamp") \
   and not os.environ.get("SCONS") and COMMAND_LINE_TARGETS != [ 'prepare' ]:
    env.Execute([ "scons prepare" ])

# Load SConscripts

SConscriptChdir(0)
SConscript("debian/SConscript")
SConscriptChdir(1)
if os.path.exists('SConscript.local') : SConscript('SConscript.local')
SConscript("senf/SConscript")
SConscript("Examples/SConscript")
SConscript("HowTos/SConscript")
SConscript("doclib/SConscript")

###########################################################################
# Define build targets

#### doc
env.Depends(SENFSCons.Doxygen(env), env.Value(env['ENV']['REVISION']))

#### libsenf.a
libsenf = env.Library("$LOCALLIBDIR/${LIBSENF}${LIBADDSUFFIX}", env['ALLOBJECTS'])
env.Default(libsenf)
env.Install('$LIBINSTALLDIR', libsenf)

def create(target, source, env): 
    file(str(target[0]), 'w').write(source[0].get_contents()+"\n")
env['BUILDERS']['CreateFile'] = Builder(action = create)

conf = env.CreateFile("${LOCALLIBDIR}/${LIBSENF}${LIBADDSUFFIX}.conf", 
                      env.Value(env.subst("$_CPPDEFFLAGS")))
env.Default(conf)
env.Install('$CONFINSTALLDIR', conf)

#### install_all, default, all_tests, all
env.Install('${SCONSINSTALLDIR}', 'site_scons/senfutil.py')

env.Alias('install_all', env.FindInstalledFiles())
env.Alias('default', DEFAULT_TARGETS)
env.Alias('all_tests', env.FindAllBoostUnitTests())
env.Alias('all', [ 'default', 'all_tests', 'examples', 'all_docs' ])

#### prepare
env.PhonyTarget('prepare', [], [])

#### valgrind
env.PhonyTarget('valgrind', [ 'all_tests' ], [ """
    find -name .test.bin 
        | while read test; do
            echo;
            echo "Running $$test";
            echo;
            valgrind --tool=memcheck --error-exitcode=99 --suppressions=valgrind.sup 
                $$test $BOOSTTESTARGS;
            [ $$? -ne 99 ] || exit 1;
        done
""".replace("\n"," ") ])

#### clean
env.Clean('all', '.prepare-stamp')
env.Clean('all', libsenf)
env.Clean('all', env.Dir('linklint')) # env.Dir to disambiguate from linklint PhonyTarget
env.Clean('all', env.Dir('dist'))

if env.GetOption('clean'):
    env.Clean('all', [ os.path.join(path,f)
                       for path, subdirs, files in os.walk('.')
                       for pattern in env['CLEAN_PATTERNS']
                       for f in fnmatch.filter(files,pattern) ])

if not env.GetOption('clean') and not os.path.exists(".prepare-stamp"):
    Execute(Touch(".prepare-stamp"))
