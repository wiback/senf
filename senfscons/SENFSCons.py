import os.path, glob
import  SCons.Options, SCons.Environment, SCons.Script.SConscript, SCons.Node.FS, SCons.Defaults

SCONS_TOOLS = [
    "Doxygen",
    "Dia2Png",
]

opts = None
finalizers = []

basedir = os.path.abspath(os.path.split(__file__)[0])

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
    # ARGHHH !!! without the [:] we are changing the target list
    #        ||| WITHIN THE DOXYGEN BUILDER
    docs = env.Doxygen(doxyfile)[:]
    xmlnode = None
    htmlnode = None
    tagnode = None
    for doc in docs:
        if isinstance(doc,SCons.Node.FS.Dir): continue
        if doc.name == 'xml.stamp' : xmlnode = doc
        if doc.name == 'html.stamp' : htmlnode = doc
        if os.path.splitext(doc.name)[1] == '.stamp' : continue # ignore other file stamps
        # otherwise it must be the tag file
        tagnode = doc

    if tagnode:
        # Postprocess the tag file to remove the (broken) namespace
        # references
        env.AddPostAction(
            docs,
            env.Action("xsltproc --nonet -o %(target)s.temp %(template)s %(target)s && mv %(target)s.temp %(target)s"
                       % { 'target': tagnode.abspath,
                           'template': os.path.join(basedir,"tagmunge.xsl") }))

    if htmlnode and env.get('DOXY_HTML_XSL'):
        xslfile = env.File(env['DOXY_HTML_XSL'])
        env.AddPostAction(
            docs,
            env.Action(("for html in %s/*.html; do " +
                        "    echo $$html;" +
                        "    sed -e 's/id=\"current\"/class=\"current\"/' $${html}" +
                        "        | tidy -ascii -q --show-warnings no" +
                        "        | xsltproc --nonet --html -o $${html}.new %s - 2>&1" +
                        "        | grep '^-'" +
                        "        | grep -v 'ID .* already defined';" +
                        "    mv $${html}.new $${html}; " +
                        "done")
                       % (htmlnode.dir.abspath, xslfile.abspath)))
        for doc in docs:
            env.Depends(doc,xslfile)

    if xmlnode:
        xrefs = []
        for type in env.get("DOXY_XREF_TYPES",[ "bug", "todo" ]):
            xref = os.path.join(xmlnode.dir.abspath,type+".xml")
            xref_pp = env.Command(xref+'i', [ xref, os.path.join(basedir,'xrefxtract.xslt'), xmlnode ],
                                  [ "test -s $SOURCE && xsltproc -o $TARGET" +
                                    " --stringparam module $MODULE" + 
                                    " --stringparam type $TYPE" +
                                    " ${SOURCES[1]} $SOURCE || touch $TARGET" ],
                                  MODULE = xmlnode.dir.dir.dir.name,
                                  TYPE = type)
            env.SideEffect(xref, xmlnode)
            env.AddPreAction(docs, "rm -f %s" % (xref,))
            env.AddPostAction(docs, "test -r %s || touch %s" % (xref,xref))
            xrefs.extend(xref_pp)
        docs.extend(xrefs)

    env.Depends(docs,extra_sources)
    for doc in docs :
        env.Alias('all_docs', doc)
        env.Clean('all_docs', doc)
        env.Clean('all', doc)
    return docs

def DoxyXRef(env, docs=None,
             HTML_HEADER = None, HTML_FOOTER = None,
             TITLE = "Cross-reference of action points"):
    if docs is None:
        docs = env.Alias('all_docs')[0].sources
    xrefs = [ doc for doc in docs if os.path.splitext(doc.name)[1] == ".xmli" ]
    xref = env.Command("doc/html/xref.xml", xrefs,
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
    
    xref = env.Command("doc/html/xref.html", sources, commands,
                       TITLE = TITLE)

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
