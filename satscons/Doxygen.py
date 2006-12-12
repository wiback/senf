import SCons.Script.SConscript
import SCons.Builder
import SCons.Defaults
import os.path

def replace_ext(n,ext):
    base,ext = os.path.splitext(n)
    return base+ext

def Doxygen(env, target, source, image=[]):
    global __file__
    path, name = os.path.split(str(target))
    stamp = os.path.join(path, '.'+name+'.stamp')
    dir = env.Dir(target)
    mak = os.path.join(os.path.split(__file__)[0],'imgconvert.mak')
    env.Depends(dir,
                [ env.Command(os.path.splitext(img)[0]+".png", img,
                              [ 'TERM=dumb make -f %s $TARGET' % mak ])
                  for img in image ] +
                [ env.Command(stamp, source + [ os.path.join(path,"Doxyfile") ],
                              [ 'cd $TARGET.dir && $DOXYGENCOM',
                                "touch $TARGET" ],
                              source_scanner = SCons.Defaults.ObjSourceScan) ])
    env.Clean(stamp, dir)
    return dir

def generate(env):
    env['DOXYGENCOM'] = 'doxygen'
    env.__class__.Doxygen = Doxygen

def exists(env):
    return 1
