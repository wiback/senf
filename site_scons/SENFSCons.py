import os.path, glob, yaptu
import SCons.Options, SCons.Environment, SCons.Script.SConscript, SCons.Node.FS
import SCons.Defaults, SCons.Action
from SCons.Script import *

def Glob(env, exclude=[], subdirs=[]):
    testSources = env.Glob("*.test.cc",strings=True)
    sources = [ x 
                for x in env.Glob("*.cc",strings=True) 
                if x not in testSources and x not in exclude ]
    for subdir in subdirs:
        testSources += env.Glob(os.path.join(subdir,"*.test.cc"),strings=True)
        sources += [ x 
                     for x in env.Glob(os.path.join(subdir,"*.cc"),strings=True)
                     if x not in testSources and x not in exclude ]
    includes = []
    for d in [ '' ] + [ x+'/' for x in subdirs ]:
        for p in env.Glob("%s*" % d, strings=True) + env.Glob("%s*" % d, strings=True, ondisk=False):
            ext = '.' + p.split('.',1)[-1]
            if ext in env['CPP_INCLUDE_EXTENSIONS'] \
               and ext not in env['CPP_EXCLUDE_EXTENSIONS'] \
               and p not in exclude:
                includes.append(p)
    includes = list(set(includes))
    sources.sort()
    testSources.sort()
    includes.sort()
    return ( sources, testSources, includes )

def Doxygen(env, doxyfile = "Doxyfile", extra_sources = [], output_directory = "doc"):
    # There is one small problem we need to solve with this builder: The Doxygen builder reads
    # the Doxyfile and thus depends on the environment variables set by site_scons/lib/doxygen.sh.
    # We thus have to provide all necessary definitions here manually via DOXYENV !

    if type(doxyfile) is type(""):
        doxyfile = env.File(doxyfile)

    # Module name is derived from the doxyfile path
    # Utils/Console/Doxyfile -> Utils_Console
    module = doxyfile.dir.get_path(env.Dir('#')).replace('/','_')
    if module == '.' : module = "Main"

    # Standard doc build vars and opts
    def vars(env=env, **kw):
        denv = { 'TOPDIR'          : env.Dir('#').abspath,
                 'LIBDIR'          : env.Dir('#/site_scons/lib').abspath,
                 'output_dir'      : '$OUTPUT_DIRECTORY',
                 'html_dir'        : 'html',
                 'html'            : 'NO',
                 'DOXYGEN'         : '$DOXYGEN' }
        denv.update(kw)
        return { 'DOXYENV'         : denv,
                 'MODULE'          : module,
                 'OUTPUT_DIRECTORY': output_directory,
                 'DOXYGENCOM'      : "site_scons/lib/doxygen.sh $DOXYOPTS $SOURCE",
                 };
    opts = [ '--tagfile-name', '"${MODULE}.tag"',
             '--output-dir', '$OUTPUT_DIRECTORY' ]

    # Rule to generate tagfile
    # (need to exclude the 'clean' case, otherwise we'll have duplicate nodes)
    if not env.GetOption('clean'):
        tagfile = env.Doxygen(doxyfile, DOXYOPTS = opts + [ '--tagfile' ],
                              **vars(generate_tagfile='${OUTPUT_DIRECTORY}/${MODULE}.tag'))
        env.Append(ALL_TAGFILES = [ tagfile[0].abspath ])
        env.Depends(tagfile, [ env.File('#/site_scons/lib/doxygen.sh'), 
                               env.File('#/site_scons/lib/tag-munge.xsl') ])

        env.Install(env.Dir('$DOCINSTALLDIR').Dir(tagfile[0].dir.get_path(env.Dir('#'))),
                    tagfile[0])

    # Rule to generate HTML documentation
    doc = env.Doxygen(doxyfile, DOXYOPTS = opts + [ '--tagfiles', '"$ALL_TAGFILES"', '--html' ],
                      **vars(html='YES', tagfiles='$ALL_TAGFILES'))
    env.Depends(doc, [ env.File('#/site_scons/lib/doxygen.sh'),
                       env.File('#/site_scons/lib/html-munge.xsl') ])

    # Copy the extra_sources (the images) into the documentation directory
    # (need to exclude the 'clean' case otherwise there are multiple ways to clean the copies)
    if extra_sources:
        if env.GetOption('clean'):
            env.Depends(doc, extra_sources)
        else:
            env.Depends(tagfile, env.CopyToDir(doc[0].dir, extra_sources))

    # Install documentation into DOCINSTALLDIR
    env.InstallDir(env.Dir('$DOCINSTALLDIR').Dir(doc[0].dir.dir.get_path(env.Dir('#'))), doc[0].dir,
                   FILTER_SUFFIXES=['.html','.css','.png','.php','.idx'])

    # Useful aliases
    env.Alias('all_docs', doc)
    env.Clean(env.Alias('all_docs'), doc)
    env.Clean(env.Alias('all'), doc)

    return doc

