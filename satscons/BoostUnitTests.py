import SCons.Script.SConscript
import SCons.Defaults
import os.path
import os

def BoostUnitTests(env, target, source, test_sources=None, **kw):
    path, name = os.path.split(target)
    if test_sources:
        if type(test_sources) is not type([]):
            test_sources = [ test_sources ]
    else:
        test_sources = []
    testEnv = env.Copy(**kw)
    testEnv.Append(LIBS = '$BOOSTTESTLIB')
    testRunner = testEnv.Program('test', env.Object(source) + test_sources)
    return env.Alias(target, env.Command(os.path.join(path,'.'+name+'.stamp'), testRunner,
                                  [ './$SOURCE $BOOSSTTESTARGS', 'touch $TARGET' ]))


def dispatcher(*arg,**kw):
    return BoostUnitTests(*arg,**kw)
    
def generate(env):
    env['BOOSTTESTLIB'] = 'boost_unit_test_framework'
    env['BOOSTTESTARGS'] = [ '--build_info=yes', '--log_level=test_suite' ]
    env.__class__.BoostUnitTests = dispatcher

def exists(env):
    return 1
