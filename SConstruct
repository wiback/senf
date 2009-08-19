# -*- python -*-

import sys, glob, os.path, datetime, pwd, time, fnmatch, string
sys.path.append(Dir('#/senfscons').abspath)
sys.path.append(Dir('#/doclib').abspath)
import SENFSCons, senfutil

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

# Options used to debug inlining:
#
# INLINE_OPTS = [ '-finline-limit=20000', '--param','large-function-growth=10000',
#                 '--param', 'large-function-insns=10000', '--param','inline-unit-growth=10000',
#                 '-fvisibility-inlines-hidden', '-fno-inline-functions', '-Winline' ]
#
# BEWARE: You need lots of ram to compile with these settings (approx 1G)

INLINE_OPTS = [ '-finline-limit=5000' ]

env.Append(
   CPPPATH         = [ '#/include' ],
   CXXFLAGS        = [ '-Wall', '-Woverloaded-virtual', '-Wno-long-long' ] + INLINE_OPTS,
   LIBS            = [ 'rt', '$BOOSTREGEXLIB', '$BOOSTIOSTREAMSLIB', '$BOOSTSIGNALSLIB',
                       '$BOOSTFSLIB' ],
   TEST_EXTRA_LIBS = [ ],
   ENV             = { 'PATH' : os.environ.get('PATH') },
   CLEAN_PATTERNS  = [ '*~', '#*#', '*.pyc', 'semantic.cache', '.sconsign', '.sconsign.dblite' ],
)

env.SetDefault(
    LIBSENF = "senf"
)

# Parse the log option command line parameter into the SENF_LOG_CONF macro
senfutil.setLogOption(env)

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

# Load SConscripts. Need to load debian and doclib first (they change the global environment)
SConscript("debian/SConscript")
SConscript("doclib/SConscript")
SConscript(list(set(glob.glob("*/SConscript")) - set(("debian/SConscript", "doclib/SConscript"))))

# Define the main targets
SENFSCons.StandardTargets(env)
SENFSCons.GlobalTargets(env)

env.Depends( SENFSCons.Doxygen(env), env.Value(env['ENV']['REVISION']) )

libsenf = env.Library(env.subst("$LIBSENF$LIBADDSUFFIX"), env['ALLOBJECTS'])
env.Default(libsenf)
env.Clean('all', libsenf)
env.Alias('default', libsenf)

SENFSCons.InstallIncludeFiles(env, [ 'config.hh' ])
env.Alias('install_all', env.Install('$LIBINSTALLDIR', libsenf))

if env.GetOption('clean'):
    env.Clean('all', [ os.path.join(path,f)
                       for path, subdirs, files in os.walk('.')
                       for pattern in env['CLEAN_PATTERNS']
                       for f in fnmatch.filter(files,pattern) ])

SENFSCons.PhonyTarget(env, 'prepare', [ 'true' ])

SENFSCons.PhonyTarget(env, 'valgrind', [ """
    find -name .test.bin 
        | while read test; do
            echo;
            echo "Running $$test";
            echo;
            valgrind --tool=memcheck --error-exitcode=99 --suppressions=valgrind.sup 
                $$test $BOOSTTESTARGS;
            [ $$? -ne 99 ] || exit 1;
        done
""".replace("\n"," ") ], [ 'all_tests' ])

if not env.GetOption('clean') and not os.path.exists(".prepare-stamp"):
    Execute(Touch(".prepare-stamp"))
