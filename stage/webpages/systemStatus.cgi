#!/usr/bin/perl
use Sys::Hostname;
require "getopts.pl";

&Getopts("h:");

$hostname = hostname();
$packedHostIP = gethostbyname($hostname);
($a, $b, $c, $d) = unpack('C4', $packedHostIP);
$hostIP = $a . "." . $b . "." . $c . "." . $d;

if($opt_h) {
    $hostIP = $opt_h;
}

$port = "33606";
$width = "320";
$height = "240";
#$class = "vocal.systemStatus.SNMPApplet.class";
$class = "vocal.systemStatus.SNMPApplet";
#for some reason the codebase has to be the name, not IP
#$codebase = "http://${hostname}/jarfiles";
$codebase = "jarfiles";
$vocalJar = "SNMPClient.jar";
$name = "SystemStatus";

print "content-type: text/html\n\n";
print <<"EndOfFile";
<HTML>
<HEAD>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=iso-8859-1">
</HEAD>
<BODY>
<!--"CONVERTED_APPLET"-->
<!-- HTML CONVERTER -->
<SCRIPT LANGUAGE="JavaScript"><!--
    var _info = navigator.userAgent; 
    var _ns = false; 
    var _ns6 = false;
    var _ie = (_info.indexOf("MSIE") > 0 && _info.indexOf("Win") > 0 && _info.indexOf("Windows 3.1") < 0);
//--></SCRIPT>
    <COMMENT>
        <SCRIPT LANGUAGE="JavaScript1.1"><!--
        var _ns = (navigator.appName.indexOf("Netscape") >= 0 && ((_info.indexOf("Win") > 0 && _info.indexOf("Win16") < 0 && java.lang.System.getProperty("os.version").indexOf("3.5") < 0) || (_info.indexOf("Sun") > 0) || (_info.indexOf("Linux") > 0) || (_info.indexOf("AIX") > 0) || (_info.indexOf("OS/2") > 0)));
        var _ns6 = ((_ns == true) && (_info.indexOf("Mozilla/5") >= 0));
//--></SCRIPT>
    </COMMENT>

<SCRIPT LANGUAGE="JavaScript"><!--
    if (_ie == true) document.writeln('<OBJECT classid="clsid:8AD9C840-044E-11D1-B3E9-00805F499D93" WIDTH = ${width} HEIGHT = ${height} NAME = ${name} ALIGN = middle VSPACE = 0 HSPACE = 0  codebase="http://java.sun.com/products/plugin/autodl/jinstall-1_4-win.cab#Version=1,4,0,0"><NOEMBED><XMP>');
    else if (_ns == true && _ns6 == false) document.writeln('<EMBED 
	    type="application/x-java-applet;version=1.4" 
            CODE = ${class}
            CODEBASE = ${codebase}
            ARCHIVE = ${vocalJar}
            NAME = ${name}
            WIDTH = ${width}
            HEIGHT = ${height}
            ALIGN = middle
            VSPACE = 0
            HSPACE = 0 
            host =  ${hostIP}
            port =  ${port}
            debug_level =  "7" 
	    scriptable=false 
	    pluginspage="http://java.sun.com/products/plugin/index.html#download"><NOEMBED><XMP>');
//--></SCRIPT>
<APPLET  CODE = ${class} CODEBASE = ${codebase} ARCHIVE = ${vocalJar} WIDTH = ${width} HEIGHT = ${height} NAME = ${name} ALIGN = middle VSPACE = 0 HSPACE = 0></XMP>
    <PARAM NAME = CODE VALUE = ${class} >
<PARAM NAME = CODEBASE VALUE = ${codebase} >
<PARAM NAME = ARCHIVE VALUE = ${vocalJar} >
<PARAM NAME = NAME VALUE = ${name} >

    <PARAM NAME="type" VALUE="application/x-java-applet;version=1.4">
    <PARAM NAME="scriptable" VALUE="false">
    <PARAM NAME = "host" VALUE   = ${hostIP}>
    <PARAM NAME = "port" VALUE   = ${port}>
    <PARAM NAME = "debug_level" VALUE   = "7">


</APPLET>
</NOEMBED>
</EMBED>
</OBJECT>

<!--
<APPLET CODE = ${class} CODEBASE = ${codebase} ARCHIVE = ${vocalJar} WIDTH = ${width} HEIGHT = ${height} NAME = ${name} ALIGN = middle VSPACE = 0 HSPACE = 0>
<PARAM NAME = "host" VALUE   = ${hostIP}>
<PARAM NAME = "port" VALUE   = ${port}>
<PARAM NAME = "debug_level" VALUE   = "7">


</APPLET>
-->
<!--"END_CONVERTED_APPLET"-->

</BODY>
</HTML>

EndOfFile
