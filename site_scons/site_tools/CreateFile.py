import SCons.Builder

def create(target, source, env): 
    file(str(target[0]), 'w').write(source[0].get_contents()+"\n")

def generate(env):
    env['BUILDERS']['CreateFile'] = SCons.Builder.Builder(action = create)

def exists(env):
    return True
