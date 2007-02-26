<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:fn="http://senf.berlios.de/xml/Extensions"
  xmlns:exsl="http://exslt.org/common"
  xmlns:str="http://exslt.org/strings"
  xmlns:func="http://exslt.org/functions"
  extension-element-prefixes="str fn exsl func"
  version="1.0">

  <xsl:include href="functions.xsl"/>

  <xsl:output method="html"/>
  <xsl:strip-space elements="*"/>
  <xsl:param name="title" select="''"/>
  <xsl:param name="types" select="'bug todo'"/>

  <xsl:template match="/">
    <div class="xref">
      <h1><xsl:value-of select="$title"/></h1>
      <xsl:variable name="doc" select="."/>
      <div class="nav">
        <xsl:text> -- </xsl:text>
        <xsl:for-each select="str:split($types)">
          <xsl:element name="a">
            <xsl:attribute name="href">#<xsl:value-of select="."/></xsl:attribute>
            <xsl:value-of select="translate(.,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/><xsl:text>S</xsl:text>
          </xsl:element>
          <xsl:text> -- </xsl:text>
        </xsl:for-each>
      </div>
      <xsl:for-each select="str:split($types)">
        <xsl:variable name="type" select="string(.)"/>
        <xsl:element name="div">
          <xsl:attribute name="class"><xsl:value-of select="$type"/></xsl:attribute>
          <xsl:element name="a">
            <xsl:attribute name="name"><xsl:value-of select="$type"/></xsl:attribute>
            <h2>Open <xsl:value-of select="translate($type,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>s</h2>
          </xsl:element>
          <xsl:for-each select="$doc//xreflist[@type=$type]">
            <xsl:sort select="@module"/>
            <h3><xsl:value-of select="@module"/> module</h3>
            <dl>
              <xsl:apply-templates/>
            </dl>
          </xsl:for-each>
        </xsl:element>
      </xsl:for-each>
    </div>
  </xsl:template>

  <xsl:template match="compound">
    <dt>
      <xsl:element name="a">
        <xsl:attribute name="href"><xsl:value-of select="fn:id2url(@id)"/></xsl:attribute>
        <b><xsl:value-of select="@name"/></b>
      </xsl:element>
    </dt>
    <dd>
      <xsl:apply-templates/>
    </dd>
  </xsl:template>

  <xsl:template match="item">
    <p><xsl:apply-templates/></p>
  </xsl:template>
  
  <!-- ====================================================================== -->
  <!-- Helper functions                                                       -->

  <fn:nsquote>
    <fn:replacement>
      <fn:match>_1_1</fn:match>
      <fn:replace>_2</fn:replace>
    </fn:replacement>
  </fn:nsquote>

  <xsl:variable name="nsquote" select="document('')//fn:nsquote/fn:replacement"/>
  
  <func:function name="fn:id2url">
    <xsl:param name="id"/>
    <!-- Yuck ... I HATE this .. why doesn't xsltproc support XPath 2.0 ... grmpf -->
    <xsl:variable name="quoted">
      <xsl:apply-templates select="str:replace($id,$nsquote/fn:match,$nsquote/fn:replace)"/>
    </xsl:variable>
    <xsl:variable name="anchor" select="substring-after($quoted,'_1')"/>
    <xsl:variable name="file">
      <xsl:apply-templates select="str:replace(substring($quoted,1,string-length($quoted) - number(boolean($anchor))*2 - string-length($anchor)),$nsquote/fn:replace,$nsquote/fn:match)"/>
    </xsl:variable>
    <xsl:variable name="sep" select="substring('#',2-number(boolean($anchor)))"/>
    <func:result>../../<xsl:value-of select="ancestor::xreflist/@module"/>/doc/html/<xsl:value-of select="$file"/>.html<xsl:value-of select="$sep"/><xsl:value-of select="$anchor"/></func:result>
  </func:function>

</xsl:stylesheet>
