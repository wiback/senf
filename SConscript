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
    _BOOST_EXT_INC_OPT = "${NEED_BOOST_EXT and '-I$$SENFINCDIR/boost_ext' or ''}")

conf = env.CreateFile("${LOCALLIBDIR}/${LIBSENF}${LIBADDSUFFIX}.conf",
                      env.Value(env.subst('$_BOOST_EXT_INC_OPT $_CPPDEFFLAGS')))
env.Default(conf)
env.Install('$CONFINSTALLDIR', conf)

### AllBundles:
cobject = env.CombinedObject('${LOCALLIBDIR}/${NAME}${OBJADDSUFFIX}', env['PACKET_BUNDLES'],
                              NAME="AllBundles")
env.Default(cobject)
env.Install('${OBJINSTALLDIR}', cobject)
