#
# Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
#                    Network Research Group (NET)
#                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
#                    Contact: support@wiback.org
#
# This file is part of the SENF code tree.
# It is licensed under the 3-clause BSD License (aka New BSD License).
# See LICENSE.txt in the top level directory for details or visit
# https://opensource.org/licenses/BSD-3-Clause
#


import SENFSCons
import os.path
import senfutil
import sys

###########################################################################
# Load utilities and setup libraries and configure build

env = Environment()

env.Decider('MD5-timestamp')
env.EnsureSConsVersion(1,2)

# Load all the local SCons tools
senfutil.loadTools(env)

env.Help("""
Additional top-level build targets:

prepare         Create all target files not part of the repository
default         Build all default targets (like calling scons with no arguments)
examples        Build all examples
all_tests       Build and run unit tests for all modules
test_changes    Build tests only for files with local changes (queries svn or git)
all_docs        Build documentation for all modules
all             Build everything
install_all     Install SENF into $$PREFIX
deb             Build debian source and binary package
debsrc          Build debian source package
debbin          Build debian binary package
linklint        Check links of doxygen documentation with 'linklint'
fixlinks        Fix broken links in doxygen documentation
all_valgrinds   Run all tests under valgrind/memcheck
lcov            Generate test coverage output in doc/lcov and lcov.info

The following additional targets may be called within subdirectories, either
using '$ scons -u <target>'  or '$ scons <directory>/<target>:

test            Build and run unit test for this module
doc             Build the documentation of this module
valgrind        Run the unit test of this module under valgrind

When cleaning up using '$ scons -c <target>', some targets are handled specially:

all             Remove everything generated by the build including temporary and
                backup files

some            Remove all files not needed for building like temporary or backup
                files. This target is only valid when called as clean target.

You may execute targets on a remote host via ssh (if the directory layout is the
same) by calling

    $ scons <target>@[<user>@]<host>

Some more elaborate unit tests may be enabled by setting appropritate variables
in the shell (unix) environment

SENF_TIMING_CRITICAL_TESTS
                Enables unit tests which depend on timing measurements. These
                unit tests should only be run on a single core and an otherwise
                idle system.

SENF_WLAN_TEST_INTERFACE
                WLAN interface to use for testing. The interface should not be
                actively in use.

SENF_ETH_TEST_INTERFACE
                Ethernet interface to use for testing. The interface should not
                be actively in use.

Some unit tests will only run when executed to 'root'.
""")

