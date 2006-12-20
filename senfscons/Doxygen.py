# Astxx, the Asterisk C++ API and Utility Library.
# Copyright (C) 2005, 2006  Matthew A. Nicholson
# Copyright (C) 2006  Tim Blechmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License version 2.1 as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import os, sys, traceback
import os.path
import glob
from fnmatch import fnmatch

def DoxyfileParse(file_contents, dir, data = None):
   """
   Parse a Doxygen source file and return a dictionary of all the values.
   Values will be strings and lists of strings.
   """
   try:
      if data is None : data = {}

      import shlex
      lex = shlex.shlex(instream = file_contents, posix = True)
      lex.wordchars += "*+./-:"
      lex.whitespace = lex.whitespace.replace("\n", "")
      lex.escape = ""

      lineno = lex.lineno
      token = lex.get_token()
      key = None
      last_token = ""
      key_token = True
      next_key = False
      new_data = True

      def append_data(data, key, new_data, token):
         if new_data or len(data[key]) == 0:
            data[key].append(token)
         else:
            data[key][-1] += token

      while token:
         if token in ['\n']:
            if last_token not in ['\\']:
               key_token = True
         elif token in ['\\']:
            pass
         elif key_token:
            if key == '@' : key += token
            else          : key = token
            if token != '@' : key_token = False
         else:
            if token == "+=" or (token == "=" and key == "@INCLUDE"):
               if not data.has_key(key):
                  data[key] = list()
            elif token == "=":
               data[key] = list()
            else:
               append_data( data, key, new_data, token )
               new_data = True
               if key == '@INCLUDE':
                  inc = os.path.join(dir,data['@INCLUDE'][-1])
                  if os.path.exists(inc) :
                     DoxyfileParse(open(inc).read(),dir,data)

         last_token = token
         token = lex.get_token()

         if last_token == '\\' and token != '\n':
            new_data = False
            append_data( data, key, new_data, '\\' )

      # compress lists of len 1 into single strings
      for (k, v) in data.items():
         if len(v) == 0:
            data.pop(k)

         # items in the following list will be kept as lists and not converted to strings
         if k in ["INPUT", "FILE_PATTERNS", "EXCLUDE_PATTERNS", "@INCLUDE", "TAGFILES"]:
            continue

         if len(v) == 1:
            data[k] = v[0]


      return data
   except:
      return {}

def DoxySourceScan(node, env, path):
   """
   Doxygen Doxyfile source scanner.  This should scan the Doxygen file and add
   any files used to generate docs to the list of source files.
   """
   dep_add_keys = [
      '@INCLUDE', 'HTML_HEADER', 'HTML_FOOTER', 'TAGFILES'
   ]
   
   default_file_patterns = [
      '*.c', '*.cc', '*.cxx', '*.cpp', '*.c++', '*.java', '*.ii', '*.ixx',
      '*.ipp', '*.i++', '*.inl', '*.h', '*.hh ', '*.hxx', '*.hpp', '*.h++',
      '*.idl', '*.odl', '*.cs', '*.php', '*.php3', '*.inc', '*.m', '*.mm',
      '*.py',
   ]

   default_exclude_patterns = [
      '*~',
   ]

   sources          = []
   basedir          = str(node.dir)
   data             = DoxyfileParse(node.get_contents(), basedir)
   recursive        = ( data.get("RECURSIVE", "NO") == "YES" )
   file_patterns    = data.get("FILE_PATTERNS", default_file_patterns)
   exclude_patterns = data.get("EXCLUDE_PATTERNS", default_exclude_patterns)

   for i in data.get("INPUT", [ "." ]):
      input = os.path.normpath(os.path.join(basedir,i))
      if os.path.isfile(input):
         sources.append(input)
      elif os.path.isdir(input):
         if recursive : entries = os.walk(input)
         else         : entries = [ (input, [], os.listdir(input)) ]
         for root, dirs, files in entries:
            for f in files:
               filename = os.path.normpath(os.path.join(root, f))
               if ( reduce(lambda x, y: x or fnmatch(filename, y),
                           file_patterns, False) 
                    and not reduce(lambda x, y: x or fnmatch(filename, y),
                                   exclude_patterns, False) ):
                  sources.append(filename)

   for key in dep_add_keys:
      if data.has_key(key):
         elt = data[key]
         if type(elt) is type ("") : elt = [ elt ]
         sources.extend([ os.path.normpath(os.path.join(basedir,f))
                          for f in elt ])

   sources = map( lambda path: env.File(path), sources )
   return sources

def DoxySourceScanCheck(node, env):
   """Check if we should scan this file"""
   return os.path.isfile(node.path)

