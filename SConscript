# -*- python -*-

Import('env')

env.Replace( ALLOBJECTS = [], PACKET_BUNDLES = [] )
env.SetDefault( ALLBUNDLES = '$LOCALLIBDIR/AllBundles${OBJADDSUFFIX}${OBJSUFFIX}' )

SConscript('senf/SConscript')

#### libsenf.a
libsenf = env.Library("$LOCALLIBDIR/${LIBSENF}${LIBADDSUFFIX}", env['ALLOBJECTS'])
env.Default(libsenf)
env.Install('$LIBINSTALLDIR', libsenf)

env.SetDefault(
    __NEED_BOOST_EXT_OPT = "-I$$SENFINCDIR/boost_ext",
    _NEED_BOOST_EXT_OPT = "${NEED_BOOST_EXT and __NEED_BOOST_EXT_OPT or ''}")

conf = env.CreateFile("${LOCALLIBDIR}/${LIBSENF}${LIBADDSUFFIX}.conf", 
                      env.Value(env.subst('$_NEED_BOOST_EXT_OPT $_CPPDEFFLAGS')))
env.Default(conf)
env.Install('$CONFINSTALLDIR', conf)

### AllBundles:
cobject = env.CombinedObject('${LOCALLIBDIR}/${NAME}${OBJADDSUFFIX}', env['PACKET_BUNDLES'],
                              NAME="AllBundles")
env.Default(cobject)
env.Install('${OBJINSTALLDIR}', cobject)
        
