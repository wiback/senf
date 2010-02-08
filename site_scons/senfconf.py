from __future__ import with_statement

_configTests = {}

def Tests():
    global _configTests
    return _configTests

def Test(func):
    global _configTests
    _configTests[func.__name__] = func

class TemporaryContext:
    def __init__(self, context):
        self._context = context
        self._env = self._context.env.Clone()

    def __enter__(self):
        return None

    def __exit__(self,*args):
        self._context.env.Replace(**self._env.Dictionary())
        return False

@Test
def CheckBoostVersion(context):
    context.Message( "Checking boost version... " )
    ret = context.TryRun("#include <boost/version.hpp>\n"
                         "#include <iostream>\n"
                         "int main(int, char **) { std::cout << BOOST_LIB_VERSION << std::endl; }",
                         ".cc")[1].strip()
    if not ret:
        context.Result("no boost includes found")
        return None
    else:
        context.Result(ret)
        return ret

@Test
def CheckBoostVariants(context, *variants):
    useVariant = None
    with TemporaryContext(context):
        for variant in variants:
            if variant : variantStr = "'%s'" % variant
            else       : variantStr = "default"
            context.Message( "Checking boost %s variant... " % variantStr )
            context.env.Replace( BOOST_VARIANT=variant )
            context.env.Append( _LIBFLAGS = ' -Wl,-Bstatic -l$BOOSTTESTLIB  -Wl,-Bdynamic' )
            ret = context.TryLink("#define BOOST_AUTO_TEST_MAIN\n"
                                  "#include <boost/test/auto_unit_test.hpp>\n"
                                  "#include <boost/test/test_tools.hpp>\n"
                                  "BOOST_AUTO_TEST_CASE(test) { BOOST_CHECK(true); }\n",
                                  ".cc")
            context.Result( ret )
            if ret and useVariant is None:
                useVariant = variant
    if useVariant is not None and not context.env.GetOption('no_progress'):
        print  "Using %s boost variant." % (
            useVariant and "'%s'" % useVariant or "default")
    return useVariant
