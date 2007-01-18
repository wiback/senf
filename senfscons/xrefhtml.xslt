<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:fn="http://senf.berlios.de/xml/Extensions"
  xmlns:exsl="http://exslt.org/common"
  xmlns:str="http://exslt.org/strings"
  extension-element-prefixes="str fn exsl"
  version="1.0">

  <xsl:include href="functions.xsl"/>

  <xsl:output method="html"/>
  <xsl:strip-space elements="*"/>
  <xsl:param name="title" select="''"/>

  <xsl:template match="/">
    <h1><xsl:value-of select="$title"/></h1>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="xreflist">
    <xsl:if test="string(preceding::xreflist[1]/@module)!=string(@module)">
      <xsl:if test="preceding::xreflist">
        <hr/>
      </xsl:if>
      <h2>The <xsl:element name="a">
        <xsl:attribute name="href">../../<xsl:value-of select="@module"/>/doc/html/index.html</xsl:attribute>
        <xsl:value-of select="@module"/>
      </xsl:element> module</h2>
    </xsl:if>
    <xsl:element name="dl">
      <xsl:attribute name="class"><xsl:value-of select="@type"/></xsl:attribute>
      <dt><h3><xsl:value-of select="translate(@type,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/> items</h3></dt>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <fn:nsquote>
    <fn:replacement>
      <fn:match>_1_1</fn:match>
      <fn:replace>_2</fn:replace>
    </fn:replacement>
  </fn:nsquote>

  <xsl:variable name="nsquote" select="document('')//fn:nsquote/fn:replacement"/>
  
  <xsl:template match="compound">
    <!-- Yuck ... I HATE this .. why doesn't xsltproc support XPath 2.0 ... grmpf -->
    <xsl:variable name="quoted">
      <xsl:apply-templates select="str:replace(@id,$nsquote/fn:match,$nsquote/fn:replace)"/>
    </xsl:variable>
    <xsl:variable name="anchor" select="substring-after($quoted,'_1')"/>
    <xsl:variable name="file">
      <xsl:apply-templates select="str:replace(substring($quoted,1,string-length($quoted) - number(boolean($anchor))*2 - string-length($anchor)),$nsquote/fn:replace,$nsquote/fn:match)"/>
    </xsl:variable>
    <xsl:variable name="sep" select="substring('#',2-number(boolean($anchor)))"/>
    <dt>
      <xsl:element name="a">
        <xsl:attribute name="href">../../<xsl:value-of select="ancestor::xreflist/@module"/>/doc/html/<xsl:value-of select="$file"/>.html<xsl:value-of select="$sep"/><xsl:value-of select="$anchor"/></xsl:attribute>
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

</xsl:stylesheet>