def AllIncludesHH(env, exclude=[]):
    exclude = exclude + ['all_includes.hh']
    headers = [ f for f in env.Glob("*.hh", source=True)
                if f.name not in exclude and not f.name.endswith('.test.hh') ]
    headers.sort(key=lambda x:x.name)
    target = env.File("all_includes.hh")
    allinch = env.CreateFile(target, 
                             env.Value("".join([ '#include <%s>\n' % f.srcnode().get_path(env.Dir('#'))
                                                 for f in headers ])))
    env.Default(allinch)
    env.Depends(allinch, headers)

INDEXPAGE="""
/** \mainpage ${TITLE}

    ${TEXT}

    \htmlonly
    <dl>

{{  for name, title in SUBPAGES:
      <dt><a href="../../${name}/doc/html/index.html">${name}</a></dt><dd>${title}</a></dd>
}}

    </dl>
    \endhtmlonly
 */
"""

def IndexPage(env, name, title, text=""):
    SUBPAGES = []
    for dox in sorted(env.Glob("*/Mainpage.dox",strings=True)):
        subtitle = ([None] + [ line.split('\\mainpage',1)[-1].strip() for line in file(dox)
                               if '\\mainpage' in line ])[-1]
        if subtitle:
            SUBPAGES.append( (dox.split('/',1)[0], subtitle) )
    file(name,"w").write(yaptu.process(
            INDEXPAGE, globals(), { 'TITLE': title, 'TEXT': text, 'SUBPAGES': SUBPAGES }))
    env.Clean('all',name)
    env.Clean('all_docs',name)

###########################################################################
# The following functions serve as simple macros for most SConscript files
#
# If you need to customize these rules, copy-and-paste the code into the
# SConscript file and adjust at will (don't forget to replace the
# parameters with their actual value. Parameters are marked with ((name)) )

def AutoRules(env, exclude=[], subdirs=[], doc_extra_sources = []):
    import SENFSCons

    sources, tests, includes = SENFSCons.Glob(env, exclude=((exclude)), subdirs=((subdirs)) )
    subscripts               = sorted(env.Glob("*/SConscript", strings=True))
    doxyfile                 = env.Glob("Doxyfile")
    objects                  = []

    if sources               : env.Append(ALLOBJECTS = env.Object(sources))
    if tests                 : env.BoostUnitTest('test', tests)
    if includes              : env.InstallSubdir('$INCLUDEINSTALLDIR', includes)
    if doxyfile              : SENFSCons.Doxygen(env, extra_sources=((doc_extra_sources)) )
    if subscripts            : SConscript(subscripts)


def AutoPacketBundle(env, name, exclude=[], subdirs=[], doc_extra_sources=[]):
    import SENFSCons

    sources, tests, includes = SENFSCons.Glob(env, exclude=((exclude)), subdirs=((subdirs)) )
    subscripts               = sorted(env.Glob("*/SConscript", strings=True))
    doxyfile                 = env.Glob("Doxyfile")

    objects = env.Object(sources)
    cobject = env.CombinedObject('${LOCALLIBDIR}/${NAME}${OBJADDSUFFIX}', objects, NAME=((name)))
    sobundle = env.SharedLibrary('${LOCALLIBDIR}/${NAME}${OBJADDSUFFIX}', sources, NAME=((name)),
                                 LIBS=[], SHLIBPREFIX='')

    env.Default(cobject)
    env.Default(sobundle)
    env.Append(ALLOBJECTS = objects, PACKET_BUNDLES = cobject)
    env.Install('$OBJINSTALLDIR', cobject)
    env.Install('$OBJINSTALLDIR', sobundle)

    if tests                 : env.BoostUnitTest('test', tests + cobject)
    if includes              : env.InstallSubdir('$INCLUDEINSTALLDIR', includes)
    if doxyfile              : SENFSCons.Doxygen(env, extra_sources=((doc_extra_sources)) )
    if subscripts            : SConscript(subscripts)


def BuildExample(env, sconstruct):
    dir     = env.File( ((sconstruct)) ).dir
    example = env.Command( dir.File('.example.phony'), env.Alias('default'),
                           [ '$SCONS -C $EXAMPLEDIR' ],
                           CONCURRENCY_LEVEL=1, EXAMPLEDIR=dir )
    env.Alias('examples', example)

    if env.GetOption('clean') and ('all' in BUILD_TARGETS or 'examples' in BUILD_TARGETS):
        env.Clone(CONCURRENCY_LEVEL=1, EXAMPLEDIR=dir).Execute([ '$SCONS -C $EXAMPLEDIR -c' ])
