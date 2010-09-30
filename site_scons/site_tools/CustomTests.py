import SCons.Environment, SCons.Util, SCons.Script, SCons.Conftest
import re, os.path

# Fix for SCons 0.97 compatibility
import SCons.SConf
try: SCons.SConf.SConfBase.Define
except AttributeError:
    import string
    def Define(self, name, value = None, comment = None):
        lines = []
        if comment:
            comment_str = "/* %s */" % comment
            lines.append(comment_str)
        if value is not None:
            define_str = "#define %s %s" % (name, value)
        else:
            define_str = "#define %s" % name
        lines.append(define_str)
        lines.append('')
        self.config_h_text = self.config_h_text + string.join(lines, '\n')
    SCons.SConf.SConfBase.Define = Define

class ConfTest(object):
    """Configuration test decorator.

This class is used as a decorator to define SCons Configure
tests. There are three ways to use this facility:


DIRECT WITH EXTERNAL REGISTRY

In this scenario, you pass the test registry, a map, as argument to
the decorator. This usage is primarily interesting for tool writers:

    from CustomTests import ConfTest

    MY_TESTS = {}

    @ConfTest(MY_TESTS)
    def CheckMyFoo(context):
        pass

    def generate(env):
        env.Append(CUSTOM_TESTS = MY_TESTS)

This will place CheckMyFoo into the MY_TESTS dictionary which you can
later add to the CUSTOM_TESTS environment variable.


USING A REGISTRY ALIAS

In this scenario, you define a local alias for the decorator which
serves as the registry. This usage is again primarily interesting for
tool and extension writers:

    import CustomTests

    MyConfTest = CustomTests.ConfTest()

    @MyConfTest
    def CheckMyFoo(context):
        pass

    def generate(env):
        env.Append(CUSTOM_TESTS = MyConfTest.tests)

Logically this is like the previous method.


USING THE ENVIRONMENT MEMBER

This usage is interesting for SConstruct and SConscript writers:

    env = Environment()
    env.Tool('CustomTests')

    @env.ConfTest()
    def CheckMyFoo(context):
        pass

    conf = env.Configure()
    conf.CheckMyFoo()
    env = conf.Finish()

The new configuration test is automatically added to
env['CUSTOM_TESTS'] and is thus automatically available to all
configuration contexts created from the environment.
"""

    def __init__(self, registry=None):
        if registry is None:
            self.tests = {}
        elif isinstance(registry, SCons.Environment.SubstitutionEnvironment):
            self.tests =registry['CUSTOM_TESTS']
        else:
            self.tests = registry

    def __call__(self, func):
        self.tests[func.__name__] = func
        return func

def Configure(self, *args, **kw):
    try: kw['custom_tests'].update(self['CUSTOM_TESTS'])
    except KeyError: kw['custom_tests'] = dict(self['CUSTOM_TESTS'])
    return self._CustomTests_orig_Configure(*args, **kw)

def Fail(context, message, condition=True):
    if condition:
        SCons.Util.display("scons: *** %s" % message)
        SCons.Script.Exit(1)

DefaultTest = ConfTest()

@DefaultTest
def CheckSymbolWithExpression(context, symbol, expression, header="", language="C"):
    lang, suffix, msg = SCons.Conftest._lang2suffix(language)
    if msg:
        context.Message("Cannot check for %s: " % symbol)
        context.Result(msg)
        return False

    text = ("#include <assert.h>\n"
            "%s\n"
            "int main() {\n"
            "%s;\n"
            "return 0;\n"
            "}\n") % (header, expression)

    context.Message("Checking for %s... " % symbol)
    ret = context.TryLink(text, suffix)
    context.Result(ret)
    if ret:
        key = symbol.upper()
        key = re.sub('[^A-Z0-9_]', '_', key)
        context.sconf.Define("HAVE_%s" % key, 1,
                             "Define to 1 if you have `%s'" % symbol)

    return ret

@DefaultTest
def CheckByteorder(context):
    context.Message("Checking byteorder... ")
    if context.env.has_key("BYTEORDER"):
        ret = context.env["BYTEORDER"]
    else:
        ret = context.TryRun('#include <stdio.h>\n'
                             'union byteorder_test { int i; char b; };\n'
                             'int main() {\n'
                             '    union byteorder_test t; t.i=1;\n'
                             '    printf(t.b ? "little\\n" : "big\\n");\n'
                             '    return 0;\n'
                             '}\n',
                             ".c")[-1].strip()
    if not ret:
        context.Result("failed")
        return False
    else:
        context.Result(ret+"-endian")
        context.sconf.Define("BYTEORDER_%s_ENDIAN" % ret.upper(), 1,
                             "Define BYTEORDER_LITTLE_ENDIAN or BYTEORDER_BIG_ENDIAN")
        return ret

@DefaultTest
def FindCHeader(context, name, dirs):
    defn = name.upper()
    defn = re.sub('[^A-Z0-9_]', '_', defn)
    defn += "_PATH"

    context.Message("Checking for %s... " % name)
    for dir in dirs:
        path = os.path.join(dir, name);
        ret = context.TryCompile("#include <%s>" % path, ".c");
        if ret:
            context.Result(path)
            context.sconf.Define(defn, "<%s>" % path,
                                 "Define %s as <path/to/%s>" % (defn, name))
            return ret
    return False

def generate(env):
    env.Append( CUSTOM_TESTS = DefaultTest.tests )
    env._CustomTests_orig_Configure = env.Configure
    env.AddMethod(Configure)
    env.AddMethod(ConfTest)
    env.AddMethod(Fail)

def exists(env):
    return True
