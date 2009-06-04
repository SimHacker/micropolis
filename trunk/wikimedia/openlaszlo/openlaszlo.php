<?php

////////////////////////////////////////////////////////////////////////
// OpenLaszlo MediaWiki Plug-In
// By Don Hopkins.
// Licensed under GPL v3.
//
// To install, add the following to your LocalSettings.php:
// require_once("extensions/openlaszlo/openlaszlo.php");


$wgExtensionFunctions[] = 'OpenLaszloExtension';
$wgExtensionCredits['parserhook'][] = array(
    'name'=>'OpenLaslzo',
    'author'=>'Don Hopkins',
    'url'=>'http://www.DonHopkins.com',
    'description'=>'Embed OpenLaszlo applications in MediaWiki.'
);

function OpenLaszloExtension() {
    global $wgParser;
    //install parser hook for <openlaszlo> tags
    $wgParser->sethook('openlaszlo','DoOpenLaszlo');
}


function DoOpenLaszlo($input, $argv, &$parser) {
    if (!$parser) {
        $parser =& $GLOBALS['wgParser'];
    }

    global $wgOutputEncoding;
    $DefaultEncoding='ISO-8859-1';

    $src = htmlspecialchars(@$argv['src']);
    $width = htmlspecialchars(@$argv['width']);
    $height = htmlspecialchars(@$argv['height']);
    $flashvars = htmlspecialchars(@$argv['flashvars']);
    
    $out =
        "<object type=\"application/x-shockwave-flash\" data=\"$src\" width=\"$width\" height=\"$height\"><param name=\"movie\" value=\"$src\"><param name=\"FlashVars\" value=\"$flashvars\"><p>This is supposed to be a flash animation. You'll need the flash plugin and a browser that supports it to view it.</object>";

    if (!isset($openlaszloUsed)) {
        $out .=
	    "<div style=\"height:0;outline;0;padding:0;margin:0;overflow:hidden;\"><object type=\"application/x-shockwave-flash\" width=\"1\" height=\"1\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,0,0\"><param name=\"pluginspage\" value=\"http://www.macromedia.com/go/getflashplayer\"></object></div>";
    }

    $GLOBALS['openlaszloUsed'] = true;
    
    return $out;
}
