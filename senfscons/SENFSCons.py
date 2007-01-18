import os.path, glob
import  SCons.Options, SCons.Environment, SCons.Script.SConscript, SCons.Node.FS, SCons.Defaults

SCONS_TOOLS = [
    "Doxygen",
    "Dia2Png",
]

opts = None
finalizers = []

basedir = os.path.split(__file__)[0]

def InitOpts():
    global opts
    if opts is not None: return
    opts = SCons.Options.Options('SConfig')
    opts.Add('CXX', 'C++ compiler to use', 'g++')
    opts.Add('EXTRA_DEFINES', 'Additional preprocessor defines', '')
    opts.Add('EXTRA_LIBS', 'Additional libraries to link against', '')
    opts.Add(SCons.Options.BoolOption('final','Enable optimization',0))

def Finalizer(f):
    global finalizers
    finalizers.append(f)

def UseBoost():
    global opts
    InitOpts()
    opts.Add('BOOST_INCLUDES', 'Boost include directory', '')
    opts.Add('BOOST_VARIANT', 'The boost variant to use', '')
    opts.Add('BOOST_TOOLSET', 'The boost toolset to use', '')
    opts.Add('BOOST_RUNTIME', 'The boost runtime to use', '')
    opts.Add('BOOST_DEBUG_RUNTIME', 'The boost debug runtime to use', '')
    opts.Add('BOOST_LIBDIR', 'The directory of the boost libraries', '')
    Finalizer(FinalizeBoost)

def FinalizeBoost(env):
    env.Tool('BoostUnitTests', [basedir])

    if env['BOOST_TOOLSET']:
        runtime = ""
        if env['final'] : runtime += env.get('BOOST_RUNTIME','')
        else            : runtime += env.get('BOOST_DEBUG_RUNTIME','gd')
        if env['STLPORT_LIB'] : runtime += "p"
        if runtime: runtime = "-" + runtime
        env['BOOST_VARIANT'] = "-" + env['BOOST_TOOLSET'] + runtime

    env['BOOSTTESTLIB'] = 'libboost_unit_test_framework' + env['BOOST_VARIANT']

    env.Append(LIBPATH = [ '$BOOST_LIBDIR' ],
               CPPPATH = [ '$BOOST_INCLUDES' ])

def UseSTLPort():
    global opts
    InitOpts()
    opts.Add('STLPORT_INCLUDES', 'STLport include directory', '')
    opts.Add('STLPORT_LIB', 'Name of the stlport library or empty to not use stlport', '')
    opts.Add('STLPORT_DEBUGLIB', 'Name of the stlport debug library','')
    opts.Add('STLPORT_LIBDIR', 'The directory of the stlport libraries','')
    Finalizer(FinalizeSTLPort)

def FinalizeSTLPort(env):
    if env['STLPORT_LIB']:
        if not env['STLPORT_DEBUGLIB']:
            env['STLPORT_DEBUGLIB'] = env['STLPORT_LIB'] + '_stldebug'
        env.Append(LIBPATH = [ '$STLPORT_LIBDIR' ],
                   CPPPATH = [ '$STLPORT_INCLUDES' ])
        if env['final']:
            env.Append(LIBS = [ '$STLPORT_LIB' ])
        else:
            env.Append(LIBS = [ '$STLPORT_DEBUGLIB' ],
                       CPPDEFINES = [ '_STLP_DEBUG' ])

def MakeEnvironment():
    global opts, finalizers
    InitOpts()
    env = SCons.Environment.Environment(options=opts)
    if SCons.Script.SConscript.Arguments.get('final'):
        env['final'] = 1
    env.Help(opts.GenerateHelpText(env))
    #conf = env.Configure()
    #env = conf.env
    if os.environ.has_key('SSH_AUTH_SOCK'):
        env.Append( ENV = { 'SSH_AUTH_SOCK': os.environ['SSH_AUTH_SOCK'] } )

    for finalizer in finalizers:
        finalizer(env)

    for tool in SCONS_TOOLS:
        env.Tool(tool, [basedir])

    env.Append(CXXFLAGS = [ '-Wall', '-Woverloaded-virtual', '-Wno-long-long' ],
               LOCALLIBDIR = [ '#' ],
               LIBPATH = [ '$LOCALLIBDIR' ])

    if env['final']:
        env.Append(CXXFLAGS = [ '-O3' ],
                   CPPDEFINES = [ 'NDEBUG' ])
    else:
        env.Append(CXXFLAGS = [ '-O0', '-g', '-fno-inline' ],
                   LINKFLAGS = [ '-g' ])

    env.Append(CPPDEFINES = [ '$EXTRA_DEFINES' ],
               LIBS = [ '$EXTRA_LIBS' ])

    #return conf.Finish()
    return env

def GlobSources(exclude=[]):
    testSources = glob.glob("*.test.cc")
    sources = [ x for x in glob.glob("*.cc") if x not in testSources and x not in exclude ]
    return (sources, testSources)
    
def StandardTargets(env):
    all = env.Alias('all')
    env.Clean(all, [ '.sconsign', '.sconf_temp', 'config.log' ])
    env.Depends(all, '.')

def GlobalTargets(env):
    pass

def LibPath(lib): return '$LOCALLIBDIR/lib%s.a' % lib
    
