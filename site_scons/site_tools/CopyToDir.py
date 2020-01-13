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


import os.path
import SCons.Builder, SCons.Defaults

def emitter(source, target, env):
    source = env.arg2nodes(source, env.fs.File)
    target = env.arg2nodes(target, env.fs.Dir)
    return ([ target[0].File(src.name) for src in source ], source)

def generator(source, target, env, for_signature):
    return [ SCons.Defaults.Copy(t, s) for s, t in zip(source, target) ]

CopyToDir = SCons.Builder.Builder(emitter = emitter,
                                  generator = generator)

def generate(env):
    env['BUILDERS']['CopyToDir'] = CopyToDir

def exists(env):
    return 1
