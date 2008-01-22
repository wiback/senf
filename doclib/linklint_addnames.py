#!/usr/bin/python

import HTMLParser, re, sys, os.path

class ScanLinks(HTMLParser.HTMLParser):

    def __init__(self, target, base):
        HTMLParser.HTMLParser.__init__(self)
        self._target = target
        self._base = base
        self._collect = False
        self._data = ""
        self._value = ""

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

    def handle_endtag(self,tag):
        m = getattr(self,'_e_'+tag.upper(),None)
        if m:
            m()

    def handle_data(self,data):
        if self._collect:
            self._data += data

    def handle_charref(self,name):
        self.handle_data(name)

    def handle_entityref(self,name):
        self.handle_data(name)

    def value(self):
        return self._value

    ###########################################################################

    SCHEME_RE=re.compile("[a-z]+:")

    def _s_A(self,attrs):
        attrs = dict(attrs)
        url = attrs.get('href')
        if url and not self.SCHEME_RE.match(url):
            if '#' in self._target:
                p = os.path.abspath(os.path.join(self._base,url))
            else:
                p = os.path.abspath(os.path.join(self._base,url.split('#')[0]))
            if  p == self._target:
                self.startCollect()
        
    def _e_A(self):
        if self.collecting():
            self._value = self.endCollect()

WS_RE=re.compile("\\s+")

def linkName(target,f):
    scanner = ScanLinks(target,os.path.split(os.path.abspath(f))[0])
    scanner.feed(file(f).read())
    return WS_RE.sub(' ',scanner.value().strip())

process = 0
target = 0
for line in sys.stdin:
    if line.startswith('<a href='):
        target = line.split(':')[1]
        target = target[2:].split('"')[0]
    elif line.startswith('    <a href='):
        f = line.split(':')[1]
        f = f[2:].split('"')[0]
        line = '%s (%s)</a>\n'  % (line[:-5], linkName(target,f))
    sys.stdout.write(line)
