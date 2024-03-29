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


import sys,os.path,fnmatch, HTMLParser, getopt, re, codecs

class HTMLFilter(HTMLParser.HTMLParser):

    def __init__(self, out=None):
        HTMLParser.HTMLParser.__init__(self)
        self._out = out
        self._collect = False
        self._data = ""

    def startCollect(self):
        self._collect = True
        self._data = ""

    def endCollect(self):
        self._collect = False
        return self._data

    def collecting(self):
        return self._collect

    def handle_starttag(self,tag,attrs):
        m = getattr(self,'_s_'+tag.upper(),None)
        if m:
            m(attrs)
        else:
            self.emit_starttag(tag,attrs)

    def handle_endtag(self,tag):
        m = getattr(self,'_e_'+tag.upper(),None)
        if m:
            m()
        else:
            self.emit_endtag(tag)

    def handle_data(self,data):
        if self._collect:
            self._data += data
        if self._out:
            self._out.write(data)

    def handle_charref(self,name):
        self.handle_data('&#%s;' % name)

    def handle_entityref(self,name):
        self.handle_data('&%s;' % name)

    def emit_starttag(self,tag,attrs):
        self.handle_data('<%s%s>' % (tag, "".join([' %s="%s"' % attr for attr in attrs])))

    def emit_endtag(self,tag):
        self.handle_data('</%s>' % tag)


class AnchorIndex:

    def __init__(self, skipdirs = ('.svn',)):
        self._anchors = {}
        self._skipdirs = skipdirs

    def build(self):
        sys.stderr.write("Building anchor index ")
        nf = 0
        for dirname, subdirs, files in os.walk('.'):
            for d in self._skipdirs:
                if d in subdirs:
                    subdirs.remove(d)
            for f in fnmatch.filter(files,'*.html'):
                nf += 1
                path = os.path.normpath(os.path.join(dirname, f))
                self._addAnchor(f, path)
                self._extractAnchors(path)
        sys.stderr.write(" Done.\n")
        dups = 0
        for k in self._anchors.keys():
            if not self._anchors[k]:
                dups += 1
                del self._anchors[k]
        sys.stderr.write("%d unique anchors in %d files (%d duplicates)\n"
                         % (len(self._anchors), nf, dups))

    def _addAnchor(self, anchor, path):
        if self._anchors.has_key(anchor):
            self._anchors[anchor] = None
        else:
            self._anchors[anchor] = path
            if len(self._anchors) % 100 == 0:
                sys.stderr.write('.')

    def __getitem__(self, key):
        return self._anchors.get(key)

    class AnchorExtractor(HTMLFilter):

        def __init__(self):
            HTMLFilter.__init__(self)
            self._anchors = {}

        def _s_A(self,attrs):
            attrs = dict(attrs)
            if attrs.has_key('name'):
                self._anchors[attrs['name']] = None

        def anchors(self):
            return self._anchors.keys()

    def _extractAnchors(self, f):
        extractor = AnchorIndex.AnchorExtractor()
        extractor.feed(file(f).read())
        extractor.close()
        for anchor in extractor.anchors():
            self._addAnchor(anchor, f)

TAG_RE = re.compile("<[^>]*>")
REF_RE = re.compile("&[^;]*;")

def stripHTML(s):
    s = TAG_RE.sub("",s)
    s = s.replace("&nbsp;"," ").replace("\n"," ")
    s = REF_RE.sub("?",s)
    return s.strip()

class LinkFixer:

    def __init__(self, skipdirs=('.svn',)):
        self._index = AnchorIndex(skipdirs)

    def init(self):
        self._index.build()

    class LinkFilter(HTMLFilter):

        def __init__(self, index, topdir, out):
            HTMLFilter.__init__(self, out)
            self._index = index
            self._topdir = topdir

        def _check(self, tag, linkAttr, attrs):
            ix = target = None
            for i,(k,v) in enumerate(attrs):
                if k == linkAttr:
                    ix, target = i, v
                    break
            if target:
                fix = False
                tdir = anchor = None
                if '#' in target : target, anchor = target.rsplit('#',1)
                if '/' in target : tdir, target = target.rsplit('/', 1)
                newTarget = None
                if anchor:
                    newTarget = self.anchorLookup(anchor)
                if newTarget is None:
                    newTarget = self.fileLookup(target)
                    if newTarget and anchor:
                        newTarget = '%s#%s' % (newTarget, anchor)
                if newTarget:
                    attrs[ix] = (attrs[ix][0], '/'.join((self._topdir, newTarget)))
            self.emit_starttag(tag, attrs)

        def anchorLookup(self,anchor):
            target = None
            while not target:
                target = self._index[anchor]
                if target:
                    target = '%s#%s' % (target, anchor)
                elif anchor.startswith('g'):
                    anchor = anchor[1:]
                else:
                    break
            return target

        def fileLookup(self,target):
            return self._index[target]

        def _s_A(self, attrs):
            self._check('a', 'href', attrs)

        def _s_AREA(self, attrs):
            self._check('area', 'href', attrs)

    def fix(self, path):
        data = codecs.open(path, "r", "utf-8").read()
        filt = LinkFixer.LinkFilter(self._index,
                                    ("../" * (len(os.path.split(path)[0].split("/"))))[:-1],
                                    codecs.open(path, "w", "utf-8") )
        filt.feed(data)
        filt.close()

(opts, args) = getopt.getopt(sys.argv[1:], "vs:")
if len(args) != 0:
    sys.stderr.write("""Usage: fix-links.py [-s skip-dir]...

Check all links and try to find the correct target. If a target is
found, the link is changed accordingly, otherwise the link is removed.

To find anchors, fix-links.py generates a complete index of all
anchors defined in any HTML file in the current directory or some
subdirectory. The directories named 'skiped-dir' (at any level) will
not be scanned for '*.html' files.
""")
    sys.exit(1)

skipdirs = [ val for opt, val in opts if opt == '-s' ]
verbose = ( '-v', '' ) in opts

fixer = LinkFixer(skipdirs)
fixer.init()

for dirname, subdirs, files in os.walk('.'):
    for d in skipdirs:
        if d in subdirs:
            subdirs.remove(d)
    for f in fnmatch.filter(files,'*.html'):
        path = os.path.normpath(os.path.join(dirname, f))
        print path
        fixer.fix(path)
