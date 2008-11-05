## \file
# \brief SENFSCons package

## \package senfscons.SENFSCons
# \brief Build helpers and utilities
#
# The SENFSCons package contains a number of build helpers and
# utilities which are used to simplify commmon tasks.
#
# The utitlities of this package are grouped into:
# <dl><dt>\ref use</dt><dd>help using complex environments and
# configure the construction environmen correspondingly</dd>
#
# <dt>\ref target</dt><dd>simplify building common targest and include
# enhanced functionality like unit-testing.</dd></dl>
#
# Additionally for external use are
# <dl><dt>MakeEnvironment()</dt><dd>Build construction
# environment</dd>
#
# <dt>GlobSources()</dt><dd>Utility to find source files</dd></dl>
#
# All other functions are for internal use only.

import os.path, glob
import SCons.Options, SCons.Environment, SCons.Script.SConscript, SCons.Node.FS
import SCons.Defaults, SCons.Action
from SCons.Script import *

## \defgroup use Predefined Framework Configurators
#
# The following framework configurators are used in the top level \c
# SConstruct file to simplify more complex configurations.
#
# Each of the framework configurators introduces additional
# configuration parameters to \ref sconfig

## \defgroup target Target Helpers
#
# To specify standard targets, the following helpers can be used. They
# automatically integrate several modules (like documentation,
# unit-testing etc).

## \defgroup builder Builders
#
# The SENFSCons framework includes a series of builders. Each builder
# is defined in it's own package.

# Tools to load in MakeEnvironment
SCONS_TOOLS = [
    "Doxygen",
    "Dia2Png",
    "CopyToDir",
    "InstallIncludes",
    "ProgramNoScan",
    "CompileCheck",
]

opts = None
finalizers = []

# This is the directory SENFSCons.py resides
basedir = os.path.abspath(os.path.split(__file__)[0])

## \brief Initialize configuration options
# \internal
def InitOpts():
    global opts
    if opts is not None: return
    opts = SCons.Options.Options('SConfig')
    opts.Add('CXX', 'C++ compiler to use', 'g++')
    opts.Add('EXTRA_DEFINES', 'Additional preprocessor defines', '')
    opts.Add('EXTRA_LIBS', 'Additional libraries to link against', '')
    opts.Add(SCons.Options.BoolOption('final','Enable optimization',0))
    opts.Add('PREFIX', 'Installation prefix', '/usr/local')
    opts.Add('LIBINSTALLDIR', 'Library install dir', '$PREFIX/lib')
    opts.Add('BININSTALLDIR', 'Executable install dir', '$PREFIX/bin')
    opts.Add('INCLUDEINSTALLDIR', 'Include-file install dir', '$PREFIX/include')
    opts.Add('OBJINSTALLDIR', 'Static object file install dir', '$LIBINSTALLDIR')
    opts.Add('DOCINSTALLDIR', 'Documentation install dir', '$PREFIX/doc')
    opts.Add('CPP_INCLUDE_EXTENSIONS', 'File extensions to include in source install',
             [ '.h', '.hh', '.ih', '.mpp', '.cci', '.ct', '.cti', '.mpp' ])
    opts.Add('CPP_EXCLUDE_EXTENSIONS', 'File extensions to exclude from source install',
             [ '.test.hh' ])

# A finalizer is any callable object. All finalizers will be called
# in MakeEnvironment. We use them so every finalizer has knowledge of
# all frameworks in use (e.g.: the boost runtime depends on the use of
# stlport).

## \brief Register finalizer
# \internal
def Finalizer(f):
    global finalizers
    finalizers.append(f)

