import subprocess, os.path
import SCons.Script
import SCons.Script.SConscript
import SCons.Defaults
import CustomTests

ConfTest = CustomTests.ConfTest()

@ConfTest
def CheckCXXVersion(context, fail=False, min=None, max=None):
    """Check for compiler version."""

    if min and max:
        msg = ' in range %s to %s' % (min,max)
    elif min:
        msg = ' at least %s' % min
    elif max:
        msg = ' at most %s' % max
    else:
        msg = ''
    context.Message( "Checking for %s version%s ... " % (context.env['CXX'], msg))

    version = None
    if os.path.basename(context.env['CXX']).startswith('g++'):
        proc = subprocess.Popen((context.env['CXX'], '-dumpversion'), stdout=subprocess.PIPE)
        version = proc.communicate()[0][:-1]
        if proc.returncode < 0: 
            version = None
    if os.path.basename(context.env['CXX']).startswith('clang++'):
        proc = subprocess.Popen((context.env['CXX'], '-v'), stderr=subprocess.PIPE)
        try:
            version = proc.communicate()[1].split()[2]
            if proc.returncode < 0: 
                version = None
        except IndexError: pass
                    
    if not version:
        msg = 'no'
        ret = False
    else:
        msg = version
        ret = True
        if min or max:
            try: vmap = map(int, version.split('.'))
            except ValueError:
                msg = "[%s] invalid version syntax" % version
                ret = False
            else:
                if min : ret = ret and (vmap >= map(int,min.split('.')))
                if max : ret = ret and (vmap <= map(int,max.split('.')))
                msg = '[%s] %s' % (msg, ret and "yes" or "no")
    context.Result(msg)
    if fail and not ret:
         context.env.Fail('No supported compiler found.\n'
                          'You can use CXX=... set the c++ compiler to use.')
    return ret


def generate(env):
    env.Append(
        CUSTOM_TESTS = ConfTest.tests,
    )

def exists(env):
    return True
