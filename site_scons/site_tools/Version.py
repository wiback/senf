import re, subprocess, os 
import SCons.Builder 
     
def createFile(target, source, env):
    file(str(target[0]), 'w').write(source[0].get_contents()+"\n") 
     
def _getOutput(cmd, pattern):
    FNULL = open('/dev/null', 'w') 
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=FNULL) 
    o = re.search(pattern, p.communicate()[0].strip().lower())
    if o: o = o.groups()[0] 
    return (p.wait(), o) 
     

def CalculateRevision(env, local_revision_file=None):
    rev = None
    if local_revision_file is not None:
        try:
            m = re.search( '.*last changed rev: (\d+).*', file(local_revision_file).read().strip().lower())
            if m: 
                rev = m.groups()[0]
            else:
                rev = int( file(local_revision_file).read(), 16)
        except:
            pass
    if rev is None:
        r, rev = _getOutput( 'env LC_ALL=C svn info', '.*last changed rev: (\d+).*')
        if r != 0:
            r, rev = _getOutput( 'env LC_ALL=C git svn info', '.*last changed rev: (\d+).*')
            if r != 0:
                r, rev = _getOutput( 'git rev-parse --short HEAD', '(\d+)')
                if r != 0: rev = 0
                else:      rev =  int(rev, 16)
    return int(rev) 

     
builder = SCons.Builder.Builder(action = createFile) 

def createVersionFile(env, source, target, name='name_missing', directory='', major_version=0, minor_version=0, revision=None, local_revision_file=None, **kw):
    if source is None:
        source = (
           '// auto-generated file.\n\n' 
           '#ifndef HH_%(name)s_Version\n' 
           '#define HH_%(name)s_Version\n\n' 
           '// %(name)s_VERSION %% 100000 is the revision number\n' 
           '// %(name)s_VERSION / 100000 %% 100 is the minor version\n' 
           '// %(name)s_VERSION / 10000000 is the major version\n' 
           '#define %(name)s_VERSION %(major_version)d%(minor_version)02d%(revision)05d\n\n' 
           '#define %(name)s_LIB_VERSION "%(major_version)d.%(minor_version)02d"\n\n' 
           '#define %(name)s_REVISION "%(revision)d"\n\n' 
           '#endif\n')
    else:
        source,target = target[0],source

    if target is None or len(target) != 1:
        raise SCons.Errors.UserError, "Need exactly one target for CreateVersionFile()"
    
    if revision is None:
        curdir = os.curdir
        if directory: os.chdir(directory)  
        revision = CalculateRevision(env, local_revision_file)
        if directory: os.chdir(curdir)

    source = env.arg2nodes(env.Value(source % dict(
         name=name, major_version=major_version, minor_version=minor_version, revision=revision, **kw)), env.fs.Entry)
    
    env.Replace( **{name+'_VERSION' : int('%(major_version)d%(minor_version)02d%(revision)05d' % locals()) })
    env.Replace( **{name+'_REVISION' : revision })
    
    return builder(env, target, source)
        
    
def generate(env):
    env['BUILDERS']['CreateVersionFile'] = createVersionFile
    env.AddMethod(CalculateRevision)
     
def exists(env):
    return True 
