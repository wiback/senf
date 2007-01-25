<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

  <xsl:output method="html" encoding="html" />

  <xsl:template match="/">
    <html>
      <body bgcolor="#FFFFFF">
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="dl[dt/b/a/text()='Bug:']">
    <xsl:copy>
      <xsl:for-each select="@*"><xsl:copy/></xsl:for-each>
      <xsl:attribute name="class">xref-bug</xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="dl[dt/b/a/text()='Fixme:']">
    <xsl:copy>
      <xsl:for-each select="@*"><xsl:copy/></xsl:for-each>
      <xsl:attribute name="class">xref-fixme</xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="dl[dt/b/a/text()='Todo:']">
    <xsl:copy>
      <xsl:for-each select="@*"><xsl:copy/></xsl:for-each>
      <xsl:attribute name="class">xref-todo</xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="dl[dt/b/a/text()='Idea:']">
    <xsl:copy>
      <xsl:for-each select="@*"><xsl:copy/></xsl:for-each>
      <xsl:attribute name="class">xref-idea</xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="*">
    <xsl:copy>
      <xsl:for-each select="@*"><xsl:copy/></xsl:for-each>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
