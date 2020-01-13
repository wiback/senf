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


def generate(env):

    builder = env.Builder(
        action = [ '$LDCOMBINECOM $TARGET $SOURCES' ],
        suffix = '.o',
        ensure_suffix = 1,
        src_suffix = '.o',
        src_builder = 'Object')

    env['BUILDERS']['CombinedObject'] = builder

    env.SetDefault(
        LD = 'ld',
        LDCOMBINECOM = '$LD -r -o',
        )

def exists(env):
    return True
