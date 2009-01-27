## \file
# \brief PkgDraw builder

## \package senfscons.PkgDraw
# \brief Generate network packet diagram
#
# This builder will call pkgdraw to scan a header file and create network package diagrams
# from SENF_PARSER macro calls.
#
# \par Construction Envrionment Variables:
# <table class="senf">
# <tr><td>\c PKGDRAWCOM</td><td>pkgdraw command, defaults to \c #/doclib/pkgdraw</td></tr>
# <tr><td>\c PKGDRAWPACKETS</td><td>list packet types to include in
#            the diagram, defaults to empty (include all packets)</td></tr>
# </table>
#
# \ingroup builder

import os
import SCons.Builder, SCons.Action

PkgDraw = SCons.Builder.Builder(
    suffix = ".png",
    src_suffix = ".hh",
    action = SCons.Action.Action("$PKGDRAWCOM $PKGDRAWPACKETS <$SOURCE >$TARGET"),
    single_source = 1)

def generate(env):
    env['BUILDERS']['PkgDraw'] = PkgDraw
    env['PKGDRAWCOM'] = env.Dir('#').abspath + "/doclib/pkgdraw"
    env['PKGDRAWPACKETS'] = ''

def exists(env):
    return env.Detect("#/doclib/pkgdraw")
