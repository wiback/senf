# -*- python -*-

import sys, glob, os.path, datetime
sys.path.append('senfscons')
import SENFSCons

###########################################################################

# Load subversion information
svninfo = dict(
    [ map(lambda y:y.strip(),x.split(":",1))
      for x in os.popen("svn info").read().split("\n")
      if ':' in x ] )
svninfo['commited'] = not(os.popen("svn status -q").read())

# Load utilities and setup libraries
SENFSCons.UseBoost()
SENFSCons.UseSTLPort()
env = SENFSCons.MakeEnvironment()

# Configure build
env.Append(
   CPPPATH = [ '#' ],
   LIBS = [ 'iberty' ],
   DOXY_XREF_TYPES = [ 'bug', 'fixme', 'todo', 'idea' ],
   DOXY_HTML_XSL = '#/doclib/html-munge.xsl',
   ENV = { 'TODAY' : str(datetime.date.today()),
           'REVISION' : svninfo['Revision'] + (not(svninfo['commited']) and " + local changes" or ""),
           },
)

Export('env')

# Build modules (that is, instruct to build ... the build happens later)
SConscript(glob.glob("*/SConscript"))

SENFSCons.StandardTargets(env)
SENFSCons.GlobalTargets(env)
SENFSCons.Doxygen(env, extra_sources = [
    'Sniffer/Sniffer.cc',
])
SENFSCons.DoxyXRef(env,
                   HTML_HEADER = '#/doclib/doxy-header-overview.html',
                   HTML_FOOTER = '#/doclib/doxy-footer.html')

# Create Doxyfile.local if not cleaning and the file does not exist
# otherwise doxygen will barf on this non-existent file
if not env.GetOption('clean') and not os.path.exists("Doxyfile.local"):
    Execute(Touch("Doxyfile.local"))