env.Replace(
    expandLogOption        = senfutil.expandLogOption,
    CXXFLAGS_              = env.BuildTypeOptions('CXXFLAGS'),
    CPPDEFINES_            = env.BuildTypeOptions('CPPDEFINES'),
    LINKFLAGS_             = env.BuildTypeOptions('LINKFLAGS'),
)
env.Append(
    IMPORT_ENV             = [ 'PATH', 'HOME', 'SSH_*', 'SENF*', 'CCACHE_*', 'DISTCC_*' ],

    CLEAN_SOME_PATTERNS    = [ '*~', '#*#', '*.pyc', 'semantic.cache' ],
    CLEAN_PATTERNS         = [ '.sconsign*', '.sconf_temp' ],

    CPPPATH                = [ ],
    CPPFLAGS               = [ ],
    LIBPATH                = [ '$LOCALLIBDIR' ],
    LIBS                   = [ '$EXTRA_LIBS' ],
    EXTRA_LIBS             = [ 'rt' ],
    TEST_EXTRA_LIBS        = [  ],
    VALGRINDARGS           = [ '--num-callers=50' ],

    CPP_INCLUDE_EXTENSIONS = [ '.h', '.hh', '.ih', '.mpp', '.cci', '.ct', '.cti' ],
    CPP_EXCLUDE_EXTENSIONS = [ '.test.hh' ],

    # INLINE_OPTS_DEBUG are insane. Only useful for inline debugging. Need at least 1G free RAM
    INLINE_OPTS_DEBUG      = [ '-finline-limit=20000', '-fvisibility-inlines-hidden',
                               '-fno-inline-functions', '-Winline'
                               '--param','large-function-growth=10000',
                               '--param', 'large-function-insns=10000',
                               '--param','inline-unit-growth=10000' ],
    #INLINE_OPTS_GCC        = [ '-finline-limit=5000', '--param', 'inline-unit-growth=60' ],
    INLINE_OPTS            = [ '${str(CXX).split("/")[-1] == "g++" and "$INLINE_OPTS_GCC" or None}' ],
    CXXFLAGS_CLANG         = [ '-Wno-unneeded-internal-declaration', '-Wheader-hygiene', '-Wno-deprecated'],
    CXXFLAGS_GCC           = [ '${HAVE_GCC_47 and CXXFLAGS_GCC_47 or []}'],
    HAVE_GCC_47            = False,
    CXXFLAGS_GCC_47        = [ '-Wdouble-promotion', '-fipa-pta' ],
    CXXFLAGS               = [ '-Wall', '-Wextra', '-Werror',
                              '-Woverloaded-virtual', '-Wnon-virtual-dtor', '-Wcast-align', '-Winit-self',                              
                               '-Wdisabled-optimization', '-Wpointer-arith', '-Wcast-qual', '-Wsign-promo',
                               '-Wno-long-long', '-Wno-unused-parameter',
                               '$INLINE_OPTS', '-pipe', '$CXXFLAGS_', '-fno-strict-aliasing',
                               '${profile and "-pg" or None}',
                               '${lto and "-flto" or None}',
                               '${cxx11 and "-std=c++11" or None}',
                               '${str(CXX).split("/")[-1].startswith("g++") and "$CXXFLAGS_GCC" or None}',
                               '${str(CXX).split("/")[-1] == "clang++" and "$CXXFLAGS_CLANG" or None}',
                               '${debug_final and "-g" or None}'],
    CXXFLAGS_final         = [ '-O3', '-fno-threadsafe-statics','-fno-stack-protector',
                               "${profile and ' ' or '-ffunction-sections'}" ],
    CXXFLAGS_normal        = [ '-O2', '-g' ],
    CXXFLAGS_debug         = [ '-O0', '-g' ],

    CPPDEFINES             = [ '$expandLogOption', '$CPPDEFINES_' ],
    CPPDEFINES_final       = [ 'SENF_PPI_NOTRACE', 'NDEBUG',
                               'BOOST_NO_MT', 'BOOST_DISABLE_ASSERTS', 'BOOST_DISABLE_THREADS' ],
    CPPDEFINES_normal      = [ 'SENF_DEBUG' ],
    CPPDEFINES_debug       = [ '$CPPDEFINES_normal' ],

    LINKFLAGS              = [ '-rdynamic', '$LINKFLAGS_', "${profile and '-pg' or None}",
                               "${lto and '-flto -fwhole-program' or None}" ],
    LINKFLAGS_final        = [ "${profile and ' ' or '-Wl,--gc-sections'}" ],
    LINKFLAGS_normal       = [ '-Wl,-S' ],
    LINKFLAGS_debug        = [ '-g' ],
)
env.SetDefault(
    PREFIX                 = '#/dist',
    LIBINSTALLDIR          = '$PREFIX${syslayout and "/lib" or ""}',
    BININSTALLDIR          = '$PREFIX${syslayout and "/bin" or ""}',
    INCLUDEINSTALLDIR      = '$PREFIX${syslayout and "/include" or ""}',
    CONFINSTALLDIR         = '${syslayout and "$LIBINSTALLDIR/senf" or "$PREFIX"}',
    OBJINSTALLDIR          = '$CONFINSTALLDIR',
    DOCINSTALLDIR          = '$PREFIX${syslayout and "/share/doc/senf" or "/manual"}',
    SCONSINSTALLDIR        = '$CONFINSTALLDIR/site_scons',

    LIBSENF                = "senf",
    LCOV                   = "lcov",
    GENHTML                = "genhtml",
    VALGRIND               = "valgrind",
    SCONSBIN               = "scons",
    SCONSARGS              = ([ '-Q', '-j$CONCURRENCY_LEVEL' ] +
                              [ '%s=%s' % (k,v) for k,v in ARGLIST ]),
    SCONS                  = "@$SCONSBIN $SCONSARGS",
    CONCURRENCY_LEVEL      = env.GetOption('num_jobs') or 1,
    TOPDIR                 = env.Dir('#').abspath,
    LIBADDSUFFIX           = '${FLAVOR and "_$FLAVOR" or ""}',
    OBJADDSUFFIX           = '${LIBADDSUFFIX}',
    FLAVOR                 = '',

    PARSEFLAGS             = '',
)

