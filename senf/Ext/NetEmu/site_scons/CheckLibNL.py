#!/usr/bin/python
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


def CheckLibNLVersion(context,fail=True, min=None, max=None):
    for lp in ( ('nl','nl-genl'), ('nl-3','nl-genl-3') ):
        libnl_libs = []
        for l in lp:
            if context.sconf.CheckLib( l, language='C', autoadd=False):
                libnl_libs.append( l)
            else:
                break
        else:
            break
    else:
        context.env.Fail( 'no netlink library (libnl) found')
        return None 
    context.env.Replace( LIBNL_LIBS = libnl_libs)
   
    cppPath = context.env['CPPPATH']
    for inc in ('', '/usr/include/libnl3'):
        if inc: 
            context.env.Replace( CPPPATH = cppPath + [inc])
            context.Display('trying %s ...\n' % inc)   
        if context.sconf.CheckHeader( 'netlink/netlink.h', language='C'):
            break
    else:
        context.env.Fail( 'no netlink header found')
        return None
    context.env.Replace( LIBNL_CPPPATH = inc)   
    
    if min and max:
        vermsg = ' in range %s to %s' % (min,max)
    elif min:
        vermsg = ' at least %s' % min
    elif max:
        vermsg = ' at most %s' % max
    else:
        vermsg = ''
        
    if not context.sconf.CheckHeader('netlink/version.h', language='C'):
        context.env.Replace( LIBNL_VERSION = '1.0' )
        ret = '1.0'
        msg = 'Assuming libnl version 1.0'
        context.Message( 'Checking libnl version%s. ' % vermsg)  

    else:
        context.Message( "Checking libnl version%s... " % vermsg )
        ret = context.TryRun("""
                #include <netlink/version.h>
                #include <iostream>
                int main(int, char **) {
                std::cout << LIBNL_VERSION << std::endl; }""", ".cc")[-1].strip()
        if not ret:
            msg = "failed"
            context.env.Replace( LIBNL_VERSION = None )
        else:
            context.env.Replace( LIBNL_VERSION = ret )
            msg = ret

    context.env.Replace( CPPPATH = cppPath)
    
    if min or max:
        try: version = map(int,ret.split('.'))
        except ValueError:
            msg = "[%s] invalid version syntax" % ret
            ret = None
        else:
            if min : ret = ret and (version>=map(int,min.split('.')))
            if max : ret = ret and (version<=map(int,max.split('.')))
            msg = '[%s] %s' % (msg, ret and "ok" or "not ok")
    
    context.Result(msg)
    if not ret: 
        context.env.Fail( 'No valid libnl version found. ')
    return ret

###########################################################################

