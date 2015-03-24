<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE stylesheet [ <!ENTITY n "&#x0A;"> ]>
<xsl:stylesheet 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:carmen="http://satcom.fokus.fraunhofer.de/CARMEN/parsexml/1.0"
  version="1.0">

  <xsl:output method="text"/>

  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="parameters"/>
  <xsl:template match="text()" mode="enable"/>
  <xsl:template match="text()" mode="interface"/>
  <xsl:template match="text()" mode="coordinates"/>
  <xsl:template match="text()" mode="register"/>

  <xsl:template match="/">
    <xsl:document href="coordinates.conf" method="text">
      <xsl:apply-templates mode="coordinates"/>
    </xsl:document>
    <xsl:apply-templates/>
  </xsl:template>

  <!-- ========================================================================================= -->
  <!-- Node coordinates (coordinates.conf)                                                       -->
  <!-- ========================================================================================= -->

  <!-- This template matches every node having a gpsCoordinate child                             -->
  <xsl:template mode="coordinates" match="node[gpsCoordinates]">
    <xsl:text>nodeCoordinates </xsl:text><xsl:value-of select="@nodeId"/>
    <xsl:text> </xsl:text><xsl:value-of select="gpsCoordinates/latitude/text()"/>
    <xsl:text> </xsl:text><xsl:value-of select="gpsCoordinates/longitude/text()"/>
    <xsl:text>;&n;</xsl:text>
  </xsl:template>

  <!-- ========================================================================================= -->
  <!-- Node configuration                                                                        -->
  <!-- ========================================================================================= -->

  <!-- This template creates a new configuration file for each node found.                       -->
  <!-- It calls the interface template for each interface of that node.                          -->
  <xsl:template match="node">
    <xsl:document href="node-{@nodeId}.conf" method="text">
      <xsl:text>id </xsl:text><xsl:value-of select="@nodeId"/><xsl:text>;&n;</xsl:text>
      <xsl:apply-templates mode="interface"/>
    </xsl:document>
  </xsl:template>

  <!-- ========================================================================================= -->
  <!-- Interfaces                                                                                -->

  <!-- = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -->
  <!-- emulatedWLANInterface                                                                     -->

  <xsl:template mode="interface" match="interface[./emulatedRadio][substring-before(./macType,'_') = 'WLAN']">
    <xsl:text>interfaces/emulatedWLANInterface {&n;</xsl:text>
    <xsl:apply-templates mode="register">
      <xsl:with-param name="type" select="'WLAN'"/>
    </xsl:apply-templates>
    <xsl:apply-templates mode="parameters"/>
    <xsl:apply-templates mode="enable"/>
    <xsl:text>}&n;</xsl:text>
  </xsl:template>

  <!-- = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -->
  <!-- emulatedDVBSenderInterface                                                                -->

  <xsl:template mode="interface" match="interface[./emulatedRadio][./macType='DVB_SENDER']">
    <xsl:text>interfaces/emulatedDVBSenderInterface {&n;</xsl:text>
    <xsl:apply-templates mode="register">
      <xsl:with-param name="type" select="'DVBT'"/>
    </xsl:apply-templates>
    <xsl:apply-templates mode="parameters"/>
    <xsl:apply-templates mode="enable"/>
    <xsl:text>}&n;</xsl:text>
  </xsl:template>

  <!-- = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -->
  <!-- emulatedDVBReceiverInterface                                                              -->

  <xsl:template mode="interface" match="interface[./emulatedRadio][./macType='DVB_RECEIVER']">
    <xsl:text>interfaces/emulatedDVBReceiverInterface {&n;</xsl:text>
    <xsl:apply-templates mode="register">
      <xsl:with-param name="type" select="'DVBT'"/>
    </xsl:apply-templates>
    <xsl:apply-templates mode="parameters"/>
    <xsl:apply-templates mode="enable"/>
    <xsl:text>}&n;</xsl:text>
  </xsl:template>

  <!-- = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -->
  <!-- hardwareWLANInterface                                                                     -->

  <xsl:template mode="interface" match="interface[./realRadio]">
    <xsl:text>interfaces/hardwareWLANInterface {&n;</xsl:text>
    <xsl:text>    device </xsl:text><xsl:value-of select="@name"/><xsl:text>;&n;</xsl:text>
    <xsl:apply-templates mode="parameters"/>
    <xsl:apply-templates mode="enable"/>
    <xsl:text>}&n;</xsl:text>
  </xsl:template>

  <!-- = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -->
  <!-- hardwareEthernetInterface                                                                 -->

  <xsl:template mode="interface" match="interface[./realFixed]">
    <xsl:text>interfaces/hardwareEthernetInterface {&n;</xsl:text>
    <xsl:text>    device </xsl:text><xsl:value-of select="@name"/><xsl:text>;&n;</xsl:text>
    <xsl:apply-templates mode="parameters"/>
    <xsl:apply-templates mode="enable"/>
    <xsl:text>}&n;</xsl:text>
  </xsl:template>

  <!-- = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -->
  <!-- Registration of configuration options                                                     -->

  <!-- We anchor this to availableFrequencies even though it should be just called ... -->
  <xsl:template mode="register" match="availableFrequencies">
    <xsl:variable name="base" select=".."/>
    <xsl:text>    registerFrequency ( </xsl:text>
    <xsl:for-each select="$base/availableFrequencies/frequency">
      <xsl:value-of select=".*1000"/>
      <xsl:text> </xsl:text>
    </xsl:for-each>
    <xsl:text>) ( </xsl:text>
    <xsl:for-each select="$base/availableBandwidths/bandwidth">
      <xsl:value-of select=".*1000"/>
      <xsl:text> </xsl:text>
    </xsl:for-each>
    <xsl:text>);&n;</xsl:text>
  </xsl:template>

  <xsl:template mode="register" match="availableModulations/modulation">
    <xsl:param name="type" select="''"/>
    <xsl:variable name="pm" select="document('')//carmen:parameters[@type=$type][@modulation=current()]"/>
    <xsl:text>    registerParameter "</xsl:text>
    <xsl:value-of select="$pm/@modulation"/>
    <xsl:text>" </xsl:text>
    <xsl:value-of select="$pm/@efficiency"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$pm/@minrssi"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$pm/@minsnr"/>
    <xsl:text>;&n;</xsl:text>
  </xsl:template>

  <xsl:template mode="register" match="availableTxpower">
    <xsl:text>    registerTxPower ( </xsl:text>
    <xsl:for-each select="txPower">
      <xsl:value-of select="."/>
      <xsl:text> </xsl:text>
    </xsl:for-each>
    <xsl:text>);&n;</xsl:text>
  </xsl:template>

  <!-- = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -->
  <!-- Interface configuration parameters                                                        -->

  <!-- hardwareAddress -->
  <xsl:template mode="parameters" match="l2Addr[./text()!='RANDOM']">
    <xsl:text>    hardwareAddress </xsl:text><xsl:value-of select="."/><xsl:text>;&n;</xsl:text>
  </xsl:template>

  <!-- frequency / bandwidth-->
  <xsl:template mode="parameters" match="preConfig/frequency">
    <xsl:text>    frequency </xsl:text>
    <xsl:value-of select=".*1000"/>
    <xsl:if test="../bandwidth">
      <xsl:text> </xsl:text>
      <xsl:value-of select="../bandwidth"/>
    </xsl:if>
    <xsl:text>000;&n;</xsl:text>
  </xsl:template>

  <!-- parameter (modulation) -->
  <xsl:template mode="parameters" match="preConfig/modulation[text()!='AUTO']">
    <xsl:text>    parameter "</xsl:text>
    <xsl:value-of select="text()"/>
    <xsl:text>";&n;</xsl:text>
  </xsl:template>

  <!-- parameter (txPower) -->
  <xsl:template mode="parameters" match="preConfig/txPower">
    <xsl:text>    txpower </xsl:text>
    <xsl:value-of select="text()"/>
    <xsl:text>;&n;</xsl:text>
  </xsl:template>
  
  <!-- enable -->
  <!-- This entry is in a special mode since it must be placed at the end of the interface       -->
  <!-- configuration independent of where it is in the XML file                                  -->
  <xsl:template mode="enable" match="enabled[text()='true']">
    <xsl:text>    enable;&n;</xsl:text>
  </xsl:template>

  <!-- ========================================================================================= -->
  <!-- Parameter table                                                                           -->
  <!-- ========================================================================================= -->
  
  <!-- This table is referenced via document('') to get the complete parameter configuration     -->
  <!-- given the technology and modulation.                                                      -->

  <carmen:parameters type="WLAN" modulation="BPSK_1/2"  efficiency="300"  minrssi="-82" minsnr="1"/>
  <carmen:parameters type="WLAN" modulation="BPSK_3/4"  efficiency="450"  minrssi="-81" minsnr="2"/>
  <carmen:parameters type="WLAN" modulation="QPSK_1/2"  efficiency="600"  minrssi="-79" minsnr="3"/>
  <carmen:parameters type="WLAN" modulation="QPSK_3/4"  efficiency="900"  minrssi="-77" minsnr="4"/>
  <carmen:parameters type="WLAN" modulation="16QAM_1/2" efficiency="1200" minrssi="-74" minsnr="5"/>
  <carmen:parameters type="WLAN" modulation="16QAM_3/4" efficiency="1800" minrssi="-70" minsnr="6"/>
  <carmen:parameters type="WLAN" modulation="64QAM_2/3" efficiency="2400" minrssi="-66" minsnr="7"/>
  <carmen:parameters type="WLAN" modulation="64QAM_3/4" efficiency="2700" minrssi="-65" minsnr="8"/>

  <carmen:parameters type="DVBT" modulation="TEST"      efficiency="1000" minrssi="-70" minsnr="5"/>

</xsl:stylesheet>
