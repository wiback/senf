import os.path, glob, site_tools.Yaptu
from SCons.Script import *
import SCons.Defaults, SCons.Util
import senfconf
import types, re

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
    if env.subst('$LOGLEVELS'):
        return [ 'SENF_LOG_CONF="' + ''.join( parseLogOption(x) for x in env.subst('$LOGLEVELS').split() )+'"']
    else:
        return []

class BuildTypeOptions:
    def __init__(self, var):
        self._var = var

    def __call__(self, target, source, env, for_signature):
        type = env['final'] and "final" or env['debug'] and "debug" or "normal"
        return env[self._var + "_" + type]

_DOLLAR_RE = re.compile(r'\$([a-zA-Z_][\.\w]*)|\${([^}]*)}')

def _expandDefines(defs, target, source, env, for_signature):
    rv = []
    if SCons.Util.is_Dict(defs):
        keys = defs.keys()
        keys.sort()
        defs = [ (k,defs[k]) for k in keys ]
    elif not SCons.Util.is_List(defs):
        defs = [ defs ]
    for elt in defs:
        if SCons.Util.is_String(elt):
            m = _DOLLAR_RE.match(elt)
            if m:
                match = m.group(1) or m.group(2)
                try: rv.extend(_expandDefines(eval(match, env.gvars(), env.lvars()),
                                              target, source, env, for_signature))
                except NameError: pass
                except IndexError: pass
            else:
                rv.append(env.subst(elt))
        elif callable(elt):
            rv.extend(_expandDefines(elt(target, source, env, for_signature),
                                     target, source, env, for_signature))
        elif SCons.Util.is_Sequence(elt):
            if len(elt)<2 or elt[1] is None:
                rv.append(env.subst(elt[0]))
            else:
                rv.append(env.subst(elt[0]) + "=" + env.subst(elt[1]))
        else:
            rv.append(str(elt))
    return rv

def expandDefines(prefix, defs, suffix, env):
    """Expand defines in <defs> using <env>. Calls SCons.Defaults._concat_ixes
to append prefix/suffix before/after each define.

    callable
        Call the callable and replace it with the call result. If the result
        is a list, the list is processed recursively. It the result is a
        dictionary it is converted into a list of typles and processed
        recursively.
    '$<variable>' or '${<variable>}'
        Replace the element with the variable expansion. If the result is a
        list, the list is processed recursively. It the result is a
        dictionary it is converted into a list of typles and processed
        recursively.
    '<any other string>'
        Define a symbol with that (expanded!) name
    iterable
        The iteratble must have two elments. The first element defines the
        symbol name, the second the symbol value."""

    defs = _expandDefines(defs, None, None, env, False)
    return SCons.Defaults._concat_ixes(prefix, defs, suffix, env)
    
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
    for k,v in unknv.iteritems():
        if k.endswith('+'):
            env.Append(**{k[:-1]: v})
            env.Append(ARGUMENT_VARIABLES = {k[:-1]:v})
        else:
            env.Replace(**{k: v})
            env.Append(ARGUMENT_VARIABLES = {k:v})

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
    senf_path.extend((os.path.dirname(senfutildir), '/usr/local', '/usr'))

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


def SetupForSENF(env, senf_path = [], flavor=None):
    try_flavors = [ '', 'g' ]
    if flavor is not None:
        try_flavors[0:0] = [ flavor ]
        
    res = detect_senf(env, senf_path, try_flavors)
    if not env.GetOption('no_progress'):
        if res:
            print env.subst("scons: Using${SENFSYSLAYOUT and ' system' or ''} 'libsenf${LIBADDSUFFIX}' in '$SENFDIR'")
        else:
            print "scons: SENF library not found, trying to build anyway ..."

    loadTools(env)

    # For a non-system installed senf, we add library and include search path here
    if not env.get('SENFSYSLAYOUT', True):
        env.Append(
            CPPPATH       = [ '$SENFINCDIR' ],
            LIBPATH       = [ '$SENFDIR' ],
            )
    
    env.Replace(
        _defines          = expandDefines,
        expandLogOption   = expandLogOption,
        )
    env.SetDefault(
        FLAVOR            = flavor,
        LIBADDSUFFIX      = '${FLAVOR and "_$FLAVOR" or ""}',
        OBJADDSUFFIX      = '${LIBADDSUFFIX}',
        PROJECTNAME       = "Unnamed project",
        DOCLINKS          = [],
        PROJECTEMAIL      = "nobody@nowhere.org",
        COPYRIGHT         = "nobody",
        REVISION          = "unknown",
        BUNDLEDIR         = '$SENFDIR${SENFSYSLAYOUT and "/lib/senf" or ""}',
        SENFINCDIR        = '$SENFDIR${SENFSYSLAYOUT and "/include" or ""}',
        )
    env.Append(
        CPPPATH           = [ '${NEED_BOOST_EXT and "$SENFINCDIR/boost_ext" or None}' ],
        CPPDEFINES        = [ '$expandLogOption' ],
        CXXFLAGS          = [ '-Wno-long-long', '-fno-strict-aliasing' ],
        LIBS              = [ 'senf$LIBADDSUFFIX', 'rt',
                              '$BOOSTREGEXLIB', '$BOOSTIOSTREAMSLIB', '$BOOSTSIGNALSLIB', '$BOOSTFSLIB' ],
        LINKFLAGS         = [ '-rdynamic' ],
        )
        
    try:
        env.MergeFlags(file(env.File('$BUNDLEDIR/senf${LIBADDSUFFIX}.conf').abspath).read())
    except IOError:
        # Really should never happen since detect_senf looks for this file ...
        pass