## \brief Initialize the use of the <a href="http://www.boost.org/">Boost</a> library
#
# Configure the use of the <a href="http://www.boost.org">Boost</a>
# libraries. Most of these libraries are header-only, some however
# depend on a built library. The library selection is somewhat
# involved and depends on the threading model and the type of build
# (debug or final).
#
# \par Configuration Parameters:
#     <table class="senf">
#     <tr><td>\c BOOST_INCLUDES</td><td>Include directory.</td></tr>
#     <tr><td>\c BOOST_LIBDIR</td><td>Library directory</td></tr>
#     <tr><td>\c BOOST_VARIANT</td><td>Complete variant specification</td></tr>
#     <tr><td>\c BOOST_TOOLSET</td><td>Toolset to use</td></tr>
#     <tr><td>\c BOOST_RUNTIME</td><td>Runtime to use</td></tr>
#     <tr><td>\c BOOST_DEBUG_RUNTIME</td><td>Explicit debug runtime</td></tr>
#     </table>
#
# You can either specify \c BOOST_VARIANT explicitly or specify \c
# BOOST_TOOLSET and \c BOOST_RUNTIME. If you give \c BOOST_TOOLSET, \c
# BOOST_RUNTIME defaults to empty and \c BOOST_DEBUG_RUNTIME defaults
# to \c gd, If \c BOOST_TOOLSET is specified and you have included
# STLPort support (UseSTLPort()), then \c p is appended to both
# runtimes.
#
# The Boost configuration can get realtively complex. If the boost
# libraries are provided by the distribution, you probably don't need
# to specify any parameters. If your configuration is more complex,
# refer to the <a
# href="http://www.boost.org/tools/build/v2/index.html">Boost.Build</a>
# documentation for a definition of the terms used above (toolset,
# variant, runtime ...).
#
# \ingroup use
def UseBoost():
    global opts
    InitOpts()
    opts.Add('BOOST_INCLUDES', 'Boost include directory', '')
    opts.Add('BOOST_VARIANT', 'The boost variant to use', '')
    opts.Add('BOOST_TOOLSET', 'The boost toolset to use', '')
    opts.Add('BOOST_RUNTIME', 'The boost runtime to use', '')
    opts.Add('BOOST_DEBUG_RUNTIME', 'The boost debug runtime to use', '')
    opts.Add('BOOST_LIBDIR', 'The directory of the boost libraries', '')
    opts.Add('BOOST_PREFIX', 'The prefix into which boost is installed', '')
    opts.Add('BOOST_VERSION', 'The version of boost to use', '')
    Finalizer(FinalizeBoost)

## \brief Finalize Boost environment
# \internal
def FinalizeBoost(env):
    env.Tool('BoostUnitTests', [basedir])

    if env['BOOST_TOOLSET']:
        runtime = ""
        if env['final'] : runtime += env.get('BOOST_RUNTIME','')
        else            : runtime += env.get('BOOST_DEBUG_RUNTIME','gd')
        if env['STLPORT_LIB'] : runtime += "p"
        if runtime: runtime = "-" + runtime
        env['BOOST_VARIANT'] = "-" + env['BOOST_TOOLSET'] + runtime

    if env['BOOST_VARIANT'] and env['BOOST_VERSION']:
        env['BOOST_VARIANT'] = env['BOOST_VARIANT'] + '-%s' % env['BOOST_VERSION'].replace('.','_')

    env['BOOSTTESTLIB'] = 'boost_unit_test_framework' + env['BOOST_VARIANT']
    env['BOOSTREGEXLIB'] = 'boost_regex' + env['BOOST_VARIANT']
    env['BOOSTFSLIB'] = 'boost_filesystem' + env['BOOST_VARIANT']
    env['BOOSTIOSTREAMSLIB'] = 'boost_iostreams' + env['BOOST_VARIANT']

    if env['BOOST_PREFIX']:
        env['BOOST_LIBDIR'] = os.path.join(env['BOOST_PREFIX'], 'lib')
        env['BOOST_INCLUDES'] = os.path.join(env['BOOST_PREFIX'],
                                             'include/boost-%s'
                                                 % env['BOOST_VERSION'].replace('.','_'))

    env.Append(LIBPATH = [ '$BOOST_LIBDIR' ],
               CPPPATH = [ '$BOOST_INCLUDES' ])

    if env['BOOST_LIBDIR']:
        env.Append(ENV = { 'LD_LIBRARY_PATH': env['BOOST_LIBDIR'] })

