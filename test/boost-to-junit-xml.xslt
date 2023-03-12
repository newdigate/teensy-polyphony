<?xml version="1.0" encoding="utf-8"?>
<!--  [Stuart Lange](https://stackoverflow.com/a/2975928/4634140) -->
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:msxsl="urn:schemas-microsoft-com:xslt"
                exclude-result-prefixes="msxsl">

  <xsl:output method="xml"
              indent="yes"/>

  <xsl:template match="TestResult">
      <xsl:call-template name="testSuite" />
  </xsl:template>

  <xsl:template name="testSuite">
    <xsl:for-each select="TestSuite">
      <testsuite>
        <xsl:attribute name="errors">
            <xsl:value-of select="@test_cases_failed + @test_cases_aborted"/>
        </xsl:attribute>

        <xsl:attribute name="tests">
            <xsl:value-of select="@test_cases_passed + @test_cases_failed + @test_cases_skipped + @test_cases_aborted"/>
        </xsl:attribute>

        <xsl:attribute name="skipped">
            <xsl:value-of select="@test_cases_skipped"/>
        </xsl:attribute>

        <xsl:attribute name="failures">
            <xsl:value-of select="@test_cases_failed"/>
        </xsl:attribute>

        <xsl:call-template name="testAttributes" />
        <!--results-->
          <xsl:call-template name="testSuite" />
          <xsl:for-each select="TestCase">
            <testcase>
              <xsl:call-template name="testAttributes" />
              <xsl:call-template name="testCaseElements" />
            </testcase>
          </xsl:for-each>
        <!--/results-->
      </testsuite>
    </xsl:for-each>
  </xsl:template>

  <xsl:template name="testAttributes">
    <xsl:attribute name="name">
      <xsl:value-of select="@name"/>
    </xsl:attribute>
    <xsl:attribute name="success">
      <xsl:choose>
        <xsl:when test="@result = 'passed'">True</xsl:when>
        <xsl:when test="@result != 'passed'">False</xsl:when>
      </xsl:choose>
    </xsl:attribute>
    <xsl:attribute name="executed">True</xsl:attribute>
    <xsl:attribute name="time">0</xsl:attribute>
    <xsl:attribute name="asserts">
      <xsl:value-of select="@assertions_failed + @assertions_passed"/>
    </xsl:attribute>
  </xsl:template>

  <xsl:template name="testCaseElements">
    <xsl:if test="@result != 'passed'">
        <failure type="No type reported" message="No message reported"/>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>