def DoxyEmitter(source, target, env):
   """Doxygen Doxyfile emitter"""
   # possible output formats and their default values and output locations
   output_formats = {
      "HTML": ("YES", "html"),
      "LATEX": ("YES", "latex"),
      "RTF": ("NO", "rtf"),
      "MAN": ("YES", "man"),
      "XML": ("NO", "xml"),
   }

   data = DoxyfileParse(source[0].get_contents(), str(source[0].dir))

   targets = []
   out_dir = data.get("OUTPUT_DIRECTORY", ".")

   # add our output locations
   for (k, v) in output_formats.items():
      if data.get("GENERATE_" + k, v[0]) == "YES":
         # Grmpf ... need to use a File object here. The problem is, that
         # Dir.scan() is implemented to just return the directory entries
         # and does *not* invoke the source-file scanners .. ARGH !!
         dir = env.Dir( os.path.join(str(source[0].dir), out_dir, data.get(k + "_OUTPUT", v[1])) )
         node = env.File( os.path.join(str(dir), ".stamp" ) )
         env.Clean(node, dir)
         targets.append( node )

   if data.has_key("GENERATE_TAGFILE"):
      targets.append(env.File( os.path.join(str(source[0].dir), data["GENERATE_TAGFILE"]) ))

   # don't clobber targets
   for node in targets:
      env.Precious(node)

   return (targets, source)

def doxyNodeHtmlDir(node):
   if not node.sources : return None
   data = DoxyfileParse(node.sources[0].get_contents(), str(node.sources[0].dir))
   if data.get("GENERATE_HTML",'YES') != 'YES' : return None
   return os.path.normpath(os.path.join( str(node.sources[0].dir),
                                         data.get("OUTPUT_DIRECTORY","."),
                                         data.get("HTML_OUTPUT","html") ))

def relpath(source, target):
   source = os.path.normpath(source)
   target = os.path.normpath(target)
   prefix = os.path.dirname(os.path.commonprefix((source,target)))
   prefix_len = prefix and len(prefix.split(os.sep)) or 0
   source_elts = source.split(os.sep)
   target_elts = target.split(os.sep)
   if source_elts[0] == '..' or target_elts[0] == '..':
      raise ValueError, "invalid relapth args"
   return os.path.join(*([".."] * (len(source_elts) - prefix_len) +
                         target_elts[prefix_len:]))

def DoxyGenerator(source, target, env, for_signature):

   data = DoxyfileParse(source[0].get_contents(), str(source[0].dir))

   actions = [ env.Action("cd ${SOURCE.dir}  &&  ${DOXYGEN} ${SOURCE.file}") ]

   # This will add automatic 'installdox' calls.
   #
   # For every referenced tagfile, the generator first checks for the
   # existence of a construction variable '<name>_DOXY_URL' where
   # '<name>' is the uppercased name of the tagfile sans extension
   # (e.g. 'Utils.tag' -> 'UTILS_DOXY_URL'). If this variable exists,
   # it must contain the url or path to the installed documentation
   # corresponding to the tag file.
   #
   # Is the variable is not found and if a referenced tag file is a
   # target within this same build, the generator will parse the
   # 'Doxyfile' from which the tag file is built. It will
   # automatically create the html directory from the information in
   # that 'Doxyfile'.
   #
   # If for any referenced tagfile no url can be found, 'installdox'
   # will *not* be called and a warning about the missing url is
   # generated.
   
   if data.get('GENERATE_HTML','YES') == "YES":
      output_dir = os.path.normpath(os.path.join( str(source[0].dir),
                                                  data.get("OUTPUT_DIRECTORY","."),
                                                  data.get("HTML_OUTPUT","html") ))
      args = []
      for tagfile in data.get('TAGFILES',[]):
         url = env.get(os.path.splitext(os.path.basename(tagfile))[0].upper()+"_DOXY_URL", None)
         if not url:
            url = doxyNodeHtmlDir(
               env.File(os.path.normpath(os.path.join( str(source[0].dir), tagfile ))))
            if url : url = relpath(output_dir, url)
         if not url:
            print "WARNING:",str(node.sources[0]),": missing tagfile url for",tagfile
            args = None
         if args is not None and url:
            args.append("-l %s@%s" % ( os.path.basename(tagfile), url ))
      if args:
         actions.append(env.Action('cd %s && ./installdox %s' % (output_dir, " ".join(args))))
   
   actions.append(env.Action([ "touch $TARGETS" ]))

   return actions

def generate(env):
   """
   Add builders and construction variables for the
   Doxygen tool.  This is currently for Doxygen 1.4.6.
   """
   doxyfile_scanner = env.Scanner(
      DoxySourceScan,
      "DoxySourceScan",
      scan_check = DoxySourceScanCheck,
   )

   doxyfile_builder = env.Builder(
      # scons 0.96.93 hang on the next line but I don't know hot to FIX the problem
      generator = DoxyGenerator,
      emitter = DoxyEmitter,
      target_factory = env.fs.Entry,
      single_source = True,
      source_scanner =  doxyfile_scanner
   )

   env.Append(BUILDERS = {
      'Doxygen': doxyfile_builder,
   })

   env.AppendUnique(
      DOXYGEN = 'doxygen',
   )

def exists(env):
   """
   Make sure doxygen exists.
   """
   return env.Detect("doxygen")