## \brief Use STLPort as STL replacement if available
#
# Use <a href="http://www.stlport.org">STLPort</a> as a replacement
# for the system STL. STLPort has the added feature of providing fully
# checked containers and iterators. This can greatly simplify
# debugging. However, STLPort and Boost interact in a non-trivial way
# so the configuration is relatively complex. This command does not
# enforce the use of STLPort, it is only used if available.
#
# \par Configuration Parameters:
#     <table class="senf">
#     <tr><td>\c STLPORT_INCLUDES</td><td>Include directory.</td></tr>
#     <tr><td>\c STLPORT_LIBDIR</td><td>Library directory</td></tr>
#     <tr><td>\c STLPORT_LIB</td><td>Name of STLPort library</td></tr>
#     <tr><td>\c STLPORT_DEBUGLIB</td><td>Name of STLPort debug library</td></tr>
#     </table>
#
# If \c STLPORT_LIB is specified, \c STLPORT_DEBUGLIB defaults to \c
# STLPORT_LIB with \c _stldebug appended. The STLPort library will
# only be used, if \c STLPORT_LIB is set in \c SConfig.
#
# \ingroup use
def UseSTLPort():
    global opts
    InitOpts()
    opts.Add('STLPORT_INCLUDES', 'STLport include directory', '')
    opts.Add('STLPORT_LIB', 'Name of the stlport library or empty to not use stlport', '')
    opts.Add('STLPORT_DEBUGLIB', 'Name of the stlport debug library','')
    opts.Add('STLPORT_LIBDIR', 'The directory of the stlport libraries','')
    Finalizer(FinalizeSTLPort)

# \}

## \brief Finalize STLPort environment
# \internal
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

## \brief Build a configured construction environment
#
# This function is called after all frameworks are specified to build
# a tailored construction environment. You can then use this
# construction environment just like an ordinary SCons construction
# environment (which it is ...)
#
# This call will set some default compilation parameters depending on
# the \c final command line option: specifying <tt>final=1</tt> will
# built a release version of the code.
def MakeEnvironment():
    global opts, finalizers
    InitOpts()
    env = SCons.Environment.Environment(options=opts)
    env.Replace(**SCons.Script.SConscript.Arguments)
    #for opt in opts.options:
    #    if SCons.Script.SConscript.Arguments.get(opt.key):
    #        env[opt.key] = SCons.Script.SConscript.Arguments.get(opt.key)
    #if SCons.Script.SConscript.Arguments.get('final'):
    #    env['final'] = 1
    env.Help("\nSupported build variables (either in SConfig or on the command line:\n")
    env.Help(opts.GenerateHelpText(env))

    # We want to pass the SSH_AUTH_SOCK system env-var so we can ssh
    # into other hosts from within SCons rules. I have used rules like
    # this e.g. to automatically install stuff on a remote system ...
    if os.environ.has_key('SSH_AUTH_SOCK'):
        env.Append( ENV = { 'SSH_AUTH_SOCK': os.environ['SSH_AUTH_SOCK'] } )

    for finalizer in finalizers:
        finalizer(env)

    for tool in SCONS_TOOLS:
        env.Tool(tool, [basedir])

    # These are the default compilation parameters. We should probably
    # make these configurable
    env.Append(LOCALLIBDIR = [ '#' ],
               LIBPATH = [ '$LOCALLIBDIR' ])

    if env['final']:
        env.Append(CXXFLAGS = [ '-O3' ])
    else:
        # The boost-regex library is not compiled with _GLIBCXX_DEBUG so this fails:
        #          CPPDEFINES = [ '_GLIBCXX_DEBUG' ],
        env.Append(CXXFLAGS = [ '-O0', '-g' ],
                   CPPDEFINES = { 'SENF_DEBUG': ''},
                   LINKFLAGS = [ '-g', '-rdynamic' ])

    env.Append(CPPDEFINES = [ '$EXTRA_DEFINES' ],
               LIBS = [ '$EXTRA_LIBS' ],
               ALLLIBS = [])

    return env

## \brief Find normal and test C++ sources
#
# GlobSources() will return a list of all C++ source files (named
# "*.cc") as well as a list of all unit-test files (named "*.test.cc")
# in the current directory. The sources will be returned as a tuple of
# sources, test-sources. The target helpers all accept such a tuple as
# their source argument.
def GlobSources(exclude=[], subdirs=[]):
    testSources = glob.glob("*.test.cc")
    sources = [ x for x in glob.glob("*.cc") if x not in testSources and x not in exclude ]
    for subdir in subdirs:
        testSources += glob.glob(os.path.join(subdir,"*.test.cc"))
        sources += [ x for x in glob.glob(os.path.join(subdir,"*.cc"))
                     if x not in testSources and x not in exclude ]
    return (sources, testSources)

