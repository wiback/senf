#
# Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
#                    Network Research Group (NET)
#                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
#                    Contact: http://wiback.org
#
# This file is part of the SENF code tree.
# It is licensed under the 3-clause BSD License (aka New BSD License).
# See LICENSE.txt in the top level directory for details or visit
# https://opensource.org/licenses/BSD-3-Clause
#


from CheckLibNL import CheckLibNLVersion

def SetupForNetEmu(env):
    if env.GetOption('clean'):
        return
    conf = env.Configure(clean=False, help=False)
    conf.AddTest('CheckLibNLVersion', CheckLibNLVersion)
    conf.CheckLibNLVersion(min='2.0')
    conf.Finish()
    
    env.Append( LIBS = env['LIBNL_LIBS'])
    env.Append( CPPPATH = env['LIBNL_CPPPATH'])
    
