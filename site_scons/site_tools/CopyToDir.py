import os.path
import SCons.Builder, SCons.Defaults

def emitter(source, target, env):
    source = env.arg2nodes(source, env.fs.File)
    target = env.arg2nodes(target, env.fs.Dir)
    return ([ target[0].File(src.name) for src in source ], source)

def generator(source, target, env, for_signature):
    return [ SCons.Defaults.Copy(t, s) for s, t in zip(source, target) ]

CopyToDir = SCons.Builder.Builder(emitter = emitter,
                                  generator = generator)

def generate(env):
    env['BUILDERS']['CopyToDir'] = CopyToDir

def exists(env):
    return 1
