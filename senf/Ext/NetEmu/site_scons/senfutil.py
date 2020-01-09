# -*- python -*-
# $Id:$
#
# Copyright (C) 2008
# Fraunhofer Institute for Open Communication Systems (FOKUS)
#
# The contents of this file are subject to the Fraunhofer FOKUS Public License
# Version 1.0 (the "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at 
# http://senf.fit.fraunhofer.de/license.html
#
# The Fraunhofer FOKUS Public License Version 1.0 is based on, 
# but modifies the Mozilla Public License Version 1.1.
# See the full license text for the amendments.
#
# Software distributed under the License is distributed on an "AS IS" basis, 
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
# for the specific language governing rights and limitations under the License.
#
# The Original Code is Fraunhofer FOKUS code.
#
# The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
# (registered association), Hansastrasse 27 c, 80686 Munich, Germany.
# All Rights Reserved.

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
    
