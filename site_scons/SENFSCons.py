import os.path, glob
import SCons.Options, SCons.Environment, SCons.Script.SConscript, SCons.Node.FS
import SCons.Defaults, SCons.Action
from SCons.Script import *

def Glob(env, exclude=[], subdirs=[]):
    testSources = glob.glob("*.test.cc")
    sources = [ x for x in glob.glob("*.cc") if x not in testSources and x not in exclude ]
    for subdir in subdirs:
        testSources += glob.glob(os.path.join(subdir,"*.test.cc"))
        sources += [ x for x in glob.glob(os.path.join(subdir,"*.cc"))
                     if x not in testSources and x not in exclude ]
    includes = []
    for d in [ '.' ] + subdirs:
        for f in os.listdir(d):
            ext = '.' + f.split('.',1)[-1]
            p = os.path.join(d,f)
            if ext in env['CPP_INCLUDE_EXTENSIONS'] \
               and ext not in env['CPP_EXCLUDE_EXTENSIONS'] \
               and p not in exclude:
                includes.append(p)
    return ( sources, testSources, includes )

def Doxygen(env, doxyfile = "Doxyfile", extra_sources = []):
    # There is one small problem we need to solve with this builder: The Doxygen builder reads
    # the Doxyfile and thus depends on the environment variables set by doclib/doxygen.sh. We
    # thus have to provide all necessary definitions here manually via DOXYENV !

    if type(doxyfile) is type(""):
        doxyfile = env.File(doxyfile)

    # Module name is derived from the doxyfile path
    # Utils/Console/Doxyfile -> Utils_Console
    module = doxyfile.dir.get_path(env.Dir('#')).replace('/','_')
    if module == '.' : module = "Main"

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
        env.Depends(tagfile, [ env.File('#/doclib/doxygen.sh'), 
                               env.File('#/doclib/tag-munge.xsl') ])

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
    env.Depends(doc, [ env.File('#/doclib/doxygen.sh'),
                       env.File('#/doclib/html-munge.xsl') ])

    # Copy the extra_sources (the images) into the documentation directory
    # (need to exclude the 'clean' case otherwise there are multiple ways to clean the copies)
    if not env.GetOption('clean'):
        if extra_sources:
            env.Depends(doc, env.CopyToDir(doc[0].dir, extra_sources))

    # Install documentation into DOCINSTALLDIR
    l = len(env.Dir('#').abspath)
    env.Install(env.Dir('$DOCINSTALLDIR').Dir(doc[0].dir.get_path('#')), doc[0].dir)

    # Useful aliases
    env.Alias('all_docs', doc)
    env.Clean('all_docs', doc)
    env.Clean('all', doc)

    return doc

def AllIncludesHH(env, exclude=[]):
    exclude = exclude[:] + ['all_includes.hh'] # Make a copy !!
    headers = [ f for f in glob.glob("*.hh")
                if f not in exclude and not f.endswith('.test.hh') ]
    headers.sort()
    target = env.File("all_includes.hh")
    file(target.abspath,"w").write("".join([ '#include "%s"\n' % f
                                             for f in headers ]))
    env.Clean('all', target)