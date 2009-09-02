#!/usr/bin/python

import sys, os, gzip
import Ft.Xml.Xslt.Processor
import Ft.Xml.Domlette
from Ft.Xml.InputSource import DefaultFactory as InputFactory
import cStringIO

stylesheet=u"""
<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:dia="http://www.lysator.liu.se/~alla/dia/"
  version="1.0">

  <xsl:output method="text"/>

  <xsl:template match="text()"/>

  <xsl:template match="dia:object[@type='UML - Class']">
    <xsl:choose>
      <xsl:when test="dia:attribute[@name='comment']/dia:string!='##'">
        <xsl:value-of select="dia:attribute[@name='comment']/dia:string"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="dia:attribute[@name='name']/dia:string"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text> </xsl:text>
    <xsl:value-of select="dia:attribute[@name='obj_bb']/dia:rectangle/@val"/>
    <xsl:text>&#x0a;</xsl:text>
  </xsl:template>

  <xsl:template match="dia:attribute[@name='obj_bb']/dia:rectangle">
    <xsl:text>!! </xsl:text>
    <xsl:value-of select="@val"/>
    <xsl:text>&#x0a;</xsl:text>
  </xsl:template>

</xsl:stylesheet>
""".strip().encode('utf-8')

def xsltproc(source, stylesheet):
    processor = Ft.Xml.Xslt.Processor.Processor()
    processor.setDocumentReader(Ft.Xml.Domlette.NonvalidatingReader)
    processor.appendStylesheet(stylesheet)
    output = cStringIO.StringIO()
    processor.run(source, outputStream = output)
    return output.getvalue()

minx = miny = maxx = maxy = None
classes = {}

basename = sys.argv[1].rsplit('.',1)[0]
png = os.popen("pngtopnm %s.png 2>/dev/null" % basename)
png.readline()
size = map(int, png.readline().strip().split(' '))
png.read()

for line in xsltproc(InputFactory.fromStream(gzip.open(sys.argv[1]), sys.argv[1]),
                     InputFactory.fromString(stylesheet, '<string>')).strip().split("\n"):
    name,box = line.rsplit(' ',1)
    tlx, tly, brx, bry = (float(val)
                          for point in box.split(';')
                          for val in point.split(','))
    name = name[1:-1]
    if minx is None or tlx<minx : minx = tlx
    if maxx is None or tlx>maxx : maxx = tlx
    if minx is None or brx<minx : minx = brx
    if maxx is None or brx>maxx : maxx = brx
    if miny is None or tly<miny : miny = tly
    if maxy is None or tly>maxy : maxy = tly
    if miny is None or bry<miny : miny = bry
    if maxy is None or bry>maxy : maxy = bry 
    if name:
        classes[name] = (tlx, tly, brx, bry)

print '<div class="diamap" name="%s">' % basename
for name,box in classes.iteritems():
    print '<span coords="%d,%d,%d,%d">\\ref %s</span>' % (
        (box[0]-minx)*size[0]/(maxx-minx),
        (box[1]-miny)*size[1]/(maxy-miny),
        (box[2]-minx)*size[0]/(maxx-minx),
        (box[3]-miny)*size[1]/(maxy-miny),
        name)
print '</div>'
print '\\htmlonly <img src="%s.png" border="0" alt="%s" usemap="#%s"> \\endhtmlonly' % (
    basename,basename,basename)
