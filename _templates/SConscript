# -*- python -*-

Import('env')
import SENFSCons

###########################################################################

## Most modules will be built using the auto macros from site_scons/SENFSCons.py 
## (see below). Arguments are:
##     env - The construction environment
##     name - name of target (AutoPacketBundle only)
##     exclude - Files to exclude from the build
##     subdirs - Subdirectories to recurse into
##     doc_extra_sources - Extra source files (images) to build for the documentation
##
## If the macros don't do exactly what you want you may either copy the function definition
## body from site_scons/SENFSCons.py into the SConscript file or add additional targets

## Build a standard library module
#
# SENFSCons.AutoRules(env, exclude=[...], subdirs=[...], doc_extra_sources=[...]

## Build a packet bundle
#
# SENFSCons.AutoPacketBundle(env, name, exclude=[...], subdirs=[...], doc_extra_sources=[...]

## Build an (additional) executable. Build it as default target (built when calling scons
## without arguments or with the 'default' argument
#
# env.Default(env.Program(name, [ sources...]))
