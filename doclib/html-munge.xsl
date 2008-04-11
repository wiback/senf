<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"  version="1.0"
  xmlns:str="http://exslt.org/strings">

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

  <!-- Hack Glossary menu highlight -->

  <xsl:template match="div[@class='tabs menu']/ul[1]">
    <xsl:choose>
      <xsl:when test="//h1[.//text()='Glossary']">
        <xsl:call-template name="add-class">
          <xsl:with-param name="class">glossary</xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:copy>
          <xsl:call-template name="copy-attributes"/>
          <xsl:apply-templates/>
        </xsl:copy>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Autogenerate table-of-contents for a page -->

  <xsl:template match="div[@id='autotoc']">
    <xsl:copy>
      <xsl:call-template name="copy-attributes"/>
      <h1>Contents</h1>
      <ul>
        <xsl:for-each select="following::h2|following::h3|following::h4">
          <xsl:element name="li">
            <xsl:attribute name="class">
              <xsl:value-of select="concat('level_',local-name())"/>
            </xsl:attribute>
            <b><xsl:call-template name="section-number"/><xsl:text> </xsl:text></b>
            <xsl:element name="a">
              <xsl:choose>
                <xsl:when test="a/@name">
                  <xsl:attribute name="href">
                    <xsl:value-of select="concat('#',a/@name)"/>
                  </xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:attribute name="href">
                    <xsl:text>#autotoc-</xsl:text>
                    <xsl:call-template name="section-number"/>
                  </xsl:attribute>
                </xsl:otherwise>
              </xsl:choose>
              <xsl:value-of select="string(current())"/>
            </xsl:element>
          </xsl:element>
        </xsl:for-each>
      </ul>
    </xsl:copy>
  </xsl:template>

  <xsl:template name="section-number">
    <xsl:number level="any" from="div[@id='autotoc']" count="h2"/>
    <xsl:text>.</xsl:text>
    <xsl:if test="self::h3|self::h4">
      <xsl:number level="any" from="h2" count="h3"/>
      <xsl:text>.</xsl:text>
    </xsl:if>
    <xsl:if test="self::h4">
      <xsl:number level="any" from="h3" count="h4"/>
      <xsl:text>.</xsl:text>
    </xsl:if>
  </xsl:template>
  
  <xsl:template match="h2|h3|h4">
    <xsl:copy>
      <xsl:call-template name="copy-attributes"/>
      <xsl:choose>
        <xsl:when test="preceding::div[@id='autotoc']">
          <xsl:call-template name="section-number"/>
          <xsl:text> </xsl:text>
          <xsl:choose>
            <xsl:when test="a">
              <xsl:apply-templates/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:element name="a">
                <xsl:attribute name="class"><xsl:text>anchor</xsl:text></xsl:attribute>
                <xsl:attribute name="name">
                  <xsl:text>autotoc-</xsl:text>
                  <xsl:call-template name="section-number"/>
                </xsl:attribute>
                <xsl:apply-templates/>
              </xsl:element>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:copy>
  </xsl:template>

  <!-- Build dia image-map from special div/span elements -->
  <xsl:template match="div[@class='diamap']">
    <xsl:element name="map">
      <xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
      <xsl:for-each select="span">
        <xsl:if test="a">
          <xsl:variable name="name" select="string(a[1])"/>
          <xsl:element name="area">
            <xsl:attribute name="shape">rect</xsl:attribute>
            <xsl:attribute name="alt"><xsl:value-of select="$name"/></xsl:attribute>
            <xsl:attribute name="title"><xsl:value-of select="$name"/></xsl:attribute>
            <xsl:attribute name="href"><xsl:value-of select="a[1]/@href"/></xsl:attribute>
            <xsl:attribute name="coords"><xsl:value-of select="@coords"/></xsl:attribute>
          </xsl:element>
        </xsl:if>
      </xsl:for-each>
    </xsl:element>
  </xsl:template>

  <!-- Add grouping to all-members page -->

  <xsl:template match="table[preceding-sibling::h1[1][contains(text(),'Member List')]]">
    <table class="allmembers">
      <tr><td colspan="3"><h2>Public static member functions</h2></td></tr>
      <xsl:apply-templates select="tr[contains(td[3],'static')][not(contains(td[3],'protected'))][not(contains(td[3],'private'))][not(contains(td[3],'friend'))][str:split(substring-before(concat(td[2]/a,'&lt;'),'&lt;'),'::')[position()=last()]!=string(td[1]/a)][not(starts-with(td[1]/a,'~'))]"/>

      <tr><td colspan="3"><h2>Public member functions</h2></td></tr>
      <xsl:apply-templates select="tr[not(contains(td[1],'typedef'))][not(contains(td[3],'static'))][not(contains(td[3],'protected'))][not(contains(td[3],'private'))][not(contains(td[3],'friend'))][str:split(substring-before(concat(td[2]/a,'&lt;'),'&lt;'),'::')[position()=last()]!=string(td[1]/a)][not(starts-with(td[1]/a,'~'))][not(contains(td[3],'pure virtual'))]"/>

      <tr><td colspan="r"><h2>Public typedefs</h2></td></tr>
      <xsl:apply-templates select="tr[contains(td[1],'typedef')][not(contains(td[3],'protected'))][not(contains(td[3],'private'))][not(contains(td[3],'friend'))][str:split(substring-before(concat(td[2]/a,'&lt;'),'&lt;'),'::')[position()=last()]!=string(td[1]/a)][not(starts-with(td[1]/a,'~'))][not(contains(td[3],'pure virtual'))]"/>

      <tr><td colspan="3"><h2>Non-public members</h2></td></tr>
      <xsl:apply-templates select="tr[contains(td[3],'protected') or contains(td[3],'private') or contains(td[3],'friend')][str:split(substring-before(concat(td[2]/a,'&lt;'),'&lt;'),'::')[position()=last()]!=string(td[1]/a)][not(starts-with(td[1]/a,'~'))][not(contains(td[3],'pure virtual'))]"/>

    </table>
  </xsl:template>

  <xsl:template match="table[preceding-sibling::h1[1][contains(text(),'Member List')]]/tr/td[2]/a/text()[contains(.,'&lt;')]">
    <xsl:value-of select="substring-before(.,'&lt;')"/>
  </xsl:template>

  <xsl:template match="table[preceding-sibling::h1[1][contains(text(),'Member List')]]/tr/td[1]/a/text()[contains(.,'::')]">
    <xsl:value-of select="str:split(.,'::')[position()=last()]"/>
  </xsl:template>

  <!-- Remove the automatically inserted search form (we build our own) -->
  <xsl:template match="li[form]"> 
  </xsl:template>

  <xsl:template match="table[preceding-sibling::*[1][self::div][@class='qindex']]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">qindextable</xsl:with-param>
    </xsl:call-template>
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

  <xsl:template match="dl[dt/b/text()='Parameters:']|dl[dt/b/text()='Template Parameters:']">
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

  <!-- Insert 'senf/'  into include paths -->
  <xsl:template match="code[starts-with(text(),'#include &lt;')]/a">
    <xsl:copy>
      <xsl:call-template name="copy-attributes"/>
      <xsl:text>senf/</xsl:text>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
