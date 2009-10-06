import os.path, glob, site_tools.Yaptu
from SCons.Script import *

senfutildir = os.path.dirname(__file__)

# Fix for SCons 0.97 compatibility
try:
    Variables
except NameError: 
    Variables = Options
    BoolVariable = BoolOption

def parseLogOption(value):
    stream, area, level = ( x.strip() for x in value.strip().split('|') )
    stream = ''.join('(%s)' % x for x in stream.split('::') )
    if area : area = ''.join( '(%s)' % x for x in area.split('::') )
    else    : area = '(_)'
    return '((%s,%s,%s))' % (stream,area,level)

def expandLogOption(target, source, env, for_signature):
    if env.get('LOGLEVELS'):
        return [ 'SENF_LOG_CONF="' + ''.join( parseLogOption(x) for x in env.subst('$LOGLEVELS').split() )+'"']
    else:
        return []

class BuildTypeOptions:
    def __init__(self, var):
        self._var = var

    def __call__(self, target, source, env, for_signature):
        type = env['final'] and "final" or env['debug'] and "debug" or "normal"
        return env[self._var + "_" + type]

def loadTools(env):
    global senfutildir
    tooldir = os.path.join(senfutildir, 'site_tools')
    for tool in os.listdir(tooldir):
        name, ext = os.path.splitext(tool)
        if ext == '.py' and name != "__init__" : env.Tool(name, [ tooldir ])

def parseArguments(env, *defs):
    vars = Variables(args=ARGUMENTS)
    for d in defs : vars.Add(d)
    vars.Update(env)
    env.Help("""
Any construction environment variable may be set from the scons
command line (see SConstruct file and SCons documentation for a list
of variables) using

   VARNAME=value    Assign new value  
   VARNAME+=value   Append value at end

Special command line parameters:
""")
    env.Help(vars.GenerateHelpText(env))
    try                  : unknv = vars.UnknownVariables()
    except AttributeError: unknv = vars.UnknownOptions()
    for k,v in unknv.iteritems():
        if k.endswith('+'):
            env.Append(**{k[:-1]: v})
        else:
            env.Replace(**{k: v})


###########################################################################
# This looks much more complicated than it is: We do three things here:
# a) switch between final or debug options
# b) parse the LOGLEVELS parameter into the correct SENF_LOG_CONF syntax
# c) check for a local SENF, set options accordingly and update that SENF if needed

def SetupForSENF(env, senf_path = []):
    global senfutildir
    senf_path.extend(('senf', os.path.dirname(senfutildir), '/usr/local', '/usr'))

    loadTools(env)

    env.Append(
        LIBS              = [ 'senf', 'rt', '$BOOSTREGEXLIB',
                              '$BOOSTIOSTREAMSLIB', '$BOOSTSIGNALSLIB',
                              '$BOOSTFSLIB' ],
        
        CXXFLAGS          = [ '-Wno-long-long', '$CXXFLAGS_' ],
        CXXFLAGS_         = BuildTypeOptions('CXXFLAGS'),
        
        CPPDEFINES        = [ '$expandLogOption', '$CPPDEFINES_' ],
        expandLogOption   = expandLogOption,
        CPPDEFINES_       = BuildTypeOptions('CPPDEFINES'),
        
        LINKFLAGS         = [ '-rdynamic', '$LINKFLAGS_' ],
        LINKFLAGS_        = BuildTypeOptions('LINKFLAGS'),

        LOGLEVELS         = [ '$LOGLEVELS_' ],
        LOGLEVELS_        = BuildTypeOptions('LOGLEVELS'),
        )

    env.SetDefault( 
        CXXFLAGS_final    = [],
        CXXFLAGS_normal   = [],
        CXXFLAGS_debug    = [],

        CPPDEFINES_final  = [],
        CPPDEFINES_normal = [],
        CPPDEFINES_debug  = [],

        LINKFLAGS_final   = [],
        LINKFLAGS_normal  = [],
        LINKFLAGS_debug   = [],

        LOGLEVELS_final   = [],
        LOGLEVELS_normal  = [],
        LOGLEVELS_debug   = [],

        PROJECTNAME       = "Unnamed project",
        DOCLINKS          = [],
        PROJECTEMAIL      = "nobody@nowhere.org",
        COPYRIGHT         = "nobody",
        REVISION          = "unknown",
        )

    # Interpret command line options
    parseArguments(
        env, 
        BoolVariable('final', 'Build final (optimized) build', False),
        BoolVariable('debug', 'Link in debug symbols', False),
    )

    # If we have a symbolic link (or directory) 'senf', we use it as our
    # senf repository
    for path in senf_path:
        if not path.startswith('/') : sconspath = '#/%s' % path
        else                        : sconspath = path
        if os.path.exists(os.path.join(path,"senf/config.hh")):
            if not env.GetOption('no_progress'):
                print "\nUsing SENF in '%s'\n" \
                    % ('/..' in sconspath and os.path.abspath(path) or sconspath)
            env.Append( LIBPATH = [ sconspath ],
                        CPPPATH = [ sconspath ],
                        BUNDLEDIR = sconspath,
                        SENFDIR = sconspath,
                        SENFSYSLAYOUT = False)
            try:
                env.MergeFlags(file(os.path.join(path,"senf.conf")).read())
            except IOError:
                if not env.GetOption('no_progress'):
                    print "(SENF configuration file 'senf.conf' not found, assuming non-final SENF)"
                env.Append(CPPDEFINES = [ 'SENF_DEBUG' ])
            break
        elif os.path.exists(os.path.join(path,"include/senf/config.hh")):
            if not env.GetOption('no_progress'):
                print "\nUsing system SENF in '%s/'\n" % sconspath
            env.Append(BUNDLEDIR = os.path.join(sconspath,"lib/senf"),
                       SENFDIR = sconspath,
                       SENFSYSLAYOUT = True)
            break
    else:
        if not env.GetOption('no_progress'):
            print "\nSENF library not found .. trying build anyway !!\n"

    env.Alias('all', '#')


