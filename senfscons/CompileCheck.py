import os, os.path, sys
import tempfile
from SCons.Script import *
import SCons.Scanner.C

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
    Action('$CXXCOM').execute(target, source, cenv)
    passedTests = {}
    delay_name = None
    out.seek(0)
    for error in out.read().splitlines():
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
    file(target[0].abspath,"w").close()
    return 0

CompileCheck = Action(CompileCheck)

def generate(env):

    builder = env.Builder(
        action = CompileCheck,
        suffix = '.checked',
        src_suffix = '.cc',
        source_scanner = SCons.Scanner.C.CScanner(),
        single_source=1
    )

    env.Append(BUILDERS = { 'CompileCheck': builder })

def exists(env):
    return True
