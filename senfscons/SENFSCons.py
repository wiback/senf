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

## \brief Find normal and test C++ sources
#
# GlobSources() will return a list of all C++ source files (named
# "*.cc") as well as a list of all unit-test files (named "*.test.cc")
# in the current directory. The sources will be returned as a tuple of
# sources, test-sources. The target helpers all accept such a tuple as
# their source argument.
def GlobSources(env, exclude=[], subdirs=[]):
    testSources = glob.glob("*.test.cc")
    sources = [ x for x in glob.glob("*.cc") if x not in testSources and x not in exclude ]
    for subdir in subdirs:
        testSources += glob.glob(os.path.join(subdir,"*.test.cc"))
        sources += [ x for x in glob.glob(os.path.join(subdir,"*.cc"))
                     if x not in testSources and x not in exclude ]
    return (sources, testSources)

def GlobIncludes(env, exclude=[], subdirs=[]):
    includes = []
    for d in [ '.' ] + subdirs:
        for f in os.listdir(d):
            ext = '.' + f.split('.',1)[-1]
            p = os.path.join(d,f)
            if ext in env['CPP_INCLUDE_EXTENSIONS'] \
               and ext not in env['CPP_EXCLUDE_EXTENSIONS'] \
               and p not in exclude:
                includes.append(p)
    return includes

def Glob(env, exclude=[], subdirs=[]):
    return ( GlobSources(env, exclude, subdirs),
             GlobIncludes(env, exclude, subdirs) )

## \brief Return path of a built library within $LOCALLIBDIR
# \internal
def LibPath(lib): return '${LOCALLIBDIR}/${LIBPREFIX}%s${LIBADDSUFFIX}${LIBSUFFIX}' % lib

## \brief Add explicit test
#
# This target helper will add an explicit test. This is like a unit test but is
# built directly against the completed library
#
# \ingroup target
def Test(env, sources, LIBS = [], OBJECTS = []):
    test = [ env.BoostUnitTests(
        target = 'test',
        objects = [],
        test_sources = sources,
        LIBS = [ '$LIBSENF$LIBADDSUFFIX' ],
        OBJECTS = OBJECTS,
        DEPENDS = [ env.File(LibPath(env['LIBSENF'])) ]) ]
    compileTestSources = [ src for src in sources
                           if 'COMPILE_CHECK' in file(src).read() ]
    if compileTestSources:
        test.extend(env.CompileCheck(source = compileTestSources))
    env.Alias('all_tests', test)
    env.Command(env.File('test'), test, [ 'true' ])
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
def Objects(env, sources, testSources = None, OBJECTS = []):
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
            LIBS = [ '$LIBSENF$LIBADDSUFFIX' ],
            OBJECTS = OBJECTS,
            DEPENDS = [ env.File(LibPath(env['LIBSENF'])) ]) ]
        compileTestSources = [ src for src in testSources
                               if 'COMPILE_CHECK' in file(src).read() ]
        if compileTestSources:
            test.extend(env.CompileCheck(source = compileTestSources))
        env.Alias('all_tests', test)
        # Hmm ... here I'd like to use an Alias instead of a file
        # however the alias does not seem to live in the subdirectory
        # which breaks 'scons -u test'
        env.Command(env.File('test'), test, [ 'true' ])
        #env.Alias(env.File('test'), test)

    return objects

def InstallIncludeFiles(env, files):
    # Hrmpf ... why do I need this in 0.97??
    if env.GetOption('clean'):
        return
    target = env.Dir(env['INCLUDEINSTALLDIR'])
    base = env.Dir('#')
    for f in files:
        src = env.File(f)
        env.Alias('install_all', env.Install(target.Dir(src.dir.get_path(base)), src))

