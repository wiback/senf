import SCons.Environment
import SCons.Util, SCons.Script

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

# Hmm .. no default tests for now ...

def generate(env):
    env.Append( CUSTOM_TESTS = DefaultTest.tests )
    env._CustomTests_orig_Configure = env.Configure
    env.AddMethod(Configure)
    env.AddMethod(ConfTest)
    env.AddMethod(Fail)

def exists(env):
    return True
