#!/usr/bin/python
#
# Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
#                    Network Research Group (NET)
#                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
#                    Contact: support@wiback.org
#
# This file is part of the SENF code tree.
# It is licensed under the 3-clause BSD License (aka New BSD License).
# See LICENSE.txt in the top level directory for details or visit
# https://opensource.org/licenses/BSD-3-Clause
#


import os, sys, traceback, string
import os.path
import glob, re
import SCons.Action
from fnmatch import fnmatch

class DoxyfileLexer:

   def __init__(self,stream):
      self._stream = stream
      self._buffer = ""
      self.lineno = 0
      self._eof = False
      self._fillbuffer()
      
   VARIABLE_RE = re.compile("[@A-Z_]+")
   OPERATOR_RE = re.compile("\\+?=")
   VALUE_RE = re.compile("\\S+")

   def _readline(self):
      if self._eof:
         self._buffer = ""
         return
      self._buffer = self._stream.readline()
      if not self._buffer:
         self._eof = True
         return
      self._buffer = self._buffer.strip()
      self.lineno += 1

   def _skip(self, nchars=0):
      self._buffer = self._buffer[nchars:].strip()
      while self._buffer[:1] == '\\' and not self.eof():
         self._readline()
      if self._buffer[:1] == '#':
         self._buffer=""
      
   def _fillbuffer(self):
      while not self._buffer and not self.eof():
         self._readline()
         self._skip()

   def _token(self, re, read=False):
      if not self._buffer and read:
         self._fillbuffer()
      if not self._buffer:
         return ""
      m = re.match(self._buffer)
      if m:
         v = self._buffer[:m.end()]
         self._skip(m.end())
         return v
      else:
         raise ValueError,"Invalid input"

   def var(self): return self._token(self.VARIABLE_RE, True)
   def op(self): return self._token(self.OPERATOR_RE)

   def next(self):
      if not self._buffer:
         raise StopIteration
      if self._buffer[0] == '"':
         return self._qstr()
      m = self.VALUE_RE.match(self._buffer)
      if m:
         v = self._buffer[:m.end()]
         self._skip(m.end())
         return v
      else:
         raise ValueError

   def __iter__(self):
      return self

   QSKIP_RE = re.compile("[^\\\"]+")
   
   def _qstr(self):
      self._buffer = self._buffer[1:]
      v = ""
      while self._buffer:
          m = self.QSKIP_RE.match(self._buffer)
          if m:
             v += self._buffer[:m.end()]
             self._buffer = self._buffer[m.end():]
          if self._buffer[:1] == '"':
             self._skip(1)
             return v
          if self._buffer[:1] == '\\' and len(self._buffer)>1:
             v += self._buffer[1]
             self._buffer = self._buffer[2:]
          else:
             raise ValueError,"Unexpected charachter in string"
      raise ValueError,"Unterminated string"

   def eof(self):
      return self._eof

class DoxyfileParser:

   ENVVAR_RE = re.compile(r"\$\(([0-9A-Za-z_-]+)\)")

   def __init__(self, node, env, include_path=None, items = None):
      self._node = node
      self._env = env
      self._include_path = include_path or []
      self._lexer = DoxyfileLexer(file(node.srcnode().get_path()))
      self._dir = node.dir
      self._items = items or {}

   def parse(self):
      while True:
         var = self._lexer.var()
         if not var: break;
         op = self._lexer.op()
         value = [ self._envsub(v) for v in self._lexer ]
         if not value:
            raise ValueError,"Missing value in assignment"
         if var[0] == '@':
            self._meta(var,op,value)
         elif op == '=':
            self._items[var] = value
         else:
            self._items.setdefault(var,[]).extend(value)

   def _envsub(self,value):
      return self.ENVVAR_RE.sub(lambda m, env=self._env : str(env.get(m.group(1),"")), value)

   def _meta(self, cmd, op, value):
      m = '_'+cmd[1:]
      try:
         m = getattr(self,m)
      except AttributeError:
         raise ValueError,'Unknown meta command ' + cmd
      m(op,value)

   def _INCLUDE(self, op, value):
      if len(value) != 1:
         raise ValueError,"Invalid argument to @INCLUDE"
      
      for d in [ self._dir.get_path() ] + self._include_path:
         p = os.path.join(d,value[0])
         if os.path.exists(p):
            self._items.setdefault('@INCLUDE',[]).append(p)
            parser = DoxyfileParser(self._node.dir.File(p), self._env, self._include_path, self._items)
            parser.parse()
            return

      raise ValueError,"@INCLUDE file '%s' not found" % value[0]

   def _INCLUDE_PATH(self, op, value):
      self._include_path.extend(value)

   def items(self):
      return self._items

def DoxyfileParse(env,node):
   # We don't parse source files which do not contain the word 'doxyfile'. SCons will
   # pass other dependencies to DoxyfileParse which are not doxyfiles ... grmpf ...
   ENV = {}
   ENV.update(env.get("ENV",{}))
   ENV.update(env.get("DOXYENV", {}))
   parser = DoxyfileParser(node,ENV)
   try:
      parser.parse()
   except ValueError, v:
      print "WARNING: Error while parsing doxygen configuration '%s': %s" % (str(node),str(v))
      return {}
   data = parser.items()
   for k,v in data.items():
      if not v : del data[k]
      elif k in ("LAYOUT_FILE", "INPUT", "FILE_PATTERNS", "EXCLUDE_PATTERNS", "@INCLUDE", "TAGFILES") : continue
      elif len(v)==1 : data[k] = v[0]
   return data

