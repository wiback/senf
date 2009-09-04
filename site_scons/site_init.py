import os.path, SCons

# SCons is at     #/tools/scons-<v>/engine/SCons/__init__.py
# site_init is at #/site_scons/site_init.py

sconsbase = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),"tools")
sconsbase = os.path.join(
    sconsbase,sorted((f for f in os.listdir(sconsbase) if f.startswith("scons-")))[-1])
sconsengine = os.path.join(sconsbase, 'engine')
sconsscript = os.path.join(sconsbase, 'script', 'scons')

if os.path.dirname(os.path.dirname(os.path.abspath(SCons.__file__))) != sconsengine:
    import os, sys, SCons.Util
    SCons.Util.display("scons: Switching version")

    # BEGIN HACK
    # This is really ugly. SCons has already switched into the
    # top-level directory when this code is executed. To make 'scons
    # -u <target>' work, we need to switch back into the orgiginal
    # startup dir. This directory however is only available in a local
    # variable called 'target_top' in SCons.Script.Main._main.
    #
    # We walk the stack to find a local variable called
    # 'target_top'. If we can't find it (could really only happen if
    # the SCons.Main code is changed considerably), we output a
    # warning, that '-u' will probably fail (we re-start scons from
    # the top-level directory, relative targets therefore will not
    # work)
    frame = sys._getframe()
    target_top = marker = []
    while frame and target_top is marker:
        target_top = frame.f_locals.get('target_top',marker)
        frame = frame.f_back
    if target_top is marker:
        SCons.Util.display("scons: WARNING: scons -u <target> will probably fail")
    if not target_top:
        target_top = '.'
    # END HACK

    os.chdir(target_top)
    os.environ['SCONS_LIB_DIR'] = sconsengine
    os.execv(sconsscript, sys.argv)