# Add UNIX env vars matching IMPORT_ENV patterns into the execution environment
senfutil.importProcessEnv(env)

# Set variables from command line
senfutil.ParseDefaultArguments(
    env,
    BoolVariable('syslayout', 'Install in to system layout directories (lib/, include/ etc)', False),
    BoolVariable('sparse_tests', 'Link tests against object files and not the senf lib', False),
    BoolVariable('builddir', 'use build dir build/{platform}_{build_type}', False),
    BoolVariable('asan', 'build with enabled AddressSanitizer', False),
    BoolVariable('ubsan', 'build with enabled UndefinedBehaviorSanitizer', False),
)

if not env.GetOption('clean') and not env['cxx11']:
    env.Fail( 'No C++11 standard enabled, which is required.. Please compile SENF with std=c++11:\n'
              '  $ scons cxx11=1 ...\n')

# Handle 'test_changes'
if 'test_changes' in COMMAND_LINE_TARGETS and not env.has_key('only_tests'):
    import SparseTestHack
    env['only_tests'] = " ".join(x.abspath for x in SparseTestHack.findSCMChanges(env))
    if not env['only_tests']:
        import SCons.Util
        SCons.Util.display("scons: no changes to be tested found")
        Exit(0)

if env.has_key('only_tests') : env['sparse_tests'] = True

if env['ubsan']:
    env.Append(
        CXXFLAGS           = [ '-fsanitize=undefined', '-fPIC' ],
        LIBS               = [ 'ubsan' ]
    )

if env['asan']:
    env.Append(
        LIBS               = [ 'asan' ],
        CXXFLAGS           = [ '-fsanitize=address', '-fPIC' ],    
    )

if env['builddir']:
    env.Replace(
        BUILDDIR           = os.path.join('build', env['VARIANT']),
        LOCALLIBDIR        = os.path.join('#', 'build', env['VARIANT'])
    )
    env.Append(
        CPPPATH            = [ os.path.join('#', 'build', env['VARIANT']) ]
    )
else:
    env.Replace(
        BUILDDIR           = '#',
        LOCALLIBDIR        = '#',
    )
    env.Append(
        CPPPATH            = [ '#' ]
    )


Export('env')

###########################################################################
# Configure

env['CONFIGUREDIR'] = os.path.join(env.subst('$BUILDDIR'), '.sconf_temp')
env['CONFIGURELOG'] = os.path.join(env.subst('$BUILDDIR'), 'config.log')
SConscript('SConfigure')

# Only add this here, after all configure checks have run

env.Prepend(LIBS = [ '$LIBSENF$LIBADDSUFFIX',
                     '$BOOSTREGEXLIB', '$BOOSTSIGNALSLIB',
                     '$BOOSTFSLIB', '$BOOSTSYSTEMLIB', '$BOOSTDATETIMELIB' ])

if env.has_key('BOOST_EXT'):
    for boostlib in env['BOOST_EXT']:
        env.Append( CPPPATH = [ '#/boost/' + boostlib ])

###########################################################################

# Create Doxyfile.local otherwise doxygen will barf on this non-existent file
# Create it even when cleaning, to silence the doxygen builder warnings
if not os.path.exists("doclib/Doxyfile.local"):
    Execute(Touch("doclib/Doxyfile.local"))

if not env.GetOption('clean') and not os.path.exists(".prepare-stamp") \
   and not os.environ.get("SCONS") and COMMAND_LINE_TARGETS != [ 'prepare' ]:
    env.Execute([ "$SCONS prepare" ])

# Load SConscripts

SConscriptChdir(0)
SConscript("debian/SConscript")
SConscriptChdir(1)
if os.path.exists('SConscript.local') : SConscript('SConscript.local')
if env['sparse_tests']:
    import SparseTestHack
    SparseTestHack.setup(env)
if env.subst('$BUILDDIR') == '#':
    SConscript("SConscript")
else:
    SConscript("SConscript", variant_dir=env.subst('$BUILDDIR'), src_dir='#')
SConscript("Examples/SConscript")
SConscript("HowTos/SConscript")
SConscript("doclib/SConscript")
if env['sparse_tests']:
    verbose = 'test_changes' in COMMAND_LINE_TARGETS
    SparseTestHack.build(env, verbose, verbose)

###########################################################################
# Define build targets

