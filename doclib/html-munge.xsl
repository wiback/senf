<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"  version="1.0">

  <xsl:output 
    method="html" 
    encoding="html" 
    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
    doctype-system="http://www.w3.org/TR/html4/loose.dtd"/>

  <xsl:param name="topdir" select="''"/>
  
  <xsl:template match="*" name="copy">
    <xsl:copy>
      <xsl:call-template name="copy-attributes"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template name="copy-attributes">
    <xsl:for-each select="@*">
      <xsl:attribute name="{name(current())}">
        <xsl:choose>
          <xsl:when test="contains(current(),'@TOPDIR@')">
            <xsl:value-of select="substring-before(current(),'@TOPDIR@')"/>
            <xsl:value-of select="$topdir"/>
            <xsl:value-of select="substring-after(current(),'@TOPDIR@')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="current()"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:attribute>
    </xsl:for-each>
  </xsl:template>

  <!-- Replace @TOPDIR@ with relative top directory path -->

  <xsl:template match="@*[contains(current(),'@TOPDIR@')]">
    <xsl:value-of select="substring-before(current(),'@TOPDIR@')"/>
    <xsl:value-of select="$topdir"/>
    <xsl:value-of select="substring-after(current(),'@TOPDIR@')"/>
  </xsl:template>
  
  <!-- Add 'class' attribute to some special paragraphs/lists -->
  
  <xsl:template name="add-class">
    <xsl:param name="class"/>
    <xsl:copy>
      <xsl:call-template name="copy-attributes"/>
      <xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <!-- Add '<br/>' tag after every ', ' -->
  <!-- This code is not very robust, it works with the doxygen output though -->

  <xsl:template name="break-comma">
    <xsl:copy>
      <xsl:call-template name="copy-attributes"/>
      <xsl:attribute name="class">commalist</xsl:attribute>
      <xsl:apply-templates mode="break-comma"/>
    </xsl:copy>
  </xsl:template>
 
  <xsl:template match="text()[1]" mode="break-comma">
    <xsl:value-of select="current()"/><br/>
  </xsl:template>

  <xsl:template match="*" mode="break-comma">
    <xsl:call-template name="copy"/>
  </xsl:template>

  <xsl:template match="text()[contains(current(),' and') or contains(current(),'and ')]" mode="break-comma" priority="1">
    <xsl:value-of select="substring-before(current(),'and')"/>
    <br/>
    <xsl:value-of select="substring-after(current(),'and')"/>
  </xsl:template>

  <xsl:template match="text()[contains(current(),',')]" mode="break-comma">
    <xsl:value-of select="substring-before(current(),',')"/>
    <xsl:text>,</xsl:text><br/>
    <xsl:value-of select="substring-after(current(),',')"/>
  </xsl:template>

  <!-- ====================================================================== -->

  <!-- Remove the automatically inserted search form (we build our own) -->
  <xsl:template match="li[form]"> 
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

  <xsl:template match="dl[dt/b/text()='Implementation note:']">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">implementation</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="table[descendant::td[@class='memItemLeft']]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">members</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="a[@href=string(current())]" priority="1">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">literal</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="a[contains(@href,'http://')]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">ext</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="a[not(@href)]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">anchor</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="div[@class='memdoc']/p[1]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">memtitle</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="p[starts-with(text(),'Definition at line ')]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">sourceline</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="div[@class='memdoc']/p[starts-with(text(),'References ')]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">references</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="div[@class='memdoc']/p[starts-with(text(),'Referenced by ')]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">referencedby</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="div[@class='memdoc']/p[starts-with(text(),'Reimplemented from ')]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">reimplementedfrom</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="div[@class='memdoc']/p[starts-with(text(),'Reimplemented in ')]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">reimplementedin</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="div[@class='memdoc']/p[starts-with(text(),'Implemented in ')]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">implementedin</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="p[starts-with(text(),'Inherited by ')]">
    <xsl:call-template name="break-comma"/>
  </xsl:template>

  <xsl:template match="p[starts-with(text(),'Inherits ')]">
    <xsl:call-template name="break-comma"/>
  </xsl:template>

  <!-- Remove external items from the namespace index -->
  <xsl:template match="div[@id='content2']/table[contains(preceding-sibling::h1/text(),'Namespace Reference')]/tr[td[@class='memItemRight']/a[1][@class='elRef'][@doxygen]]">
  </xsl:template>
  
  <!-- Remove [external] references from the modules page -->
  <xsl:template match="div[@id='content2']/ul/li[a/@class='elRef'][a/@doxygen][code/text()='[external]'][not(ul)]">
  </xsl:template>
  
</xsl:stylesheet>
