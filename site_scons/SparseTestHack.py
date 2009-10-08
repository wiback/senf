import SCons.Node, SCons.Node.FS, SCons.Util

#####################################################################
# This IS a hack .. but a very useful one: The hack will remove the
# 'senf' library from the unit-tests and instead add all the object
# files needed explicitly.
#
# This works by building a list of all children (recursively) of the
# test sources. For each child we check, wether a file with the same
# name but an $OBJSUFFIX extension exists as a build target. In that
# case, we add it to the list of needed objects (AND recursively scan
# that objects children).
#
# Additionally, of the variable 'only_tests' is set, it contains a
# list of test source files to be built exclusively: All other test
# sources will NOT be compiled (and their children will therefore not
# be scanned).
#
# There is one restriction: CompileCheck tests are not run when this
# hack is enabled.
#####################################################################

# Build a list of all object file targets which are needed by 'nodes'
def neededObjects(env,nodes):
    if not SCons.Util.is_List(nodes) : nodes = [nodes]

    def walk(nodes):
        for node in nodes:
            walker = SCons.Node.Walker(node)
            while True:
                n = walker.next()
                if n is None : break
                yield n

    seen = {}
    rv = {}

    def check(nodes, env=env, seen=seen, rv=rv):
        for c in walk(nodes):
            if isinstance(c,SCons.Node.FS.File) and not seen.get(c):
                seen[c] = True
                ob = c.File(env.subst("${SOURCE.dir.abspath}/${OBJPREFIX}${SOURCE.filebase}${OBJSUFFIX}", source=c))
                if ob.is_derived():
                    rv[ob] = True
                    if not seen.get(ob):
                        seen[ob] = True
                        check([ob])

    check(nodes)

    for node in nodes:
        rv[node] = True
    rv = [ (str(x),x) for x in rv.iterkeys() ]
    rv.sort()
    return (x[1] for x in rv)

# Return a map witch has all sources (recursively) of 'nodes' as keys
def recursiveSourcesMap(nodes):
    rv = {}

    def check(node, rv=rv):
        for src in node.sources:
            if not rv.get(src):
                rv[src] = True
                check(src)
    
    for node in nodes:
        check(node)
    
    return rv

# Replacement BoostUnitTest builder. This build just defers the real
# builder call until later
def AutoObjectBoostUnitTest(env, target, tests, **kw):
    target = env.arg2nodes(target)[0]
    tests = env.arg2nodes(tests)
    env.Append(_UNIT_TEST_LIST = (target, tests, kw))

def setup(env):
    env['BUILDERS']['RealBoostUnitTest'] = env['BUILDERS']['BoostUnitTest']
    env['BUILDERS']['BoostUnitTest'] = AutoObjectBoostUnitTest
    env['_UNIT_TEST_LIST'] = []

# This needs to be called after all build targets have been set
# up. This is important since the list of object targets needs to be
# complete.
def build(env):
    env = env.Clone(LIBS = [ '$EXTRA_LIBS' ])
    if env.has_key("only_tests"):
        only_tests = [ env.Dir(env.GetLaunchDir()).File(test)
                       for test in env.Split(env['only_tests']) ]
    else:
        only_tests = []
    for target, tests, kw in env['_UNIT_TEST_LIST']:
        objects = []
        build = False
        for test in tests:
            if test.suffix == env['OBJSUFFIX']:
                objects.append(test)
            else:
                if not only_tests or test in only_tests:
                    objects.extend(env.Object(test))
                    build = True
                elif test.name == 'main.test.cc':
                    objects.extend(env.Object(test))
        if not build : continue

        # First step: Get all objects which are needed by any of 'objects'
        objects = list(neededObjects(env,objects))
        # Get all the source files needed by those objects
        sources = recursiveSourcesMap(objects)
        # Now remove any objects which are also sources: When an
        # object is a source for another object this means, the target
        # object has been built by CombinedObject. Since the target
        # object includes all it's source objects, we need to remove
        # all those source objects from the object list (if any).
        objects = [ ob for ob in objects if not sources.get(ob) ]

        env.RealBoostUnitTest(target, objects, **kw)