#### install_all, default, all_tests, all
env.Install('${SCONSINSTALLDIR}', [ 'site_scons/__init__.py',
                                    'site_scons/senfutil.py',
                                    'site_scons/yaptu.py' ])
env.InstallDir('${SCONSINSTALLDIR}', [ 'site_scons/site_tools', 'site_scons/lib' ],
               FILTER_SUFFIXES=[ '','.css','.pl','.py','.sh','.sty','.xml','.xsl','.yap' ])

env.Alias('install_all', env.FindInstalledFiles())
env.Alias('default', DEFAULT_TARGETS)
env.Alias('all_tests', env.FindAllBoostUnitTests())
env.Alias('test_changes', 'all_tests')
env.Alias('all', [ 'default', 'all_tests', 'examples', 'all_docs' ])

#### prepare and -c some
env.PhonyTarget('prepare', [], [])
env.PhonyTarget('some', [], [])

#### valgrind
env.Alias('all_valgrinds')
if env.get('HAVE_VALGRIND'):
    for test in env.FindAllBoostUnitTests():
        stamp = env.Command(test[0].dir.File('.test-valgrind.stamp'),
                            [ test[0].dir.File('.test.bin'), 'tools/valgrind.sup' ],
                            [ """$VALGRIND --tool=memcheck
                                          --error-exitcode=1
                                          --suppressions=${SOURCES[1]}
                                          $VALGRINDARGS
                                              ${SOURCES[0]} --result_code=no $BOOSTTESTARGS
                              """.replace("\n"," "),
                              Touch("$TARGET") ])
        alias = env.Command(test[0].dir.File('valgrind'), stamp, [ env.NopAction() ])
        env.Alias('all_valgrinds', alias)

### lcov
env.PhonyTarget('lcov', [], [
        '$SCONS'
        '    debug=1'
        '    BUILDDIR="#/build/lcov"'
        '    CCFLAGS+="-fprofile-arcs -ftest-coverage"'
        '    LIBS+="gcov"'
        '        all_tests',
        '$LCOV'
        '    --follow'
        '    --directory $TOPDIR/build/lcov/senf'
        '    --capture'
        '    --output-file /tmp/senf_lcov.info'
        '    --base-directory $TOPDIR',
        '$LCOV'
        '    --output-file lcov.info'
        '    --remove /tmp/senf_lcov.info "*/include/*" "*/boost/*" "*.test.*"',
        '$GENHTML'
        '    --output-directory doc/lcov'
        '    --title all_tests lcov.info',
        'rm /tmp/senf_lcov.info' ])
senfutil.CleanGlob(env, ['lcov','some','all'], [ '*.gcno', '*.gcda', '*.gcov' ])
env.Clean(['lcov', 'all'], [ 'lcov.info', env.Dir('doc/lcov'), env.Dir('build/lcov') ])

#### clean

env.Clean('all', ('.prepare-stamp', env.Dir('dist'), env.Dir('build')))

senfutil.CleanGlob(env, 'all', '$CLEAN_PATTERNS')
senfutil.CleanGlob(env, ['some', 'all'], '$CLEAN_SOME_PATTERNS')

if env.GetOption('clean') and 'all' in BUILD_TARGETS:
    env.Depends('all', ('lcov', 'all_valgrinds'))
    # Disable writing to the deleted .sconsign file
    import SCons.SConsign
    SCons.SConsign.write = lambda : None

if not env.GetOption('clean') and not os.path.exists(".prepare-stamp"):
    Execute(Touch(".prepare-stamp"))

### execute targets on remote hosts
for target in COMMAND_LINE_TARGETS:
    if '@' in target:
        realtarget, host = target.split('@',1)
        cwd=env.GetLaunchDir()
        home=os.environ['HOME']+'/'
        if cwd.startswith(home) : cwd = cwd[len(home):]
        args = [ '$SCONSARGS' ]
        if env.GetLaunchDir() != os.getcwd():
            args.append('-u')
        env.PhonyTarget(target, [], [ "ssh $HOST scons $SCONSARGS -C $DIR $RTARGET" ],
                        HOST=host, RTARGET=realtarget, DIR=cwd, SCONSARGS=args)

env.PhonyTarget('clean', [], [
        lambda **args: sys.stderr.write(
            "=================================================================\n"
            "'clean' is not a valid target, use the '-c' option instead:\n"
            "    $ scons -c all\n"
            "=================================================================\n") ])
