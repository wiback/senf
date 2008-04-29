import os.path
from SCons.Script import *

###########################################################################
# This looks much more complicated than it is: We do three things here:
# a) switch between final or debug options
# b) parse the LOGLEVELS parameter into the correct SENF_LOG_CONF syntax
# c) check for a local SENF, set options accordingly and update that SENF if needed

def SetupForSENF(env):
    env.Append( LIBS           = [ 'senf', 'iberty', 'boost_regex', 'boost_iostreams' ],
                CXXFLAGS       = [ '-Wno-long-long',
                                   '${"$CXXFLAGS_"+(final and "final" or "debug")}' ],
                LINKFLAGS      = [ '${"$LINKFLAGS_"+(final and "final" or "debug")}' ],
                SENF_BUILDOPTS = [ '-j%s' % (env.GetOption('num_jobs') or "1") ],
                CXXFLAGS_debug  = [ '-O0', '-g', '-fno-inline' ],
                LINKFLAGS_debug = [ '-g', '-rdynamic' ],
                )

    # Parse LOGLEVELS parameter
    def parseLogOption(value):
        stream, area, level = ( x.strip() for x in value.strip().split('|') )
        stream = ''.join('(%s)' % x for x in stream.split('::') )
        if area : area = ''.join( '(%s)' % x for x in elts[1].split('::') )
        else    : area = '(_)'
        return '(( %s,%s,%s ))' % (stream,area,level)

    def expandLogOption(target, source, env, for_signature):
        return ' '.join( parseLogOption(x) for x in env.subst('$LOGLEVELS').split() )

    # Add command-line options: 'LOGLEVELS' and 'final'
    opts = Options()
    opts.Add( 'LOGLEVELS', 'Special log levels. Syntax: <stream>|[<area>]|<level> ...',
              '${"$LOGLEVELS_"+(final and "final" or "debug")}' )
    opts.Add( BoolOption('final', 'Build final (optimized) build', False) )
    opts.Update(env)

    if env.subst('$LOGLEVELS'):
        env.Append( expandLogOption=expandLogOption )
        env.Append( CPPDEFINES = { 'SENF_LOG_CONF': '$expandLogOption' } )

    env.Help(opts.GenerateHelpText(env))

    # If we have a symbolic link (or directory) 'senf', we use it as our
    # senf repository
    if os.path.exists('senf'):
        print "\nUsing SENF in './senf'\n"
        env.Append( LIBPATH = [ 'senf' ],
                    CPPPATH = [ 'senf/include' ],
                    SENF_BUILDOPTS = [ '${final and "final=1" or None}' ],
                    CPPDEFINES = [ '${not(final) and "SENF_DEBUG" or None}' ] )

        env.Default(
            env.AlwaysBuild(
                env.Command('senf/libsenf.a', [],  [ 'scons -C senf $SENF_BUILDOPTS libsenf.a' ])))
    else:
        print '\nUsing global SENF\n'
