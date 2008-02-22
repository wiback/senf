<?xml version="1.0"?>
<xsl:stylesheet 
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:str="http://exslt.org/strings"
  xmlns:func="http://exslt.org/functions"
  xmlns:exsl="http://exslt.org/common"
  extension-element-prefixes="str exsl func">
  
<func:function name="str:split">
  <xsl:param name="string" select="''" />
  <xsl:param name="pattern" select="' '" />
  <xsl:choose>
    <xsl:when test="not($string)">
      <func:result select="/.." />
    </xsl:when>
    <xsl:when test="not(function-available('exsl:node-set'))">
      <xsl:message terminate="yes">
        ERROR: EXSLT - Functions implementation of str:split relies on exsl:node-set().
      </xsl:message>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="tokens">
        <xsl:choose>
          <xsl:when test="not($pattern)">
            <xsl:call-template name="str:_split-characters">
              <xsl:with-param name="string" select="$string" />
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="str:_split-pattern">
              <xsl:with-param name="string" select="$string" />
              <xsl:with-param name="pattern" select="$pattern" />
            </xsl:call-template>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <func:result select="exsl:node-set($tokens)/token" />
    </xsl:otherwise>
  </xsl:choose>
</func:function>

<xsl:template name="str:_split-characters">
  <xsl:param name="string" />
  <xsl:if test="$string">
    <token><xsl:value-of select="substring($string, 1, 1)" /></token>
    <xsl:call-template name="str:_split-characters">
      <xsl:with-param name="string" select="substring($string, 2)" />
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="str:_split-pattern">
  <xsl:param name="string" />
  <xsl:param name="pattern" />
  <xsl:choose>
    <xsl:when test="contains($string, $pattern)">
      <xsl:if test="not(starts-with($string, $pattern))">
        <token><xsl:value-of select="substring-before($string, $pattern)" /></token>
      </xsl:if>
      <xsl:call-template name="str:_split-pattern">
        <xsl:with-param name="string" select="substring-after($string, $pattern)" />
        <xsl:with-param name="pattern" select="$pattern" />
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <token><xsl:value-of select="$string" /></token>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

  <!-- ==================================================================== -->
  <!-- node-set str:replace(string,object,object)                           -->
  <!--                                                                      -->
  <!-- This implements the EXSLT str:replace function                       -->
  <!--                                                                      -->
  <!-- Copyright Jeni Tenison                                               -->
  <!-- ==================================================================== -->

  <func:function name="str:replace">
     <xsl:param name="string" select="''" />
     <xsl:param name="search" select="/.." />
     <xsl:param name="replace" select="/.." />
     <xsl:choose>
        <xsl:when test="not($string)">
          <func:result select="/.." />
        </xsl:when>
        <xsl:when test="function-available('exsl:node-set')">
           <!-- this converts the search and replace arguments to node sets
                if they are one of the other XPath types -->
           <xsl:variable name="search-nodes-rtf">
             <xsl:copy-of select="$search" />
           </xsl:variable>
           <xsl:variable name="replace-nodes-rtf">
             <xsl:copy-of select="$replace" />
           </xsl:variable>
           <xsl:variable name="replacements-rtf">
              <xsl:for-each select="exsl:node-set($search-nodes-rtf)/node()">
                 <xsl:variable name="pos" select="position()" />
                 <replace search="{.}">
                    <xsl:copy-of select="exsl:node-set($replace-nodes-rtf)/node()[$pos]" />
                 </replace>
              </xsl:for-each>
           </xsl:variable>
           <xsl:variable name="sorted-replacements-rtf">
              <xsl:for-each select="exsl:node-set($replacements-rtf)/replace">
                 <xsl:sort select="string-length(@search)" data-type="number" order="descending" />
                 <xsl:copy-of select="." />
              </xsl:for-each>
           </xsl:variable>
           <xsl:variable name="result">
             <xsl:choose>
                <xsl:when test="not($search)">
                  <xsl:value-of select="$string" />
                </xsl:when>
               <xsl:otherwise>
                 <xsl:call-template name="str:_replace">
                    <xsl:with-param name="string" select="$string" />
                    <xsl:with-param name="replacements" select="exsl:node-set($sorted-replacements-rtf)/replace" />
                 </xsl:call-template>
               </xsl:otherwise>
             </xsl:choose>
           </xsl:variable>
           <func:result select="exsl:node-set($result)/node()" />
        </xsl:when>
        <xsl:otherwise>
           <xsl:message terminate="yes">
              ERROR: function implementation of str:replace() relies on exsl:node-set().
           </xsl:message>
        </xsl:otherwise>
     </xsl:choose>
  </func:function>

  <xsl:template name="str:_replace">
    <xsl:param name="string" select="''" />
    <xsl:param name="replacements" select="/.." />
    <xsl:choose>
      <xsl:when test="not($string)" />
      <xsl:when test="not($replacements)">
        <xsl:value-of select="$string" />
      </xsl:when>
      <xsl:otherwise>
        <xsl:variable name="replacement" select="$replacements[1]" />
        <xsl:variable name="search" select="$replacement/@search" />
        <xsl:choose>
          <xsl:when test="not(string($search))">
            <xsl:value-of select="substring($string, 1, 1)" />
            <xsl:copy-of select="$replacement/node()" />
            <xsl:call-template name="str:_replace">
              <xsl:with-param name="string" select="substring($string, 2)" />
              <xsl:with-param name="replacements" select="$replacements" />
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="contains($string, $search)">
            <xsl:call-template name="str:_replace">
              <xsl:with-param name="string" select="substring-before($string, $search)" />
              <xsl:with-param name="replacements" select="$replacements[position() > 1]" />
            </xsl:call-template>      
            <xsl:copy-of select="$replacement/node()" />
            <xsl:call-template name="str:_replace">
              <xsl:with-param name="string" select="substring-after($string, $search)" />
              <xsl:with-param name="replacements" select="$replacements" />
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="str:_replace">
              <xsl:with-param name="string" select="$string" />
              <xsl:with-param name="replacements" select="$replacements[position() > 1]" />
            </xsl:call-template>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>