<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:str="http://exslt.org/strings"
  xmlns:set="http://exslt.org/sets"
  xmlns:exslt="http://exslt.org/common"
  extension-element-prefixes="str">

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
  
  <!-- Add 'class' attribute  -->
  
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

  <!-- Reformat detailed member documentation -->

  <xsl:template match="table[@class='memname']">
    <xsl:variable name="name1"><xsl:value-of select="str:split(tr/td[@class='memname'],'::')[position()=last()]"/></xsl:variable>
    <xsl:variable name="name2"><xsl:value-of select="str:split(tr/td[@class='memname'],' ')[position()=last()]"/></xsl:variable>
    <xsl:variable name="name"><xsl:value-of select="substring($name1,string-length($name1)-string-length($name2))"/></xsl:variable>
    <xsl:element name="table">
      <xsl:attribute name="class">
        <xsl:text>memname</xsl:text>
        <xsl:if test="contains(tr/td[@class='memname'],'#define')"><xsl:text> macro</xsl:text></xsl:if>
      </xsl:attribute>
      <tr>
        <td class="memtype" colspan="5">
          <xsl:for-each select="tr/td[@class='memname']/*|tr/td[@class='memname']/text()">
            <xsl:choose>
              <xsl:when test="position()=last()">
                <xsl:value-of select="substring(.,1,string-length(.)-string-length($name))"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:copy-of select="."/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:for-each>
        </td>
      </tr>

      <xsl:choose>
        <xsl:when test="tr/td[@class='paramtype']">
          <tr>
            <td class="memname"><xsl:copy-of select="$name"/></td>
            <td class="memparen">(</td>
            <xsl:copy-of select="tr[1]/td[@class='paramtype']"/>
            <xsl:copy-of select="tr[1]/td[@class='paramname']"/>
            <td class="memparen"><xsl:if test="not(tr[td[@class='paramkey']])">)</xsl:if></td>
          </tr>

          <xsl:for-each select="tr[td[@class='paramkey']]">
            <tr>
              <td class="memname"></td>
              <td class="memparen"></td>
              <xsl:copy-of select="td[@class='paramtype']"/>
              <xsl:copy-of select="td[@class='paramname']"/>
              <td class="memparen"><xsl:if test="position()=last()">)</xsl:if></td>
            </tr>
          </xsl:for-each>
        </xsl:when>
        <xsl:otherwise>
          <tr>
            <td class="memname"><xsl:copy-of select="$name"/></td>
            <td class="memparen"><xsl:if test="tr/td[.='(']">()</xsl:if></td>
            <td class="paramtype"></td>
            <td class="paramname"></td>
            <td class="memparen"></td>
          </tr>
        </xsl:otherwise>
      </xsl:choose>
      <tr>
        <td colspan="5" class="memattrs"><xsl:copy-of select="tr/td[@width='100%']/*|tr/td[@width='100%']/text()"/></td>
      </tr>
    </xsl:element>
  </xsl:template>

  <!-- no idea, where these &nbsp;'s come frome ... -->
  <xsl:template match="div[@class='memproto']/text()[.='&#160;&#x0a;']">
  </xsl:template>

  <!-- Add grouping to all-members page -->

  <xsl:template match="table[preceding-sibling::h1[1][contains(text(),'Member List')]]">
    <table class="allmembers">

      <!-- We need to filter the table rows by looking for indications on the object type       -->
      <!-- The table has 3 acolumns:                                                            -->
      <!--    td[1] is the name of the object,                                                  -->
      <!--    td[2] is the name of the class the object is defined in                           -->
      <!--    td[3] contains additional flags                                                   -->
      <!--                                                                                      -->
      <!-- The conditions we have are:                                                          -->
      <!--                                                                                      -->
      <!--    contains(td[3],'static')        static member (variable or function)              -->
      <!--    contains(td[3],'protected')     protected member of arbitrary type                -->
      <!--    contains(td[3],'private')       private member of arbitrary type                  -->
      <!--    contains(td{3],'friend')        friend declaration (function or class)            -->
      <!--    contains(td[3],'pure virtual')  entry is a pure-virtual member function           -->
      <!--    starts-with(td[1]/text(),'(')   function entry (member, static or friend)         -->
      <!--    contains(td[1], 'typedef')      entry is a typdef                                 -->
      <!--    contains(td[1], 'enum ')        entry is enum type or enumerator                  -->
      <!--    contains(td[1], 'enum name')    entry is the name of an enum type                 -->
      <!--    contains(td[1], 'enum value')   entry is an enumerator value                      -->
      <!--    str:split(substring-before(concat(td[2]/a,'&lt;'),'&lt;'),'::')[position()=last()]==string(td[1]/a) -->
      <!--                                    entry is a constructor                            -->
      <!--    not(starts-with(td[1]/a,'~'))   entry is a destructor                             -->

      <xsl:variable name="public-static-memfn">
        <xsl:apply-templates select="tr[contains(td[3],'static')][not(contains(td[3],'protected'))][not(contains(td[3],'private'))][not(contains(td[3],'friend'))][starts-with(td[1]/text(),'(')]"/>
      </xsl:variable>
      <xsl:if test="string($public-static-memfn)">
        <tr><td colspan="3"><h2>Static Public Member Functions</h2></td></tr>
        <xsl:copy-of select="$public-static-memfn"/>
      </xsl:if>

      <xsl:variable name="public-static-var">
        <xsl:apply-templates select="tr[not(contains(td[1],'typedef'))][contains(td[3],'static')][not(contains(td[3],'protected'))][not(contains(td[3],'private'))][not(starts-with(td[1]/text(),'('))]"/>
      </xsl:variable>
      <xsl:if test="string($public-static-var)">
        <tr><td colspan="3"><h2>Static Public Attributes</h2></td></tr>
        <xsl:copy-of select="$public-static-var"/>
      </xsl:if>
      
      <xsl:variable name="public-memfn">
        <xsl:apply-templates select="tr[not(contains(td[1],'typedef'))][not(contains(td[3],'static'))][not(contains(td[3],'protected'))][not(contains(td[3],'private'))][not(contains(td[3],'friend'))][starts-with(td[1]/text(),'(')][str:split(substring-before(concat(td[2]/a,'&lt;'),'&lt;'),'::')[position()=last()]!=string(td[1]/a)][not(starts-with(td[1]/a,'~'))][not(contains(td[3],'pure virtual'))]"/>
      </xsl:variable>
      <xsl:if test="string($public-memfn)">
        <tr><td colspan="3"><h2>Public Member Functions</h2></td></tr>
        <xsl:copy-of select="set:distinct(exslt:node-set($public-memfn)/tr)"/>
      </xsl:if>

      <xsl:variable name="public-var">
        <xsl:apply-templates select="tr[not(contains(td[1],'typedef'))][not(contains(td[1],'enum '))][not(contains(td[3],'static'))][not(contains(td[3],'protected'))][not(contains(td[3],'private'))][not(starts-with(td[1]/text(),'('))]"/>
      </xsl:variable>
      <xsl:if test="string($public-var)">
        <tr><td colspan="3"><h2>Public Attributes</h2></td></tr>
        <xsl:copy-of select="$public-var"/>
      </xsl:if>
      
      <xsl:variable name="public-enum">
        <xsl:apply-templates select="tr[contains(td[1],'enum value')][not(contains(td[3],'protected'))][not(contains(td[3],'private'))]"/>
      </xsl:variable>
      <xsl:if test="string($public-enum)">
        <tr><td colspan="3"><h2>Public Enumerators</h2></td></tr>
        <xsl:copy-of select="$public-enum"/>
      </xsl:if>
      
      <xsl:variable name="public-type">
        <xsl:apply-templates select="tr[contains(td[1],'typedef') or contains(td[1],'enum name')][not(contains(td[3],'protected'))][not(contains(td[3],'private'))][not(contains(td[3],'friend'))]"/>
      </xsl:variable>
      <xsl:if test="string($public-type)">
        <tr><td colspan="r"><h2>Public Types</h2></td></tr>
        <xsl:copy-of select="$public-type"/>
      </xsl:if>
      
      <xsl:variable name="non-public">
        <xsl:apply-templates select="tr[contains(td[3],'protected') or contains(td[3],'private') or contains(td[3],'friend')][str:split(substring-before(concat(td[2]/a,'&lt;'),'&lt;'),'::')[position()=last()]!=string(td[1]/a)][not(starts-with(td[1]/a,'~'))][not(contains(td[3],'pure virtual'))]"/>
      </xsl:variable>
      <xsl:if test="string($non-public)">
        <tr><td colspan="3"><h2>Non-Public Members</h2></td></tr>
        <xsl:copy-of select="$non-public"/>
      </xsl:if>

    </table>
  </xsl:template>

  <xsl:template match="table[preceding-sibling::h1[1][contains(text(),'Member List')]]/tr/td[2]/a/text()[contains(.,'&lt;')]">
    <!-- this removes the template args form the second column to make the table more compact -->
    <xsl:value-of select="substring-before(.,'&lt;')"/>
  </xsl:template>

  <xsl:template match="table[preceding-sibling::h1[1][contains(text(),'Member List')]]/tr/td[1]/a/text()[contains(.,'::')]">
    <!-- for some weird reason, some members have a fully qualified name in the first column -->
    <!-- remove the qualification here -->
    <xsl:value-of select="str:split(.,'::')[position()=last()]"/>
  </xsl:template>

  <!-- Remove the automatically inserted search form (we build our own) -->
  <xsl:template match="li[form]"> 
  </xsl:template>

  <!-- Add CSS class to alphabetical class index table -->
  <xsl:template match="table[preceding-sibling::*[1][self::div][@class='qindex']]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">qindextable</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Add CSS class to special paragraphs -->

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

  <!-- Break the following lists after each komma -->

  <xsl:template match="p[starts-with(text(),'Inherited by ')]">
    <xsl:call-template name="break-comma"/>
  </xsl:template>

  <xsl:template match="p[starts-with(text(),'Inherits ')]">
    <xsl:call-template name="break-comma"/>
  </xsl:template>

  <!-- Add CSS class to the member overview table of a class documentation -->
  <xsl:template match="table[descendant::td[@class='memItemLeft']]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">members</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Add CSS class to literal links (links, where link text and href attribute are the same -->
  <xsl:template match="a[@href=string(current())]" priority="1">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">literal</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Add CSS class to external links -->
  <xsl:template match="a[contains(@href,'http://')]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">ext</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Add CSS class to anchor-only links -->
  <xsl:template match="a[not(@href)]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">anchor</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Add CSS class to the brief documentation paragraph of the member documentation -->
  <xsl:template match="div[@class='memdoc']/p[1][not(contains(.,'Definition at line'))]">
    <xsl:call-template name="add-class">
      <xsl:with-param name="class">memtitle</xsl:with-param>
    </xsl:call-template>
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