## \brief Add generic standard targets for every module
#
# This target helper should be called in the top-level \c SConstruct file
# as well as in every module \c SConscipt file. It adds general
# targets. Right now, these are
# \li clean up \c .sconsign, \c .sconf_temp and \c config.log on
#   <tt>scons -c all</tt>
#
# \ingroup target
def StandardTargets(env):
    env.Clean(env.Alias('all'), [ '.sconsign', '.sconf_temp', 'config.log' ])

## \brief Add generic global targets
#
# This target helper should be called in the top-level \c SConstruct
# file. It adds general global targets. Right now theese are
# \li Make <tt>scons all</tt> build all targets.
#
# \ingroup target
def GlobalTargets(env):
    env.Alias('all', [ 'default', 'all_tests', 'all_docs' ])

## \brief Return path of a built library within $LOCALLIBDIR
# \internal
def LibPath(lib): return '${LOCALLIBDIR}/${LIBPREFIX}%s${LIBADDSUFFIX}${LIBSUFFIX}' % lib

def Test(env, sources, LIBS = [], OBJECTS = []):
    test = [ env.BoostUnitTests(
        target = 'test',
        objects = [],
        test_sources = sources,
        LIBS = [ x + '$LIBADDSUFFIX' for x in LIBS ],
        OBJECTS = OBJECTS,
        DEPENDS = [ env.File(LibPath(x)) for x in LIBS ]) ]
    compileTestSources = [ src for src in sources
                           if 'COMPILE_CHECK' in file(src).read() ]
    if compileTestSources:
        test.extend(env.CompileCheck(source = compileTestSources))
    env.Alias('all_tests', test)
    env.Command(env.File('test'), test, [])
    #env.Alias(env.File('test'), test)
    

## \brief Build object files
#
# This target helper will build object files from the given
# sources.
#
# If \a testSources are given, a unit test will be built using the <a
# href="http://www.boost.org/libs/test/doc/index.html">Boost.Test</a>
# library. \a LIBS may specify any additional library modules <em>from
# the same project</em> on which the test depends. Those libraries
# will be linked into the final test executable. The test will
# automatically be run if the \c test or \c all_tests targets are
# given.
#
# If \a sources is a 2-tuple as returned by GlobSources(), it will
# provide both \a sources and \a testSources.
#
# \ingroup target
def Objects(env, sources, testSources = None, LIBS = [], OBJECTS = [], no_includes = False):
    if type(sources) == type(()):
        testSources = sources[1]
        sources = sources[0]
    if type(sources) is not type([]):
        sources = [ sources ]

    objects = None
    if sources:
        obsources = [ source
                      for source in sources
                      if type(source) is type('') and not source.endswith('.o') ]
        objects = [ source
                    for source in sources
                    if type(source) is not type('') or source.endswith('.o') ]
        if obsources:
            objects += env.Object(obsources)

    if testSources:
        test = [ env.BoostUnitTests(
            target = 'test',
            objects = objects,
            test_sources = testSources,
            LIBS = [ x + '$LIBADDSUFFIX' for x in LIBS ],
            OBJECTS = OBJECTS,
            DEPENDS = [ env.File(LibPath(x)) for x in LIBS ]) ]
        compileTestSources = [ src for src in testSources
                               if 'COMPILE_CHECK' in file(src).read() ]
        if compileTestSources:
            test.extend(env.CompileCheck(source = compileTestSources))
        env.Alias('all_tests', test)
        # Hmm ... here I'd like to use an Alias instead of a file
        # however the alias does not seem to live in the subdirectory
        # which breaks 'scons -u test'
        env.Command(env.File('test'), test, [])
        #env.Alias(env.File('test'), test)

    return objects

