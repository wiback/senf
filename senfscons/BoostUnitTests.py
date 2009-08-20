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

def BoostUnitTests(env, target=None, source=None,  **kw):
    target = env.arg2nodes(target)[0]

    binnode = target.dir.File('.' + target.name + '.bin')
    stampnode = target.dir.File('.' + target.name + '.stamp')

    bin = env.Program(binnode, source, 
                      LIBS = env['LIBS'] + [ '$TEST_EXTRA_LIBS' ],
                      _LIBFLAGS = ' -Wl,-Bstatic -l$BOOSTTESTLIB -Wl,-Bdynamic ' + env['_LIBFLAGS'],
                      **kw)

    stamp = env.Command(stampnode, bin,
                        [ '$SOURCE $BOOSTTESTARGS',
                          'touch $TARGET' ],
                        **kw)

    return env.Command(env.File(target), stamp, [ 'true' ])

def generate(env):
    env['BOOSTTESTLIB'] = 'boost_unit_test_framework'
    env['BOOSTTESTARGS'] = [ '--build_info=yes', '--log_level=test_suite' ]
    env['BUILDERS']['BoostUnitTests'] = BoostUnitTests

def exists(env):
    return 1
