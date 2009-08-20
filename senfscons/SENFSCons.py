import os.path, glob
import SCons.Options, SCons.Environment, SCons.Script.SConscript, SCons.Node.FS
import SCons.Defaults, SCons.Action
from SCons.Script import *

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

def Test(env, sources):
    test=env.BoostUnitTest( target = 'test', source = sources )
    env.Alias('all_tests', test)
    return test
    
def Objects(env, sources, testSources = None):
    if type(sources) == type(()):
        testSources = sources[1]
        sources = sources[0]
    if type(sources) is not type([]):
        sources = [ sources ]

    objects = env.Object(sources)

    if testSources:
        Test(env, testSources)

    return objects

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

def Lib(env, sources, testSources = None, OBJECTS = []):
    objects = Objects(env,sources,testSources)
    env.Append(ALLOBJECTS = objects)
    return objects

def Object(env, target, sources, testSources = None, OBJECTS = []):
    objects = Objects(env,sources,testSources)
    ob = env.Command(target+"${OBJADDSUFFIX}${OBJSUFFIX}", objects+OBJECTS, 
                     [ "ld -r -o $TARGET $SOURCES" ])
    env.Default(ob)
    env.Alias('default', ob)
    env.Alias('install_all', env.Install("$OBJINSTALLDIR", ob))
    return ob

def Binary(env, binary, sources, testSources = None, OBJECTS = []):
    objects = Objects(env, sources, testSources)
    program = env.Program(target = binary, 
                          source = objects+OBJECTS,
                          LIBS   = [ '$LIBSENF$LIBADDSUFFIX' ] + env['LIBS'])
    env.Default(program)
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
