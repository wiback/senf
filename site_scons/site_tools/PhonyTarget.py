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


def PhonyTarget(env, target, source = None, action = None, **kw):
    return env.AlwaysBuild(env.Alias(target, source, env.Action(action), **kw))

def generate(env):
    env['BUILDERS']['PhonyTarget'] = PhonyTarget

def exists(env):
    return True
