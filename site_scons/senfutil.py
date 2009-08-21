import os.path
from SCons.Script import *

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


###########################################################################
# This looks much more complicated than it is: We do three things here:
# a) switch between final or debug options
# b) parse the LOGLEVELS parameter into the correct SENF_LOG_CONF syntax
# c) check for a local SENF, set options accordingly and update that SENF if needed

def SetupForSENF(env, senf_paths = []):
    senf_paths.extend(('senf', '../senf', os.path.dirname(os.path.dirname(__file__))))
    env.Append( LIBS           = [ 'senf', 'rt', '$BOOSTREGEXLIB',
                                   '$BOOSTIOSTREAMSLIB', '$BOOSTSIGNALSLIB',
                                   '$BOOSTFSLIB' ],
                BOOSTREGEXLIB  = 'boost_regex',
                BOOSTIOSTREAMSLIB = 'boost_iostreams',
                BOOSTSIGNALSLIB = 'boost_signals',
                BOOSTFSLIB = 'boost_filesystem',
                CXXFLAGS       = [ '-Wno-long-long',
                                   '${"$CXXFLAGS_"+(final and "final" or "debug")}',
                                   '${profile and ("-g","-pg") or None}' ],
                LINKFLAGS      = [ '${"$LINKFLAGS_"+(final and "final" or "debug")}',
                                   '${profile and "-pg" or None}' ],
                SENF_BUILDOPTS = [ '-j%s' % (env.GetOption('num_jobs') or "1") ],
                CXXFLAGS_debug  = [ '-O0', '-g', '-fno-inline' ],
                LINKFLAGS_debug = [ '-g', '-rdynamic' ],
                
                expandLogOption = expandLogOption,
                CPPDEFINES      = [ '$expandLogOptions' ],
                )

    # Add command-line options: 'LOGLEVELS' and 'final'
    opts = Options()
    opts.Add( 'LOGLEVELS', 'Special log levels. Syntax: <stream>|[<area>]|<level> ...',
              '${"$LOGLEVELS_"+(final and "final" or "debug")}' )
    opts.Add( BoolOption('final', 'Build final (optimized) build', False) )
    opts.Add( BoolOption('debug', 'Link in debug symbols', False) )
    opts.Add( BoolOption('profile', 'Add profile information', False) )
    opts.Update(env)

    env.Help(opts.GenerateHelpText(env))

    # If we have a symbolic link (or directory) 'senf', we use it as our
    # senf repository
    for path in senf_paths:
        if os.path.exists(path):
            print "\nUsing SENF in '%s/'\n" % os.path.abspath(path)
            env.Append( LIBPATH = [ path ],
                        CPPPATH = [ path ],
                        SENF_BUILDOPTS = [ '${final and "final=1" or None}',
                                           '${debug and "debug=1" or None}',
                                           '${profile and "profile=1" or None}' ],
                        CPPDEFINES = [ '${not(final) and "SENF_DEBUG" or None}' ] )
            #env.Default(
            #    env.AlwaysBuild(
            #        env.Command('senf/libsenf.a', [],  [ 'scons -C %s $SENF_BUILDOPTS libsenf.a' % os.path.realpath('senf')])))
            break
    else:
        print '\nUsing global SENF\n'