def InstallIncludeFiles(env, files):
    # Hrmpf ... why do I need this in 0.97??
    if env.GetOption('clean'):
        return
    target = env.Dir(env['INCLUDEINSTALLDIR'])
    base = env.Dir(env['INSTALL_BASE'])
    for f in files:
        src = env.File(f)
        env.Alias('install_all', env.Install(target.Dir(src.dir.get_path(base)), src))

def InstallSourceIncludes(env, sources):
    target = env.Dir(env['INCLUDEINSTALLDIR']).Dir(
        env.Dir('.').get_path(env.Dir(env['INSTALL_BASE'])))
    install = env.InstallIncludes( target = target,
                                   source = [ type(x) is str and env.File(x) or x
                                              for x in sources ],
                                   INSTALL_BASE = env.Dir('.') )
    env.Alias( 'install_all', install )

def InstallWithSources(env, targets, dir, sources, testSources = [], no_includes = False):
    if type(sources) is type(()):
        sources, testSources = sources
    if type(sources) is not type([]):
        sources = [ sources ]
    if type(testSources) is not type([]):
        testSources = [ testSources ]

    installs = [ env.Install(dir, targets) ]
    env.Alias( 'install_all', installs[:] )

    if not no_includes:
        sources = targets
        if testSources:
            sources.append( env.File('.test.bin') )
        installs.append(
            InstallSourceIncludes(env, sources))

    return installs

