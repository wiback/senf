import SCons.Errors

def InstallSubdir(env, target=None, source=None, base='#', **kw):
    builds = []
    target = env.arg2nodes(target, env.fs.Dir)
    if len(target) != 1:
        raise SCons.Errors.UserError, "Need exactly one target for InstallWithDir()"
    base = env.arg2nodes(base, env.fs.Dir)[0]
    for src in env.arg2nodes(source, env.fs.Entry):
        builds.extend(apply(env.Install, (target[0].Dir(src.dir.get_path(base)), src), kw))
    return builds

def generate(env):
    env['BUILDERS']['InstallSubdir'] = InstallSubdir

def exists(env):
    return 1
