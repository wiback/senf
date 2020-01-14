#
# Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
#                    Network Research Group (NET)
#                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
#                    Contact: support@wiback.org
#
# This file is part of the SENF code tree.
# It is licensed under the 3-clause BSD License (aka New BSD License).
# See LICENSE.txt in the top level directory for details or visit
# https://opensource.org/licenses/BSD-3-Clause
#


Import('env')

env.Replace( ALLOBJECTS = [], PACKET_BUNDLES = [] )
env.SetDefault( ALLBUNDLES = '$LOCALLIBDIR/AllBundles${OBJADDSUFFIX}${OBJSUFFIX}' )

SConscript('senf/SConscript')

#### libsenf.a
libsenf = env.Library("$LOCALLIBDIR/${LIBSENF}${LIBADDSUFFIX}", env['ALLOBJECTS'])
env.Default(libsenf)
env.Install('$LIBINSTALLDIR', libsenf)

conf = env.CreateFile("${LOCALLIBDIR}/${LIBSENF}${LIBADDSUFFIX}.conf", env.Value( ' '.join( [
            env.subst('$_CPPDEFFLAGS'), env.subst('${cxx11 and "-std=c++11" or None}') ] +
            [ '-I$SENFINCDIR/boost/%s' % lib for lib in env.get('BOOST_EXT', []) ])))
env.Default(conf)
env.Install('$CONFINSTALLDIR', conf)

### AllBundles:
cobject = env.CombinedObject('${LOCALLIBDIR}/${NAME}${OBJADDSUFFIX}', env['PACKET_BUNDLES'],
                              NAME="AllBundles")
env.Default(cobject)
env.Alias('bundles', cobject)
env.Install('${OBJINSTALLDIR}', cobject)

### checkIncludeGuards.py
checkIncludeGuards = env.PhonyTarget('checkIncludeGuards', [], [ '/usr/bin/python ./tools/checkIncludeGuards.py' ])
env.Alias('all_tests', checkIncludeGuards)
env.Default(checkIncludeGuards)
