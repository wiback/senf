## \file
# \brief BoostUnitTests build

## \package senfscons.BoostUnitTests
# \brief Builder utilizing the <a href="http://www.boost.org/libs/test/doc/index.html">Boost.Test</a> unit-test framework
#
# The BoostUnitTests builder will build a unit-test executable using
# the <a
# href="http://www.boost.org/libs/test/doc/index.html">Boost.Test</a>
# library. After building, the unit-test will be executed.
#
# This builder is used by the SENFSCons.Object() helper to build the
# unit test.
#
# \par Construction Envrionment Variables:
# <table class="senf">
# <tr><td>\c BOOSTTESTLIB</td><td>Name of the library to use, defaults to \c boost_unit_test_framework</td></tr>
# <tr><td>\c BOOSTTESTARGS</td><td>Command line arguments of the test, defaults to <tt>--build_info=yes --log_level=test_suite</tt></td></tr>
# </table>
#
# \todo This is not really a builder. This should be rewritten as one
# \ingroup builder

import SCons.Script.SConscript
import SCons.Defaults
import os.path
import os

def BoostUnitTests(env, target, source, test_source=None, LIBS = [], DEPENDS = [], **kw):
    path, name = os.path.split(target)
    if test_source:
        if type(test_source) is not type([]):
            test_source = [ test_source ]
    else:
        test_source = []
    testEnv = env.Copy(**kw)
    testEnv.Prepend(LIBS = '$BOOSTTESTLIB')
    testEnv.Prepend(LIBS = LIBS)
    sources = []
    if source:
        sources = sources + env.Object(source)
    sources = sources + test_source
    binName = os.path.join(path,'.' + name +'.bin')
    testRunner = testEnv.Program(binName, sources)
    stamp = os.path.join(path,'.' + os.path.splitext(name)[0]+'.stamp')
    if DEPENDS:
        env.Depends(testRunner, DEPENDS)
    return env.Command([ stamp ], testRunner,
                       [ '$SOURCE $BOOSTTESTARGS',
                         'touch $TARGET' ])

def dispatcher(*arg,**kw):
    return BoostUnitTests(*arg,**kw)

def generate(env):
    env['BOOSTTESTLIB'] = 'boost_unit_test_framework'
    env['BOOSTTESTARGS'] = [ '--build_info=yes', '--log_level=test_suite' ]
    env.__class__.BoostUnitTests = dispatcher

def exists(env):
    return 1
