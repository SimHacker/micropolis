var $runtime="dhtml";var $dhtml=true;var $as3=false;var $as2=false;var $swf10=false;var $j2me=false;var $debug=false;var $js1=true;var $backtrace=false;var $swf7=false;var $swf9=false;var $swf8=false;var $svg=false;var $profile=false;try{
if(lz){}}
catch(e){
lz={}};lz.embed={options:{cancelkeyboardcontrol:false,serverroot:null,approot:"",usemastersprite:false},swf:function($0,$1){
var $2=lz.embed;if($1==null)$1=10.1;if(!$0.id){
$0.id="lzapp"+Math.round(Math.random()*1000000)
};var $3=$0.url;var $4=this.__getqueryurl($3);for(var $5 in $4.options){
var $6=$4.options[$5];if($6!=null){
$0[$5]=$6
}};if($0.accessible&&$0.accessible!="false"){
$4.flashvars+="&accessible=true"
};if($0.history){
$4.flashvars+="&history=true"
};if($0.bgcolor!=null){
$4.flashvars+="&bgcolor="+escape($0.bgcolor)
};var $7=lz.embed.options;if($0.cancelkeyboardcontrol){
$7.cancelkeyboardcontrol=$0.cancelkeyboardcontrol
};$4.flashvars+="&width="+escape($0.width);$4.flashvars+="&height="+escape($0.height);$4.flashvars+="&__lzurl="+escape($3);$4.flashvars+="&__lzminimumversion="+escape($1);$4.flashvars+="&id="+escape($0.id);var $3=$4.url+"?"+$4.query;var $8=$2._getAppendDiv($0.id,$0.appenddivid);var $9={width:"100%",height:"100%",id:$0.id,bgcolor:$0.bgcolor,wmode:$0.wmode,flashvars:$4.flashvars,allowfullscreen:$0.allowfullscreen,flash8:$3,appenddiv:$8};if($2[$0.id])alert("Warning: an app with the id: "+$0.id+" already exists.");var app=$2[$0.id]=$2.applications[$0.id]={runtime:"swf",_id:$0.id,appenddiv:$8,setCanvasAttribute:$2._setCanvasAttributeSWF,getCanvasAttribute:$2._getCanvasAttributeSWF,callMethod:$2._callMethodSWF,_ready:$2._ready,_onload:[],_getSWFDiv:$2._getSWFDiv,loaded:false,_sendMouseWheel:$2._sendMouseWheel,_sendAllKeysUp:$2._sendAllKeysUpSWF,_setCanvasAttributeDequeue:$2._setCanvasAttributeDequeue,_setCanvasAttributeQ:[],_sendPercLoad:$2._sendPercLoad,setGlobalFocusTrap:$2.__setGlobalFocusTrapSWF,initargs:$4.initargs};if($0.history==false){
$2.history.active=false
};$2.dojo.addLoadedListener($2._loaded,app);$2.dojo.setSwf($9,$1);$8.style.height=$2.CSSDimension($0.height);$8.style.width=$2.CSSDimension($0.width);if($0.cancelmousewheel!=true){
if($2["mousewheel"]){
$2.mousewheel.setCallback(app,"_sendMouseWheel")
}};if(($9.wmode=="transparent"||$9.wmode=="opaque")&&$2.browser.OS=="Windows"&&($2.browser.isOpera||$2.browser.isFirefox)){
var div=$9.appenddiv;div.onmouseout=function($0){
div.mouseisoutside=true
};div.onmouseover=function($0){
div.mouseisoutside=false
};div._gotmouseup=document.onmouseup=function($0){
if(div.mouseisoutside){
app.callMethod("lz.GlobalMouse.__mouseUpOutsideHandler()")
}}};if($2.browser.isIE&&$3.indexOf("swf8")==-1&&!$7.cancelkeyboardcontrol){
document.onkeydown=function($0){
if(!$0)$0=window.event;if($0.keyCode==9){
app.callMethod("lz.Keys.__browserTabEvent("+$0.shiftKey+")");return false
}}}},__swfSetAppAppendDivStyle:function($0,$1,$2){
var $3=lz.embed.applications[$0].appenddiv;return $3.style[$1]=$2
},lfc:function($0,$1){
if($1==""){
$1="."
}else if(!$1||typeof $1!="string"){
alert("WARNING: lz.embed.lfc() requires a valid serverroot to be specified.");return
};lz.embed.options.serverroot=$1;if(lz.embed.browser.isIE){
if(!window["G_vmlCanvasManager"]){
alert('WARNING: excanvas.js was not loaded, and is required for IE DHTML.  Please ensure your HTML wrapper has a script include in the <head></head>, e.g. <!--[if IE]><script type="text/javascript" src="'+$1+'lps/includes/excanvas.js"></script><![endif]-->"')
};if(lz.embed.browser.version<7){
this.__dhtmlLoadLibrary("http://ajax.googleapis.com/ajax/libs/chrome-frame/1/CFInstall.min.js")
}};lz.embed.lfcloaded=false;this.__dhtmlLoadLibrary($0,lz.embed.__lfcloaded)
},dhtml:function($0){
var $1=lz.embed;if($1.dhtmlapploaded){
alert("Warning: skipping lz.embed.dhtml() call for "+$0.url+". Only one DHTML application can be loaded per window.  Use iframes to load more than one DHTML application.");return
};if(!$0.id){
$0.id="lzapp"+Math.round(Math.random()*1000000)
};var $2=this.__getqueryurl($0.url);for(var $3 in $2.options){
var $4=$2.options[$3];if($4!=null){
$0[$3]=$4
}};var $5=$2.url+"?lzt=object&"+$2.query;var appenddiv=$1._getAppendDiv($0.id,$0.appenddivid);if(!$0.skipchromeinstall&&$1.browser.isIE&&$1.browser.version<7){
if(window["CFInstall"]){
CFInstall.check({onmissing:function(){
appenddiv.style.display="none"
},oninstall:function(){
window.location=window.location
}})
}};appenddiv.style.height=$1.CSSDimension($0.height);appenddiv.style.width=$1.CSSDimension($0.width);var $6=$1.options;if($0.cancelkeyboardcontrol){
$6.cancelkeyboardcontrol=$0.cancelkeyboardcontrol
};if($0.serverroot!=null){
$6.serverroot=$0.serverroot
};if($0.approot!=null&&typeof $0.approot=="string"){
$6.approot=$0.approot
};if($0.usemastersprite!=null){
$6.usemastersprite=$0.usemastersprite
};$1.__propcache={bgcolor:$0.bgcolor,width:$0.width,height:$0.height,id:$0.id,appenddiv:$1._getAppendDiv($0.id,$0.appenddivid),url:$5,options:$6};if($1[$0.id])alert("Warning: an app with the id: "+$0.id+" already exists.");var $7=$1[$0.id]=$1.applications[$0.id]={runtime:"dhtml",_id:$0.id,_ready:$1._ready,_onload:[],loaded:false,setCanvasAttribute:$1._setCanvasAttributeDHTML,getCanvasAttribute:$1._getCanvasAttributeDHTML,_setCanvasAttributeDequeue:$1._setCanvasAttributeDequeue,_setCanvasAttributeQ:[],callMethod:$1._callMethodDHTML,_sendAllKeysUp:$1._sendAllKeysUpDHTML,initargs:$2.initargs};if($0.history==false){
$1.history.active=false
};$1.mousewheel.setEnabled(!$0.cancelmousewheel);if($1.browser.OS=="Windows"&&$1.browser.isFirefox){
window.focus()
};if(!$1.lfcloaded){
$1.__appqueue.push($5);if($0.lfcurl){
$1.lfc($0.lfcurl,$6.serverroot)
}else if($1.lfcloaded!=null){}}else{
$1.dhtmlapploaded=true;this.__dhtmlLoadLibrary($5)
}},applications:{},__dhtmlLoadScript:function($0){
var $1='<script type="text/javascript" language="JavaScript1.5" src="'+$0+'"></script>';document.writeln($1);return $1
},__dhtmlLoadLibrary:function($0,callback){
var script=document.createElement("script");this.__setAttr(script,"type","text/javascript");this.__setAttr(script,"defer","defer");if(callback){
if(script.readyState){
script.onreadystatechange=function(){
if(script.readyState=="loaded"||script.readyState=="complete"){
script.onreadystatechange=null;callback()
}}}else{
script.onload=function(){
callback()
}}};this.__setAttr(script,"src",$0);document.getElementsByTagName("head")[0].appendChild(script)
},__getqueryurl:function($0){
var $1=$0.split("?");$0=$1[0];if($1.length==1)return {url:$0,flashvars:"",query:"",initargs:{}};var $2=this.__parseQuery($1[1]);var $3="";var $4="";var $5={};var $6={};var $7=new RegExp("\\+","g");for(var $8 in $2){
if($8==""||$8==null)continue;var $9=$2[$8];if($8=="lzr"||$8=="lzt"||$8=="debug"||$8=="profile"||$8=="lzbacktrace"||$8=="lzconsoledebug"||$8=="lzdebug"||$8=="lzkrank"||$8=="lzprofile"||$8=="lzcopyresources"||$8=="fb"||$8=="sourcelocators"||$8=="_canvas_debug"||$8=="lzsourceannotations"){
$3+=$8+"="+$9+"&"
};if($8=="lzusemastersprite"||$8=="lzskipchromeinstall"||$8=="lzcancelkeyboardcontrol"||$8=="lzcancelmousewheel"||$8=="lzhistory"||$8=="lzaccessible"){
$5[$8.substring(2)]=$9=="true"
};if($8=="lzapproot"||$8=="lzserverroot"||$8=="lzwmode"){
$5[$8.substring(2)]=$9
};if($6[$8]==null){
$6[$8]=unescape($9.replace($7," "))
};$4+=$8+"="+$9+"&"
};$3=$3.substr(0,$3.length-1);$4=$4.substr(0,$4.length-1);return {url:$0,flashvars:$4,query:$3,options:$5,initargs:$6}},__parseQuery:function($0){
if($0.indexOf("=")==-1)return;var $1=$0.split("&");var $2={};for(var $3=0;$3<$1.length;$3++){
var $4=$1[$3].split("=");if($4.length==1)continue;var $5=$4[0];var $6=$4[1];$2[$5]=$6
};return $2
},__setAttr:function($0,$1,$2){
$0.setAttribute($1,$2)
},_setCanvasAttributeSWF:function($0,$1,$2){
if(this.loaded&&lz.embed.dojo.comm[this._id]&&lz.embed.dojo.comm[this._id]["callMethod"]){
if($2){
lz.embed.history._store($0,$1)
}else{
lz.embed.dojo.comm[this._id].setCanvasAttribute($0,$1+"")
}}else{
this._setCanvasAttributeQ.push([$0,$1,$2])
}},_setCanvasAttributeDHTML:function($0,$1,$2){
if(this.loaded&&canvas){
if($2){
lz.embed.history._store($0,$1)
}else if(canvas){
canvas.setAttribute($0,$1)
}}else{
this._setCanvasAttributeQ.push([$0,$1,$2])
}},_loaded:function($0){
if(lz.embed[$0].loaded)return;if(lz.embed.dojo.info.commVersion==8){
setTimeout('lz.embed["'+$0+'"]._ready.call(lz.embed["'+$0+'"])',100)
}else{
lz.embed[$0]._ready.call(lz.embed[$0])
}},_setCanvasAttributeDequeue:function(){
while(this._setCanvasAttributeQ.length>0){
var $0=this._setCanvasAttributeQ.pop();this.setCanvasAttribute($0[0],$0[1],$0[2])
}},_ready:function($0){
this.loaded=true;if(this._callmethod){
for(var $1=0;$1<this._callmethod.length;$1++){
this.callMethod(this._callmethod[$1])
};this._callmethod=null
};if(this._setCanvasAttributeQ.length>0){
this._setCanvasAttributeDequeue()
};if($0)this.canvas=$0;for(var $1=0;$1<this._onload.length;$1++){
var $2=this._onload[$1];if(typeof $2=="function")$2(this)
};if(this.onload&&typeof this.onload=="function"){
this.onload(this)
}},_getCanvasAttributeSWF:function($0){
if(this.loaded){
return lz.embed.dojo.comm[this._id].getCanvasAttribute($0)
}else{
alert("Flash is not ready: getCanvasAttribute"+$0)
}},_getCanvasAttributeDHTML:function($0){
return canvas[$0]
},browser:{init:function(){
if(this.initted)return;this.browser=this.searchString(this.dataBrowser)||"An unknown browser";this.version=this.searchVersion(navigator.userAgent)||this.searchVersion(navigator.appVersion)||"an unknown version";this.osversion=this.searchOSVersion(navigator.userAgent)||"an unknown osversion";this.subversion=this.searchSubVersion(navigator.userAgent);this.OS=this.searchString(this.dataOS)||"an unknown OS";this.initted=true;this.isNetscape=this.isSafari=this.isOpera=this.isFirefox=this.isIE=this.isIphone=this.isChrome=false;if(this.browser=="Netscape"){
this.isNetscape=true
}else if(this.browser=="Safari"){
this.isSafari=true
}else if(this.browser=="Opera"){
this.isOpera=true
}else if(this.browser=="Firefox"){
this.isFirefox=true
}else if(this.browser=="Explorer"){
this.isIE=true
}else if(this.browser=="iPhone"||this.browser=="iPad"){
this.isSafari=true;this.isIphone=true
}else if(this.OS=="Android"){
this.isSafari=true
}else if(this.browser=="Chrome"){
this.isChrome=true
}},searchString:function($0){
for(var $1=0;$1<$0.length;$1++){
var $2=$0[$1].string;var $3=$0[$1].prop;this.versionSearchString=$0[$1].versionSearch||$0[$1].identity;this.osversionSearchString=$0[$1].osversionSearch||"";if($2){
if($2.indexOf($0[$1].subString)!=-1)return $0[$1].identity
}else if($3)return $0[$1].identity
}},searchVersion:function($0){
var $1=$0.indexOf(this.versionSearchString);if($1==-1)return;return parseFloat($0.substring($1+this.versionSearchString.length+1))
},searchSubVersion:function($0){
var $1=new RegExp(this.versionSearchString+".\\d+\\.\\d+\\.([\\d.]+)");var $2=$1.exec($0);if($2&&$2.length>1)return parseFloat($2[1])
},searchOSVersion:function($0){
var $1=$0.indexOf(this.osversionSearchString);if($1==-1)return;return parseFloat($0.substring($1+this.osversionSearchString.length+1))
},dataBrowser:[{string:navigator.userAgent,subString:"iPhone",identity:"iPhone",versionSearch:"WebKit"},{string:navigator.userAgent,subString:"iPad",identity:"iPad",versionSearch:"WebKit"},{string:navigator.userAgent,subString:"Android",identity:"Android",versionSearch:"WebKit"},{string:navigator.userAgent,subString:"Chrome",identity:"Chrome",versionSearch:"WebKit"},{string:navigator.userAgent,subString:"OmniWeb",versionSearch:"OmniWeb/",identity:"OmniWeb"},{string:navigator.vendor,subString:"Apple",identity:"Safari",versionSearch:"WebKit"},{string:navigator.userAgent,identity:"Opera",versionSearch:"Version"},{string:navigator.vendor,subString:"iCab",identity:"iCab"},{string:navigator.vendor,subString:"KDE",identity:"Konqueror"},{string:navigator.userAgent,subString:"Firefox",identity:"Firefox"},{string:navigator.userAgent,subString:"Iceweasel",versionSearch:"Iceweasel",identity:"Firefox"},{string:navigator.vendor,subString:"Camino",identity:"Camino"},{string:navigator.userAgent,subString:"Netscape",identity:"Netscape"},{string:navigator.userAgent,subString:"MSIE",identity:"Explorer",versionSearch:"MSIE",osversionSearch:"Windows NT"},{string:navigator.userAgent,subString:"Gecko",identity:"Mozilla",versionSearch:"rv"},{string:navigator.userAgent,subString:"Mozilla",identity:"Netscape",versionSearch:"Mozilla"}],dataOS:[{string:navigator.platform,subString:"Win",identity:"Windows"},{string:navigator.platform,subString:"Mac",identity:"Mac"},{string:navigator.userAgent,subString:"iPhone",identity:"iPhone/iPod/iPad"},{string:navigator.userAgent,subString:"iPad",identity:"iPhone/iPod/iPad"},{string:navigator.userAgent,subString:"Android",identity:"Android"},{string:navigator.platform,subString:"Linux",identity:"Linux"}]},_callMethodSWF:function($0){
if(this.loaded){
return lz.embed.dojo.comm[this._id].callMethod($0)
}else{
if(!this._callmethod)this._callmethod=[];this._callmethod.push($0)
}},_callMethodDHTML:function($0){
if(this.loaded){
return eval($0)
}else{
if(!this._callmethod)this._callmethod=[];this._callmethod.push($0)
}},_broadcastMethod:function($0){
var $1=[].slice.call(arguments,1);for(var $2 in lz.embed.applications){
var $3=lz.embed.applications[$2];if($3[$0]){
$3[$0].apply($3,$1)
}}},setCanvasAttribute:function($0,$1,$2){
lz.embed._broadcastMethod("setCanvasAttribute",$0,$1,$2)
},callMethod:function($0){
lz.embed._broadcastMethod("callMethod",$0)
},_getAppendDiv:function($0,$1){
var $2=$1?$1:$0+"Container";var $3=document.getElementById($2);if(!$3){
$3=document.createElement("div");this.__setAttr($3,"id",$2);var $4=document.getElementsByTagName("script");var $5=$4[$4.length-1];if(!$5){
document.body.appendChild($3)
}else{
$5.parentNode.insertBefore($3,$5.nextSibling)
}}else{
$3.innerHTML=""
};return $3
},_getSWFDiv:function(){
return lz.embed.dojo.obj[this._id].get()
},_sendMouseWheel:function($0){
if($0!=null)this.callMethod("lz.Keys.__mousewheelEvent("+$0+")")
},_gotFocus:function(){
lz.embed._broadcastMethod("_sendAllKeysUp")
},_sendAllKeysUpSWF:function(){
this.callMethod("lz.Keys.__allKeysUp()")
},_sendAllKeysUpDHTML:function(){
if(lz["Keys"]&&lz.Keys["__allKeysUp"]){
lz.Keys.__allKeysUp()
}},_sendPercLoad:function($0){
if(this.onloadstatus&&typeof this.onloadstatus=="function"){
this.onloadstatus($0)
}},attachEventHandler:function($0,$1,callbackscope,callbackname,closure){
if(!(callbackscope&&callbackname&&typeof callbackscope[callbackname]=="function")){
return
};var $2=$0+$1+callbackscope+callbackname;var $3=this._handlers[$2];if($3!=null){
if($3 instanceof Array){
for(var $4=$3.length-1;$4>=0;--$4){
if($3[$4].$e===$0&&$3[$4].$c===callbackscope){
return
}}}else{
if($3.$e===$0&&$3.$c===callbackscope){
return
}}};var $5=function(){
var $0=window.event?[window.event]:[].slice.call(arguments,0);if(closure)$0.push(closure);callbackscope[callbackname].apply(callbackscope,$0)
};$5.$e=$0;$5.$c=callbackscope;if($3!=null){
if($3 instanceof Array){
$3.push($5)
}else{
$3=[$3,$5]
}}else{
$3=$5
};this._handlers[$2]=$3;if($0["addEventListener"]){
$0.addEventListener($1,$5,false);return true
}else if($0["attachEvent"]){
return $0.attachEvent("on"+$1,$5)
}},removeEventHandler:function($0,$1,$2,$3){
var $4=$0+$1+$2+$3;var $5,$6=this._handlers[$4];if($6!=null){
if($6 instanceof Array){
for(var $7=$6.length-1;$7>=0;--$7){
if($6[$7].$e===$0&&$6[$7].$c===$2){
$5=$6[$7];$6.splice($7,1);if($6.length==0){
delete this._handlers[$4]
}}}}else if($6.$e===$0&&$6.$c===$2){
$5=$6;delete this._handlers[$4]
}};if(!$5){
return
};if($0["removeEventListener"]){
$0.removeEventListener($1,$5,false);return true
}else if($0["detachEvent"]){
return $0.detachEvent("on"+$1,$5)
}},_handlers:{},_cleanupHandlers:function(){
lz.embed._handlers={}},getAbsolutePosition:function($0){
var $1=null;var $2={};var $3;var $4=lz.embed.browser;if(!($4.isFirefox&&$0==document.body)&&$0.getBoundingClientRect){
if(!$0.parentNode){
return {x:0,y:0}};$3=$0.getBoundingClientRect();var $5=document.documentElement.scrollTop||document.body.scrollTop;var $6=document.documentElement.scrollLeft||document.body.scrollLeft;return {x:Math.floor($3.left+$6),y:Math.floor($3.top+$5)}}else if(document.getBoxObjectFor){
$3=document.getBoxObjectFor($0);$2={x:$3.x,y:$3.y}}else{
$2={x:$0.offsetLeft,y:$0.offsetTop};$1=$0.offsetParent;if($1!=$0){
while($1){
$2.x+=$1.offsetLeft;$2.y+=$1.offsetTop;$1=$1.offsetParent
}};if($4.isSafari&&document.defaultView&&document.defaultView.getComputedStyle){
var $7=document.defaultView.getComputedStyle($0,"")
};if($4.isOpera||$4.isSafari){
$2.y-=document.body.offsetTop
}};if($0.parentNode){
$1=$0.parentNode
}else{
return $2
};while($1&&$1.tagName!="BODY"&&$1.tagName!="HTML"){
$2.x-=$1.scrollLeft;$2.y-=$1.scrollTop;if($1.parentNode){
$1=$1.parentNode
}else{
return $2
}};return $2
},CSSDimension:function($0,$1){
var $2=$0;if(isNaN($0)){
if($0.indexOf("%")==$0.length-1&&!isNaN($0.substring(0,$0.length-1))){
return $0
}else{
$2=0
}}else if($0===Infinity){
$2=~0>>>1
}else if($0===-Infinity){
$2=~(~0>>>1)
};return $2+($1?$1:"px")
},__setGlobalFocusTrapSWF:function($0){
var div=this._getSWFDiv();if($0){
div.onblur=function(){
div.focus()
}}else{
div.onblur=null
}},__unescapestring:function($0){
var $1=arguments[0];var $2=eval($1);var $3=null;var $4=$1.lastIndexOf(".");if($4>-1){
$3=eval($1.substring(0,$4))
};if(!$2||(function($0,$1){
return $1["$lzsc$isa"]?$1.$lzsc$isa($0):$0 instanceof $1
})(!$2,Function)){
return
};var $5=[];for(var $6=1,$7=arguments.length;$6<$7;$6++){
var $8=arguments[$6];if($8=="__#lznull"){
$8=""
};$5[$6-1]=$8
};var $9=$2.apply($3,$5);if($9==""){
return "__#lznull"
}else{
return $9
}},__lfcloaded:function(){
lz.embed.lfcloaded=true;var $0=lz.embed.__appqueue;if($0.length){
for(var $1=0,$2=$0.length;$1<$2;$1++){
lz.embed.__dhtmlLoadLibrary($0[$1])
}}},__appqueue:[],resizeWindow:function($0,$1){
if($0.indexOf("%")>-1){
$0=null
}else{
$0=parseInt($0)
};if($1.indexOf("%")>-1){
$1=null
}else{
$1=parseInt($1)
};if(window.innerHeight){
window.resizeTo($0?$0+window.outerWidth-window.innerWidth:window.outerWidth,$1?$1+window.outerHeight-window.innerHeight:window.outerHeight)
}}};lz.embed.browser.init();lz.embed.attachEventHandler(window,"beforeunload",lz.embed,"_cleanupHandlers");lz.embed.attachEventHandler(window,"focus",lz.embed,"_gotFocus");if(lz.embed.browser.isIE){
lz.embed.attachEventHandler(window,"activate",lz.embed,"_gotFocus")
};lz.embed.dojo=function(){};lz.embed.dojo={defaults:{flash8:null,ready:false,visible:true,width:500,height:400,bgcolor:"#ffffff",allowfullscreen:false,wmode:"window",flashvars:"",minimumVersion:8,id:"flashObject",appenddiv:null},obj:{},comm:{},_loadedListeners:[],_loadedListenerScopes:[],_installingListeners:[],_installingListenerScopes:[],setSwf:function($0,$1){
if($0==null){
return
};var $2={};for(var $3 in this.defaults){
var $4=$0[$3];if($4!=null){
$2[$3]=$4
}else{
$2[$3]=this.defaults[$3]
}};if($1!=null){
this.minimumVersion=$1
};this._initialize($2)
},addLoadedListener:function($0,$1){
this._loadedListeners.push($0);this._loadedListenerScopes.push($1)
},addInstallingListener:function($0,$1){
this._installingListeners.push($0);this._installingListenerScopes.push($1)
},loaded:function($0){
var $1=lz.embed.dojo;if($1._isinstaller){
top.location=top.location+""
};$1.info.installing=false;$1.ready=true;if($1._loadedListeners.length>0){
for(var $2=0;$2<$1._loadedListeners.length;$2++){
var $3=$1._loadedListenerScopes[$2];if($0!=$3._id)continue;lz.embed.dojo._loadedListeners[$2].apply($3,[$3._id])
}}},installing:function(){
var $0=lz.embed.dojo;if($0._installingListeners.length>0){
for(var $1=0;$1<$0._installingListeners.length;$1++){
var $2=$0._installingListenerScopes[$1];$0._installingListeners[$1].apply($2,[$2._id])
}}},_initialize:function($0){
var $1=lz.embed.dojo;var $2=new ($1.Install)($0.id);$1.installer=$2;var $3=new ($1.Embed)($0);$1.obj[$0.id]=$3;if($2.needed()==true){
$2.install()
}else{
$3.write($1.info.commVersion);$1.comm[$0.id]=new ($1.Communicator)($0.id)
}}};lz.embed.dojo.Info=function(){
if(lz.embed.browser.isIE){
document.writeln('<script language="VBScript" type="text/vbscript">');document.writeln("Function VBGetSwfVer(i)");document.writeln("  on error resume next");document.writeln("  Dim swControl, swVersion");document.writeln("  swVersion = 0");document.writeln('  set swControl = CreateObject("ShockwaveFlash.ShockwaveFlash." + CStr(i))');document.writeln("  if (IsObject(swControl)) then");document.writeln('    swVersion = swControl.GetVariable("$version")');document.writeln("  end if");document.writeln("  VBGetSwfVer = swVersion");document.writeln("End Function");document.writeln("</script>")
};this._detectVersion();this._detectCommunicationVersion()
};lz.embed.dojo.Info.prototype={version:-1,versionMajor:-1,versionMinor:-1,versionRevision:-1,capable:false,commVersion:8,installing:false,isVersionOrAbove:function($0,$1,$2){
if(this.versionMajor<9){
$2=parseFloat("."+$2)
};if(this.versionMajor>=$0&&this.versionMinor>=$1&&this.versionRevision>=$2){
return true
}else{
return false
}},_detectVersion:function(){
var $0;var $1=lz.embed.browser.isIE;for(var $2=25;$2>0;$2--){
if($1){
$0=VBGetSwfVer($2)
}else{
$0=this._JSFlashInfo($2)
};if($0==-1){
this.capable=false;return
}else if($0!=0){
var $3;if($1){
var $4=$0.split(" ");var $5=$4[1];$3=$5.split(",")
}else{
$3=$0.split(".")
};this.versionMajor=$3[0];this.versionMinor=$3[1];this.versionRevision=$3[2];var $6=this.versionMajor+"."+this.versionMinor;this.version=parseFloat($6);this.capable=true;break
}}},_JSFlashInfo:function($0){
if(navigator.plugins!=null&&navigator.plugins.length>0){
if(navigator.plugins["Shockwave Flash 2.0"]||navigator.plugins["Shockwave Flash"]){
var $1=navigator.plugins["Shockwave Flash 2.0"]?" 2.0":"";var $2=navigator.plugins["Shockwave Flash"+$1].description;var $3=$2.split(" ");var $4=$3[2].split(".");var $5=$4[0];var $6=$4[1];if($3[3]!=""){
var $7=$3[3].split("r")
}else{
var $7=$3[4].split("r")
};var $8=$7[1]>0?$7[1]:0;var $9=$5+"."+$6+"."+$8;return $9
}};return -1
},_detectCommunicationVersion:function(){
if(this.capable==false){
this.commVersion=null;return
}}};lz.embed.dojo.Embed=function($0){
this.properties=$0;if(!this.properties.width)this.properties.width="100%";if(!this.properties.height)this.properties.height="100%";if(!this.properties.bgcolor)this.properties.bgcolor="#ffffff";if(!this.properties.visible)this.properties.visible=true
};lz.embed.dojo.Embed.prototype={protocol:function(){
switch(window.location.protocol){
case "https:":
return "https";break;
default:
return "http";break;

}},__getCSSValue:function($0){
if($0&&$0.length&&$0.indexOf("%")!=-1){
return "100%"
}else{
return $0+"px"
}},write:function($0,$1){
var $2="";$2+="width: "+this.__getCSSValue(this.properties.width)+";";$2+="height: "+this.__getCSSValue(this.properties.height)+";";if(this.properties.visible==false){
$2+="position: absolute; ";$2+="z-index: 10000; ";$2+="top: -1000px; ";$2+="left: -1000px; "
};var $3;var $4;var $5=lz.embed;if($0>$5.dojo.version)$1=true;$4=this.properties.flash8;var $6=this.properties.flashvars;var $7=this.properties.flashvars;if($1){
var $8=escape(window.location);document.title=document.title.slice(0,47)+" - Flash Player Installation";var $9=escape(document.title);$6+="&MMredirectURL="+$8+"&MMplayerType=ActiveX"+"&MMdoctitle="+$9;$7+="&MMredirectURL="+$8+"&MMplayerType=PlugIn"
};if($5.browser.isIE){
$3='<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" '+'codebase="'+this.protocol()+"://fpdownload.macromedia.com/pub/shockwave/cabs/flash/"+'swflash.cab#version=8,0,0,0" '+'width="'+this.properties.width+'" '+'height="'+this.properties.height+'" '+'id="'+this.properties.id+'" '+'align="middle"> '+'<param name="allowScriptAccess" value="sameDomain" /> '+'<param name="movie" value="'+$4+'" /> '+'<param name="quality" value="high" /> '+'<param name="FlashVars" value="'+$6+'" /> '+'<param name="bgcolor" value="'+this.properties.bgcolor+'" /> '+'<param name="wmode" value="'+this.properties.wmode+'" /> '+'<param name="allowFullScreen" value="'+this.properties.allowfullscreen+'" /> '+"</object>"
}else{
$3='<embed src="'+$4+'" '+'quality="high" '+'bgcolor="'+this.properties.bgcolor+'" '+'wmode="'+this.properties.wmode+'" '+'allowFullScreen="'+this.properties.allowfullscreen+'" '+'width="'+this.properties.width+'" '+'height="'+this.properties.height+'" '+'id="'+this.properties.id+'" '+'name="'+this.properties.id+'" '+'FlashVars="'+$7+'" '+'swLiveConnect="true" '+'align="middle" '+'allowScriptAccess="sameDomain" '+'type="application/x-shockwave-flash" '+'pluginspage="'+this.protocol()+'://www.macromedia.com/go/getflashplayer" />'
};var $a=this.properties.id+"Container";var $b=this.properties.appenddiv;if($b){
$b.innerHTML=$3;$b.setAttribute("style",$2)
}else{
$3='<div id="'+$a+'" style="'+$2+'"> '+$3+"</div>";document.writeln($3)
}},get:function(){
try{
var $0=document.getElementById(this.properties.id+"")
}
catch($1){};return $0
},setVisible:function($0){
var $1=document.getElementById(this.properties.id+"Container");if($0==true){
$1.style.visibility="visible"
}else{
$1.style.position="absolute";$1.style.x="-1000px";$1.style.y="-1000px";$1.style.visibility="hidden"
}},center:function(){
var $0=this.properties.width;var $1=this.properties.height;var $2=0;var $3=0;var $4=document.getElementById(this.properties.id+"Container");$4.style.top=$3+"px";$4.style.left=$2+"px"
}};lz.embed.dojo.Communicator=function($0){
this._id=$0
};lz.embed.dojo.Communicator.prototype={_addExternalInterfaceCallback:function(methodName,id){
var dojo=lz.embed.dojo;var $0=function(){
var $0=[];for(var $1=0;$1<arguments.length;$1++){
$0[$1]=arguments[$1]
};$0.length=arguments.length;return dojo.comm[id]._execFlash(methodName,$0,id)
};dojo.comm[id][methodName]=$0
},_encodeData:function($0){
var $1=RegExp("\\&([^;]*)\\;","g");$0=$0.replace($1,"&amp;$1;");$0=$0.replace(RegExp("<","g"),"&lt;");$0=$0.replace(RegExp(">","g"),"&gt;");$0=$0.replace("\\","&custom_backslash;");$0=$0.replace(RegExp("\\n","g"),"\\n");$0=$0.replace(RegExp("\\r","g"),"\\r");$0=$0.replace(RegExp("\\f","g"),"\\f");$0=$0.replace(RegExp("\\0","g"),"\\0");$0=$0.replace(RegExp("\\'","g"),"\\'");$0=$0.replace(RegExp('\\"',"g"),'\\"');return $0
},_decodeData:function($0){
if($0==null||typeof $0=="undefined"){
return $0
};$0=$0.replace(RegExp("\\&custom_lt\\;","g"),"<");$0=$0.replace(RegExp("\\&custom_gt\\;","g"),">");$0=eval('"'+$0+'"');return $0
},_chunkArgumentData:function($0,$1,$2){
var $3=lz.embed.dojo.obj[$2].get();var $4=Math.ceil($0.length/1024);for(var $5=0;$5<$4;$5++){
var $6=$5*1024;var $7=$5*1024+1024;if($5==$4-1){
$7=$5*1024+$0.length
};var $8=$0.substring($6,$7);$8=this._encodeData($8);$3.CallFunction('<invoke name="chunkArgumentData" '+'returntype="javascript">'+"<arguments>"+"<string>"+$8+"</string>"+"<number>"+$1+"</number>"+"</arguments>"+"</invoke>")
}},_chunkReturnData:function($0){
var $1=lz.embed.dojo.obj[$0].get();var $2=$1.getReturnLength();var $3=[];for(var $4=0;$4<$2;$4++){
var $5=$1.CallFunction('<invoke name="chunkReturnData" '+'returntype="javascript">'+"<arguments>"+"<number>"+$4+"</number>"+"</arguments>"+"</invoke>");if($5=='""'||$5=="''"){
$5=""
}else{
$5=$5.substring(1,$5.length-1)
};$3.push($5)
};var $6=$3.join("");return $6
},_execFlash:function($0,$1,$2){
var $3=lz.embed.dojo.obj[$2].get();$3.startExec();$3.setNumberArguments($1.length);for(var $4=0;$4<$1.length;$4++){
this._chunkArgumentData($1[$4],$4,$2)
};$3.exec($0);var $5=this._chunkReturnData($2);$5=this._decodeData($5);$3.endExec();return $5
}};lz.embed.dojo.Install=function($0){
this._id=$0
};lz.embed.dojo.Install.prototype={needed:function(){
var $0=lz.embed.dojo;if($0.info.capable==false){
return true
};if(lz.embed.browser.isSafari==true&&!$0.info.isVersionOrAbove(8,0,0)){
return true
};if($0.minimumVersion>$0.info.version){
return true
};if(!$0.info.isVersionOrAbove(8,0,0)){
return true
};return false
},install:function(){
var $0=lz.embed.dojo;$0.info.installing=true;$0.installing();var $1=$0.obj[this._id].properties;var $2=$1.flash8;var $3=$2.indexOf("swf7");if($3!=-1){
$0._tempurl=$2;$2=$2.substring(0,$3+3)+"8"+$2.substring($3+4,$2.length);$1.flash8=$2
};var $3=$2.indexOf("swf9");if($3!=-1){
$0._tempurl=$2;$2=$2.substring(0,$3+3)+"8"+$2.substring($3+4,$2.length);$1.flash8=$2
};$0.ready=false;if($0.info.capable==false){
$0._isinstaller=true;var $4=new ($0.Embed)($1);$4.write($0.minimumVersion)
}else if($0.info.isVersionOrAbove(6,0,65)){
var $4=new ($0.Embed)($1);$4.write($0.minimumVersion,true);$4.setVisible(true);$4.center()
}else{
alert("This content requires a more recent version of the Macromedia "+" Flash Player.");window.location="http://www.macromedia.com/go/getflashplayer"
}},_onInstallStatus:function($0){
if($0=="Download.Complete"){
if(lz.embed.browser.isIE){
top.location=top.location+""
}}else if($0=="Download.Cancelled"){
alert("This content requires a more recent version of the Macromedia "+" Flash Player.");window.location="http://www.macromedia.com/go/getflashplayer"
}else if($0=="Download.Failed"){
alert("There was an error downloading the Flash Player update. "+"Please try again later, or visit macromedia.com to download "+"the latest version of the Flash plugin.");window.location="http://www.macromedia.com/go/getflashplayer"
}}};lz.embed.dojo.info=new (lz.embed.dojo.Info)();lz.embed.iframemanager={__counter:0,__frames:{},__namebyid:{},__loading:{},__callqueue:{},__calljsqueue:{},__sendmouseevents:{},__hidenativecontextmenu:{},__selectionbookmarks:{},create:function($0,$1,$2,$3,$4,$5){
var $6="__lz"+lz.embed.iframemanager.__counter++;var $7='javascript:""';var $8='lz.embed.iframemanager.__gotload("'+$6+'")';if($1==null||$1=="null"||$1=="")$1=$6;lz.embed.iframemanager.__namebyid[$6]=$1;if($3==null||$3=="undefined"){
$3=document.body
};if(document.all){
var $9="<iframe name='"+$1+"' id='"+$6+"' src='"+$7+"' onload='"+$8+"' frameBorder='0'";if($2!=true)$9+=" scrolling='no'";$9+="></iframe>";var $a=document.createElement("div");lz.embed.__setAttr($a,"id",$6+"Container");$3.appendChild($a);$a.style.position="absolute";$a.style.display="none";$a.style.top="0px";$a.style.left="0px";$a.innerHTML=$9;var $b=document.getElementById($6)
}else{
var $b=document.createElement("iframe");lz.embed.__setAttr($b,"name",$1);lz.embed.__setAttr($b,"src",$7);lz.embed.__setAttr($b,"id",$6);lz.embed.__setAttr($b,"onload",$8);lz.embed.__setAttr($b,"tabindex","-1");if($2!=true)lz.embed.__setAttr($b,"scrolling","no");this.appendTo($b,$3)
};if($b){
this.__finishCreate($6,$0,$1,$2,$3,$4,$5)
}else{
this.__callqueue[$6]=[["__finishCreate",$6,$0,$1,$2,$3,$4,$5]];setTimeout('lz.embed.iframemanager.__checkiframe("'+$6+'")',10)
};return $6+""
},__checkiframe:function($0){
var $1=document.getElementById($0);if($1){
var $2=lz.embed.iframemanager.__callqueue[$0];delete lz.embed.iframemanager.__callqueue[$0];lz.embed.iframemanager.__playQueue($2)
}else{
setTimeout('lz.embed.iframemanager.__checkiframe("'+$0+'")',10)
}},__playQueue:function($0){
var $1=lz.embed.iframemanager;for(var $2=0;$2<$0.length;$2++){
var $3=$0[$2];var $4=$3.splice(0,1);$1[$4].apply($1,$3)
}},__finishCreate:function($0,$1,$2,$3,$4,$5,$6){
var $7=document.getElementById($0);if(typeof $1=="string"){
$7.appcontainer=lz.embed.applications[$1]._getSWFDiv()
};$7.owner=$1;lz.embed.iframemanager.__frames[$0]=$7;this.__namebyid[$0]=$2;var $8=lz.embed.iframemanager.getFrame($0);$8.__gotload=lz.embed.iframemanager.__gotload;$8._defaultz=$5?$5:99900;this.setZ($0,$8._defaultz);lz.embed.iframemanager.__topiframe=$0;if(document.getElementById&&!document.all){
$8.style.border="0"
}else if(document.all){
lz.embed.__setAttr($8,"allowtransparency","true");var $9=lz.embed[$8.owner];if($9&&$9.runtime=="swf"){
var $a=$9._getSWFDiv();$a.onfocus=lz.embed.iframemanager.__refresh
}};$8.style.position="absolute"
},appendTo:function($0,$1){
if($1.__appended==$1)return;if($0.__appended){
old=$0.__appended.removeChild($0);$1.appendChild(old)
}else{
$1.appendChild($0)
};$0.__appended=$1
},getFrame:function($0){
return lz.embed.iframemanager.__frames[$0]
},getFrameWindow:function($0){
if(!this["framesColl"]){
if(document.frames){
this.framesColl=document.frames
}else{
this.framesColl=window.frames
}};return this.framesColl[$0]
},setSrc:function($0,$1,$2){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["setSrc",$0,$1,$2]);return
};this.__setSendMouseEvents($0,false);if($2){
var $3=lz.embed.iframemanager.getFrame($0);if(!$3)return;lz.embed.__setAttr($3,"src",$1)
}else{
var $0=lz.embed.iframemanager.__namebyid[$0];var $3=window[$0];if(!$3)return;$3.location.replace($1)
};this.__loading[$0]=true
},setPosition:function($0,$1,$2,$3,$4,$5,$6){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["setPosition",$0,$1,$2,$3,$4,$5,$6]);return
};var $7=lz.embed.iframemanager.getFrame($0);if(!$7)return;if($7.appcontainer){
var $8=lz.embed.getAbsolutePosition($7.appcontainer)
}else{
var $8={x:0,y:0}};if($1!=null&&!isNaN($1))$7.style.left=$1+$8.x+"px";if($2!=null&&!isNaN($2))$7.style.top=$2+$8.y+"px";if($3!=null&&!isNaN($3))$7.style.width=$3+"px";if($4!=null&&!isNaN($4))$7.style.height=$4+"px";if($5!=null){
if(typeof $5=="string"){
$5=$5=="true"
};$7.style.display=$5?"block":"none"
};if($6!=null)this.setZ($0,$6+$7._defaultz)
},setVisible:function($0,$1){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["setVisible",$0,$1]);return
};if(typeof $1=="string"){
$1=$1=="true"
};var $2=lz.embed.iframemanager.getFrame($0);if(!$2)return;$2.style.display=$1?"block":"none"
},bringToFront:function($0){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["bringToFront",$0]);return
};var $1=lz.embed.iframemanager.getFrame($0);if(!$1)return;$1._defaultz=100000;this.setZ($0,$1._defaultz);lz.embed.iframemanager.__topiframe=$0
},sendToBack:function($0){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["sendToBack",$0]);return
};var $1=lz.embed.iframemanager.getFrame($0);if(!$1)return;$1._defaultz=99900;this.setZ($0,$1._defaultz)
},__gotload:function($0){
var $1=lz.embed.iframemanager.getFrame($0);if(!$1||!$1.owner)return;if($1.owner.__gotload){
$1.owner.__gotload()
}else{
if(lz.embed[$1.owner]){
lz.embed[$1.owner].callMethod("lz.embed.iframemanager.__gotload('"+$0+"')")
}else{
return
}};this.__loading[$0]=false;if(document.all){
if($1.parentElement){
$1.parentElement.style.display=""
}};if(this.__sendmouseevents[$0]){
this.__setSendMouseEvents($0,true)
};if(this.__calljsqueue[$0]){
this.__playQueue(this.__calljsqueue[$0]);delete this.__calljsqueue[$0]
}},__refresh:function(){
var $0=lz.embed.iframemanager.__frames;for(var $1 in $0){
var $2=$0[$1];if($2&&$2.style.display=="block"){
$2.style.display="none";$2.style.display="block"
}}},setZ:function($0,$1){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["setZ",$0,$1]);return
};var $2=lz.embed.iframemanager.getFrame($0);if(!$2)return;$2.style.zIndex=$1
},scrollBy:function($0,$1,$2){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["scrollBy",$0,$1,$2]);return
};var $0=lz.embed.iframemanager.__namebyid[$0];var $3=window.frames[$0];if(!$3)return;$3.scrollBy($1,$2)
},__destroy:function($0){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["__destroy",$0]);return
};var $1=lz.embed.iframemanager.__frames[$0];if($1){
this.__setSendMouseEvents($0,false);$1.owner=null;$1.appcontainer=null;LzSprite.prototype.__discardElement($1);delete lz.embed.iframemanager.__frames[$0];delete lz.embed.iframemanager.__namebyid[$0]
}},callJavascript:function($0,$1,$2,$3){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["callJavascript",$0,$1,$2,$3]);return
};if(this.__loading[$0]){
if(!this.__calljsqueue[$0]){
this.__calljsqueue[$0]=[]
};this.__calljsqueue[$0].push(["callJavascript",$0,$1,$2,$3]);return
};var $4=lz.embed.iframemanager.getFrameWindow($0);if(!$3)$3=[];try{
var $5=$4.eval($1);if($5){
var $6=$5.apply($4,$3);if($2)$2.execute($6);return $6
}}
catch($7){}},__mouseEvent:function($0,$1){
var $2=lz.embed;var $3=$2.iframemanager.getFrame($1);if(!$3)return;if(!$0){
$0=window.event
};var $4="on"+$0.type;if($3.owner&&$3.owner.sprite&&$3.owner.sprite.__mouseEvent){
if($4=="oncontextmenu"){
if(!$2.iframemanager.__hidenativecontextmenu[$1]){
return
}else{
var $5=$2.getAbsolutePosition($3);LzMouseKernel.__sendMouseMove($0,$5.x,$5.y);return LzMouseKernel.__showContextMenu($0)
}};$3.owner.sprite.__mouseEvent($0);if($4=="onmouseup"){
if(LzMouseKernel.__lastMouseDown==$3.owner.sprite){
LzMouseKernel.__lastMouseDown=null
}}}else{
if($4=="onmouseleave"){
$4="onmouseout"
}else if($4=="onmouseenter"){
$4="onmouseover"
}else if($4=="oncontextmenu"){
return
};$2[$3.owner].callMethod("lz.embed.iframemanager.__gotMouseEvent('"+$1+"','"+$4+"')")
}},setSendMouseEvents:function($0,$1){
if(this.__callqueue[$0]){
this.__callqueue[$0].push(["setSendMouseEvents",$0,$1]);return
};this.__sendmouseevents[$0]=$1
},__setSendMouseEvents:function(id,$0){
var iframe=lz.embed.iframemanager.getFrameWindow(id);if(!iframe){
return
};if($0){
try{
lz.embed.attachEventHandler(iframe.document,"mousedown",lz.embed.iframemanager,"__mouseEvent",id);lz.embed.attachEventHandler(iframe.document,"mouseup",lz.embed.iframemanager,"__mouseEvent",id);lz.embed.attachEventHandler(iframe.document,"click",lz.embed.iframemanager,"__mouseEvent",id);iframe.document.oncontextmenu=function($0){
if(!$0)$0=iframe.event;return lz.embed.iframemanager.__mouseEvent($0,id)
};if(lz.embed.browser.isIE){
lz.embed.attachEventHandler(iframe.document,"mouseenter",lz.embed.iframemanager,"__mouseEvent",id);lz.embed.attachEventHandler(iframe.document,"mouseleave",lz.embed.iframemanager,"__mouseEvent",id)
}else{
lz.embed.attachEventHandler(iframe.document,"mouseover",lz.embed.iframemanager,"__mouseEvent",id);lz.embed.attachEventHandler(iframe.document,"mouseout",lz.embed.iframemanager,"__mouseEvent",id)
}}
catch($1){}}else{
try{
lz.embed.removeEventHandler(iframe.document,"mousedown",lz.embed.iframemanager,"__mouseEvent");lz.embed.removeEventHandler(iframe.document,"mouseup",lz.embed.iframemanager,"__mouseEvent");lz.embed.removeEventHandler(iframe.document,"click",lz.embed.iframemanager,"__mouseEvent");iframe.document.oncontextmenu=null;if(lz.embed.browser.isIE){
lz.embed.removeEventHandler(iframe.document,"mouseenter",lz.embed.iframemanager,"__mouseEvent");lz.embed.removeEventHandler(iframe.document,"mouseleave",lz.embed.iframemanager,"__mouseEvent")
}else{
lz.embed.removeEventHandler(iframe.document,"mouseover",lz.embed.iframemanager,"__mouseEvent");lz.embed.removeEventHandler(iframe.document,"mouseout",lz.embed.iframemanager,"__mouseEvent")
}}
catch($1){}}},setShowNativeContextMenu:function($0,$1){
this.__hidenativecontextmenu[$0]=!$1
},storeSelection:function($0){
var $1=lz.embed.iframemanager;var $2=$1.getFrameWindow($0);if($2&&$2.document&&$2.document.selection&&$2.document.selection.type=="Text"){
$1.__selectionbookmarks[$0]=$2.document.selection.createRange().getBookmark()
}},restoreSelection:function($0){
var $1=lz.embed.iframemanager;var $2=$1.getFrameWindow($0);if($1.__selectionbookmarks[$0]&&$2){
var $3=$1.__selectionbookmarks[$0];var $4=$2.document.body.createTextRange();$4.moveToBookmark($3);$4.select()
}}};lz.embed.mousewheel={__mousewheelEvent:function($0){
var $1=lz.embed;if(!$0)$0=window.event;var $2=0;if($0.wheelDelta){
$2=$0.wheelDelta/120;if($1.browser.isOpera){
$2=-$2
}}else if($0.detail){
$2=-$0.detail/3
};if($0.preventDefault)$0.preventDefault();$0.returnValue=false;var $3=$1.mousewheel.__callbacks.length;if($2!=null&&$3>0){
for(var $4=0;$4<$3;$4+=2){
var $5=$1.mousewheel.__callbacks[$4];var $6=$1.mousewheel.__callbacks[$4+1];if($5&&$5[$6])$5[$6]($2)
}}},__callbacks:[],setCallback:function($0,$1){
var $2=lz.embed.mousewheel;if($2.__callbacks.length==0)$2.setEnabled(this.__enabled);$2.__callbacks.push($0,$1)
},__enabled:false,setEnabled:function($0){
var $1=lz.embed;if($1.mousewheel.__enabled==$0)return;$1.mousewheel.__enabled=$0;if($0&&$1.options&&$1.options.cancelkeyboardcontrol==true){
return
};var $2=$0?"attachEventHandler":"removeEventHandler";if(window.addEventListener){
$1[$2](window,"DOMMouseScroll",$1.mousewheel,"__mousewheelEvent")
};$1[$2](document,"mousewheel",$1.mousewheel,"__mousewheelEvent")
}};lz.embed.history={active:null,_currentstate:null,_apps:[],_intervalID:null,_registeredapps:{},intervaltime:200,init:function(){
var $0=lz.embed.history;if($0.active||$0.active==false)return;$0.active=true;var $1=$0.get();var $2=lz.embed.browser;if($2.isSafari&&$2.version<523.1){
$0._historylength=history.length;$0._history=[];for(var $3=1;$3<$0._historylength;$3++){
$0._history.push("")
};$0._history.push($1);var $4=document.createElement("form");$4.method="get";document.body.appendChild($4);$4.style.display="none";$0._form=$4;if(!top.document.location.lzaddr){
top.document.location.lzaddr={}};if(top.document.location.lzaddr.history){
$0._history=top.document.location.lzaddr.history.split(",")
};if($1!=""){
$0.set($1)
}}else if($2.isIE){
var $1=top.location.hash;if($1)$1=$1.substring(1);var $3=document.createElement("iframe");lz.embed.__setAttr($3,"id","lzHistory");lz.embed.__setAttr($3,"frameborder","no");lz.embed.__setAttr($3,"scrolling","no");lz.embed.__setAttr($3,"width","0");lz.embed.__setAttr($3,"height","0");lz.embed.__setAttr($3,"src",'javascript:""');document.body.appendChild($3);$3=document.getElementById("lzHistory");$0._iframe=$3;$3.style.display="none";$3.style.position="absolute";$3.style.left="-999px";var $5=$3.contentDocument||$3.contentWindow.document;$5.open();$5.close();if($1!=""){
$5.location.hash="#"+$1;$0._parse($1)
}}else{
if($1!=""){
$0._parse($1);$0._currentstate=$1
}};if($0._intervalID!=null){
clearInterval($0._intervalID)
};if($0.intervaltime>0){
$0._intervalID=setInterval("lz.embed.history._checklocationhash()",$0.intervaltime)
}},listen:function($0){
if(typeof $0=="string"){
$0=lz.embed.applications[$0];if(!$0||!$0.runtime){
return
}};if(!$0)return;var $1=lz.embed.history;if($1._registeredapps[$0._id]){
return
};$1._registeredapps[$0.id]=true;$1._apps.push($0);$1.init()
},_checklocationhash:function(){
var $0=lz.embed;if($0.dojo&&$0.dojo.info&&$0.dojo.info.installing)return;if($0.browser.isSafari&&$0.browser.version<523.1){
var $1=this._history[this._historylength-1];if($1==""||$1=="#")$1="#0";if(!this._skip&&this._historylength!=history.length){
this._historylength=history.length;if(typeof $1!="undefined"){
$1=$1.substring(1);this._currentstate=$1;this._parse($1)
}}else{
this._parse($1.substring(1))
}}else{
var $1=$0.history.get();if($1=="")$1="0";if($0.browser.isIE){
if($1!=this._currentstate){
top.location.hash=$1=="0"?"":"#"+$1;this._currentstate=$1;this._parse($1)
}}else{
this._currentstate=$1;this._parse($1)
}}},set:function($0){
var $1=lz.embed;if($1.history.active==false)return;if($0==null)$0="";if($1.history._currentstate==$0)return;$1.history._currentstate=$0;var $2="#"+$0;if($1.browser.isIE){
top.location.hash=$2=="#0"?"":$2;var $3=$1.history._iframe.contentDocument||$1.history._iframe.contentWindow.document;$3.open();$3.close();$3.location.hash=$2;$1.history._parse($0+"")
}else if($1.browser.isSafari&&$1.browser.version<523.1){
$1.history._history[history.length]=$2;$1.history._historylength=history.length+1;if($1.browser.version<412){
if(top.location.search==""){
$1.history._form.action=$2;top.document.location.lzaddr.history=$1.history._history.toString();$1.history._skip=true;$1.history._form.submit();$1.history._skip=false
}}else{
var $4=document.createEvent("MouseEvents");$4.initEvent("click",true,true);var $5=document.createElement("a");$5.href=$2;$5.dispatchEvent($4)
}}else{
top.location.hash=$2;$1.history._parse($0+"")
};return true
},get:function(){
var $0="";if(lz.embed.browser.isIE){
if(lz.embed.history._iframe){
var $1=lz.embed.history._iframe.contentDocument||lz.embed.history._iframe.contentWindow.document;$0=$1.location.hash
}}else{
$0=top.location.href
};var $2=$0.indexOf("#");if($2!=-1){
return $0.substring($2+1)
};return ""
},_parse:function($0){
var $1=lz.embed.history;if($0.length==0)return;for(var $2=0,$3=lz.embed.history._apps.length;$2<$3;$2++){
var $4=lz.embed.history._apps[$2];if(!$4.loaded||$4._lasthash==$0)continue;$4._lasthash=$0;if($0.indexOf("_lz")!=-1){
$0=$0.substring(3);var $5=$0.split(",");for(var $6=0;$6<$5.length;$6++){
var $7=$5[$6];var $8=$7.indexOf("=");var $9=unescape($7.substring(0,$8));var $a=unescape($7.substring($8+1));lz.embed.setCanvasAttribute($9,$a);if(window["canvas"])canvas.setAttribute($9,$a)
}}else{
if($4.runtime=="swf"){
$1.__setFlash($0,$4._id)
}else if(window["lz"]&&lz["History"]&&lz.History["isReady"]&&lz.History["receiveHistory"]){
lz.History.receiveHistory($0)
}}}},_store:function($0,$1){
if($0 instanceof Object){
var $2="";for(var $3 in $0){
if($2!="")$2+=",";$2+=escape($3)+"="+escape($0[$3])
}}else{
var $2=escape($0)+"="+escape($1)
};this.set("_lz"+$2)
},__setFlash:function($0,$1){
var $2=lz.embed[$1];if($2&&$2.loaded&&$2.runtime=="swf"){
var $3=$2._getSWFDiv();if($3){
var $4=$2.callMethod("lz.History.receiveHistory("+$0+")");$2._lasthash=$0
}}}};if(lz.embed.browser.isFirefox){
window.onunload=function(){}};lz.embed.regex={cache:{},create:function($0,$1,$2){
try{
var $3=lz.embed.regex;$3.cache[$0]=new RegExp($3.unmask($1),$3.unmask($2));return true
}
catch($4){
return $4.name+": "+$4.message
}},test:function($0,$1,$2){
var $3=lz.embed.regex;var $4=$3.cache[$0];$4.lastIndex=$2;return [$4.test($3.unmask($1)),$4.lastIndex]
},exec:function($0,$1,$2){
var $3=lz.embed.regex;var $4=$3.cache[$0];$4.lastIndex=$2;var $5=$4.exec($3.unmask($1));if($5){
($5=$3.maskArr($5)).push($5.index,$4.lastIndex);return $5
}else{
return null
}},match:function($0,$1){
var $2=lz.embed.regex;var $3=$2.unmask($1).match($2.cache[$0]);return $3?$2.maskArr($3):null
},replace:function($0,$1,$2){
var $3=lz.embed.regex;return $3.mask($3.unmask($1).replace($3.cache[$0],$3.unmask($2)))
},search:function($0,$1){
var $2=lz.embed.regex;return $2.unmask($1).search($2.cache[$0])
},split:function($0,$1,$2){
var $3=lz.embed.regex;return $3.maskArr($3.unmask($1).split($3.cache[$0],$2))
},remove:function($0){
delete lz.embed.regex.cache[$0]
},mask:function($0){

var re = /^\s*$/;
var re2 = /\s/g;
;return( $0==null||!re.test($0)?$0:"__#lznull"+$0.replace(re2,function($0){
switch($0){
case " ":
return "w";
case "\f":
return "f";
case "\n":
return "n";
case "\r":
return "r";
case "\t":
return "t";
case "\xA0":
return "s";
case "\u2028":
return "l";
case "\u2029":
return "p";

}}))
},unmask:function($0){
return $0=="__#lznull"?"":$0
},maskArr:function($0){
var $1=lz.embed.regex;for(var $2=0;$2<$0.length;++$2)$0[$2]=$1.mask($0[$2]);return $0
}};