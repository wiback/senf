import CustomTests
import SCons.Defaults
import SCons.Scanner.C
import SCons.Script
import SCons.Script.SConscript
import os
import os.path
import tempfile

# ARGH ... Why do they put a '+' in the module name ????????
SCons.Tool.cplusplus=getattr(__import__('SCons.Tool.c++', globals(), locals(), []).Tool, 'c++')

_ALL_TESTS = []

def scanTests(f):
    tests = {}
    name = start = None
    linenr= 0
    for line in f:
        linenr += 1
        if line.startswith('COMPILE_FAIL(') and ')' in line:
            name = line.split('(',1)[-1].split(')',1)[0]
            start = linenr
        elif line.startswith('}') and name and start:
            tests[name] = (start, linenr)
    return tests

def CompileCheck(target, source, env):
    tests = scanTests(file(source[0].abspath))
    cenv = env.Clone()
    cenv.Append( CPPDEFINES = [ 'COMPILE_CHECK' ] )
    out = tempfile.TemporaryFile()
    cenv['SPAWN'] = lambda sh, escape, cmd, args, env, pspawn=cenv['PSPAWN'], out=out: \
                    pspawn(sh, escape, cmd, args, env, out, out)
    SCons.Script.Action('$CXXCOM').execute(target, source, cenv)
    passedTests = {}
    delay_name = None
    out.seek(0)
    result = out.read();
    for error in result.splitlines():
        elts = error.split(':',2)
        if len(elts) != 3 : continue
        filename, line, message = elts
        if not os.path.exists(filename) : continue
        try: line = int(line)
        except ValueError : continue
        # strip column number if given
        message = message.split(':', 1)[-1].strip()

        if delay_name and not message.startswith('instantiated from '):
            print "Passed test '%s': %s" % (delay_name, message)
            delay_name = None
            continue

        filename = os.path.abspath(filename)
        if filename != source[0].abspath : continue

        for name,lines in tests.iteritems():
            if line >= lines[0] and line <= lines[1]:
                passedTests[name] = 1
                if message.startswith('instantiated from '):
                    delay_name = name
                else:
                    print "Passed test '%s': %s" % (name, message)
    if delay_name:
        print "Passed test '%s': <unknown message ??>" % delay_name
    failedTests = set(tests.iterkeys()) - set(passedTests.iterkeys())
    if failedTests:
        for test in failedTests:
            print "Test '%s' FAILED" % test
        print
        print "*** %d tests FAILED" % len(failedTests)
        if os.path.exists(target[0].abspath):
            os.unlink(target[0].abspath)
        return 1
    file(target[0].abspath,"w").write(result)
    return 0

CompileCheck = SCons.Script.Action(CompileCheck)

def BoostUnitTest(env, target=None, source=None,  **kw):
    global _ALL_TESTS

    target = env.arg2nodes(target)[0]
    source = env.arg2nodes(source)

    binnode = target.dir.File('.' + target.name + '.bin')
    stampnode = target.dir.File('.' + target.name + '.stamp')

    bin = env.Program(binnode, source,
                      LIBS = env['LIBS'] + kw.pop('TEST_EXTRA_LIBS', ['$TEST_EXTRA_LIBS']),
                      _LIBFLAGS = ' -Wl,-Bstatic -l$BOOSTTESTLIB -Wl,-Bdynamic ' + env['_LIBFLAGS'],
                      **kw)

    stamp = env.Command(stampnode, bin,
                        [ './$SOURCE $BOOSTTESTARGS', SCons.Script.Touch('$TARGET')],
                        **kw)

    alias = env.Command(env.File(target), stamp, [ env.NopAction() ] )

    compileTests = [ src for src in source
                     if src.suffix in SCons.Tool.cplusplus.CXXSuffixes \
                         and src.exists() \
                         and 'COMPILE_CHECK' in file(str(src)).read() ]
    if compileTests:
        env.Depends(alias, env.CompileCheck(source = compileTests))

    _ALL_TESTS.append(alias)

    return alias

def FindAllBoostUnitTests(env, target, source):
    global _ALL_TESTS
    return _ALL_TESTS

def NopAction(env, target, source):
    def nop(target, source, env) : return None
    def nopstr(target, source, env) : return ''
    return env.Action(nop, nopstr)

ConfTest = CustomTests.ConfTest()

