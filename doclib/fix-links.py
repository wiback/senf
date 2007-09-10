#!/usr/bin/python

import sys,os.path,fnmatch, HTMLParser, getopt

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
        self.handle_data(name)

    def handle_entityref(self,name):
        self.handle_data(name)

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


class LinkFixer:

    def __init__(self, skipdirs=('.svn',)):
        self._index = AnchorIndex(skipdirs)

    def init(self):
        self._index.build()
        self._files = 0
        self._found = 0
        self._fixed = 0

    class LinkFilter(HTMLFilter):

        def __init__(self, index, key, topdir, out):
            HTMLFilter.__init__(self, out)
            self._index = index
            self._key = key
            self._topdir = topdir
            self._skip_a = False
            self._found = 0
            self._fixed = 0

        def _s_A(self, attrs):
            self._skip_a = False
            if self._key in dict(attrs).get('href',''):
                self._found += 1
                ix = [ i for i, attr in enumerate(attrs) if attr[0] == 'href' ][0]
                target = attrs[ix][1]
                if '#' in target:
                    anchor = target.split('#')[1]
                    target = self._index[anchor]
                    if target:
                        target = '%s#%s' % (target, anchor)
                else:
                    target = self._index[os.path.split(target)[1]]
                if target:
                    self._fixed += 1
                    attrs[ix] = ('href', os.path.join(self._topdir,target))
                else:
                    self._skip_a = True
                    return
            self.emit_starttag('a',attrs)

        def _e_A(self):
            if self._skip_a:
                self._skip_a = False
            else:
                self.emit_endtag('a')

        def stats(self):
            return (self._found, self._fixed)

    def fix(self, path, target):
        self._files += 1
        data = file(path).read()
        filt = LinkFixer.LinkFilter(self._index,
                                    target,
                                    "../" * (len(os.path.split(path)[0].split("/"))),
                                    file(path,"w"))
        filt.feed(data)
        filt.close()
        self._found += filt.stats()[0]
        self._fixed += filt.stats()[1]

    def stats(self):
        return (self._files, self._found, self._fixed)
    

(opts, args) = getopt.getopt(sys.argv[1:], "s:")
if len(args) != 2:
    sys.stderr.write("""Usage:
	fix-links.py [-s skip-dir]... <errrorX.txt> <errorAX.txt>

Process the 'errorX.txt' and 'errorAX.txt' files as generated by
'linklint': Check all invalid links and try to find the correct
target. If a target is found, the link is changed accordingly,
otherwise the link is removed.

To find anchors, fix-links.py generates a complete index of all
anchors defined in any HTML file in the current directory or some
subdirectory. The directories named 'skiped-dir' (at any level) will
not be scanned for '*.html' files.
""")
    sys.exit(1)

skipdirs = [ val for opt, val in opts if opt == '-s' ]

fixer = LinkFixer(skipdirs)
fixer.init()

target = None
for l in file(args[0]):
    l = l.rstrip()
    if l.startswith('/'):
        target = '#' + os.path.split(l)[1]
    elif l.startswith('    /') and not l.endswith('/'):
        sys.stderr.write("%s\n" % l)
        fixer.fix(l[5:], target)

for l in file(args[1]):
    l = l.rstrip()
    if l.startswith('/'):
        target = l.split('#')[1]
    elif l.startswith('    /') and not l.endswith('/'):
        sys.stderr.write("%s\n" % l)
        fixer.fix(l[5:], target)

files, found, fixed = fixer.stats()

sys.stderr.write("""
Files processed : %5d
Links processed : %5d
Links fixed     : %5d
Links removed   : %5d
""" % (files, found, fixed, found-fixed))