## \brief Build documentation with doxygen
#
# \ingroup target
def Doxygen(env, doxyfile = "Doxyfile", extra_sources = []):
    # There is one small problem we need to solve with this builder: The Doxygen builder reads
    # the Doxyfile and thus depends on the environment variables set by doclib/doxygen.sh. We
    # thus have to provide all necessary definitions here manually via DOXYENV !

    if type(doxyfile) is type(""):
        doxyfile = env.File(doxyfile)

    # Module name is derived from the doxyfile path
    # Utils/Console/Doxyfile -> Utils_Console
    module = doxyfile.dir.abspath[len(env.Dir('#').abspath)+1:].replace('/','_')
    if not module : module = "Main"

    # Rule to generate tagfile
    # (need to exclude the 'clean' case, otherwise we'll have duplicate nodes)
    if not env.GetOption('clean'):
        tagfile = env.Doxygen(doxyfile,
                              DOXYOPTS = [ '--tagfile-name', '"${MODULE}.tag"',
                                           '--tagfile' ],
                              DOXYENV  = { 'TOPDIR'          : env.Dir('#').abspath,
                                           'output_dir'      : 'doc',
                                           'html_dir'        : 'html',
                                           'html'            : 'NO',
                                           'generate_tagfile': 'doc/${MODULE}.tag' },
                              MODULE   = module )
        env.Append(ALL_TAGFILES = tagfile[0].abspath)
        env.Depends(tagfile, env.File('#/doclib/doxygen.sh'))

    # Rule to generate HTML documentation
    doc = env.Doxygen(doxyfile,
                      DOXYOPTS = [ '--tagfiles', '"$ALL_TAGFILES"',
                                   '--tagfile-name', '"${MODULE}.tag"',
                                   '--html' ],
                      MODULE   = module,
                      DOXYENV  = { 'TOPDIR'          : env.Dir('#').abspath,
                                   'tagfiles'        : '${ALL_TAGFILES}',
                                   'output_dir'      : 'doc',
                                   'html_dir'        : 'html',
                                   'html'            : 'YES' } )
    env.Depends(doc, env.File('#/doclib/doxygen.sh'))

    # Copy the extra_sources (the images) into the documentation directory
    # (need to exclude the 'clean' case otherwise there are multiple ways to clean the copies)
    if not env.GetOption('clean'):
        if extra_sources:
            env.Depends(doc,
                        [ env.CopyToDir( source=source, target=doc[0].dir )
                          for source in extra_sources ])

    # Install documentation into DOCINSTALLDIR
    l = len(env.Dir('#').abspath)
    env.Alias('install_all',
              env.Command('$DOCINSTALLDIR' + doc[0].dir.abspath[l:], doc[0].dir,
                          [ SCons.Defaults.Copy('$TARGET','$SOURCE') ]))

    # Useful aliases
    env.Alias('all_docs', doc)
    env.Clean('all_docs', doc)
    env.Clean('all', doc)

    return doc

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
def Lib(env, sources, testSources = None, OBJECTS = []):
    objects = Objects(env,sources,testSources,OBJECTS=OBJECTS)
    if objects:
        env.Append(ALLOBJECTS = objects)
    return objects

## \brief Build Object from multiple sources
def Object(env, target, sources, testSources = None, OBJECTS = []):
    objects = Objects(env,sources,testSources,OBJECTS=OBJECTS)
    ob = None
    if objects:
        ob = env.Command(target+"${OBJADDSUFFIX}${OBJSUFFIX}", objects, "ld -r -o $TARGET $SOURCES")
        env.Default(ob)
        env.Alias('default', ob)
        env.Alias('install_all', env.Install("$OBJINSTALLDIR", ob))
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
def Binary(env, binary, sources, testSources = None, OBJECTS = []):
    objects = Objects(env,sources,testSources,OBJECTS=OBJECTS)
    program = None
    if objects:
        progEnv = env.Clone()
        progEnv.Prepend(LIBS = [ '$LIBSENF$LIBADDSUFFIX' ])
        program = progEnv.ProgramNoScan(target=binary,source=objects+OBJECTS)
        env.Default(program)
        env.Depends(program, [ env.File(LibPath(env['LIBSENF'])) ])
        env.Alias('default', program)
        env.Alias('install_all', env.Install('$BININSTALLDIR', program))
    return program

def AllIncludesHH(env, headers):
    headers.sort()
    target = env.File("all_includes.hh")
    file(target.abspath,"w").write("".join([ '#include "%s"\n' % f
                                             for f in headers ]))
    env.Clean('all', target)

def PhonyTarget(env, target, action, sources=[]):
    env.AlwaysBuild(env.Alias(target, sources, env.Action(action)))
