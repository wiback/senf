import SCons.Defaults

import md5, os.path
import SCons.Action, SCons.Builder, SCons.Util, SCons.Errors, SCons.Environment

PDFLaTeXAction = SCons.Action.CommandAction("$PDFLATEXCOM")

# copied from Action.py ... changed to tage explicit path-names as
# target/source instead of node objects
def my_executeCommand(cmd_list, target, source, env):
    import SCons.Util 
    escape = env.get('ESCAPE', lambda x: x)
    if env.has_key('SHELL'): shell = env['SHELL']
    else: raise SCons.Errors.UserError('Missing SHELL construction variable.')
    if env.has_key('PIPE_BUILD'):
        pipe_build = 1
        if env.has_key('PSPAWN'): pspawn = env['PSPAWN']
        else: raise SCons.Errors.UserError('Missing PSPAWN construction variable.')
        if env.has_key('PSTDOUT'): pstdout = env['PSTDOUT']
        else: raise SCons.Errors.UserError('Missing PSTDOUT construction variable.')
        if env.has_key('PSTDERR'): pstderr = env['PSTDERR']
        else: raise SCons.Errors.UserError('Missing PSTDOUT construction variable.')
    else:
        pipe_build = 0
        if env.has_key('SPAWN'): spawn = env['SPAWN']
        else: raise SCons.Errors.UserError('Missing SPAWN construction variable.')
    # Here, replaced the source and target argumnets with a dict argument
    cmd_list = env.subst_list(cmd_list, 0, dict = { 'TARGET': target, 'TARGETS': [ target ],
                                                    'SOURCE': source, 'SOURCES': [ source ] })
    for cmd_line in cmd_list:
        if len(cmd_line):
            try: ENV = env['ENV']
            except KeyError:
                global default_ENV
                if not default_ENV:
                    import SCons.Environment
                    default_ENV = SCons.Environment.Environment()['ENV']
                ENV = default_ENV
            for key, value in ENV.items():
                if SCons.Util.is_List(value):
                    value = SCons.Util.flatten(value)
                    ENV[key] = string.join(map(str, value), os.pathsep)
                elif not SCons.Util.is_String(value):
                    ENV[key] = str(value)
            cmd_line = SCons.Util.escape_list(cmd_line, escape)
            if pipe_build: ret = pspawn( shell, escape, cmd_line[0], cmd_line,
                                         ENV, pstdout, pstderr )
            else: ret = spawn(shell, escape, cmd_line[0], cmd_line, ENV)
            if ret: return ret
    return 0

def TeXchecksum(files):
    m = md5.new()
    for f in files:
        try:
            m.update(file(f,"rb").read())
        except IOError: pass
    return m.digest()

def TeXPdfAction(target, source, env):
    src = os.path.abspath(str(source[0]))
    path, pdf = os.path.split(str(target[0]))
    base, ext = os.path.splitext(pdf)
    cwd = os.getcwd()
    if path: os.chdir(path)
    checkfiles = [ base + ext for ext in env["TEXCHECKEXT"] ]
    checksum = TeXchecksum(checkfiles)
    rv = 0
    for i in range(env["TEXMAXRECURSE"]+1):
        if i==env["TEXMAXRECURSE"]:
            print "\nWARNING: TeX recursion depth exceeded. They generated file may not be final.\n"
            break
        rv = my_executeCommand("$PDFLATEXCOM", pdf, src, env)
        if rv: break
        new_checksum = TeXchecksum(checkfiles)
        if new_checksum == checksum: break
        checksum = new_checksum
    os.chdir(cwd)
    return rv

class TeXPdfBuilder(SCons.Builder.BuilderBase) :
    def __call__(self, env, target = None, source = SCons.Builder._null, **kw):
        tlist = SCons.Builder.BuilderBase.__call__(self, env, target, source, **kw)
        exts = env["TEXCHECKEXT"] + env["TEXCLEANEXT"]
        for t in tlist:
            base, ext = os.path.splitext(str(t))
            for ext in exts:
                env.Clean(t,base+ext)
        return tlist


def generate(env):
    env['TEXCHECKEXT'] = [ '.aux', '.toc' ]
    env['TEXCLEANEXT'] = [ '.log', '.dvi' ]
    env['TEXMAXRECURSE'] = 5
    env['BUILDERS']['PDF'] = TeXPdfBuilder(action = TeXPdfAction, suffix='.pdf')

def exists(env):
    return 1