@ConfTest
def CheckBoostInstallation(context, min=None, max=None):
    BOOST_LIBS = (
            ('BOOSTTESTLIB',     'boost_unit_test_framework'),
            ('BOOSTREGEXLIB',    'boost_regex'),
            ('BOOSTFSLIB',       'boost_filesystem'),
            ('BOOSTSIGNALSLIB',  'boost_signals'),
            ('BOOSTDATETIMELIB', 'boost_date_time'),
            ('BOOSTSYSTEMLIB',   'boost_system') )

    # first check the default
    r = checkBoostVersion(context, min, max)
    if r:
        for envVar, boostlib in BOOST_LIBS:
            if not checkBoostLib(context, envVar, boostlib):
                return False
        return r
    # check for other boost installations
    dirs = [d for d in glob.glob('/usr/local/include/boost-*') if os.path.isdir(d)] + \
           [d for d in glob.glob('/usr/include/boost-*') if os.path.isdir(d)]
    for d in dirs:
        r = checkBoostVersion(context, min, max, d)
        if r:
            # we have found the boost headers in a non-standard directory
            # now search the specific libraries
            for envVar, boostlib in BOOST_LIBS:
                if not checkBoostLib(context, envVar, boostlib, context.env['BOOST_VERSION']):
                    context.env.Fail('No valid boost installation found')
            return r
    context.env.Fail('No valid boost installation found')


def checkBoostLib(context, envVar, libName, version=None):
    if version is None:
        if context.sconf.CheckLib(libName, language='CXX', autoadd=False):
            context.env.SetDefault( **{ envVar : libName })
            return True
    lib = '%s-%s' % (libName, version)
    if context.sconf.CheckLib(lib, language='CXX', autoadd=False):
        context.env.SetDefault( **{ envVar : lib })
        return True
    return False


def checkBoostVersion(context, min=None, max=None, includeDir=None):
    """Check for boost includes.

    Will place the boost version number (BOOST_LIB_VERSION) into the
    BOOST_VERSION environment variable.

    Options:
        min/max   compare boost version against given range.
        fail      if fail is set to True, the build will be terminated,
                  when no valid boost includes are found."""
    if min and max:
        msg = ' in range %s to %s' % (min,max)
    elif min:
        msg = ' at least %s' % min
    elif max:
        msg = ' at most %s' % max
    else:
        msg = ''
    if includeDir:
        msg += ' (%s)' % includeDir
    context.Message( "Checking boost version%s... " % msg )
    if context.env.has_key('BOOST_VERSION') and context.env['BOOST_VERSION']:
        ret = context.env['BOOST_VERSION']
    else:
        cppPath = context.env['CPPPATH']
        context.env.Replace( CPPPATH = cppPath + [includeDir])
        ret = context.TryRun("#include <boost/version.hpp>\n"
                             "#include <iostream>\n"
                             "int main(int, char **)\n"
                             "{ std::cout << BOOST_LIB_VERSION << std::endl; }",
                             ".cc")[-1].strip()
        context.env.Replace( CPPPATH = cppPath)

    if not ret:
        msg = "no boost includes found"
    else:
        context.env.Replace( BOOST_VERSION = ret )
        msg = ret
        if min or max:
            try: version = map(int, ret.split('_'))
            except ValueError:
                msg = "[%s] invalid version syntax" % ret
                ret = None
            else:
                if min : ret = ret and (version>=map(int,min.split('_')))
                if max : ret = ret and (version<=map(int,max.split('_')))
                msg = '[%s] %s' % (msg, ret and "yes" or "no")
    context.Result(msg)
    if ret:
        if includeDir:
            context.env.Replace( BOOST_INCLUDE_PATH = includeDir)
            context.env.Append( CPPPATH = [includeDir])
            context.env.Append( CXXFLAGS = ['-isystem%s' % includeDir])
    else:
        context.env.Replace( BOOST_VERSION = None )
    return ret


def generate(env):
    env.SetDefault(
        BOOSTTESTARGS     = [ '--build_info=yes', '--log_level=test_suite' ] )
    env.Append(
        CUSTOM_TESTS      = ConfTest.tests )

    env['BUILDERS']['BoostUnitTest'] = BoostUnitTest
    env['BUILDERS']['FindAllBoostUnitTests'] = FindAllBoostUnitTests
    env['BUILDERS']['CompileCheck'] = env.Builder(
        action = CompileCheck,
        suffix = '.checked',
        src_suffix = '.cc',
        source_scanner = SCons.Scanner.C.CScanner(),
        single_source=1
        )
    env['BUILDERS']['NopAction'] = NopAction

def exists(env):
    return True
