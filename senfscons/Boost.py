# Provide environment variables for boost libraries

def generate(env):
    env.SetDefault(
        BOOST_VARIANT     = '',

        BOOSTTESTLIB      = 'boost_unit_test_framework$BOOST_VARIANT',
        BOOSTREGEXLIB     = 'boost_regex$BOOST_VARIANT',
        BOOSTFSLIB        = 'boost_filesystem$BOOST_VARIANT',
        BOOSTIOSTREAMSLIB = 'boost_iostreams$BOOST_VARIANT',
        BOOSTSIGNALSLIB   = 'boost_signals$BOOST_VARIANT',
    )

def exists(env):
    return True