def DefaultOptions(env):
    env.Replace(
        _defines          = expandDefines,
        expandLogOption   = expandLogOption,
        )
    env.Append(
        CXXFLAGS          = [ '$CXXFLAGS_' ],
        CXXFLAGS_         = BuildTypeOptions('CXXFLAGS'),
        
        CPPDEFINES        = [ '$CPPDEFINES_' ],
        CPPDEFINES_       = BuildTypeOptions('CPPDEFINES'),
        
        LINKFLAGS         = [ '$LINKFLAGS_' ],
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
        )

    # Interpret command line options
    parseArguments(
        env, 
        BoolVariable('final', 'Build final (optimized) build', False),
        BoolVariable('debug', 'Link in debug symbols', False),
    )

    # Set nice default options
    env.Append(
        CXXFLAGS         = [ '-Wall', '-Woverloaded-virtual' ],
        CXXFLAGS_final   = [ '-O3' ],
        CXXFLAGS_normal  = [ '-O2', '-g' ],
        CXXFLAGS_debug   = [ '-O0', '-g' ],

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


@senfconf.Test
def CheckSTLCopyN(context):
    context.Message("Checking for 'copy_n' implementation... ")
    versions = [ ('<algorithm>',     'std::copy_n',       'STD'),
                 ('<ext/algorithm>', '__gnu_cxx::copy_n', 'GNUCXX') ]
    for include, name, define in versions:
        ret = context.TryCompile("#include %s\n"
                                 "int main(int,char**) { int *a,*b; %s(a,0,b); }\n"
                                 % (include, name),
                                 ".cc")
        if ret:
            context.Result(name)
            context.sconf.Define("HAVE_%s_COPYN" % define,
                                 1,
                                 "Define one of " 
                                 + ", ".join(("HAVE_%s_COPYN" % elt[2] for elt in versions)))
            return ret

    context.Result(False)
    return False


@senfconf.Test
def CheckTempBufferStrategy(context):
    context.Message("Checking for optimal temporary buffer strategy... ")

    def check():
      # locals
      ret = context.TryCompile("void test(int n){int a[n];}",".cc")
      if ret: return "locals"

      # alloca
      ret = context.TryCompile("#include <alloca.h>\n"
                               "void test(int a){void *b(alloca(a));}"
                               ".cc")
      if ret: return "alloca"
      
      # fallback: new
      return "new"

    ret = check()
    context.Result(ret)
    context.sconf.Define("SENF_BUFFER_USE_%s" % ret.upper(),
                         1,
                         "Define one of SENF_BUFFER_USE_LOCALS, SENF_BUFFER_USE_ALLOCA, "
                         "SENF_BUFFER_USE_NEW")
    return ret


def Fail(msg):
    SCons.Util.display("scons: *** %s" % msg)
    Exit(1)


def Configure(env, customChecks=None):
    conf = env.Configure(clean=False, 
                         help=False, 
                         custom_tests=senfconf.Tests(), 
                         config_h="#/senf/autoconf.hh")

    # Boost
    if not conf.CheckBoostVersion():
        Fail("Boost includes not found")
    if not conf.env['ARGUMENT_VARIABLES'].has_key('BOOST_VARIANT'): conf.CheckBoostVariants( '', 'mt' )
    conf.env.Replace(NEED_BOOST_EXT = not conf.CheckCXXHeader("boost/bimap.hpp"))
    conf.CheckCXXHeader("boost/spirit/include/classic.hpp")
        
    # Compiler support
    conf.CheckTempBufferStrategy()

    # Standard library stuff
    if not conf.CheckSTLCopyN():
        Fail("No 'copy_n' implementation found")
    conf.CheckFunc("timerfd_create")

    # User checks
    if customChecks:
        customChecks(conf)

    conf.Finish()

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
