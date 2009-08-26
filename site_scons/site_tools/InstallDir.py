import os.path, os
import SCons.Action, SCons.Builder, SCons.Node.FS

def installDirFilter(target, source, env):
    install = env['INSTALL']
    suffixes = env.get('FILTER_SUFFIXES',[])
    
    for t,s in zip(target,source):
        for path, dirs, files in os.walk(s.get_path()):
            sdir = env.Dir(path)
            tdir = t.Dir(sdir.get_path(s))
            if not os.path.exists(tdir.get_path()):
                os.makedirs(tdir.get_path())
            for f in files:
                sfile = sdir.File(f)
                if suffixes and sfile.suffix not in suffixes : continue
                tfile = tdir.File(sfile.name)
                if install(tfile.get_path(), sfile.get_path(), env):
                    return 1
    return 0

def installDirFilterStr(target, source, env):
    suffixes = env.get('FILTER_SUFFIXES')
    if suffixes : suffixes = " (only %s)" % " ".join(suffixes)
    return 'Install directory: "%s" as "%s"%s' % (str(source[0]), str(target[0]), suffixes)
    
InstallDirAction = SCons.Action.Action(installDirFilter, installDirFilterStr)

InstallDirBuilder = SCons.Builder.Builder(
    action = InstallDirAction,
    target_factory = SCons.Node.FS.Dir,
    source_factory = SCons.Node.FS.Dir,
    emitter = [ SCons.Tool.install.add_targets_to_INSTALLED_FILES, ])

def InstallDirWrapper(env, target=None, source=None, **kw):
    target = env.arg2nodes(target, env.fs.Dir)
    source = env.arg2nodes(source, env.fs.Dir)
    builds = []
    for t in target:
        for s in source:
            builds.extend(InstallDirBuilder(env, t.Dir(s.name), s, **kw))
    return builds

def generate(env):
    env['BUILDERS']['InstallDir'] = InstallDirWrapper

def exists(env):
    return True
