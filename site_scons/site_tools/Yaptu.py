import SCons.Action, SCons.Builder, yaptu

def yaptuAction(target, source, env):
    source = env.arg2nodes(source)
    target = env.arg2nodes(target)
    file(target[0].get_path(),"w").write(
        yaptu.process(file(source[0].get_path(),"r").read(), globals(), env.Dictionary()))

YaptuAction = SCons.Action.Action(yaptuAction,
                                  lambda t,s,e=None: 'Yaptu("%s", "%s")' % (t[0],s[0]))
YaptuBuilder = SCons.Builder.Builder(action=YaptuAction, single_source=1)

def generate(env):
    env['BUILDERS']['Yaptu'] = YaptuBuilder

def exists(env):
    return True
