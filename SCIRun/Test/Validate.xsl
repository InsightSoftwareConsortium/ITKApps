<?xml version="1.0"?> 
<xsl:stylesheet 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text" indent="yes"/>

<xsl:template match="/filter-itk">
<xsl:call-template name="includes"/>
<xsl:call-template name="create_validate"/>
<xsl:call-template name="create_main"/>
</xsl:template>


<!-- Set filter include file -->
<xsl:template name="includes">
<xsl:for-each select="/filter-itk/includes/file">
#include &lt;<xsl:value-of select="."/>&gt;

</xsl:for-each>
</xsl:template>


<!-- Create the validate function. This function is templated
     over the filter templates, the input types, the parameter
     types, and the output types. A filter will be instantiated
     and then all the inputs and parameters will be set. The
     get output calls will also be made.  This is only to check
     that the XML files are in sync with the Insight code and is
     not meant to be an actual working program.
-->
<xsl:template name="create_validate">
<xsl:text>template&lt;</xsl:text>

<!-- template for each template type, input, parameter, and output -->
<xsl:for-each select="/filter-itk/templated/template">
<xsl:choose>
   <xsl:when test="@type"><xsl:value-of select="@type"/> T<xsl:value-of select="position()"/>, </xsl:when>
   <xsl:otherwise>class T<xsl:value-of select="position()"/>, </xsl:otherwise>
</xsl:choose></xsl:for-each>
<xsl:for-each select="/filter-itk/inputs/input">class I<xsl:value-of select="position()"/>, </xsl:for-each>

<xsl:for-each select="/filter-itk/parameters/param">class P<xsl:value-of select="position()"/>, </xsl:for-each>

<xsl:for-each select="/filter-itk/outputs/output">class O<xsl:value-of select="position()"/><xsl:if test="position() &lt; last()">
<xsl:text>, </xsl:text>
</xsl:if></xsl:for-each><xsl:text>&gt;
void validate(</xsl:text><xsl:for-each select="/filter-itk/templated/template">
<xsl:choose>
   <xsl:when test="@type"></xsl:when>
   <xsl:otherwise>T<xsl:value-of select="position()"/> varT<xsl:value-of select="position()"/>, </xsl:otherwise>
</xsl:choose></xsl:for-each>
<xsl:for-each select="/filter-itk/inputs/input">I<xsl:value-of select="position()"/> varI<xsl:value-of select="position()"/>, </xsl:for-each>

<xsl:for-each select="/filter-itk/parameters/param">P<xsl:value-of select="position()"/> varP<xsl:value-of select="position()"/>, </xsl:for-each>

<xsl:for-each select="/filter-itk/outputs/output">O<xsl:value-of select="position()"/><xsl:if test="position() &lt; last()"> varO<xsl:value-of select="position()"/>
<xsl:text>, </xsl:text>
</xsl:if></xsl:for-each><xsl:text> ) {
</xsl:text>
  // instantiate the filter
  typename <xsl:value-of select="/filter-itk/@name"/>&lt;<xsl:for-each select="/filter-itk/templated/template">T<xsl:value-of select="position()"/><xsl:if test="position() &lt; last()">
<xsl:text>, </xsl:text>
</xsl:if></xsl:for-each>&gt;::Pointer filter = <xsl:value-of select="/filter-itk/@name"/>&lt;<xsl:for-each select="/filter-itk/templated/template">T<xsl:value-of select="position()"/><xsl:if test="position() &lt; last()">
<xsl:text>, </xsl:text>
</xsl:if></xsl:for-each>&gt;::New();

  // set inputs
<xsl:for-each select="/filter-itk/inputs/input">
  filter-><xsl:value-of select="call"/>( varI<xsl:value-of select="position()"/> );
</xsl:for-each>

  // set paramters
<xsl:for-each select="/filter-itk/parameters/param">
<xsl:variable name="type"><xsl:value-of select="type"/></xsl:variable>
<xsl:choose>
<xsl:when test="$type='bool'">
  <xsl:choose>
  <xsl:when test="count(call) = 2">
  filter-><xsl:value-of select="call[@value='on']"/>( );
  filter-><xsl:value-of select="call[@value='off']"/>( );
  </xsl:when>
  <xsl:otherwise>
  filter-><xsl:value-of select="call"/>( varP<xsl:value-of select="position()"/> );   
  </xsl:otherwise>
  </xsl:choose>
</xsl:when>
<xsl:otherwise>
  filter-><xsl:value-of select="call"/>( varP<xsl:value-of select="position()"/> );
</xsl:otherwise>
</xsl:choose>
</xsl:for-each>

  // set outputs
<xsl:for-each select="/filter-itk/outputs/output">
  filter-><xsl:value-of select="call"/>();
</xsl:for-each>
<xsl:text>
}
</xsl:text>
</xsl:template>

<xsl:template name="create_main">
int main() {
  return 1;
}
</xsl:template>

</xsl:stylesheet>
