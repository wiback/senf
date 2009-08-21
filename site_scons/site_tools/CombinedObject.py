def generate(env):

    builder = env.Builder(
        action = [ '$LDCOMBINECOM $TARGET $SOURCES' ],
        suffix = '.o',
        ensure_suffix = 1,
        src_suffix = '.o',
        src_builder = 'Object')

    env['BUILDERS']['CombinedObject'] = builder

    env.SetDefault(LDCOMBINECOM = 'ld -r -o')

def exists(env):
    return True
