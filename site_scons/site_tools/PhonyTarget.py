
def PhonyTarget(env, target, source = None, action = None):
    return env.AlwaysBuild(env.Alias(target, source, env.Action(action)))

def generate(env):
    env['BUILDERS']['PhonyTarget'] = PhonyTarget

def exists(env):
    return True
