import os, os.path, site_tools.Yaptu, types, re, fnmatch, sys, platform
import SCons.Util
from SCons.Script import *

senfutildir = os.path.dirname(__file__)

# Fix for SCons 0.97 compatibility
try:
    Variables
except NameError:
    Variables = Options
    BoolVariable = BoolOption

###########################################################################
extdir = os.path.join(senfutildir, '../senf/Ext')
sys.path.append(extdir)

for ext in os.listdir(extdir):
    if not os.path.isdir( os.path.join(extdir, ext)): continue
    if ext.startswith('.'): continue
    try:
        setattr( sys.modules[__name__], ext, 
                __import__('%s.site_scons' % ext, fromlist=['senfutil']).senfutil )
    except ImportError:
        pass

###########################################################################

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
    env.SetDefault(ARGUMENT_VARIABLES = {})
    for k,v in ARGLIST:
        if not unknv.has_key(k) : continue
        if k.endswith('+'):
            env.Append(**{k[:-1]: v})
            env.Append(ARGUMENT_VARIABLES = {k[:-1]:v})
        else:
            env.Replace(**{k: v})
            env.Append(ARGUMENT_VARIABLES = {k:v})
    if env.get('PARSEFLAGS', None):
        env.MergeFlags(env['PARSEFLAGS'])

def importProcessEnv(env):
    env.Append( ENV = dict(( (k,v)
                             for pattern in env.get('IMPORT_ENV',[])
                             for k,v in os.environ.iteritems()
                             if fnmatch.fnmatchcase(k,pattern) )) )


###########################################################################$
# SENF log option parsing

def parseLogOption(value):
    stream, area, level = ( x.strip() for x in value.strip().split('|') )
    stream = ''.join('(%s)' % x for x in stream.split('::') )
    if area : area = ''.join( '(%s)' % x for x in area.split('::') )
    else    : area = '(_)'
    return '((%s,%s,%s))' % (stream,area,level)

def expandLogOption(target, source, env, for_signature):
    if env.subst('$LOGLEVELS'):
        return [ 'SENF_LOG_CONF="' + ''.join( parseLogOption(x) for x in env.subst('$LOGLEVELS').split() )+'"']
    else:
        return []

###########################################################################
# client SENF detection/configuration

def detect_senf(env,senf_path, try_flavors):
    """Detect senf with flavor in 'try_flavors' somewhere in 'senf_path'.

This function returns True, if senf is found, False otherwise.
The environment 'env' is updated in the following way:

    SENFSYSLAYOUT  set to True, if the install is a system installation,
                   False otherwise

    FLAVOR         set to the detected senf flavor

    SENFDIR        set to the base directory of the senf installation.
"""
    global senfutildir
    if env['builddir']:
        senf_path = [
             os.path.join( os.path.dirname(senfutildir), 'build', env.subst('$VARIANT')) ]
    else:
        senf_path.extend( (os.path.dirname(senfutildir), '/usr/local', '/usr') )

    for path in senf_path:
        if not path.startswith('/') : sconspath = '#/%s' % path
        else                        : sconspath = path
        for flavor in try_flavors:
            suffix = flavor and "_"+flavor or ""
            local_path = os.path.join(path,"senf%s.conf" % suffix)
            sys_path = os.path.join(path, "lib", "senf", "senf%s.conf" % suffix)
            if os.path.exists(local_path):
                env.SetDefault( SENFSYSLAYOUT = False )
            elif os.path.exists(sys_path):
                env.SetDefault( SENFSYSLAYOUT  = True )
            else:
                continue
            env.SetDefault( FLAVOR = flavor, SENFDIR = sconspath )
            return True
    return False

def SetupForSENF(env, senf_path = [], flavor=None, exit_if_not_found=False):
    ParseDefaultArguments(env)

    try_flavors = [ '', 'g' ]
    if flavor is not None:
        try_flavors[0:0] = [ flavor ]

    if not env.GetOption('clean'):
        res = detect_senf(env, senf_path, try_flavors)
        if res:
            if not env.GetOption('no_progress'):
                print env.subst("scons: Using${SENFSYSLAYOUT and ' system' or ''} "
                                "'libsenf${LIBADDSUFFIX}' in '$SENFDIR'")
        else:
            print "scons: SENF library not found,",
            if exit_if_not_found:
                print "abort."
                sys.exit(1)
            else:
                "trying to build anyway ..."

    loadTools(env)

    # For a non-system installed senf, we add library and include search path here
    if not env.get('SENFSYSLAYOUT', True):
        env.Append(
            CPPPATH       = [ '$SENFINCDIR' ],
            LIBPATH       = [ '$SENFDIR' ],
            )

    if env['BOOST_VARIANT'] is None:
        conf = env.Configure(clean=False, help=False)
        conf.CheckBoostVersion(fail=True)
        conf.CheckBoostVariants()
        conf.Finish()

    env.Replace(
        expandLogOption   = expandLogOption,
        )
    env.SetDefault(
        LIBADDSUFFIX      = '${FLAVOR and "_$FLAVOR" or ""}',
        OBJADDSUFFIX      = '${LIBADDSUFFIX}',
        BUNDLEDIR         = '$SENFDIR${SENFSYSLAYOUT and "/lib/senf" or ""}',
        SENFINCDIR        = '$SENFDIR${SENFSYSLAYOUT and "/include" or ""}',

        PROJECTNAME       = "Unnamed project",
        DOCLINKS          = [],
        PROJECTEMAIL      = "nobody@nowhere.org",
        COPYRIGHT         = "nobody",
        REVISION          = "unknown",
        )
    env.Append(
        CPPDEFINES        = [ '$expandLogOption' ],
        CXXFLAGS          = [ '-Wno-long-long', '-fno-strict-aliasing' ],
        LINKFLAGS         = [ '-rdynamic' ],
        LIBS              = [ 'senf$LIBADDSUFFIX', 'rt', '$BOOSTREGEXLIB',
                              '$BOOSTSIGNALSLIB', '$BOOSTFSLIB', '$BOOSTSYSTEMLIB',
                              '$BOOSTDATETIMELIB' ],
        )

    try:
        path = env.File('$BUNDLEDIR/senf${LIBADDSUFFIX}.conf').abspath
        env.MergeFlags(file(path).read())
    except IOError:
        # Really should never happen since detect_senf looks for this file ...
        pass

