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
    os.chdir(sys._getframe(2).f_locals['target_top'] or '.')
    os.environ['SCONS_LIB_DIR'] = sconsengine
    os.execv(sconsscript, sys.argv)