def DoxySourceScan(node, env, path):
   """
   Doxygen Doxyfile source scanner.  This should scan the Doxygen file and add
   any files used to generate docs to the list of source files.
   """
   dep_add_keys = (
      ('HTML', 'LAYOUT_FILE'), 
      (None,   '@INCLUDE'), 
      ('HTML', 'HTML_HEADER'), 
      ('HTML', 'HTML_FOOTER'), 
      (None,   'TAGFILES'), 
      (None,   'INPUT_FILTER'),
   )

   output_formats = {
      "HTML"  : ("YES", "html"),
      "LATEX" : ("YES", "latex"),
      "RTF"   : ("NO",  "rtf"),
      "MAN"   : ("YES", "man"),
      "XML"   : ("NO",  "xml"),
   }

   default_file_patterns = (
      '*.c', '*.cc', '*.cxx', '*.cpp', '*.c++', '*.java', '*.ii', '*.ixx',
      '*.ipp', '*.i++', '*.inl', '*.h', '*.hh ', '*.hxx', '*.hpp', '*.h++',
      '*.idl', '*.odl', '*.cs', '*.php', '*.php3', '*.inc', '*.m', '*.mm',
      '*.py',
   )

   default_exclude_patterns = (
      '*~',
   )

   sources          = []
   basedir          = node.dir.abspath
   data             = DoxyfileParse(env, node)
   recursive        = data.get("RECURSIVE", "NO").upper()=="YES"
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
               if (         reduce(lambda x, y: x or fnmatch(f, y), file_patterns,    False)
                    and not reduce(lambda x, y: x or fnmatch(f, y), exclude_patterns, False) ):
                  sources.append(filename)

   for fmt, key in dep_add_keys:
      if data.has_key(key) and \
             (fmt is None or data.get("GENERATE_%s" % fmt, output_formats[fmt][0]).upper() == "YES"):
         elt = env.Flatten(env.subst_list(data[key]))
         sources.extend([ os.path.normpath(os.path.join(basedir,f))
                          for f in elt if f ])

   sources = map( lambda path: env.File(path), sources )
   return sources

def DoxySourceScanCheck(node, env):
   """Check if we should scan this file"""
   return os.path.isfile(node.path) and 'doxyfile' in node.name.lower()

def DoxyEmitter(source, target, env):
   """Doxygen Doxyfile emitter"""
   # possible output formats and their default values and output locations
   output_formats = {
      "HTML"  : ("YES", "html"),
      "LATEX" : ("YES", "latex"),
      "RTF"   : ("NO",  "rtf"),
      "MAN"   : ("YES", "man"),
      "XML"   : ("NO",  "xml"),
   }

   data = DoxyfileParse(env, source[0])

   targets = []
   if data.get("OUTPUT_DIRECTORY",""):
      out_dir = data["OUTPUT_DIRECTORY"]
      dir = env.Dir( os.path.join(source[0].dir.abspath, out_dir) )
      dir.sources = source
      if env.GetOption('clean'):
         targets.append(dir)
         return (targets, source)
   else:
      out_dir = '.'

   # add our output locations
   html_dir = None
   for (k, v) in output_formats.iteritems():
      if data.get("GENERATE_" + k, v[0]).upper() == "YES" and data.get(k + "_OUTPUT", v[1]):
         dir = env.Dir( os.path.join(source[0].dir.abspath, out_dir, data.get(k + "_OUTPUT", v[1])) )
         if k == "HTML" : html_dir = dir
         dir.sources = source
         node = env.File( os.path.join(dir.abspath, k.lower()+".stamp" ) )
         targets.append(node)
         if env.GetOption('clean'): targets.append(dir)

   if data.get("GENERATE_TAGFILE",""):
      targets.append(env.File( os.path.join(source[0].dir.abspath, data["GENERATE_TAGFILE"]) ))

   if data.get("SEARCHENGINE","NO").upper() == "YES" and html_dir:
      targets.append(env.File( os.path.join(html_dir.abspath, "search.idx") ))

   # don't clobber targets
   for node in targets:
      env.Precious(node)

   return (targets, source)

def doxyNodeHtmlDir(env,node):
   if not node.sources : return None
   data = DoxyfileParse(env, node.sources[0])
   if data.get("GENERATE_HTML",'YES').upper() != 'YES' : return None
   return os.path.normpath(os.path.join( node.sources[0].dir.abspath,
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

def doxyAction(target, source, env):
   e = {}
   e.update(env['ENV'])
   for k,v in env.get('DOXYENV',[]).iteritems() : e[k] = env.subst(v)
   SCons.Action.Action("$DOXYGENCOM")(target, source, env.Clone(ENV = e), show=False)

def doxyActionStr(target, source, env):
   return env.subst("$DOXYGENCOM",target=target,source=source)

def generate(env):
   """
   Add builders and construction variables for the
   Doxygen tool. This is currently for Doxygen 1.4.6.
   """
   doxyfile_scanner = env.Scanner(
      DoxySourceScan,
      "DoxySourceScan",
      scan_check = DoxySourceScanCheck,
   )

   doxyfile_builder = env.Builder(
      action = [ SCons.Action.Action(doxyAction, doxyActionStr),
                 SCons.Action.Action([ "touch $TARGETS" ]) ],
      emitter = DoxyEmitter,
      target_factory = env.fs.Entry,
      single_source = True,
      source_scanner = doxyfile_scanner
   )

   env.Append(BUILDERS = {
      'Doxygen': doxyfile_builder,
   })

   env.SetDefault(
      DOXYGEN = "doxygen",
      DOXYGENCOM = 'cd ${SOURCE.dir} && ${DOXYGEN} ${SOURCE.file}'
   )

def exists(env):
   """
   Make sure doxygen exists.
   """
   return env.Detect("doxygen")
