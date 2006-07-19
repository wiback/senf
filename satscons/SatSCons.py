import os.path, SCons.Options, SCons.Environment, SCons.Script.SConscript, glob

opts = None
finalizers = []

testTargets = []

def InitOpts():
    global opts
    if opts is not None: return
    opts = SCons.Options.Options('SConfig')
    opts.Add('CXX', 'C++ compiler to use', 'g++')
    opts.Add(SCons.Options.BoolOption('final','Enable optimization',0))

def Finalizer(f):
    global finalizers
    finalizers.append(f)

def UseBoost():
    global opts
    InitOpts()
    opts.Add('BOOST_INCLUDES', 'Boost include directory', '')
    opts.Add('BOOST_VARIANT', 'The boost variant to use', '')
    opts.Add('BOOST_LIBDIR', 'The directory of the boost libraries', '')
    Finalizer(FinalizeBoost)

def FinalizeBoost(env):
    env.Tool('BoostUnitTests', [os.path.split(__file__)[0]])
    env['BOOSTTESTLIB'] = 'libboost_unit_test_framework' + env['BOOST_VARIANT']
    env.Append(LIBPATH = [ '$BOOST_LIBDIR' ],
               CPPPATH = [ '$BOOST_INCLUDES' ])
    
def UseSTLPort():
    global opts
    InitOpts()
    opts.Add('STLPORT_INCLUDES', 'STLport include directory', '')
    opts.Add('STLPORT_LIB', 'Name of the stlport library or empty to not use stlport', '')
    opts.Add('STLPORT_LIBDIR', 'The directory of the stlport libraries','')
    Finalizer(FinalizeSTLPort)

def FinalizeSTLPort(env):
    env['STLPORT_DEBUGLIB'] = ''
    if env['STLPORT_LIB']:
        env['STLPORT_DEBUGLIB'] = env['STLPORT_LIB'] + '_stldebug'
        env.Append(LIBPATH = [ '$STLPORT_LIBDIR' ],
                   CPPPATH = [ '$STLPORT_INCLUDES' ])
        if env['final']:
            env.Append(LIBS = [ '$STLPORT_LIB' ])
        else:
            env.Append(LIBS = [ '$STLPORT_DEBUGLIB' ],
                       CPPDEFINES = [ '_STLP_DEBUG' ])

def UseDoxygen():
    Finalizer(FinalizeDoxygen)

def FinalizeDoxygen(env):
    env.Tool('Doxygen', [os.path.split(__file__)[0]])
    
def MakeEnvironment():
    global opts, finalizers
    InitOpts()
    env = SCons.Environment.Environment(options=opts)
    env.Help(opts.GenerateHelpText(env))
    conf = env.Configure()
    if os.environ.has_key('SSH_AUTH_SOCK'):
        conf.env.Append( ENV = { 'SSH_AUTH_SOCK': os.environ['SSH_AUTH_SOCK'] } )

    for finalizer in finalizers:
        finalizer(conf.env)

    conf.env.Append(CXXFLAGS = [ '-Wall', '-Woverloaded-virtual', '-Wno-long-long',
                                 '-pedantic', '-ansi' ])

    if conf.env['final']:
        conf.env.Append(CXXFLAGS = [ '-O3' ])
    else:
        conf.env.Append(CXXFLAGS = [ '-O0', '-g', '-fno-inline' ],
                        LINKFLAGS = [ '-g' ])

    return conf.Finish()

def GlobSources():
    testSources = glob.glob("*.test.cc")
    sources = [ x for x in glob.glob("*.cc") if x not in testSources ]
    return (sources, testSources)
    
def StandardTargets(env):
    all = env.Alias('all')
    env.Clean(all, [ '.sconsign', '.sconf_temp', 'config.log', 'ChangeLog.bak', '.clean'
                     ] + glob.glob("*~"))
    env.Depends(all, '.')
    #env.AlwaysBuild(env.Command('ChangeLog', '', [ "cvs2cl -rS --no-wrap --summary" ]))
    
def StandardObjects(env, sources, testSources = None, LIBS = []):
    global testTargets
    if type(sources) == type(()):
        testSources = sources[1]
        sources = sources[0]
    
    StandardTargets(env)

    objects = env.Object(sources)

    LOCAL_LIBS = [ x for x in LIBS if x.startswith('$LIB_') ]
    LIBS = [ x for x in LIBS if x not in LOCAL_LIBS ]
    if testSources:
        testTargets.append(env.BoostUnitTests(
            target = 'test_runner',
            source = sources,
            test_source = testSources + LOCAL_LIBS,
            LIBS = LIBS,
            DEPENDS = LOCAL_LIBS))

    env.Doxygen(
        target = 'doc',
        source = sources )

    return objects

def StandardLib(env, library, sources, testSources = None, LIBS = []):
    objects = StandardObjects(env,sources,testSources,LIBS=LIBS)
    lib = env.Library(library,objects)
    env.Default(lib)
    env.Append(**{ 'LIB_' + library : lib })
    return lib

def StandardBinary(env, binary, sources, testSources = None, LIBS = []):
    objects = StandardObjects(env,sources,testSources,LIBS=LIBS)
    program = env.Program(binary,objects)
    env.Default(program)
    return program
