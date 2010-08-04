import SCons.Script
import SCons.Script.SConscript
import SCons.Defaults
import os.path
import os
import sys
import tempfile
import SCons.Scanner.C

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
        message = message.strip()

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
                      LIBS = env['LIBS'] + [ '$TEST_EXTRA_LIBS' ],
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

def generate(env):
    env.SetDefault(
        BOOST_VARIANT     = '',
        _BOOST_VARIANT    = '${BOOST_VARIANT and "-" or None}$BOOST_VARIANT',

        BOOSTTESTLIB      = 'boost_unit_test_framework$_BOOST_VARIANT',
        BOOSTREGEXLIB     = 'boost_regex$_BOOST_VARIANT',
        BOOSTFSLIB        = 'boost_filesystem$_BOOST_VARIANT',
        BOOSTIOSTREAMSLIB = 'boost_iostreams$_BOOST_VARIANT',
        BOOSTSIGNALSLIB   = 'boost_signals$_BOOST_VARIANT',

        BOOSTTESTARGS     = [ '--build_info=yes', '--log_level=test_suite' ],
    )

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
