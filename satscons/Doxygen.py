import SCons.Script.SConscript
import SCons.Builder
import SCons.Defaults
import os.path

def replace_ext(n,ext):
    base,ext = os.path.splitext(n)
    return base+ext

def Doxygen(env, target, source, image):
    path, name = os.path.split(str(target))
    stamp = os.path.join(path, '.'+name+'.stamp')
    alias = env.Alias(target,
                      [ env.Command(os.path.splitext(img)[0]+".png", img,
                                    [ 'TERM=dumb make -f imgconvert.mak $TARGET' ])
                        for img in image ] +
                      [ env.Command(stamp, source,
                                    [ '$DOXYGENCOM',
                                      'cd doc/html && (sed -ne \'1,/<table>/p\' <annotated.html && grep -F \'<tr>\' <annotated.html | sort -ft\'>\' -k4 && sed -ne \'/<\\/table>/,$$p\' <annotated.html) >annotated.html.new && mv annotated.html.new annotated.html',
                                      'touch $TARGET' ],
                                    source_scanner = SCons.Defaults.ObjSourceScan) ])
    env.Clean(stamp, target)
    return alias

def generate(env):
    env['DOXYGENCOM'] = 'doxygen'
    env.__class__.Doxygen = Doxygen

def exists(env):
    return 1
