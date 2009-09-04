import os.path, SCons

# SCons is at     #/scons/scons-<v>/engine/SCons/__init__.py
# site_init is at #/site_scons/site_init.py

sconsbase = os.path.join(os.path.dirname(os.path.dirname(__file__)),"scons")
sconsbase = os.path.abspath(os.path.join(sconsbase, sorted(os.listdir(sconsbase))[-1]))
sconsengine = os.path.join(sconsbase, 'engine')
sconsscript = os.path.join(os.path.join(sconsbase, 'script'),'scons')

if os.path.abspath(os.path.dirname(os.path.dirname(os.path.dirname(SCons.__file__)))) != sconsbase:
    import os, sys, SCons.Util
    SCons.Util.display("scons: Switching version")
    frame = sys._getframe()
    marker = []
    target_top = marker
    while frame and target_top is marker:
        target_top = frame.f_locals.get('target_top',marker)
        frame = frame.f_back
    if target_top is marker:
        SCons.Util.display("scons: WARNING: scons -u will fail")
    elif target_top is not None:
        os.chdir(target_top)
    os.environ['SCONS_LIB_DIR'] = sconsengine
    os.execv(sconsscript, sys.argv)
