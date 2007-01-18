<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

  <xsl:output method="xml" omit-xml-declaration="yes"/>
  <xsl:param name="module" select="''"/>
  <xsl:param name="type" select="'todo'"/>
  <xsl:strip-space elements="*"/>

  <xsl:template match="variablelist">
    <xsl:if test="string(preceding::variablelist[1]/varlistentry/term/ref/@refid)!=string(varlistentry/term/ref/@refid)">
      <xsl:element name="compound">
        <xsl:attribute name="id">
          <xsl:value-of select="varlistentry/term/ref/@refid"/>
        </xsl:attribute>
        <xsl:attribute name="name">
          <xsl:value-of select="string(varlistentry/term)"/>
        </xsl:attribute>
        <xsl:text>&#xA;</xsl:text>
        <xsl:variable name="curid" select="varlistentry/term/ref/@refid"/>
        <xsl:apply-templates 
          select="//variablelist[varlistentry/term/ref/@refid=$curid]"
          mode="inlist"/>
      </xsl:element>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
  </xsl:template>
  
  <xsl:template match="variablelist" mode="inlist">
    <item>
      <xsl:value-of select="string(listitem)"/>
    </item>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <xsl:template match="/">
    <xsl:element name="xreflist">
      <xsl:attribute name="type">
        <xsl:value-of select="$type"/>
      </xsl:attribute>
      <xsl:attribute name="module">
        <xsl:value-of select="$module"/>
      </xsl:attribute>
      <xsl:text>&#xA;</xsl:text>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="compoundname"/>
  <xsl:template match="title"/>
  <xsl:template match="anchor"/>

</xsl:stylesheet>
