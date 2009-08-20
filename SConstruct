# -*- python -*-

import sys, glob, os.path, datetime, pwd, time, fnmatch, string
sys.path.append(Dir('#/senfscons').abspath)
sys.path.append(Dir('#/doclib').abspath)
import SENFSCons, senfutil

###########################################################################
# Load utilities and setup libraries and configure build

env = Environment()

# Load all the local SCons tools
env.Tool('Doxygen', [ 'senfscons' ])
env.Tool('Dia2Png', [ 'senfscons' ])
env.Tool('PkgDraw', [ 'senfscons' ])
env.Tool('InstallSubdir', [ 'senfscons' ])
env.Tool('CopyToDir', [ 'senfscons' ])
env.Tool('Boost', [ 'senfscons' ])
env.Tool('CombinedObject', [ 'senfscons' ])
env.Tool('PhonyTarget', [ 'senfscons' ])

env.Help("""
Additional top-level build targets:

prepare      Create all target files not part of the repository
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

class BuildTypeOptions:
    def __init__(self, var):
        self._var = var

    def __call__(self, target, source, env, for_signature):
        type = env['final'] and "final" or env['debug'] and "debug" or "normal"
        return env[self._var + "_" + type]

env.Replace(
   PKGDRAW                = 'doclib/pkgdraw',
)

env.Append(
   ENV                    = { 'PATH' : os.environ.get('PATH') },
   CLEAN_PATTERNS         = [ '*~', '#*#', '*.pyc', 'semantic.cache', '.sconsign*', '.sconsign' ],

   CPPPATH                = [ '#/include' ],
   LOCALLIBDIR            = '#',
   LIBPATH                = [ '$LOCALLIBDIR' ],
   LIBS                   = [ '$LIBSENF$LIBADDSUFFIX', 'rt', '$BOOSTREGEXLIB', 
                              '$BOOSTIOSTREAMSLIB', '$BOOSTSIGNALSLIB', '$BOOSTFSLIB' ], 
   TEST_EXTRA_LIBS        = [  ],

   PREFIX                 = '/usr/local',
   LIBINSTALLDIR          = '$PREFIX/lib',
   BININSTALLDIR          = '$PREFIX/bin',
   INCLUDEINSTALLDIR      = '$PREFIX/include',
   OBJINSTALLDIR          = '$LIBINSTALLDIR',
   DOCINSTALLDIR          = '$PREFIX/doc',
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
   CXXFLAGS_              = BuildTypeOptions('CXXFLAGS'),
   CXXFLAGS_final         = [ '-O3' ],
   CXXFLAGS_normal        = [ '-O0', '-g' ],
   CXXFLAGS_debug         = [ '$CXXFLAGS_normal' ],

   CPPDEFINES             = [ '$expandLogOption', '$CPPDEFINES_' ],
   expandLogOption        = senfutil.expandLogOption,
   CPPDEFINES_            = BuildTypeOptions('CPPDEFINES'),
   CPPDEFINES_final       = [ ],
   CPPDEFINES_normal      = [ 'SENF_DEBUG' ],
   CPPDEFINES_debug       = [ '$CPPDEFINES_normal' ],

   LINKFLAGS              = [ '-rdynamic', '$LINKFLAGS_' ],
   LINKFLAGS_             = BuildTypeOptions('LINKFLAGS'),
   LINKFLAGS_final        = [ ],
   LINKFLAGS_normal       = [ '-Wl,-S' ],
   LINKFLAGS_debug        = [ ],
)

env.SetDefault(
    LIBSENF = "senf",
    final   = 0,
    debug   = 0,
)

# Set variables from command line
env.Replace(**ARGUMENTS)

Export('env')

# Create Doxyfile.local otherwise doxygen will barf on this non-existent file
# Create it even when cleaning, to silence the doxygen builder warnings
if not os.path.exists("Doxyfile.local"):
    Execute(Touch("Doxyfile.local"))

# Create local_config.h
if not env.GetOption('clean') and not os.path.exists("local_config.hh"):
    Execute(Touch("local_config.hh"))

if not env.GetOption('clean') and not os.path.exists(".prepare-stamp") \
   and not os.environ.get("SCONS") and COMMAND_LINE_TARGETS != [ 'prepare' ]:
    env.Execute([ "scons prepare" ])

# Load SConscripts. Need to load some first (they change the global environment)
initSConscripts = [ 
    "debian/SConscript",
    "doclib/SConscript",
]

SConscript(initSConscripts)

if os.path.exists('SConscript.local'):
    SConscript('SConscript.local')

SConscript(list(set(glob.glob("*/SConscript")) - set(initSConscripts)))

###########################################################################
# Define build targets

#### doc
env.Depends(SENFSCons.Doxygen(env), env.Value(env['ENV']['REVISION']))

#### libsenf.a
libsenf = env.Library(env.subst("$LIBSENF$LIBADDSUFFIX"), env['ALLOBJECTS'])
env.Default(libsenf)

env.InstallSubdir(target = '$INCLUDEINSTALLDIR', source = [ 'config.hh' ])
env.Install('$LIBINSTALLDIR', libsenf)

#### install_all, default, all_tests, all
env.Alias('install_all', env.FindInstalledFiles())
env.Alias('default', DEFAULT_TARGETS)
env.Alias('all_tests', env.FindAllBoostUnitTests())
env.Alias('all', [ 'default', 'all_tests', 'all_docs' ])

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
env.Clean('all', 'linklint')

if env.GetOption('clean'):
    env.Clean('all', [ os.path.join(path,f)
                       for path, subdirs, files in os.walk('.')
                       for pattern in env['CLEAN_PATTERNS']
                       for f in fnmatch.filter(files,pattern) ])

if not env.GetOption('clean') and not os.path.exists(".prepare-stamp"):
    Execute(Touch(".prepare-stamp"))