## \brief Build documentation with doxygen
#
# The doxygen target helper will build software documentation using
# the given \a doxyfile (which defaults to \c Doxyfile). The Doxygen
# builder used supports automatic dependency generation (dependencies
# are automatically generated from the parameters specified in the \a
# doxyfile), automatic target emitters (the exact targets created are
# found parsing the \a doxyfile) and lots of other features. See the
# Doxygen builder documentation
#
# If \a extra_sources are given, the generated documentation will
# depend on them. This can be used to build images or other
# supplementary files.
#
# The doxygen target helper extends the builder with additional
# functionality:
#
# \li Fix tagfiles by removing namespace entries. These entries only
#     work for namespaces completely defined in a single module. As
#     soon as another module (which references the tagfile) has it's
#     own members in that namespace, the crosslinking will break.
# \li If \c DOXY_HTML_XSL is defined as a construction environment
#     variable, preprocess all generated html files (if html files are
#     generated) by the given XSLT stylesheet. Since the HTML
#     generated by doxygen is broken, we first filter the code through
#     HTML-\c tidy and filter out some error messages.
# \li If xml output is generated we create files \c bug.xmli and \c
#     todo.xmli which contain all bugs and todo items specified in the
#     sources. The format of these files is much more suited to
#     postprocessing and is a more database like format as the doxygen
#     generated files (which are more presentation oriented). if \c
#     DOXY_XREF_TYPES is given, it will specify the cross reference
#     types to support (defaults to \c bug and \c todo). See <a
#     href="http://www.stack.nl/~dimitri/doxygen/commands.html#cmdxrefitem">\xrefitem</a>
#     in the doxygen documentation.
#
# \ingroup target
def Doxygen(env, doxyfile = "Doxyfile", extra_sources = []):
    if not 'all' in BUILD_TARGETS and not 'doc' in BUILD_TARGETS and not 'all_docs' in BUILD_TARGETS:
        return []
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
        if doc.name == 'search.idx' : continue
        if os.path.splitext(doc.name)[1] == '.stamp' : continue # ignore other file stamps
        # otherwise it must be the tag file
        tagnode = doc

    if tagnode:
        # Postprocess the tag file to remove the (broken) namespace
        # references
        env.AddPostAction(
            docs,
            SCons.Action.Action("xsltproc --nonet -o %(target)s.temp %(template)s %(target)s && mv %(target)s.temp %(target)s"
                       % { 'target': tagnode.abspath,
                           'template': os.path.join(basedir,"tagmunge.xsl") }))

    if htmlnode and env.get('DOXY_HTML_XSL'):
        xslfile = env.File(env['DOXY_HTML_XSL'])
        reltopdir = '../' * len(htmlnode.dir.abspath[len(env.Dir('#').abspath)+1:].split('/'))
        if reltopdir : reltopdir = reltopdir[:-1]
        else         : reltopdir = '.'
        env.AddPostAction(
            docs,
            SCons.Action.Action(("for html in %s/*.html; do " +
                        "    echo $$html;" +
                        "    mv $${html} $${html}.orig;" +
                        "    sed -e 's/id=\"current\"/class=\"current\"/' $${html}.orig" +
                        "        | tidy -ascii -q --wrap 0 --show-warnings no --fix-uri no " +
                        "        | sed -e 's/name=\"\([^\"]*\)\"\([^>]*\) id=\"\\1\"/name=\"\\1\"\\2/g'" +
                        "        | xsltproc --novalid --nonet --html --stringparam topdir %s -o $${html} %s -;"
                        "done; true")
                       % (htmlnode.dir.abspath, reltopdir, xslfile.abspath)))
        for doc in docs:
            env.Depends(doc, xslfile)

    if xmlnode:
        xrefs = []
        for type in env.get("DOXY_XREF_TYPES",[ "bug", "todo" ]):
            xref = os.path.join(xmlnode.dir.abspath,type+".xml")
            xref_pp = env.Command(xref+'i', [ xref, os.path.join(basedir,'xrefxtract.xslt'), xmlnode ],
                                  [ "test -s $SOURCE && xsltproc --nonet -o $TARGET" +
                                    " --stringparam module $MODULE" +
                                    " --stringparam type $TYPE" +
                                    " ${SOURCES[1]} $SOURCE || touch $TARGET" ],
                                  MODULE = xmlnode.dir.dir.dir.abspath[
                                               len(env.Dir('#').abspath)+1:],
                                  TYPE = type)
            env.SideEffect(xref, xmlnode)
            env.AddPreAction(docs, "rm -f %s" % (xref,))
            env.AddPostAction(docs, "test -r %s || touch %s" % (xref,xref))
            xrefs.extend(xref_pp)
        docs.extend(xrefs)

    if extra_sources and htmlnode:
        env.Depends(docs,
                    [ env.CopyToDir( source=source, target=htmlnode.dir )
                      for source in extra_sources ])

    if extra_sources and xmlnode:
        env.Depends(docs,
                    [ env.CopyToDir( source=source, target=xmlnode.dir )
                      for source in extra_sources ])

    if not htmlnode and not xmlnode:
        env.Depends(docs, extra_sources)

    for doc in docs :
        env.Alias('all_docs', doc)
        env.Clean('all_docs', doc)
        env.Clean('all', doc)

    l = len(env.Dir('#').abspath)
    if htmlnode:
        env.Alias('install_all',
                  env.Command('$DOCINSTALLDIR' + htmlnode.dir.abspath[l:], htmlnode.dir,
                              [ SCons.Defaults.Copy('$TARGET','$SOURCE') ]))
    if tagnode:
        env.Alias('install_all',
                  env.Install( '$DOCINSTALLDIR' + tagnode.dir.abspath[l:],
                               tagnode ))

    return docs

