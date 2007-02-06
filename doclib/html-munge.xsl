<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"  version="1.0">

  <xsl:output method="html" encoding="html"/>
  
  <xsl:template match="*">
    <xsl:copy>
      <xsl:for-each select="@*"><xsl:copy/></xsl:for-each>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
  
  <!-- Add 'class' attribute to some special paragraphs/lists -->
  
  <xsl:template name="add-class">
    <xsl:param name="class"/>
    <xsl:copy>
      <xsl:for-each select="@*"><xsl:copy/></xsl:for-each>
      <xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="dl[dt/b/a/text()='Bug:']">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">xref-bug</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="dl[dt/b/a/text()='Fix:']">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">xref-fix</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="dl[dt/b/a/text()='Todo:']">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">xref-todo</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="dl[dt/b/a/text()='Idea:']">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">xref-idea</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="dl[dt/b/text()='Parameters:']">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">parameters</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

</xsl:stylesheet>
