## \file
# \brief CopyToDir builder

## \package senfscons.ProgramNoScan
# \brief Program builder without target scanner
#
# This build is like env.Program() but does not scan for library dependencies.
# This is needed if library dependencies are added explicitly, which is needed
# when libraries are built in the same build and therefore might not exist
# before builting the program (and will therefore not be found by the target
# scanner)
#
# \ingroup builder

import SCons.Builder, SCons.Defaults

ProgramNoScan = SCons.Builder.Builder(action = SCons.Defaults.LinkAction,
                                      emitter = '$PROGEMITTER',
                                      prefix = '$PROGPREFIX',
                                      suffix = '$PROGSUFFIX',
                                      src_suffix = '$OBJSUFFIX',
                                      src_builder = 'Object')

def generate(env):
    env['BUILDERS']['ProgramNoScan'] = ProgramNoScan

def exists(env):
    return 1
