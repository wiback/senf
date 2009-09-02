import os, os.path
import SCons.Builder, SCons.Action

libdir = os.path.join(os.path.dirname(os.path.dirname(__file__)),'lib')

PkgDraw = SCons.Builder.Builder(
    suffix = ".png",
    src_suffix = ".hh",
    action = SCons.Action.Action("$PKGDRAWCOM"),
    single_source = 1)

def generate(env):
    env['BUILDERS']['PkgDraw'] = PkgDraw
    env['PKGDRAW'] = "%s/pkgdraw" % libdir
    env['PKGDRAWCOM'] = "$PKGDRAW $SOURCE $TARGET $PKGDRAWPACKETS -- $CPPFLAGS $_CPPINCFLAGS $_CPPDEFFLAGS"
    env['PKGDRAWPACKETS'] = ''

def exists(env):
    return env.Detect("#/doclib/pkgdraw")
