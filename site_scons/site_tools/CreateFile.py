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


import SCons.Builder

def create(target, source, env): 
    file(str(target[0]), 'w').write(source[0].get_contents()+"\n")

def generate(env):
    env['BUILDERS']['CreateFile'] = SCons.Builder.Builder(action = create)

def exists(env):
    return True