def Objects(env, sources, testSources = None, LIBS = []):
    if type(sources) == type(()):
        testSources = sources[1]
        sources = sources[0]

    objects = None
    if sources:
        objects = env.Object(sources)

    if testSources:
        test = env.BoostUnitTests(
            target = 'test',
            source = sources,
            test_source = testSources,
            LIBS = LIBS,
            DEPENDS = [ env.File(LibPath(x)) for x in LIBS ])
        env.Alias('all_tests', test)
        # Hmm ... here I'd like to use an Alias instead of a file
        # however the alias does not seem to live in the subdirectory
        # which breaks 'scons -u test'
        env.Alias(env.File('test'), test)

    return objects

def Doxygen(env, doxyfile = "Doxyfile", extra_sources = []):
    docs = env.Doxygen(doxyfile)
    for doc in docs:
        if isinstance(doc,SCons.Node.FS.Dir): continue
        if os.path.basename(str(doc)) == '.stamp' : continue # file stamp
        # otherwise it must be the tag file
        break
    else:
        doc = None
    if doc:
        # Postprocess the tag file to remove the (broken) namespace
        # references
        env.AddPostAction(
            doc,
            env.Action([ "xsltproc -o TARGET.temp %s TARGET"
                         % os.path.join(basedir,"tagmunge.xsl"),
                         "mv TARGET.temp TARGET" ]))
        env.Clean(doc,"$TARGET.temp")
    env.Depends(docs,extra_sources)
    for doc in docs :
        env.Alias('all_docs', doc)
        env.Clean('all_docs', doc)
        env.Clean('all', doc)
    return docs

def DoxyXRef(env, 
             TYPES = ('bug','todo'),
             HTML_HEADER = None, HTML_FOOTER = None,
             TITLE = "Cross-reference of action points"):
    # Hmm .. this looks a bit scary :-) ...
    xrefis = []

    # This iterates over all doc targets. These are all .stamp and .tag files
    for node in env.Alias('all_docs')[0].sources:
        # We are only interested in the xml targets. This is Doxyfile dependent :-(
        if node.abspath.endswith('/xml/.stamp'):
            # This is the list of xref categories
            for type in TYPES:
                # Here we construct the pathname of the xml file for the category
                xref = os.path.join(node.dir.abspath,type+'.xml')
                # And now apply the xrefxtract.xslt tempalte to it. However, we must
                # only call xsltproc if the source xml file is not empty (therefore the
                # 'test')
                xrefi = env.Command(xref+'i', [ xref, '%s/xrefxtract.xslt' % basedir, node ],
                                    [ "test -s $SOURCE && xsltproc -o $TARGET" +
                                      " --stringparam module $MODULE" + 
                                      " --stringparam type $TYPE" +
                                      " ${SOURCES[1]} $SOURCE || touch $TARGET" ],
                                    MODULE = node.dir.dir.dir.name,
                                    TYPE = type)
                # If the xref xml file does not exist we create it here as an empty
                # file since doxygen will only create it if it is non-empty.
                if not env.GetOption('clean') and not os.path.exists(xref):
                    if not os.path.exists(node.dir.abspath):
                        env.Execute(SCons.Defaults.Mkdir(node.dir.abspath))
                    env.Execute(SCons.Defaults.Touch(xref))
                xrefis.append(xrefi)

    # And here we can now simply combine all the xrefi files
    xref = env.Command("doc/html/xref.xml", xrefis,
                       [ "echo -e '<?xml version=\"1.0\"?>\\n<xref>' >$TARGET",
                         "cat $SOURCES >> $TARGET",
                         "echo '</xref>' >>$TARGET" ])

    # Lastly we create the html file
    sources = [ xref, "%s/xrefhtml.xslt" % basedir ]
    if HTML_HEADER : sources.append(HTML_HEADER)
    if HTML_FOOTER : sources.append(HTML_FOOTER)

    commands = []
    if HTML_HEADER:
        commands.append(
            "sed -e 's/\\$$title/$TITLE/g' -e 's/\\$$projectname/Overview/g' ${SOURCES[2]} > $TARGET")
    commands.append("xsltproc --stringparam title '$TITLE' ${SOURCES[1]} $SOURCE >> $TARGET")
    if HTML_FOOTER:
        commands.append(
            "sed -e 's/\\$$title/$TITLE/g' -e 's/\\$$projectname/Overview/g' ${SOURCES[%d]} >> $TARGET"
            % (HTML_HEADER and 3 or 2))
    
    xref = env.Command("doc/html/xref.html", sources, commands)

    env.Alias('all_docs',xref)
    return xref

def Lib(env, library, sources, testSources = None, LIBS = []):
    objects = Objects(env,sources,testSources,LIBS=LIBS)
    lib = None
    if objects:
        lib = env.Library(env.File(LibPath(library)),objects)
        env.Default(lib)
        env.Append(ALLLIBS = library)
    return lib

def Binary(env, binary, sources, testSources = None, LIBS = []):
    objects = Objects(env,sources,testSources,LIBS=LIBS)
    program = None
    if objects:
        progEnv = env.Copy()
        progEnv.Prepend(LIBS = LIBS)
        program = progEnv.Program(target=binary,source=objects)
        env.Default(program)
        env.Depends(program, [ env.File(LibPath(x)) for x in LIBS ])
    return program
