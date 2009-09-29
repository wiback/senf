
def PhonyTarget(env, target, source = None, action = None, **kw):
    return env.AlwaysBuild(env.Alias(target, source, env.Action(action), **kw))

def generate(env):
    env['BUILDERS']['PhonyTarget'] = PhonyTarget

def exists(env):
    return True