## \brief Build combined doxygen cross-reference
#
# This command will build a complete cross-reference of \c xrefitems
# accross all modules.
#
# Right now, this command is very project specific. It needs to be
# generalized.
#
# \ingroup target
def DoxyXRef(env, docs=None,
             HTML_HEADER = None, HTML_FOOTER = None,
             TITLE = "Cross-reference of action points"):
    if docs is None:
        docs = env.Alias('all_docs')[0].sources
    xrefs = [ doc for doc in docs if os.path.splitext(doc.name)[1] == ".xmli" ]
    xref = env.Command("doc/html/xref.xml", xrefs,
                       [ "echo '<?xml version=\"1.0\"?>' > $TARGET",
                         "echo '<xref>' >> $TARGET",
                         "cat $SOURCES >> $TARGET",
                         "echo '</xref>' >>$TARGET" ])

    # Lastly we create the html file
    sources = [ xref, "%s/xrefhtml.xslt" % basedir ]
    if HTML_HEADER : sources.append(HTML_HEADER)
    if HTML_FOOTER : sources.append(HTML_FOOTER)

    commands = []
    if HTML_HEADER:
        commands.append("sed" +
                        " -e 's/\\$$title/$TITLE/g'" +
                        " -e 's/\\$$projectname/Overview/g'" +
                        " ${SOURCES[2]} > $TARGET")
    commands.append("xsltproc" +
                    " --stringparam title '$TITLE'" +
                    " --stringparam types '$DOXY_XREF_TYPES'" +
                    " ${SOURCES[1]} $SOURCE >> $TARGET")
    if HTML_FOOTER:
        commands.append(
            "sed -e 's/\\$$title/$TITLE/g' -e 's/\\$$projectname/Overview/g' ${SOURCES[%d]} >> $TARGET"
            % (HTML_HEADER and 3 or 2))

    if env.get('DOXY_HTML_XSL'):
        xslfile = env.File(env['DOXY_HTML_XSL'])
        reltopdir = '../' * len(xref[0].dir.abspath[len(env.Dir('#').abspath)+1:].split('/'))
        if reltopdir : reltopdir = reltopdir[:-1]
        else         : reltopdir = '.'
        commands.append(("xsltproc -o ${TARGET}.tmp" +
                         " --nonet --html" +
                         " --stringparam topdir %s" +
                         " ${SOURCES[-1]} $TARGET 2>/dev/null")
                        % reltopdir)
        commands.append("mv ${TARGET}.tmp ${TARGET}")
        sources.append(xslfile)
        
    xref = env.Command("doc/html/xref.html", sources, commands,
                       TITLE = TITLE)

    env.Alias('all_docs',xref)
    return xref


## \brief Build library
#
# This target helper will build the given library. The library will be
# called lib<i>library</i>.a as is customary on UNIX systems. \a
# sources, \a testSources and \a LIBS are directly forwarded to the
# Objects build helper.
#
# The library is added to the list of default targets.
#
#\ingroup target
def Lib(env, library, sources, testSources = None, LIBS = [], OBJECTS = [], no_includes = False):
    objects = Objects(env,sources,testSources,LIBS=LIBS,OBJECTS=OBJECTS)
    lib = None
    if objects:
        lib = env.Library(env.File(LibPath(library)),objects)
        env.Default(lib)
        env.Append(ALLLIBS = library)
        env.Alias('default', lib)
        InstallWithSources(env, lib, '$LIBINSTALLDIR', sources, testSources, no_includes)
    return lib

## \brief Build Object from multiple sources
def Object(env, target, sources, testSources = None, LIBS = [], OBJECTS = [], no_includes = False):
    objects = Objects(env,sources,testSources,LIBS=LIBS,OBJECTS=OBJECTS)
    ob = None
    if objects:
        ob = env.Command(target+"${OBJADDSUFFIX}${OBJSUFFIX}", objects, "ld -r -o $TARGET $SOURCES")
        env.Default(ob)
        env.Alias('default', ob)
        InstallWithSources(env, ob, '$OBJINSTALLDIR', sources, testSources, no_includes)
    return ob

## \brief Build executable
#
# This target helper will build the given binary.  The \a sources, \a
# testSources and \a LIBS arguments are forwarded to the Objects
# builder. The final program will be linked against all the library
# modules specified in \a LIBS (those are libraries which are built as
# part of the same proejct). To specify non-module libraries, use the
# construction environment parameters or the framework helpers.
#
# \ingroup target
def Binary(env, binary, sources, testSources = None, LIBS = [], OBJECTS = [], no_includes = False):
    objects = Objects(env,sources,testSources,LIBS=LIBS,OBJECTS=OBJECTS)
    program = None
    if objects:
        progEnv = env.Clone()
        progEnv.Prepend(LIBS = [ x + '$LIBADDSUFFIX' for x in LIBS ])
        program = progEnv.ProgramNoScan(target=binary,source=objects+OBJECTS)
        env.Default(program)
        env.Depends(program, [ env.File(LibPath(x)) for x in LIBS ])
        env.Alias('default', program)
        InstallWithSources(env, program, '$BININSTALLDIR', sources, testSources, no_includes)
    return program

def AllIncludesHH(env, headers):
    headers.sort()
    target = env.File("all_includes.hh")
    file(target.abspath,"w").write("".join([ '#include "%s"\n' % f
                                             for f in headers ]))
    env.Clean('all', target)
