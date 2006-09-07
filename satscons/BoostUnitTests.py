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
    sources = []
    if source:
        sources = sources + env.Object(source)
    sources = sources + test_source
    binName = os.path.join(path,'.' + os.path.splitext(name)[0]+'.bin')
    testRunner = testEnv.Program(binName, sources)
    stamp = os.path.join(path,'.' + os.path.splitext(name)[0]+'.stamp')
    if DEPENDS:
        env.Depends(testRunner, DEPENDS)
    return env.Command([ target, stamp ], testRunner,
                       [ '( $SOURCE $BOOSTTESTARGS 2>&1 && touch ${TARGETS[1]} ) | tee ${TARGETS[0]}' ])

def dispatcher(*arg,**kw):
    return BoostUnitTests(*arg,**kw)
    
def generate(env):
    env['BOOSTTESTLIB'] = 'boost_unit_test_framework'
    env['BOOSTTESTARGS'] = [ '--build_info=yes', '--log_level=test_suite' ]
    env.__class__.BoostUnitTests = dispatcher

def exists(env):
    return 1