###########################################################################
# Helpers

def ParseDefaultArguments(env):
    # Interpret command line options
    parseArguments(
        env,
        BoolVariable('final', 'Build final (optimized) build', False),
        BoolVariable('debug', 'Link in debug symbols', False),
        BoolVariable('debug_final', 'Build final (optimized) build with debug symbols', False),
        BoolVariable('profile', 'compile and link with the profiling enabled option', False),
        BoolVariable('builddir', 'use build dir build/{platform}_{build_type}', False),
        BoolVariable('lto', 'enable link-time-optimization', False),
    )
    
    build_type = 'normal'
    for type in ('debug_final', 'final', 'debug'):
        if env[type]: 
            build_type = type
    if env['profile']:
        build_type += '-profile'
        
    issue = re.sub(r'\\\w', '', open('/etc/issue').readline()).strip().lower().replace(' ', '_').replace('/', '_')
    env.Replace(
        BUILD_TYPE = build_type,
        VARIANT    = '%s-%s-%s' % (issue, platform.machine(), build_type)
    )
    
def DefaultOptions(env):
    env.Replace(
        expandLogOption   = expandLogOption,
        CXXFLAGS_         = env.BuildTypeOptions('CXXFLAGS'),
        CPPDEFINES_       = env.BuildTypeOptions('CPPDEFINES'),
        LINKFLAGS_        = env.BuildTypeOptions('LINKFLAGS'),
        LOGLEVELS_        = env.BuildTypeOptions('LOGLEVELS'),
    )
    env.Append(
        CXXFLAGS          = [ '$CXXFLAGS_' ],
        CPPDEFINES        = [ '$CPPDEFINES_' ],
        LINKFLAGS         = [ '$LINKFLAGS_' ],
        LOGLEVELS         = [ '$LOGLEVELS_' ],
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
    )

    if env['debug_final']:
        env['final'] = True
        env.Append(CXXFLAGS = [ '-g' ])

    # Set nice default options
    env.Append(
        CXXFLAGS_CLANG   = [ '-Wno-unneeded-internal-declaration', '-Wheader-hygiene' ], 
        CXXFLAGS         = [ '-Wall', '-Wextra', '-Woverloaded-virtual', '-Wno-unused-parameter', 
                             '-Wno-unused-function',
                             "${profile and '-pg' or None}", "${lto and '-flto' or None}", 
                             '${str(CXX).split("/")[-1] == "clang++" and "$CXXFLAGS_CLANG" or None}' ],
        CXXFLAGS_final   = [ '-O3', '-fno-threadsafe-statics', '-fno-stack-protector',
                               "${profile and ' ' or '-ffunction-sections'}" ],
        CXXFLAGS_normal  = [ '-O2', '-g' ],
        CXXFLAGS_debug   = [ '-O0', '-g' ],

        LINKFLAGS        = [ "${profile and '-pg' or None}", "${lto and '-flto -fwhole-program' or None}" ],
        LINKFLAGS_final  = [ "${profile and ' ' or '-Wl,--gc-sections'}" ],
        LINKFLAGS_normal = [ '-Wl,-S' ],
        LINKFLAGS_debug  = [ '-g' ],
    )
    
    env.Alias('all', '#')


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

def CleanGlob(env, targets, patterns):
    if env.GetOption('clean'):
        targets = SCons.Util.flatten(targets)
        for target in targets:
            if target in BUILD_TARGETS:
                patterns = map(str,SCons.Util.flatten(env.subst_list(patterns)))
                files = [ os.path.join(path,f)
                          for path, subdirs, files in os.walk('.')
                          for pattern in patterns
                          for f in fnmatch.filter(files,pattern) ]
                return env.Clean(target, files)

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
                                     'html'       : 'YES',
                                     'DOXYGEN'    : '$DOXYGEN' },
                      TAGFILES   = tagfiles,
                      DOCLIBDIR  = libdir,
                      DOXYGENCOM = "$DOCLIBDIR/doxygen.sh $DOXYOPTS $SOURCE")

    env.Depends(doc, [ doxyheader, doxyfooter, doxycss ])

    return doc
