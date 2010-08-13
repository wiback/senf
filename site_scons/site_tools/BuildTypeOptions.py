import SCons.Util, SCons.Defaults
import re

class BuildTypeOptions:
    def __init__(self, var):
        self._var = var

    def __call__(self, target, source, env, for_signature):
        type = env['final'] and "final" or env['debug'] and "debug" or "normal"
        return env[self._var + "_" + type]

_DOLLAR_RE = re.compile(r'\$([a-zA-Z_][\.\w]*)|\${([^}]*)}')

def _expandDefines(defs, target, source, env, for_signature):
    rv = []
    if SCons.Util.is_Dict(defs):
        keys = defs.keys()
        keys.sort()
        defs = [ (k,defs[k]) for k in keys ]
    elif not SCons.Util.is_List(defs):
        defs = [ defs ]
    for elt in defs:
        if SCons.Util.is_String(elt):
            m = _DOLLAR_RE.match(elt)
            if m:
                match = m.group(1) or m.group(2)
                try: rv.extend(_expandDefines(eval(match, env.gvars(), env.lvars()),
                                              target, source, env, for_signature))
                except NameError: pass
                except IndexError: pass
            else:
                rv.append(env.subst(elt))
        elif callable(elt):
            rv.extend(_expandDefines(elt(target, source, env, for_signature),
                                     target, source, env, for_signature))
        elif SCons.Util.is_Sequence(elt):
            if len(elt)<2 or elt[1] is None:
                rv.append(env.subst(elt[0]))
            else:
                rv.append(env.subst(elt[0]) + "=" + env.subst(elt[1]))
        else:
            rv.append(str(elt))
    return rv

def expandDefines(prefix, defs, suffix, env):
    """Expand defines in <defs> using <env>. Calls SCons.Defaults._concat_ixes
to append prefix/suffix before/after each define.

    callable
        Call the callable and replace it with the call result. If the result
        is a list, the list is processed recursively. It the result is a
        dictionary it is converted into a list of typles and processed
        recursively.
    '$<variable>' or '${<variable>}'
        Replace the element with the variable expansion. If the result is a
        list, the list is processed recursively. It the result is a
        dictionary it is converted into a list of typles and processed
        recursively.
    '<any other string>'
        Define a symbol with that (expanded!) name
    iterable
        The iteratble must have two elments. The first element defines the
        symbol name, the second the symbol value."""

    defs = _expandDefines(defs, None, None, env, False)
    return SCons.Defaults._concat_ixes(prefix, defs, suffix, env)
    
def _BuildTypeOptions(env, var) : return BuildTypeOptions(var)

def generate(env):
    env.Replace(
        _defines          = expandDefines,
        )
    
    env.AddMethod(_BuildTypeOptions, 'BuildTypeOptions')

def exists(env):
    return True
