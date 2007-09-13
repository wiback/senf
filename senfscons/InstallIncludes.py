## \file
# \brief InstallIncludes builder

## \package senfscons.InstallIncludes
# \brief Install all include files which some targets depend upon
#
# \ingroup builder

import SCons.Builder, SCons.Action, SCons.Environment, SCons.Node.FS

def recursiveChildren(f):
    rv = {}
    map(rv.setdefault,f)    
    for c in f:
        map(rv.setdefault,recursiveChildren(c.children()))
    return rv.keys()

def filterIncludes(files, extensions):
    return [ f for f in files
             if f.get_suffix() in extensions ]

def filterDirectory(files, dir):
    return [ f for f in files
             if f.abspath.startswith(dir.abspath) ]

def excludeDirectories(files, dirs):
    return [ f for f in files
             if not [ True for d in dirs if files.abspath.startswith(dirs.abspath) ] ]

def emitter(target, source, env):
    source = recursiveChildren(source)
    source = filterIncludes(source, env['CPP_INCLUDE_EXTENSIONS'])
    source = filterDirectory(source, env['INSTALL_BASE'])
    source = excludeDirectories(source, env['INCLUDE_IGNORED_DIRECTORIES'])

    # Build target file by appending the path of 'src' relative to INSTALL_BASE to target[0]
    target = [ target[0].File(src.get_path(env.Dir(env['INSTALL_BASE'])))
               for src in source ]

    return (target, source)

class Installer:
    def __init__(self, target, source):
        self.target = target;
        self.source = source

    def __call__(self, target, source, env):
        SCons.Environment.installFunc([self.target], [self.source], env)

def generator(target, source, env, for_signature):
    return [ SCons.Action.Action( Installer(trg, src),
                                  lambda a,b,c,s=SCons.Environment.installString([trg], [src], env):s )
             for trg, src in zip(target,source) ]

InstallIncludes = SCons.Builder.Builder(emitter = emitter,
                                        generator = generator,
                                        source_factory = SCons.Node.FS.Entry,
                                        target_factory = SCons.Node.FS.Dir,
                                        name = 'InstallIncludesBuilder',
                                        )

def generate(env):
    env['BUILDERS']['InstallIncludes'] = InstallIncludes
    env['INSTALL_BASE'] = '#'
    env['INCLUDE_IGNORED_DIRECTORIES'] = []

def exists(env):
    return 1