def DefaultOptions(env):
    env.Append(
        CXXFLAGS         = [ '-Wall', '-Woverloaded-virtual' ],
        CXXFLAGS_final   = [ '-O2' ],
        CXXFLAGS_normal  = [ '-O0', '-g' ],
        CXXFLAGS_debug   = [ '$CXXFLAGS_normal' ],

        LINKFLAGS_normal = [ '-Wl,-S' ],
        LINKFLAGS_debug  = [ '-g' ],
    )

def Glob(env, exclude=[], subdirs=[]):
    testSources = env.Glob("*.test.cc", strings=True)
    sources = [ x 
                for x in env.Glob("*.cc", strings=True) 
                if x not in testSources and x not in exclude ]
    for subdir in subdirs:
        testSources += env.Glob(os.path.join(subdir,"*.test.cc"), strings=True)
        sources += [ x 
                     for x in env.Glob(os.path.join(subdir,"*.cc"), strings=True)
                     if x not in testSources and x not in exclude ]
    sources.sort()
    testSources.sort()
    return (sources, testSources)

tagfiles = None

def Doxygen(env, doxyheader=None, doxyfooter=None, doxycss=None, mydoxyfile=False, senfdoc_path=[],
            **kw):
    # Additional interesting keyword arguments or environment variables:
    #    PROJECTNAME, DOCLINKS, PROJECTEMAIL, COPYRIGHT, REVISION

    global senfutildir
    global tagfiles
    libdir=os.path.join(senfutildir, 'lib')
    
    if tagfiles is None:
        senfdocdir = None
        senfdoc_path.extend(('senfdoc', '$SENFDIR', '$SENFDIR/manual',
                             '$SENFDIR/share/doc/senf', '$SENFDIR/share/doc/libsenf-doc/html'))
        for path in senfdoc_path:
            path = env.Dir(path).get_path()
            if os.path.exists(os.path.join(path, "doc/doclib.tag")):
                senfdocdir = path
                break
        tagfiles = []
        if senfdocdir is None:
            if not env.GetOption('no_progress'):
                print "(SENF documentation not found)"
        else:
            for dir, dirs, files in os.walk(senfdocdir):
                tagfiles.extend([ os.path.join(dir,f) for f in files if f.endswith('.tag') ])
                if dir.endswith('/doc') : 
                    try: dirs.remove('html')
                    except ValueError: pass
                for d in dirs: 
                    if d.startswith('.') : dirs.remove(d)
    
    if env.GetOption('clean'):
        env.Clean('doc', env.Dir('doc'))
        if not mydoxyfile:
            env.Clean('doc', "Doxyfile")

    if not mydoxyfile:
        # Create Doxyfile NOW
        site_tools.Yaptu.yaptuAction("Doxyfile", 
                                     os.path.join(libdir, "Doxyfile.yap"),
                                     env)

    envvalues = [ env.Value('$PROJECTNAME'),
                  env.Value('$DOCLINKS'),
                  env.Value('$PROJECTEMAIL'),
                  env.Value('$COPYRIGHT'),
                  env.Value('$REVISION') ]

    # The other files are created using dependencies
    if doxyheader: 
        doxyheader = env.CopyToDir(env.Dir("doc"), doxyheader)
    else:
        doxyheader = env.Yaptu("doc/doxyheader.html", os.path.join(libdir, "doxyheader.yap"), **kw)
        env.Depends(doxyheader, envvalues)
    if doxyfooter:
        doxyfooter = env.CopyToDir(env.Dir("doc"), doxyfooter)
    else:
        doxyfooter = env.Yaptu("doc/doxyfooter.html", os.path.join(libdir, "doxyfooter.yap"), **kw)
        env.Depends(doxyfooter, envvalues)
    if doxycss:
        doxycss = env.CopyToDir(env.Dir("doc"), doxycss)
    else:
        doxycss    = env.CopyToDir(env.Dir("doc"), os.path.join(libdir, "doxy.css"))

    doc = env.Doxygen("Doxyfile",
                      DOXYOPTS   = [ '--html', '--tagfiles', '"$TAGFILES"' ],
                      DOXYENV    = { 'TOPDIR'     : env.Dir('#').abspath,
                                     'LIBDIR'     : libdir,
                                     'REVISION'   : '$REVISION',
                                     'tagfiles'   : '$TAGFILES',
                                     'output_dir' : 'doc',
                                     'html_dir'   : 'html',
                                     'html'       : 'YES' },
                      TAGFILES   = tagfiles, 
                      DOCLIBDIR  = libdir,
                      DOXYGENCOM = "$DOCLIBDIR/doxygen.sh $DOXYOPTS $SOURCE")

    env.Depends(doc, [ doxyheader, doxyfooter, doxycss ])

    return doc
