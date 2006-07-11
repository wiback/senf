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
    testEnv.Append(LIBS = '$BOOSTTESTLIB')
    testEnv.Append(LIBS = LIBS)
    testRunner = testEnv.Program('test', env.Object(source) + test_source)
    if DEPENDS:
        env.Depends(testRunner, DEPENDS)
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
