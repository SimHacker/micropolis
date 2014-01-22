function ConvivaPrivateLoader(){}
(function(){












function yR(x7,l3){
if(typeof(ConvivaPrivateModule)!="undefined"){
ConvivaPrivateModule[l3]=x7;
}else if(typeof(ConvivaPrivateTestingModule)!="undefined"){
ConvivaPrivateTestingModule[l3]=x7;
}else{

ConvivaPrivateLoader[l3]=x7;
}
}
yR(yR,"yR");


function ag(Fn){return Fn;}
yR(ag,"ag");



function sHq(l5){
if(typeof(ConvivaPrivateModule)!="undefined"&&ConvivaPrivateModule.hasOwnProperty(l5)){
return ConvivaPrivateModule[l5];
}else if(typeof(ConvivaPrivateTestingModule)!="undefined"&&ConvivaPrivateTestingModule.hasOwnProperty(l5)){
return ConvivaPrivateTestingModule[l5];
}else if(ConvivaPrivateLoader.hasOwnProperty(l5)){
return ConvivaPrivateLoader[l5];
}else{
return null;
}
}
yR(sHq,"sHq");




function at(frW,lei){
var S5="";
var Ti;
for(Ti in frW){
if(frW.hasOwnProperty(Ti)){
S5+="var "+Ti+"="+ag(lei)+"."+Ti+";"
}
}
return S5;
}
yR(at,"at");


function fj(){
return "STAT_INIT";
}
yR(fj,"fj");


function Bg(x7,l3){
x7.call(fj);
yR(x7,l3);
}
yR(Bg,"Bg");


function OZ(sq,Sj,xF){
if(sq!=fj){
if(sq[Sj]==undefined){
sq[Sj]=xF;
}else{

sq["Ph"+Sj]=xF;
}
}
}
yR(OZ,"OZ");


function LK(sq,Sj,xF){
if(sq!=fj)sq[Sj]=xF;
}
yR(LK,"LK");


function dN(sq,x7,Sj,xF){
if(sq==fj)x7[Sj]=xF;
}
yR(dN,"dN");


function ED(sq,Sj,xF){
if(sq!=fj){
if(typeof(Object.defineProperty)!='undefined'){
Object.defineProperty(sq,Sj,{
configurable:true,
enumerable:true,
get:xF
});
}else{
sq.__defineGetter__(Sj,xF);
}
}
}
yR(ED,"ED");

function Wf(sq,Sj,xF){
if(sq!=fj){
if(typeof(Object.defineProperty)!='undefined'){
Object.defineProperty(sq,Sj,{
configurable:true,
set:xF
});
}else{
sq.__defineSetter__(Sj,xF);
}
}
}
yR(Wf,"Wf");

function Va(sq,x7,Sj,xF){
if(sq==fj)ED(x7,Sj,xF);
}
yR(Va,"Va");

function Z0(sq,x7,Sj,xF){
if(sq==fj)Wf(x7,Sj,xF);
}
yR(Z0,"Z0");

function PW(k9){

if(k9.constructor==Array){
return true;
}else if(typeof(k9.length)=='undefined'){
return false;
}else{
return true;
}
}
yR(PW,"PW");

function BW(l4,Kq){
var tK=PW(l4);
if(PW(l4)){
for(var co=0;co<l4.length;co++){
Kq(l4[co]);
}
}else{
for(var Ti in l4){
if(l4.hasOwnProperty(Ti))Kq(l4[Ti]);
}
}
}
yR(BW,"BW");

function Zi(l4,Kq){
if(PW(l4)){
for(var co=0;co<l4.length;co++){
Kq(co);
}
}else{
for(var Ti in l4){
if(l4.hasOwnProperty(Ti))Kq(Ti);
}
}
}
yR(Zi,"Zi");



function D8(sq,Sj,x7,DM){
if(sq==fj)jstest.CR(Sj,x7,DM);
}
yR(D8,"D8");





function m2(sq,l3,PD,_8,DM){
if(sq==fj){
jstest.lT(l3,PD,DM);
}else{
jstest.xq(l3,PD,_8);
}
}
yR(m2,"m2");


function n4(){
n4.t3R=0x100000000;

n4.z_=function(Sk){
var ZFC=parseInt(Sk);
if(ZFC>n4.Je){
ZFC=ZFC%n4.t3R;
}else if(ZFC<0){
ZFC=(-ZFC)%n4.t3R;
ZFC=n4.t3R-ZFC;
}
return ZFC;
};

n4.QIc=new RegExp("^[0-9]+$");
n4.tn=function(Sk){
oL.qlz(Sk,n4.QIc);
return n4.z_(Sk);
}

n4.Je=n4.t3R-1;
n4.ke=0;
}
Bg(n4,"n4");


function VW(){
VW.z_=function(Sk){

var WMR=n4.z_(Sk);
if(WMR>VW.Je){
WMR=WMR-n4.t3R;
}
return WMR;
};

VW.y7A=new RegExp("^[+-]?[0-9]+$");
VW.tn=function(Sk){
oL.qlz(Sk,VW.y7A);
return VW.z_(Sk);
}

VW.Je=0x7FFFFFFF;
VW.ke=-0x80000000;
}
Bg(VW,"VW");

function L_(){
var up=this;

if(up==fj)L_.qp=4294967296.0;










function _d(){
up.XE=0;
up.Br=0;
};


dN(up,L_,"uk",ui);
function ui(co){
var S5=new L_();
S5.Br=0;
S5.XE=co;
return S5;
};


dN(up,L_,"yu",pd);
function pd(co){
var S5=new L_();
if(co>=0){
S5.Br=0;
S5.XE=n4.z_(co);
}else{
S5.Br=-1;
S5.XE=n4.z_(co);
}
return S5;
};


dN(up,L_,"Ho",Ma);
function Ma(v9){
var rn=v9%L_.qp;

if(rn<0){
rn=Number(n4.Je)+1.0+rn;
}
var S5=new L_();
S5.XE=n4.z_(rn+0.5);
S5.Br=VW.z_((v9-rn)/L_.qp);

return S5;
};



ED(up,"mg",mM);
function mM(){
return Number(up.Br)*L_.qp+Number(up.XE);
};


if(up!=fj){
this.toString=function(){
return up.mg.toString();
}
}

if(up!=fj)_d.apply(this,arguments);

};
Bg(L_,"L_");


function Q2(){

var up=this;

function _d(){

L_.call(up);
up.XE=0;
up.Br=0;
}


dN(up,Q2,"uk",ui);
function ui(co){
var S5=new Q2();
S5.Br=0;
S5.XE=co;
return S5;
};


dN(up,Q2,"Ho",Ma);
function Ma(v9){
var S5=new Q2();
S5.Br=Math.floor(v9/L_.qp);
S5.XE=v9%L_.qp;
return S5;
};


if(up!=fj)_d.apply(arguments);
};
Bg(Q2,"Q2");




function OY(hg,KN,Vv){
this.hg=hg;
this.KN=KN;
this.Vv=Vv;
}




Ff=function(qs,Wk){
this.qs=qs;
this.Wk=Wk;
};
with({Ti:Ff.prototype}){
with({Ti:(Ti.p2=function(qs,yt){
this.qs=qs||0;
this.yt=[];
this._t(yt);}).prototype}){
Ti._t=function(jV){
if(jV){

var aw=0;
var cn=jV.length;
if(jV.hasOwnProperty("KN")){
aw=jV.KN;
cn=jV.Vv;
jV=jV.hg;
}else{
cn=jV.length;
}
var co;
var jl;
for(co=cn,jl=this.yt=new Array(cn);co;jl[cn-co]=(jV.charCodeAt(--co+aw)&0xFF));
this.qs&&jl.reverse();
}
};
Ti.E_=function(GC){
return this.yt.length>=-(-GC>>3);
};
Ti.v1=function(GC){
if(!this.E_(GC))
throw new Error("checkBuffer::missing bytes");
};
Ti.Dz=function(tO,length){

function GJ(l4,jl){

for(;jl--;l4=((l4%=(0x7fffffff+1))&0x40000000)==0x40000000?l4*2:(l4-0x40000000)*2+0x7fffffff+1);
return l4;
}
if(tO<0||length<=0)
return 0;
this.v1(tO+length);
for(var dd,
uS=tO%8,
E1=this.yt.length-(tO>>3)-1,
Mc=this.yt.length+(-(tO+length)>>3),
nx=E1-Mc,
gG=((this.yt[E1]>>uS)&((1<<(nx?8-uS:length))-1))+
(nx&&(dd=(tO+length)%8)?(this.yt[Mc++]&((1<<dd)-1))<<(nx--<<3)-uS:0);

nx;

gG+=GJ(this.yt[Mc++],(nx--<<3)-uS));
return gG;
};
}
Ti.rj=function(rU){
if(this.Wk)
throw new Error(rU);
return 1;
};
Ti.e5=function(jV,Oq,vc){
var jl=new this.p2(this.qs,jV);
jl.v1(Oq+vc+1);
var qR=Math.pow(2,vc-1)-1,Pf=jl.Dz(Oq+vc,1),A8=jl.Dz(Oq,vc),FO=0,
Ve=2,E1=jl.yt.length+(-Oq>>3)-1;
do
for(var ZV=jl.yt[++E1],w9=Oq%8||8,FL=1<<w9;FL>>=1;(ZV&FL)&&(FO+=1/Ve),Ve*=2);
while(Oq-=w9);
return A8==(qR<<1)+1?FO?NaN:Pf?-Infinity:+Infinity:(1+Pf*-2)*(A8||FO?!A8?Math.pow(2,-qR+1)*FO:Math.pow(2,A8-qR)*(1+FO):0);
};
Ti._9=function(jV,IL,ef){
var jl=new this.p2(this.qs,jV);
var Fn=jl.Dz(0,IL),_j=Math.pow(2,IL);
return ef&&Fn>=_j/2?Fn-_j:Fn;
};
Ti.te=function(jV,Oq,vc){
jV=parseFloat(jV);
var qR=Math.pow(2,vc-1)-1,aH=-qR+1,gP=qR,Hc=aH-Oq,
f1=isNaN(v9=parseFloat(jV))||v9==-Infinity||v9==+Infinity?v9:0,
LU=0,Wp=2*qR+1+Oq+3,M6=new Array(Wp),
Pf=(v9=f1!==0?0:v9)<0,v9=Math.abs(v9),QA=Math.floor(v9),za=v9-QA,
co,xR,_L,Bs,lb;
for(co=Wp;co;M6[--co]=0);
for(co=qR+2;QA&&co;M6[--co]=QA%2,QA=Math.floor(QA/2));
for(co=qR+1;za>0&&co;(M6[++co]=((za*=2)>=1)-0)&&--za);
for(co=-1;++co<Wp&&!M6[co];);
if(M6[(xR=Oq-1+(co=(LU=qR+1-co)>=aH&&LU<=gP?co+1:qR+1-(LU=aH-1)))+1]){
if(!(_L=M6[xR]))
for(Bs=xR+2;!_L&&Bs<Wp;_L=M6[Bs++]);
for(Bs=xR+1;_L&&--Bs>=0;(M6[Bs]=!M6[Bs]-0)&&(_L=0));
}
for(co=co-2<0?-1:co-3;++co<Wp&&!M6[co];);
if((LU=qR+1-co)>=aH&&LU<=gP)
++co;
else if(LU<aH){
LU!=qR+1-Wp&&LU<Hc&&this.rj("encodeFloat::float underflow");
co=qR+1-(LU=aH-1);
}
if(QA||f1!==0){

LU=gP+1;
co=qR+2;
if(f1==-Infinity)
Pf=1;
else if(isNaN(f1)){
Pf=1;
M6[co]=1;
}
}
for(v9=Math.abs(LU+qR),Bs=vc+1,lb="";--Bs;lb=(v9%2)+lb,v9=v9>>=1);
for(v9=0,Bs=0,co=(lb=(Pf?"1":"0")+lb+M6.slice(co,co+Oq).join("")).length,eq=[];co;Bs=(Bs+1)%8){
v9+=(1<<Bs)*lb.charAt(--co);
if(Bs==7){
eq[eq.length]=String.fromCharCode(v9);
v9=0;
}
}
eq[eq.length]=v9?String.fromCharCode(v9):"";
return(this.qs?eq.reverse():eq).join("");
};
Ti.h6=function(jV,IL,ef){
jV=parseInt(jV);
var _j=Math.pow(2,IL);
(jV>=_j||jV<-(_j/2))&&this.rj("encodeInt::overflow")&&(jV=0);
jV<0&&(jV+=_j);
var eq=[];
while(jV){
eq[eq.length]=String.fromCharCode(jV%256);
jV=Math.floor(jV/256);
};
for(IL=-(-IL>>3)-eq.length;IL--;eq[eq.length]="\000");
return(this.qs?eq.reverse():eq).join("");
};
Ti.qb=function(jV){return this._9(jV,8,true);};
Ti.Mx=function(jV){return this.h6(jV,8,true);};
Ti.ph=function(jV){return this._9(jV,8,false);};
Ti.vb=function(jV){return this.h6(jV,8,false);};
Ti.s1=function(jV){return this._9(jV,16,true);};
Ti.kZ=function(jV){return this.h6(jV,16,true);};
Ti.eR=function(jV){return this._9(jV,16,false);};
Ti.ko=function(jV){return this.h6(jV,16,false);};
Ti.so=function(jV){return this._9(jV,32,true);};
Ti.yu=function(jV){return this.h6(jV,32,true);};
Ti.PB=function(jV){return this._9(jV,32,false);};
Ti.T5=function(jV){return this.h6(jV,32,false);};
Ti.I_=function(jV){return this.e5(jV,23,8);};
Ti.AU=function(jV){return this.te(jV,23,8);};
Ti.qM=function(jV){return this.e5(jV,52,11);};
Ti.Dv=function(jV){return this.te(jV,52,11);};
};







va=function(_W){
var ii=_W.length;
var rS=[];
var JU,cp;
var Fn,w4,BF;
for(var co=0;co<ii;co++){
JU=_W.charCodeAt(co);
if((JU&0xff80)==0){

rS.push(JU);
}else{
if((JU&0xfc00)==0xD800){
cp=_W.charCodeAt(co+1);
if((cp&0xfc00)==0xDC00){

JU=(((JU&0x03ff)<<10)|(cp&0x3ff))+0x10000;
co++;
}else{

console.log("Error decoding surrogate pair: "+JU+"; "+cp);
}
}
Fn=JU&0xff;
w4=JU&0xff00;
BF=JU&0xff0000;

if(JU<=0x0007ff){
rS.push(0xc0|(w4>>6)|(Fn>>6));
rS.push(0x80|(Fn&63));
}else if(JU<=0x00ffff){
rS.push(0xe0|(w4>>12));
rS.push(0x80|((w4>>6)&63)|(Fn>>6));
rS.push(0x80|(Fn&63));
}else if(JU<=0x10ffff){
rS.push(0xf0|(BF>>18));
rS.push(0x80|((BF>>12)&63)|(w4>>12));
rS.push(0x80|((w4>>6)&63)|(Fn>>6));
rS.push(0x80|(Fn&63));
}else{

console.log("Error encoding to utf8: "+JU+" is greater than U+10ffff");
rS.push("?".charCodeAt(0));
}
}
}
return rS;
};

IF=function(rS){
var Vn=rS.length;
var _W="";
var JU,rM,dL,V1;
for(var co=0;co<Vn;co++){
JU=rS[co];
if((JU&0x80)==0x00){
}else if((JU&0xf8)==0xf0){

rM=rS[co+1];
dL=rS[co+2];
V1=rS[co+3];
if((rM&0xc0)==0x80&&(dL&0xc0)==0x80&&(V1&0xc0)==0x80){
JU=(JU&7)<<18|(rM&63)<<12|(dL&63)<<6|(V1&63);
co+=3;
}else{

console.log("Error decoding from utf8: "+JU+","+rM+","+dL+","+V1);
continue;
}
}else if((JU&0xf0)==0xe0){

rM=rS[co+1];
dL=rS[co+2];
if((rM&0xc0)==0x80&&(dL&0xc0)==0x80){
JU=(JU&15)<<12|(rM&63)<<6|(dL&63);
co+=2;
}else{

console.log("Error decoding from utf8: "+JU+","+rM+","+dL);
continue;
}
}else if((JU&0xe0)==0xc0){

rM=rS[co+1];
if((rM&0xc0)==0x80){
JU=(JU&31)<<6|(rM&63);
co+=1;
}else{

console.log("Error decoding from utf8: "+JU+","+rM);
continue;
}
}else{



console.log("Error decoding from utf8: "+JU+" encountered not in multi-byte sequence");
continue;
}
if(JU<=0xffff){
_W+=String.fromCharCode(JU);
}else if(JU>0xffff&&JU<=0x10ffff){

JU-=0x10000;
_W+=(String.fromCharCode(0xD800|(JU>>10))+String.fromCharCode(0xDC00|(JU&1023)));
}else{
console.log("Error encoding surrogate pair: "+JU+" is greater than U+10ffff");
}
}
return _W;
};









function Vz(){
var up=this;



function _d(){
up.AJ=new Ff(true,false);
up._W="";
up.Ny=0;
};

dN(up,Vz,"UA",Zd);
function Zd(_W){
var S5=new Vz();
S5._W=_W;
return S5;
}


dN(up,Vz,"BU",S2);
function S2(f8){
var WJ=new Vz();
for(var co=0;co<f8.ug;co++){
eQ.NP(WJ,f8.tc(co));
}
return WJ;
}


OZ(up,"v_",ES);
function ES(){
up.KM();
var PA=new jh(up.X1);
for(var Ny=0;Ny<PA.ug;Ny++){
PA.a9(Ny,eQ.Jj(up));
}
return PA;
}


dN(up,Vz,"Y_",gE);
function gE(_W){
return Zd(_W);
}

OZ(up,"UX",h0);
function h0(){
return up._W;
}

OZ(up,"vo",Ua);
function Ua(){
return up._W;
}


OZ(up,"u6",ul);
function ul(){
up.KM();
return up;
}
OZ(up,"Uv",Fa);
function Fa(){
up._W="";
up.Ny=0;
return up;
}


OZ(up,"PJ",nW);
function nW(Wp){
if(Wp<0||up.Ny+Wp>up._W.length){
throw new Error("Access string outside the bounds");
}
var S5=new OY(up._W,up.Ny,Wp);

up.Ny+=Wp;
return S5;
}

OZ(up,"FF",SD);
function SD(nI){
up._W=up._W+nI;
}

OZ(up,"KM",gW);
function gW(){
up.Ny=0;
}


ED(up,"X1",gJ);
function gJ(){
return up._W.length-up.Ny;
};

if(up==fj){

Vz.hp=false;
if(window.GN!=undefined){
var Fn=new GN();
if(Fn.xh!=undefined){
Vz.hp=true;
}
}
Vz.l0=!Vz.hp;
}


dN(up,Vz,"Em",NH);
function NH(_W){
var bb=[];
var Mg=0;
var Vp=0;
var co=0;
while(1){
if(Mg<7){
if(co>=_W.length)break;
var JU=_W.charCodeAt(co)&0xFF;
co++;
Vp|=(JU<<Mg);
Mg+=8;
}
bb.push(String.fromCharCode(Vp&0x7F));
Vp>>=7;
Mg-=7;
}
if(Mg>0){
bb.push(String.fromCharCode(Vp&0x7f));
}
return bb.join("");
}

if(up!=fj)_d.apply(up,arguments);
};
Bg(Vz,"Vz");



function eQ(){
eQ.Mr=0x7062;

eQ.rb="Invalid PacketBrain magic code";
eQ.caq="PacketBrain structure too big";

eQ.oY=function(jl,Sk){
jl.FF(jl.AJ.Mx(Sk));
};

eQ.NP=function(jl,Sk){
jl.FF(jl.AJ.vb(Sk));
};

eQ._m=function(jl,Sk){
jl.FF(jl.AJ.kZ(Sk));
};
eQ.Rw=function(jl,Sk){
jl.FF(jl.AJ.ko(Sk));
};

eQ.HT=function(jl,Sk){
jl.FF(jl.AJ.yu(Sk));
};
eQ.jU=function(jl,Sk){
jl.FF(jl.AJ.T5(Sk));
};

eQ.Gx=function(jl,Sk){
jl.FF(jl.AJ.T5(Sk.Br));
jl.FF(jl.AJ.T5(Sk.XE));
};
eQ.Ch=eQ.Gx;

eQ.MO=function(jl,Sk){
jl.FF(jl.AJ.vb(Sk?1:0));
};
eQ.x1=function(jl,Sk){
jl.FF(jl.AJ.Dv(Sk));
};

eQ.REE=function(jl,Sk){
if(Sk>=0x10000){
eQ.jU(jl,Sk);
}else{
eQ.Rw(jl,Sk);
}
};

eQ.WfF=function(jl,dDD){
if(dDD){
return eQ.ip(jl);
}else{
return eQ.Kp(jl);
}
};

eQ.hvL=function(jl,Sk){
if(Sk>=0x8000){
var DNC=Sk|0x80000000;
eQ.jU(jl,DNC);
}else{
eQ.Rw(jl,Sk);
}
};

eQ.jTz=function(jl,dDD){
var r8_=eQ.Kp(jl);
if(r8_>=0x8000){
var wLn=eQ.Kp(jl);
return(r8_-0x8000)*0x10000+wLn;
}else{
return r8_;
}
};

eQ.x3=function(jl){
return jl.AJ.qb(jl.PJ(1));
};
eQ.Jj=function(jl){
return jl.AJ.ph(jl.PJ(1));
};
eQ.VA=function(jl){
return jl.AJ.s1(jl.PJ(2));
};
eQ.Kp=function(jl){
return jl.AJ.eR(jl.PJ(2));
};
eQ.YV=function(jl){
return jl.AJ.so(jl.PJ(4));
};
eQ.ip=function(jl){
return jl.AJ.PB(jl.PJ(4));
};
eQ.mr=function(jl){
var S5=new L_();
S5.Br=eQ.ip(jl);
S5.XE=eQ.ip(jl);
return S5;
};
eQ.CV=eQ.mr;

eQ.gC=function(jl){
return(0!=eQ.Jj(jl));
};

eQ.Hu=function(jl){
return jl.AJ.qM(jl.PJ(8));
};

eQ.q9=function(jl){
return jl.Ny;
};
eQ.Pa=function(jl,fb){
jl.Ny+=fb;
};

eQ.H_=function(Sk){
if(!Sk)throw new Error("PbAssert");
};



eQ.b1=function(l4,aw,Wp){
if(aw==0&&Wp==-1){

return IF(l4);
}else{
return IF(l4.slice(aw,aw+Wp));
}
};
















function Zg(Oj,nI){

var f8=va(nI);
var Z7=f8.length;
if(Z7>=256*128){

Z7=256*128;
}
var VK;
var K8;
if(Z7>=128){
VK=(Z7>>8)+128;
K8=Z7-((Z7>>8)<<8);
}else{
VK=Z7;
K8=0;
}

Oj.push(VK);
if(Z7>=128){
Oj.push(K8);
}
for(var Ny=0;Ny<Z7;Ny++){
Oj.push(f8[Ny]);
}
};





function xc(l4,xL){
var Iq=xL[0];
var Z7=l4[Iq];
if(Z7>=128){
Z7=(Z7-128)<<8;
Z7=Z7+l4[Iq+1];
Iq+=2;
}else{
Iq+=1;
}

var S5=eQ.b1(l4,Iq,Z7);
xL[0]=Iq+Z7;
return S5;
}



eQ.eb=function(aq){
var S5=new Array();
if(!aq){return jh.Y_(S5);}
var lr=aq.VO;
for(var co=0;co<lr.length;co++){
var G2=lr[co];
Zg(S5,G2);
Zg(S5,aq.tc(G2));
}
return jh.Y_(S5);
};


eQ.wj=function(l4){
if(!l4)return null;
var S5=new EW();
var Tj=null;
var Bh=new Array();
Bh[0]=0;
while(Bh[0]<l4.length){

var _W=xc(l4,Bh);
if(Tj==null){
Tj=_W;
}else{
S5.a9(Tj,_W);
Tj=null;
}
}
return S5;
};



}
Bg(eQ,"eQ");



function VI(){
var up=this;

function _d(){
up.Tw=[];
};

OZ(up,"So",cW);
function cW(){
var S5=up.Tw.length+3;
if(S5>=0x10000){
S5+=2;
}
return S5;
}

OZ(up,"VS",tH);
function tH(jl){
var n5=2;
var ic=up.So();
if(ic>=0x10000)n5=3;
eQ.NP(jl,n5);
eQ.REE(jl,ic);
var YR=up.Tw;
for(var co=0;co<YR.length;co++){
eQ.NP(jl,YR[co]);
}
}


OZ(up,"qN",hH);
function hH(jl){
var XM=eQ.q9(jl);
var n5=eQ.Jj(jl);
var x6=eQ.WfF(jl,((n5&1)==1));
var FCg=2;
if(x6>=0x10000)FCg+=2;

var x8;
if(n5==0){
x8=eQ.Kp(jl);
}else{
x8=x6-(1+FCg);
}
up.Tw.length=x8;
for(var co=0;co<x8;co++){
up.Tw[co]=eQ.Jj(jl);
}

var FK=XM+x6-eQ.q9(jl);
eQ.Pa(jl,FK);
}



OZ(up,"UE",DZ);
function DZ(Mh){
var NS=Mh.Tw.length;
up.Tw.length=NS;
for(var co=0;co<NS;co++){
up.Tw[co]=Mh.Tw[co];
}
}

OZ(up,"T3",Pl);
function Pl(){
return eQ.b1(up.Tw,0,-1);
}

OZ(up,"tg",lL);
function lL(nI){
up.Tw=va(nI);
}

if(up!=fj)_d.apply(up,arguments);
};
Bg(VI,"VI");

function dC(){
var up=this;

function _d(){
up.Tw=[];
}

OZ(up,"So",cW);
function cW(){
var S5=up.Tw.length+3;
if(S5>=0x10000){
S5+=2;
}
return S5;
}

OZ(up,"VS",tH);
function tH(jl){
var n5=1;
var ic=up.So();
if(ic>=0x10000)n5=3;
eQ.NP(jl,n5);
eQ.REE(jl,ic);
var YR=up.Tw;
for(var co=0;co<YR.length;co++){
eQ.NP(jl,YR[co]);
}
}

OZ(up,"qN",hH);
function hH(jl){
var XM=eQ.q9(jl);
var n5=eQ.Jj(jl);
var x6=eQ.WfF(jl,(n5==3));
var FCg=2;
if(x6>=0x10000)FCg+=2;

var x8;
if(n5==0){
x8=eQ.Kp(jl);
}else{
x8=x6-(1+FCg);
}
up.Tw.length=x8;
for(var co=0;co<x8;co++){
up.Tw[co]=eQ.Jj(jl);
}

var FK=XM+x6-eQ.q9(jl);
eQ.Pa(jl,FK);
}




OZ(up,"UE",DZ);
function DZ(Mh){
var NS=Mh.Tw.length;
up.Tw.length=NS;
for(var co=0;co<NS;co++){
up.Tw[co]=Mh.Tw[co];
}
}


OZ(up,"eb",Zm);
function Zm(E7){
up.Tw=eQ.eb(E7).UX();
}
OZ(up,"wj",Tn);
function Tn(){
return eQ.wj(up.Tw);
}

if(up!=fj)_d.apply(up,arguments);

};
Bg(dC,"dC");


function Ip(tL){
var F_=tL.split(",");
eQ.H_(F_.length>=2);
this.Sj=F_[0];
this.pf=this.Sj+"It";


this.fm=new Ar(F_[1]);
this.Dw=false;
this.K1=false;
this.IN=false;
this.lF=undefined;
if(this.Sj=="Kx"){
this.lF=eQ.Mr;
}
for(var co=2;co<F_.length;co++){
var JU=F_[co];

if(JU=="e"){
this.Dw=true;
continue;
}


if(JU.indexOf("d=")==0){
this.lF=JU.substr(2);
if(this.fm.sY=='Boolean'){
this.lF=Boolean(this.lF);
}else if(this.fm.sY=='Double'){
this.lF=parseFloat(this.lF);
}else if(this.fm.sY=='Integer'){
this.lF=parseInt(this.lF);
}else if(this.lF=="null"){
this.lF=null;
}else{
throw new Error("PBUtil: unrecognized default value: "+this.lF);
}
continue;
}


if(JU.indexOf("o=")==0){
var ZI=JU.substr(2).split(":");
eQ.H_(ZI.length==2);
this.K1=true;
this.R8=ZI[0]+"It";
this.GT=ZI[1];
continue;
}

if(JU.indexOf("c=")==0){
this.IN=true;
this.MC=JU.substr(2);
continue;
}
throw new Error("Unrecognized descriptor field "+JU);
}


if(this.lF===undefined){
this.lF=this.fm.lF;
}
if(this.fm.sY=='Integer'&&this.fm.mK==8){

if(this.fm.YZ){
this.lF=Q2.Ho(this.lF);
}else{
this.lF=L_.Ho(this.lF);
}
}


this.GR=function(Pu){
if(this.K1){
if((Pu[this.R8]&(1<<this.GT))==0){
return false;
}
}
if(this.IN){
return Ip.AY(this.MC,Pu);
}
return true;
};

}
yR(Ip,"Ip");



Ip.AY=function(LU,Pu){
if(LU=="true")return true;
if(LU=="false")return false;

if(LU.match(/^\d+$/))return parseInt(LU);


var xF=LU.match(/^([a-zA-Z0-9_]+)(\(\))?$/);
if(xF){
if(xF[2]=="()")
return Pu[xF[1]]();
else
return Pu[xF[1]];

}
throw new Error("PB: Cannot interpret expression: "+LU)
};


Ip.B6=function(x7){

var o2=[];
for(var co=1;co<arguments.length;co++){
o2.push(new Ip(arguments[co]));
}

x7.zw=o2;
};


Ip.ZF=function(x7,Kh){
for(var B4=0;B4<x7.zw.length;B4++){
var Kq=x7.zw[B4];
Kh(Kq);
}
};



Ip.Kt=function(Pu,KXI,x7){
Ip.ZF(x7,function(Kq){

if(Kq.Dw){
Pu["zj"+Kq.Sj]=true;
}
if(Kq.fm.sY=='Array'){

var pK=Kq.Sj+"rw";
Pu[pK]=[];

if(Kq.fm.hS!==undefined&&!Kq.K1){
Pu[pK].length=Ip.AY(Kq.fm.hS,Pu);
}

Pu[Kq.Sj+"XG"]=function(){
return Pu[pK].length;
};
Pu[Kq.Sj+"Gw"]=function(iK){
Pu[pK].length=iK;
if(Kq.K1){
Pu[Kq.R8]|=(1<<Kq.GT);
}
};
Pu[Kq.Sj+"bh"]=function(Ny){
eQ.H_(Ny<Pu[pK].length);
if(Kq.K1){
eQ.H_((Pu[Kq.R8]&(1<<Kq.GT))!=0);
}
return Pu[pK][Ny];
};
Pu[Kq.Sj+"XZ"]=function(Ny,Sk){
eQ.H_(Ny<Pu[pK].length);
if(Kq.K1){
eQ.H_((Pu[Kq.R8]&(1<<Kq.GT))!=0);
}
Pu[pK][Ny]=Sk;
};
Pu[Kq.Sj+"xz"]=function(){
if(Kq.K1){
eQ.H_((Pu[Kq.R8]&(1<<Kq.GT))!=0);
}
return jh.Y_(Pu[pK]);
};
Pu[Kq.Sj+"iy"]=function(QE){
Pu[pK]=QE.UX();
if(Kq.K1){
Pu[Kq.R8]|=(1<<Kq.GT);
}
};
}else{

Pu[Kq.pf]=Kq.lF;

ED(Pu,Kq.Sj,function(){
return Pu[Kq.pf];
});

if(Kq.Sj!="Kx"&&Kq.Sj!="ic"&&Kq.Sj!="on"){
Wf(Pu,Kq.Sj,function(Sk){
if(Kq.K1){
Pu[Kq.R8]|=(1<<Kq.GT);
}
if(Kq.Dw){
Pu["zj"+Kq.Sj]=true;
}
Pu[Kq.pf]=Sk;
});
}
}

if(Kq.K1||Kq.Dw||Kq.IN){
Pu["yW"+Kq.Sj]=function(){
if(Kq.Dw&&!Pu["zj"+Kq.Sj])return false;
return Kq.GR(Pu);
};
}
if(Kq.K1){
Pu["u2"+Kq.Sj]=function(){
Pu[Kq.R8]&=(~(1<<Kq.GT));
if(Kq.fm.sY=='Array'){
Pu[Kq.Sj+"rw"]=[];
}else{
Pu[Kq.pf]=Kq.lF;
}
};
}
});



Pu.So=function(){
var XX=0;
Ip.ZF(x7,function(Kq){
if(Kq.GR(Pu)){
XX+=Kq.fm.So(Pu,Kq.Sj);
}
});
if(XX>=0x10000){
XX+=2;
}
return XX;
};
ED(Pu,"fH",Pu.So);


Pu.qN=function(Ti){
var XM=eQ.q9(Ti);
Ip.ZF(x7,function(Kq){
if(Kq.Dw){
Pu["zj"+Kq.Sj]=(XM+Pu.ic>eQ.q9(Ti));
}
if((Kq.Dw||Kq.K1||Kq.IN)&&!Pu["yW"+Kq.Sj]()){
Pu[Kq.pf]=Kq.lF;
}else if(Kq.Sj=="ic"){
Pu[Kq.pf]=eQ.WfF(Ti,(Pu["on"+"It"]&1==1));
}else{
Kq.fm.qN(Ti,Pu,Kq.Sj);
}
if(Kq.Sj=="Kx"){
if(Pu[Kq.pf]!=eQ.Mr){
throw new Error(eQ.rb);
}
}
});
var FK=XM+Pu.icIt-eQ.q9(Ti);
eQ.Pa(Ti,FK);
};


Pu.VS=function(Ti){
var x6=Pu.So();
var n5=0;
if(x6>=0x10000)n5=1;
Ip.ZF(x7,function(Kq){
if(Kq.Sj=="Kx"){
eQ.Rw(Ti,eQ.Mr);
}else if(Kq.Sj=="on"){
eQ.NP(Ti,n5);
}else if(Kq.Sj=="ic"){
eQ.REE(Ti,x6);
}else if(Kq.GR(Pu)){
Kq.fm.VS(Ti,Pu,Kq.Sj);
}
});
};


Pu.UE=function(Mh){
Ip.ZF(x7,function(Kq){
if(Kq.GR(Pu)){
Kq.fm.UE(Mh,Pu,Kq.Sj);
}
});
};

};


function Ar(Pv){
var K2=Pv.substr(0,1);
this.lF=null;
switch(K2){
case 'u':
case 'i':
this.YZ=(K2=='u');
this.sY='Integer';
this.mK=parseInt(Pv.substr(1,1));
var ik=null;
var GS=null;
switch(Pv){
case 'u1':ik=eQ.Jj;GS=eQ.NP;break;
case 'u2':ik=eQ.Kp;GS=eQ.Rw;break;
case 'u4':ik=eQ.ip;GS=eQ.jU;break;
case 'u8':ik=eQ.CV;GS=eQ.Ch;break;
case 'i1':ik=eQ.x3;GS=eQ.oY;break;
case 'i2':ik=eQ.VA;GS=eQ._m;break;
case 'i4':ik=eQ.YV;GS=eQ.HT;break;
case 'i8':ik=eQ.mr;GS=eQ.Gx;break;
}
this.L4=ik;
this.mj=GS;
this.lF=0;
break;
case 'b':
this.sY='Boolean';
this.mK=1;
this.L4=eQ.gC;
this.mj=eQ.MO;
this.lF=false;
break;
case 'd':
this.sY='Double';
this.mK=8;
this.L4=eQ.Hu;
this.mj=eQ.x1;
this.lF=0.0;
break;
case 's':
this.sY='String';
this._V="VI";
break;
case 'm':
this.sY='Dictionary';
this._V="dC";
break;
case 'c':
this.sY='Custom';
this._V=Pv.substr(2);
break;
case 'a':
this.sY='Array';
var F_=Pv.substr(2).split(":");
this.zd=new Ar(F_[0]);
if(F_.length>1){
this.hS=F_[1];
}else{

this.hS=undefined;
}
break;
default:
throw new Error("Unrecognized type "+Pv);
}

var VL=function(sq,DW,Ny){
if(Ny===undefined){
return sq[DW+"It"];
}else{
return sq[DW][Ny];
}
};

var d0=function(Sk,sq,HE,Ny){
if(Ny===undefined){
sq[HE+"It"]=Sk;
}else{
sq[HE][Ny]=Sk;
}
};
if(K2=='c'||K2=='s'||K2=='m'){
this.LJ=false;
this.nE=function(LQ,HE,Ny){
var dl=VL(LQ,HE,Ny);
if(!dl){
var _6b=sHq(this._V);
dl=new _6b();
d0(dl,LQ,HE,Ny);
}
return dl;
};

this.So=function(LQ,HE,Ny){
var Fn=this.nE(LQ,HE,Ny);
return Fn.So();
};

this.qN=function(Ti,LQ,HE,Ny){
this.nE(LQ,HE,Ny).qN(Ti);
};
this.VS=function(Ti,LQ,HE,Ny){
VL(LQ,HE,Ny).VS(Ti);
};
this.UE=function(jN,LQ,HE,Ny){
this.nE(LQ,HE,Ny).UE(VL(jN,HE,Ny))
};
}
if(K2=='u'||K2=='i'||K2=='d'||K2=='b'){
this.LJ=true;
this.So=function(LQ,HE,Ny){
return this.mK;
};
this.qN=function(Ti,LQ,HE,Ny){
d0(this.L4(Ti),LQ,HE,Ny);
};
this.VS=function(Ti,LQ,HE,Ny){
this.mj(Ti,VL(LQ,HE,Ny));
};
this.UE=function(jN,LQ,HE,Ny){
d0(VL(jN,HE,Ny),LQ,HE,Ny);
};
}

if(K2=='a'){
this.LJ=false;
this.So=function(LQ,HE,Ny){
var pK=HE+"rw";
eQ.H_(Ny===undefined);
var XX=0;
var x8=LQ[pK].length;
if(this.hS===undefined){

if(x8>=0x8000){
XX+=4;
}else{
XX+=2;
}
}
for(var co=0;co<x8;co++){
XX+=this.zd.So(LQ,pK,co);
}
return XX;
};

this.qN=function(Ti,LQ,HE,Ny){
var pK=HE+"rw";
eQ.H_(Ny===undefined);
var x8;
if(this.hS!==undefined){
x8=Ip.AY(this.hS,LQ);
}else{

x8=eQ.jTz(Ti);
}
LQ[HE+"Gw"](x8);
for(var co=0;co<x8;co++){
this.zd.qN(Ti,LQ,pK,co);
}
};

this.VS=function(Ti,LQ,HE,Ny){
var pK=HE+"rw";
eQ.H_(Ny===undefined);
var x8=LQ[pK].length;
if(this.hS===undefined){
eQ.hvL(Ti,x8);
}
for(var co=0;co<x8;co++){
this.zd.VS(Ti,LQ,pK,co);
}
};

this.UE=function(jN,LQ,HE,Ny){
var pK=HE+"rw";
eQ.H_(Ny===undefined);

LQ[HE+"Gw"](jN[pK].length);
var x8=LQ[pK].length;
for(var co=0;co<x8;co++){
this.zd.UE(jN,LQ,pK,co);
}
};
}
}
yR(Ar,"Ar");















function CandidateStream(){
var up=this;








function ju(kQ,Su,w6){
up.kQ=kQ;
up.Su=Su;
up.w6=w6;
}




OZ(up,"wy",NQ);OZ(up,"Cleanup",NQ);
function NQ(){
up.kQ=null;
up.Su=0;
up.w6=null;
}







if(up!=fj)up._cW=undefined;
ED(up,"kQ",d4);ED(up,"id",d4);
function d4(){return up._cW;}
Wf(up,"kQ",J_F);Wf(up,"id",J_F);
function J_F(nQ){up._cW=nQ;}








if(up!=fj)up.mkQ=undefined;
ED(up,"Su",jtC);ED(up,"bitrate",jtC);
function jtC(){return up.mkQ;}
Wf(up,"Su",HQn);Wf(up,"bitrate",HQn);
function HQn(nQ){up.mkQ=nQ;}








if(up!=fj)up.CE_=undefined;
ED(up,"w6",dO);ED(up,"resource",dO);
function dO(){return up.CE_;}
Wf(up,"w6",pl3);Wf(up,"resource",pl3);
function pl3(nQ){up.CE_=nQ;}
















OZ(up,"OeK",PdJ);OZ(up,"CheckValidity",PdJ);
function PdJ(){

if(up.kQ!=null&&!((typeof up.kQ==="string"))){
return "CandidateStream.id is not a string";
}else if(up.Su!=null&&!((typeof up.Su==="number"))){
return "CandidateStream.bitrate is not an int";
}else if(up.w6!=null&&!((typeof up.w6==="string"))){
return "CandidateStream.resource is not a string";
}
return null;
}





dN(up,CandidateStream,"QKZ",UuP);dN(up,CandidateStream,"ConstructClone",UuP);
function UuP(lGX){
if(lGX==null)return null;

return lGX;
}

if(up!=fj)ju.apply(up,arguments);
}
Bg(CandidateStream,"CandidateStream");






















function kzq(){
var up=this;
function ju(nEe){
up.vtd=nEe;
}

if(up!=fj)up.vtd=undefined;

OZ(up,"o0d",qXj);
function qXj(){
var LC=new ConvivaContentInfo(up.REQ(),up.VNO(),up.YhP());

if(up.mv4()){
LC.fi=up.inN();
}
LC.N_=up.adg();
LC.cex=up.qaO();
LC.Su=up.n4x();
LC._fK=up.Iom();
LC.w6=up.RhM();
LC.xQh=up.ArX();
LC.o6d=up.v8h();
LC.yuB=up.XJR();
LC.lTq=up.BRI();
LC.nFQ=up.rTo();
LC.eQZ=up.eMb();
LC.Jqo=up.ThB();
LC.wL2=up.EYb();
LC.alT=up._Y8();
LC.qMR=up.ieU();
LC.hTs=up.c_7();
LC.TMi=up.R9p();
LC.oP9=VW.z_(up.V5s());
return LC;
}

OZ(up,"REQ",m6g);
function m6g(){
if(up.vtd!=null){
return up.vtd.REQ();
}else{
return null;
}
}

OZ(up,"VNO",mdj);
function mdj(){
if(up.vtd!=null){
return up.vtd.VNO();
}else{
return null;
}
}

OZ(up,"YhP",oTV);
function oTV(){
if(up.vtd!=null){
return up.vtd.YhP();
}else{
return null;
}
}




OZ(up,"mv4",IK2);
function IK2(){
if(up.vtd!=null){
return up.vtd.mv4();
}else{
return false;
}
}

OZ(up,"inN",X8A);
function X8A(){
if(up.vtd!=null){
return up.vtd.inN();
}else{
return false;
}
}

OZ(up,"adg",H7I);
function H7I(){
if(up.vtd!=null){
return up.vtd.adg();
}else{
return null;
}
}

OZ(up,"qaO",H0W);
function H0W(){
if(up.vtd!=null){
return up.vtd.qaO();
}else{
return false;
}
}

OZ(up,"n4x",M9P);
function M9P(){
if(up.vtd!=null){
return up.vtd.n4x();
}else{
return ConvivaContentInfo.RKy;
}
}

OZ(up,"Iom",w71);
function w71(){
if(up.vtd!=null){
return up.vtd.Iom();
}else{
return ConvivaContentInfo.jNU;
}
}

OZ(up,"RhM",D_q);
function D_q(){
if(up.vtd!=null){
return up.vtd.RhM();
}else{
return null;
}
}

OZ(up,"c_7",SNm);
function SNm(){
if(up.vtd!=null){
return up.vtd.c_7();
}else{
return null;
}
}

OZ(up,"ArX",mbi);
function mbi(){
if(up.vtd!=null){
return up.vtd.ArX();
}else{
return null;
}
}

OZ(up,"rTo",Q9h);
function Q9h(){
if(up.vtd!=null){
return up.vtd.rTo();
}else{
return null;
}
}

OZ(up,"eMb",Id7);
function Id7(){
if(up.vtd!=null){
return up.vtd.eMb();
}else{
return null;
}
}

OZ(up,"ThB",rg1);
function rg1(){
if(up.vtd!=null){
return up.vtd.ThB();
}else{
return null;
}
}

OZ(up,"EYb",NsI);
function NsI(){
if(up.vtd!=null){
return up.vtd.EYb();
}else{
return null;
}
}

OZ(up,"_Y8",JsD);
function JsD(){
if(up.vtd!=null){
return up.vtd._Y8();
}else{
return null;
}
}

OZ(up,"v8h",HnS);
function HnS(){
if(up.vtd!=null){
return up.vtd.v8h();
}else{
return null;
}
}

OZ(up,"XJR",jok);
function jok(){
if(up.vtd!=null){
return up.vtd.XJR();
}else{
return null;
}
}

OZ(up,"BRI",wqG);
function wqG(){
if(up.vtd!=null){
return up.vtd.BRI();
}else{
return null;
}
}

OZ(up,"ieU",wEU);
function wEU(){
if(up.vtd!=null){
return up.vtd.ieU();
}else{
return null;
}
}

OZ(up,"R9p",wQJ);
function wQJ(){
if(up.vtd!=null){
return up.vtd.R9p();
}else{
return 0;
}
}

OZ(up,"V5s",tZ7);
function tZ7(){
if(up.vtd!=null){
return up.vtd.V5s();
}else{
return 0;
}
}

if(up!=fj)ju.apply(up,arguments);
}
Bg(kzq,"kzq");



















function ConvivaContentInfo(){
var up=this;


























































if(up==fj)ConvivaContentInfo.r9f="AKAMAI";

if(up==fj)ConvivaContentInfo.GGN="AMAZON";

if(up==fj)ConvivaContentInfo.fGO="ATT";

if(up==fj)ConvivaContentInfo.mYS="BITGRAVITY";

if(up==fj)ConvivaContentInfo.BD8="BT";

if(up==fj)ConvivaContentInfo.jkP="CDNETWORKS";

if(up==fj)ConvivaContentInfo.J01="CDNVIDEO";

if(up==fj)ConvivaContentInfo.OU6="CHINACACHE";

if(up==fj)ConvivaContentInfo.RH2="COMCAST";

if(up==fj)ConvivaContentInfo.LZN="EDGECAST";

if(up==fj)ConvivaContentInfo.Qye="HIGHWINDS";

if(up==fj)ConvivaContentInfo.GDQ="INTERNAP";

if(up==fj)ConvivaContentInfo.mB6="IPONLY";

if(up==fj)ConvivaContentInfo.BRL="LEVEL3";

if(up==fj)ConvivaContentInfo.jbi="LIMELIGHT";

if(up==fj)ConvivaContentInfo.LBr="MICROSOFT";

if(up==fj)ConvivaContentInfo.eOQ="NGENIX";

if(up==fj)ConvivaContentInfo.pwl="NICE";

if(up==fj)ConvivaContentInfo.y1x="OCTOSHAPE";

if(up==fj)ConvivaContentInfo.J6r="QBRICK";

if(up==fj)ConvivaContentInfo.rNL="SWARMCAST";

if(up==fj)ConvivaContentInfo.mih="TELEFONICA";

if(up==fj)ConvivaContentInfo.MHE="TELENOR";

if(up==fj)ConvivaContentInfo.CVv="VELOCIX";





if(up==fj)ConvivaContentInfo.ZGk="INHOUSE";




if(up==fj)ConvivaContentInfo.jNU="OTHER";


if(up==fj)ConvivaContentInfo._jw="Brightcove";

if(up==fj)ConvivaContentInfo.RAm="Kaltura";

if(up==fj)ConvivaContentInfo.Pwz="Ooyala";

if(up==fj)ConvivaContentInfo.x4Q="thePlatform";


if(up==fj)ConvivaContentInfo.Ot6="Brightcove";

if(up==fj)ConvivaContentInfo.ycR="Kaltura";

if(up==fj)ConvivaContentInfo.jPN="Ooyala";

if(up==fj)ConvivaContentInfo.fMQ="OSMF";

if(up==fj)ConvivaContentInfo.alw="thePlatform";


if(up==fj)ConvivaContentInfo.fSH="Console";

if(up==fj)ConvivaContentInfo.wdm="Mobile";

if(up==fj)ConvivaContentInfo.mMX="PC";

if(up==fj)ConvivaContentInfo.G1s="Settop";




if(up==fj)ConvivaContentInfo._6t="no_resource";


if(up==fj)ConvivaContentInfo.Ld2="ConvivaKalturaPlugin";


if(up==fj)ConvivaContentInfo.uf3="nominalBitrateOverride";









if(up!=fj)up.a4c=false;










ED(up,"K_y",oZN);ED(up,"useStrictChecking",oZN);
function oZN(){return up.a4c;}
Wf(up,"K_y",YiA);Wf(up,"useStrictChecking",YiA);
function YiA(nQ){up.a4c=nQ;}

if(up!=fj)up.i6m=null;




ED(up,"CXF",IBV);ED(up,"assetName",IBV);
function IBV(){return up.i6m;}
Wf(up,"CXF",SMB);Wf(up,"assetName",SMB);
function SMB(nQ){up.i6m=nQ;}

if(up!=fj)up.rT7=undefined;





ED(up,"hA",ml);ED(up,"tags",ml);
function ml(){return oL.uM(up.rT7);}
Wf(up,"hA",gn);Wf(up,"tags",gn);
function gn(nQ){up.rT7=oL.PX(nQ);}

if(up!=fj)up.ta=undefined;




ED(up,"Sc",FZ);ED(up,"candidateResources",FZ);
function FZ(){return oL.R5(up.ta);}
Wf(up,"Sc",vQ);Wf(up,"candidateResources",vQ);
function vQ(nQ){
up.ta=oL.js(nQ);
}

if(up!=fj)up.oLs=false;


ED(up,"fi",fWP);ED(up,"resourceSelection",fWP);
function fWP(){return up.oLs;}
Wf(up,"fi",E8J);Wf(up,"resourceSelection",E8J);
function E8J(nQ){up.oLs=nQ;}

if(up!=fj)up.wwE=undefined;






















ED(up,"N_",C7);ED(up,"monitoringOptions",C7);
function C7(){return oL.uM(up.wwE);}
Wf(up,"N_",Bv);Wf(up,"monitoringOptions",Bv);
function Bv(nQ){up.wwE=oL.PX(nQ);}

if(up!=fj)up.LUA=null;













ED(up,"_fK",VUf);ED(up,"cdnName",VUf);
function VUf(){return up.LUA;}
Wf(up,"_fK",j6i);Wf(up,"cdnName",j6i);
function j6i(nQ){up.LUA=nQ;}





if(up!=fj)up.BYE=null;

if(up!=fj)up.wUp=null;

















ED(up,"hTs",M0F);ED(up,"resourceStateOverride",M0F);
function M0F(){return up.wUp;}
Wf(up,"hTs",fEY);Wf(up,"resourceStateOverride",fEY);
function fEY(nQ){up.wUp=nQ;}

if(up!=fj)up.D3q=null;











ED(up,"nFQ",XWl);ED(up,"ovppName",XWl);
function XWl(){return up.D3q;}
Wf(up,"nFQ",acQ);Wf(up,"ovppName",acQ);
function acQ(nQ){up.D3q=nQ;}

if(up!=fj)up.Db8=null;













ED(up,"eQZ",mH1);ED(up,"frameworkName",mH1);
function mH1(){return up.Db8;}
Wf(up,"eQZ",cx_);Wf(up,"frameworkName",cx_);
function cx_(nQ){up.Db8=nQ;}

if(up!=fj)up.TSp=null;








ED(up,"Jqo",YNQ);ED(up,"frameworkVersion",YNQ);
function YNQ(){return up.TSp;}
Wf(up,"Jqo",hHp);Wf(up,"frameworkVersion",hHp);
function hHp(nQ){up.TSp=nQ;}

if(up!=fj)up.IFU=null;











ED(up,"wL2",BVV);ED(up,"pluginName",BVV);
function BVV(){return up.IFU;}
Wf(up,"wL2",m7Y);Wf(up,"pluginName",m7Y);
function m7Y(nQ){up.IFU=nQ;}

if(up!=fj)up.XTh=null;










ED(up,"alT",KHz);ED(up,"pluginVersion",KHz);
function KHz(){return up.XTh;}
Wf(up,"alT",wTu);Wf(up,"pluginVersion",wTu);
function wTu(nQ){up.XTh=nQ;}

if(up!=fj)up.m6e=null;






ED(up,"o6d",ix_);ED(up,"viewerId",ix_);
function ix_(){return up.m6e;}
Wf(up,"o6d",Cgn);Wf(up,"viewerId",Cgn);
function Cgn(nQ){up.m6e=nQ;}

if(up!=fj)up.Ndb=null;










ED(up,"yuB",i0y);ED(up,"deviceId",i0y);
function i0y(){return up.Ndb;}
Wf(up,"yuB",Vmw);Wf(up,"deviceId",Vmw);
function Vmw(nQ){up.Ndb=nQ;}

if(up!=fj)up.J24=null;











ED(up,"lTq",OHw);ED(up,"deviceType",OHw);
function OHw(){return up.J24;}
Wf(up,"lTq",bmQ);Wf(up,"deviceType",bmQ);
function bmQ(nQ){
if(nQ!=null){
if(up.U8U.OC(nQ)){
up.J24=nQ;
}else{
ct.M3x("deviceType is not one of the recognized device types enumerated in ConvivaContentInfo.",
"Actual value: "+up.lTq+" . Set it to the default value PC.");
up.J24=ConvivaContentInfo.mMX;
}
}
}





if(up!=fj)up.U8U=null;

if(up!=fj)up.tUj=null;







ED(up,"qMR",WNr);ED(up,"playerName",WNr);
function WNr(){return up.tUj;}
Wf(up,"qMR",tz2);Wf(up,"playerName",tz2);
function tz2(nQ){up.tUj=nQ;}

if(up!=fj)up.t8r=null;







ED(up,"xQh",vS5);ED(up,"streamUrl",vS5);
function vS5(){return up.t8r;}
Wf(up,"xQh",Oib);Wf(up,"streamUrl",Oib);
function Oib(nQ){up.t8r=nQ;}

if(up==fj)ConvivaContentInfo.RKy=0;
if(up!=fj)up.fGP=ConvivaContentInfo.RKy;





ED(up,"Su",jtC);ED(up,"bitrate",jtC);
function jtC(){return up.fGP;}
Wf(up,"Su",HQn);Wf(up,"bitrate",HQn);
function HQn(nQ){up.fGP=nQ;}

if(up!=fj)up.r_o=false;





ED(up,"cex",Dz4);ED(up,"isLive",Dz4);
function Dz4(){return up.r_o;}
Wf(up,"cex",j4I);Wf(up,"isLive",j4I);
function j4I(nQ){
if(nQ==="true"){nQ=true;}
if(nQ==="false"){nQ=false;}
if(nQ!==true&&nQ!==false){
ct.Error("Invalid value for ConvivaContentInfo.isLive, expected boolean. Defaulting to false.");
nQ=false;
}
up.r_o=nQ;
}

if(up!=fj)up.go=null;











ED(up,"w6",dO);ED(up,"resource",dO);
function dO(){return up.go;}
Wf(up,"w6",pl3);Wf(up,"resource",pl3);
function pl3(nQ){up.go=nQ;}

if(up!=fj)up.FCX=0;






ED(up,"TMi",nql);ED(up,"groupId",nql);
function nql(){return up.FCX;}
Wf(up,"TMi",kaQ);Wf(up,"groupId",kaQ);
function kaQ(nQ){up.FCX=nQ;}

if(up!=fj)up.qAd=0;




ED(up,"oP9",g_3);ED(up,"groupType",g_3);
function g_3(){return up.qAd;}
Wf(up,"oP9",emE);Wf(up,"groupType",emE);
function emE(nQ){up.qAd=nQ;}






if(up==fj)ConvivaContentInfo.L1=128;

if(up==fj)ConvivaContentInfo.prP="Null title";









function ju(akg,Cr,Or){
up.SR4();
up.K_y=false;
up.CXF=akg;
up.hA=Or;
if(up.rT7==null){
up.rT7=new EW();
}


up.Sc=Cr;
up.fi=false;
if(up.ta==null){
up.ta=new Yw();
}
up.wwE=new EW();
up.fGP=0;
up.r_o=false;
up.FCX=0;
up.qAd=0;
}








dN(up,ConvivaContentInfo,"XxO",EDi);dN(up,ConvivaContentInfo,"createInfoForLightSession",EDi);
function EDi(CXF){
var LC=new ConvivaContentInfo(CXF,null,null);
return LC;
}


OZ(up,"ox",gv);OZ(up,"cleanup",gv);
function gv(){
up.FCX=0;
up.qAd=0;
}




OZ(up,"AV",_H);OZ(up,"sanitizeData",_H);
function _H(){
up.CXF=ConvivaContentInfo._C(up.CXF,ConvivaContentInfo.prP,true,"assetName");
up.nFQ=ConvivaContentInfo._C(up.nFQ,null,false,"ovppName");
up.eQZ=ConvivaContentInfo._C(up.eQZ,null,false,"frameworkName");
up.Jqo=ConvivaContentInfo._C(up.Jqo,null,false,"frameworkVersion");
up.wL2=ConvivaContentInfo._C(up.wL2,null,false,"pluginName");
up.alT=ConvivaContentInfo._C(up.alT,null,false,"pluginVersion");
up.o6d=ConvivaContentInfo._C(up.o6d,null,false,"viewerId");
up.yuB=ConvivaContentInfo._C(up.yuB,null,false,"deviceId");
up.lTq=ConvivaContentInfo._C(up.lTq,null,false,"deviceType");
up.qMR=ConvivaContentInfo._C(up.qMR,null,false,"playerName");

if(up.rT7==null){
up.rT7=new EW();
}

var WG=new Yw();
var bC=up.rT7.VO;
for(var Jo=0;Jo<bC.length;Jo++){
var BK=bC[Jo];

WG.Yb(BK);
}
var HH=WG.YC;
for(var wt=0;wt<HH.length;wt++){
var VF=HH[wt];




var Gn=ConvivaContentInfo._C(VF,"null",false,"tag name");
var Jm=ConvivaContentInfo._C(up.rT7.tc(VF),"null",false,"value for tag '"+oL.fg(VF)+"'");
if(Gn!=VF){
up.rT7.gS(VF);
up.rT7.a9(Gn,Jm);
}
if(Jm!=up.rT7.tc(Gn)){
up.rT7.a9(Gn,Jm);
}
}




}


dN(up,ConvivaContentInfo,"_C",q6);dN(up,ConvivaContentInfo,"sanitizeString",q6);
function q6(nI,Rd,vH,Sj){
if(nI==null&&Rd==null){
return null;
}

if(nI==null){
O9.Ep("ConvivaContentInfo: "+Sj+" is null",false);
nI=Rd;
}
var h7=oL.fg(nI);
h7=oL.xO(h7);


if(vH&&h7==O9.Mul){

}else if(h7.length>=3&&oL.n0(h7,0,3)=="c3."){
O9.Ep("ConvivaContentInfo: "+Sj+" is reserved IGNORE:"+h7,false);
h7="_"+h7;
}

if(h7.length>ConvivaContentInfo.L1){
O9.Ep("ConvivaContentInfo: "+Sj+" is too long IGNORE:"+h7,false);
h7=oL.n0(h7,0,ConvivaContentInfo.L1);
}
return h7;
}

LK(up,"SR4",lPt);
function lPt(){
up.BYE=new Yw();
up.BYE.Yb(ConvivaContentInfo.r9f);
up.BYE.Yb(ConvivaContentInfo.GGN);
up.BYE.Yb(ConvivaContentInfo.fGO);
up.BYE.Yb(ConvivaContentInfo.mYS);
up.BYE.Yb(ConvivaContentInfo.BD8);
up.BYE.Yb(ConvivaContentInfo.jkP);
up.BYE.Yb(ConvivaContentInfo.OU6);
up.BYE.Yb(ConvivaContentInfo.LZN);
up.BYE.Yb(ConvivaContentInfo.Qye);
up.BYE.Yb(ConvivaContentInfo.GDQ);
up.BYE.Yb(ConvivaContentInfo.BRL);
up.BYE.Yb(ConvivaContentInfo.jbi);
up.BYE.Yb(ConvivaContentInfo.y1x);
up.BYE.Yb(ConvivaContentInfo.rNL);
up.BYE.Yb(ConvivaContentInfo.CVv);
up.BYE.Yb(ConvivaContentInfo.mih);
up.BYE.Yb(ConvivaContentInfo.LBr);
up.BYE.Yb(ConvivaContentInfo.J01);
up.BYE.Yb(ConvivaContentInfo.J6r);
up.BYE.Yb(ConvivaContentInfo.eOQ);
up.BYE.Yb(ConvivaContentInfo.mB6);
up.BYE.Yb(ConvivaContentInfo.ZGk);
up.BYE.Yb(ConvivaContentInfo.RH2);
up.BYE.Yb(ConvivaContentInfo.pwl);
up.BYE.Yb(ConvivaContentInfo.MHE);
up.BYE.Yb(ConvivaContentInfo.jNU);
up.U8U=new Yw();
up.U8U.Yb(ConvivaContentInfo.fSH);
up.U8U.Yb(ConvivaContentInfo.wdm);
up.U8U.Yb(ConvivaContentInfo.mMX);
up.U8U.Yb(ConvivaContentInfo.G1s);
}



LK(up,"c_",iD);
function iD(){
var cN="{";
cN+="\n  useStrictChecking="+oL.fg(up.a4c);
cN+="\n  assetName=\""+up.i6m+"\"";
cN+="\n  bitrate="+oL.fg(up.fGP);
cN+="\n  isLive="+oL.fg(up.r_o);
cN+="\n  monitoringOptions="+Gnw.jMw(up.wwE);
cN+="\n  resourceSelection="+oL.fg(up.oLs);
cN+="\n  candidateResources="+Gnw.hJD(up.ta);
cN+="\n  cdnName="+(up._fK!=null?"\""+up._fK+"\"":"null");
cN+="\n  resource="+(up.w6!=null?"\""+up.w6+"\"":"null");
cN+="\n  resourceStateOverride="+(up.hTs!=null?"\""+up.hTs+"\"":"null");
cN+="\n  ovppName="+(up.nFQ!=null?"\""+up.nFQ+"\"":"null");
cN+="\n  frameworkName="+(up.eQZ!=null?"\""+up.eQZ+"\"":"null");
cN+="\n  frameworkVersion="+(up.Jqo!=null?"\""+up.Jqo+"\"":"null");
cN+="\n  pluginName="+(up.wL2!=null?"\""+up.wL2+"\"":"null");
cN+="\n  pluginVersion="+(up.alT!=null?"\""+up.alT+"\"":"null");
cN+="\n  streamUrl="+(up.xQh!=null?"\""+up.xQh+"\"":"null");
cN+="\n  viewerId="+(up.o6d!=null?"\""+up.o6d+"\"":"null");
cN+="\n  deviceId="+(up.yuB!=null?"\""+up.yuB+"\"":"null");
cN+="\n  deviceType="+(up.lTq!=null?"\""+up.lTq+"\"":"null");
cN+="\n  playerName="+(up.qMR!=null?"\""+up.qMR+"\"":"null");
cN+="\n  groupId="+oL.fg(up.FCX);
cN+="\n  groupType="+oL.fg(up.qAd);
cN+="\n  tags="+Gnw.jMw(up.rT7);
return cN;
}




dN(up,ConvivaContentInfo,"o1",b0);dN(up,ConvivaContentInfo,"clone",b0);
function b0(jN){




jN.AV(false);
return jN;
}

OZ(up,"obv",Ep9);OZ(up,"printContentInfo",Ep9);
function Ep9(Bl){
ct.qF("ConvivaContentInfo["+Bl+"]","user input="+up.c_());
}



















OZ(up,"XUT",rxV);OZ(up,"checkValidity",rxV);
function rxV(kZl,IiH,V7){
if(!up.K_y){
kZl=IiH;
}
if(up.CXF==null){
kZl("assetName is null.",null);
}
if(up.CXF==ConvivaContentInfo.prP){
kZl("assetName is null.",null);
}
if(up.lTq!=null&&!up.U8U.OC(up.lTq)){
kZl("deviceType is not one of the recognized device types enumerated in ConvivaContentInfo.",
"Actual value: "+up.lTq+" .");
}
if(V7){
if(up.hTs!=null){
IiH("resourceStateOverride is in use.  CDN-based metrics may be inaccurate.",null);

}else{
if(up._fK==null){
kZl("cdnName is null.",null);
}
if(up._fK!=null&&!up.BYE.OC(up._fK)){
kZl("cdnName is not one of the recognized CDN names enumerated in ConvivaContentInfo.",
"Actual value: "+up._fK+" .");
}
if(up._fK==ConvivaContentInfo.jNU){
IiH("cdnName is CDN_NAME_OTHER.  If possible, please specify a real CDN.",null);
}
}
if(up.xQh==null){
kZl("streamUrl is null.",null);
}




if(up.Su<0){
kZl("bitrate is negative.","Actual value: "+up.Su+" .");
}
}
}

dN(up,ConvivaContentInfo,"CDN_NAME_AKAMAI",ConvivaContentInfo.r9f);
dN(up,ConvivaContentInfo,"CDN_NAME_AMAZON",ConvivaContentInfo.GGN);
dN(up,ConvivaContentInfo,"CDN_NAME_ATT",ConvivaContentInfo.fGO);
dN(up,ConvivaContentInfo,"CDN_NAME_BITGRAVITY",ConvivaContentInfo.mYS);
dN(up,ConvivaContentInfo,"CDN_NAME_BT",ConvivaContentInfo.BD8);
dN(up,ConvivaContentInfo,"CDN_NAME_CDNETWORKS",ConvivaContentInfo.jkP);
dN(up,ConvivaContentInfo,"CDN_NAME_CDNVIDEO",ConvivaContentInfo.J01);
dN(up,ConvivaContentInfo,"CDN_NAME_CHINACACHE",ConvivaContentInfo.OU6);
dN(up,ConvivaContentInfo,"CDN_NAME_COMCAST",ConvivaContentInfo.RH2);
dN(up,ConvivaContentInfo,"CDN_NAME_EDGECAST",ConvivaContentInfo.LZN);
dN(up,ConvivaContentInfo,"CDN_NAME_HIGHWINDS",ConvivaContentInfo.Qye);
dN(up,ConvivaContentInfo,"CDN_NAME_INHOUSE",ConvivaContentInfo.ZGk);
dN(up,ConvivaContentInfo,"CDN_NAME_INTERNAP",ConvivaContentInfo.GDQ);
dN(up,ConvivaContentInfo,"CDN_NAME_IPONLY",ConvivaContentInfo.mB6);
dN(up,ConvivaContentInfo,"CDN_NAME_LEVEL3",ConvivaContentInfo.BRL);
dN(up,ConvivaContentInfo,"CDN_NAME_LIMELIGHT",ConvivaContentInfo.jbi);
dN(up,ConvivaContentInfo,"CDN_NAME_MICROSOFT",ConvivaContentInfo.LBr);
dN(up,ConvivaContentInfo,"CDN_NAME_NGENIX",ConvivaContentInfo.eOQ);
dN(up,ConvivaContentInfo,"CDN_NAME_NICE",ConvivaContentInfo.pwl);
dN(up,ConvivaContentInfo,"CDN_NAME_OCTOSHAPE",ConvivaContentInfo.y1x);
dN(up,ConvivaContentInfo,"CDN_NAME_OTHER",ConvivaContentInfo.jNU);
dN(up,ConvivaContentInfo,"CDN_NAME_QBRICK",ConvivaContentInfo.J6r);
dN(up,ConvivaContentInfo,"CDN_NAME_SWARMCAST",ConvivaContentInfo.rNL);
dN(up,ConvivaContentInfo,"CDN_NAME_TELEFONICA",ConvivaContentInfo.mih);
dN(up,ConvivaContentInfo,"CDN_NAME_TELENOR",ConvivaContentInfo.MHE);
dN(up,ConvivaContentInfo,"CDN_NAME_VELOCIX",ConvivaContentInfo.CVv);
dN(up,ConvivaContentInfo,"DEFAULT_ASSET_NAME",ConvivaContentInfo.prP);
dN(up,ConvivaContentInfo,"DEFAULT_BITRATE_VAL",ConvivaContentInfo.RKy);
dN(up,ConvivaContentInfo,"DEVICE_TYPE_CONSOLE",ConvivaContentInfo.fSH);
dN(up,ConvivaContentInfo,"DEVICE_TYPE_MOBILE",ConvivaContentInfo.wdm);
dN(up,ConvivaContentInfo,"DEVICE_TYPE_PC",ConvivaContentInfo.mMX);
dN(up,ConvivaContentInfo,"DEVICE_TYPE_SET_TOP_BOX",ConvivaContentInfo.G1s);
dN(up,ConvivaContentInfo,"FRAMEWORK_NAME_BRIGHTCOVE",ConvivaContentInfo.Ot6);
dN(up,ConvivaContentInfo,"FRAMEWORK_NAME_KALTURA",ConvivaContentInfo.ycR);
dN(up,ConvivaContentInfo,"FRAMEWORK_NAME_OOYALA",ConvivaContentInfo.jPN);
dN(up,ConvivaContentInfo,"FRAMEWORK_NAME_OSMF",ConvivaContentInfo.fMQ);
dN(up,ConvivaContentInfo,"FRAMEWORK_NAME_THE_PLATFORM",ConvivaContentInfo.alw);
dN(up,ConvivaContentInfo,"MAX_PARAMETER_LENGTH",ConvivaContentInfo.L1);
dN(up,ConvivaContentInfo,"MO_KEY_NOMINAL_BITRATE_OVERRIDE",ConvivaContentInfo.uf3);
dN(up,ConvivaContentInfo,"NO_RESOURCE",ConvivaContentInfo._6t);
dN(up,ConvivaContentInfo,"OVPP_NAME_BRIGHTCOVE",ConvivaContentInfo._jw);
dN(up,ConvivaContentInfo,"OVPP_NAME_KALTURA",ConvivaContentInfo.RAm);
dN(up,ConvivaContentInfo,"OVPP_NAME_OOYALA",ConvivaContentInfo.Pwz);
dN(up,ConvivaContentInfo,"OVPP_NAME_THE_PLATFORM",ConvivaContentInfo.x4Q);
dN(up,ConvivaContentInfo,"PLUGIN_NAME_KALTURA",ConvivaContentInfo.Ld2);
if(up!=fj)ju.apply(up,arguments);
}
Bg(ConvivaContentInfo,"ConvivaContentInfo");











function ConvivaNotification(){
var up=this;

if(up==fj)ConvivaNotification.g9=0;




if(up==fj)ConvivaNotification.rg=1;




if(up==fj)ConvivaNotification.cf=2;





if(up==fj)ConvivaNotification.bQ=10;




if(up==fj)ConvivaNotification.vU=11;




if(up==fj)ConvivaNotification.HBx=20;




if(up==fj)ConvivaNotification.Po=30;




if(up!=fj)up.kF=undefined;

function ju(iv,message,jz){
up.kF=new EW();
up.kF.a9("code",iv);
up.kF.a9("message",message);
up.kF.a9("objectId",jz);
}





ED(up,"iv",bc);ED(up,"code",bc);
function bc(){
return VW.z_(up.kF.tc("code"));
}





ED(up,"message",xs);ED(up,"message",xs);
function xs(){
return up.kF.tc("message");
}






ED(up,"jz",wa);ED(up,"objectId",wa);
function wa(){
return up.kF.tc("objectId");
}



OZ(up,"toString",vZ);OZ(up,"toString",vZ);
function vZ(){
var S5="ConvivaNotification ";
var l8m=oL.fg(up.iv);
if(l8m!=null){
S5=S5+"("+l8m+"): ";
}
if(up.message!=null){
S5=S5+up.message;
}
if(up.jz!=null){
S5=S5+" (for objectId "+up.jz+")";
}
return S5;
}

dN(up,ConvivaNotification,"ERROR_ARGUMENT",ConvivaNotification.rg);
dN(up,ConvivaNotification,"ERROR_INIT_TIMESOUT",ConvivaNotification.HBx);
dN(up,ConvivaNotification,"ERROR_LIVEPASS_NOT_READY",ConvivaNotification.cf);
dN(up,ConvivaNotification,"ERROR_LOAD_CONFIGURATION",ConvivaNotification.bQ);
dN(up,ConvivaNotification,"ERROR_LOAD_MODULE",ConvivaNotification.vU);
dN(up,ConvivaNotification,"ERROR_METRICS_QUOTA_EXCEEDED",ConvivaNotification.Po);
dN(up,ConvivaNotification,"SUCCESS_LIVEPASS_READY",ConvivaNotification.g9);
if(up!=fj)ju.apply(up,arguments);
}
Bg(ConvivaNotification,"ConvivaNotification");












































































function ConvivaStreamerProxy(){
var up=this;

if(up!=fj)up.Mqe=new Yw();


if(up!=fj)up.UQZ=new StreamInfo(-1,ConvivaContentInfo.jNU,ConvivaContentInfo._6t,-1,-1,-1);
if(up!=fj)up.OjE=ConvivaStreamerProxy.dHj;


if(up!=fj)up.fDr=null;


if(up!=fj)up.sOE=null;

if(up!=fj)up.FvG=new Yw();


if(up!=fj)up.BaK=-1;


if(up!=fj)up.H7g=-1;


if(up!=fj)up.Hfe=null;

if(up!=fj)up.fx3=null;

if(up!=fj)up.BW7=undefined;






if(up==fj)ConvivaStreamerProxy.yeD="PlayingStateChange";






if(up==fj)ConvivaStreamerProxy.xUm="StreamInfoChange";





if(up==fj)ConvivaStreamerProxy.pOB="AvailableStreamInfoChange";






if(up==fj)ConvivaStreamerProxy.U6H="ProxyInitialized";





if(up==fj)ConvivaStreamerProxy.GMW="MetadataChange";





if(up==fj)ConvivaStreamerProxy.kTl="LoadingStatusChange";






if(up==fj)ConvivaStreamerProxy.zJ2="DisplaySizeChange";





if(up==fj)ConvivaStreamerProxy.vHz="ErrorChange";





if(up==fj)ConvivaStreamerProxy.x1x="Log";




if(up==fj)ConvivaStreamerProxy.HwE="STOPPED";


if(up==fj)ConvivaStreamerProxy.tAQ="PLAYING";



if(up==fj)ConvivaStreamerProxy.vkg="BUFFERING";


if(up==fj)ConvivaStreamerProxy.yzw="PAUSED";




if(up==fj)ConvivaStreamerProxy.fFX="NOT_MONITORED";


if(up==fj)ConvivaStreamerProxy.FVe="ERROR";


if(up==fj)ConvivaStreamerProxy.dHj="UNKNOWN";







if(up==fj)ConvivaStreamerProxy.biP="duration";

if(up==fj)ConvivaStreamerProxy.zoT="framerate";











if(up==fj)ConvivaStreamerProxy.Khw=1;
if(up==fj)ConvivaStreamerProxy.BvQ=-1;
if(up==fj)ConvivaStreamerProxy.ObP=0;





if(up==fj)ConvivaStreamerProxy.JV=1;
if(up==fj)ConvivaStreamerProxy.rgw=2;
if(up==fj)ConvivaStreamerProxy.IrL=4;


if(up==fj)ConvivaStreamerProxy._Ci=8;
if(up==fj)ConvivaStreamerProxy.b7N=16;


if(up==fj)ConvivaStreamerProxy.asA=32;
if(up==fj)ConvivaStreamerProxy.RXQ=64;
if(up==fj)ConvivaStreamerProxy.pbC=128;

function ju(){

up.eco(ConvivaStreamerProxy.HwE);

}




OZ(up,"wy",NQ);OZ(up,"Cleanup",NQ);
function NQ(){
if(up.Mqe!=null){
up.Mqe.m3();
}
}






OZ(up,"Ukq",BmZ);OZ(up,"GetCapabilities",BmZ);
function BmZ(){
return 0;
}










OZ(up,"dD",CC);OZ(up,"GetPlayheadTimeMs",CC);
function CC(){
return-1;
}








OZ(up,"k7K",zFu);OZ(up,"GetIsPHTAccurate",zFu);
function zFu(){
return true;
}










OZ(up,"ML",zp);OZ(up,"GetBufferLengthMs",zp);
function zp(){
return-1;
}
















OZ(up,"nHx",hNf);OZ(up,"GetStartingBufferLengthMs",hNf);
function hNf(){
return-1;
}








OZ(up,"YU7",LGW);OZ(up,"SetStartingBufferLengthMs",LGW);
function LGW(OM){

}











OZ(up,"Wrt",JVG);OZ(up,"GetRenderedFrameRate",JVG);
function JVG(){
return-1.0;
}








OZ(up,"glF",dwI);OZ(up,"GetSourceFrameRate",dwI);
function dwI(){
return-1.0;
}








OZ(up,"a1",jn);OZ(up,"GetDroppedFrames",jn);
function jn(){
return-1;
}






OZ(up,"JIr",RWb);OZ(up,"GetCpuPercent",RWb);
function RWb(){
return-1.0;
}






OZ(up,"ilA",b46);OZ(up,"GetMemoryAvailable",b46);
function b46(){
return-1.0;
}










OZ(up,"TzN",Cav);OZ(up,"GetCapacityKbps",Cav);
function Cav(w6){
return-1;
}











OZ(up,"bvd",kLC);OZ(up,"GetServerAddress",kLC);
function kLC(){
return null;
}











OZ(up,"qaO",H0W);OZ(up,"GetIsLive",H0W);
function H0W(){
return ConvivaStreamerProxy.ObP;
}











OZ(up,"SNS",CDf);OZ(up,"GetStreamerVersion",CDf);
function CDf(){
return-1;
}















OZ(up,"hv7",taf);OZ(up,"FetchCandidateStreams",taf);
function taf(w6){

}

















OZ(up,"WvT",MUD);OZ(up,"Play",MUD);
function MUD(SVV){
}










OZ(up,"o1i",AnQ);OZ(up,"GetApiVersion",AnQ);
function AnQ(){
return ConvivaStreamerProxy.aM_;
}
if(up==fj)ConvivaStreamerProxy.aM_=1;


























OZ(up,"FGc",UP);OZ(up,"SetMonitoringNotifier",UP);
function UP(VW8){

if(VW8==null)return;

var MlC=up.Mqe;

var nWi=new Yw();
nWi.Yb(VW8);
up.Mqe=nWi;


if(up.Mqe!=null){



if(up.BW7){
VW8(ConvivaStreamerProxy.U6H,null);
}


var xCd=up.UQZ;
up.UQZ=new StreamInfo(-1,ConvivaContentInfo.jNU,ConvivaContentInfo._6t,-1,-1,-1);
up.cQi(xCd);


var Uy=up.OjE;
up.OjE=ConvivaStreamerProxy.dHj;
up.eco(Uy);


var Y2S=up.fDr;
up.fDr=null;
up.Pmd(Y2S);


var bC=up.FvG.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var lhg=bC[Jo];

up.rpy(lhg);
}

var OrJ=up.Hfe;
up.Hfe=null;
up.UCH(OrJ);

var kiQ=(up.fx3);
up.fx3=null;
up.oed(kiQ);

var OOD=up.BaK;
var Rkn=up.BaK;
up.BaK=-1;
up.H7g=-1;
up.fJw(OOD,Rkn);
}

MlC.Yb(VW8);
up.Mqe=MlC;
}











OZ(up,"FkN",puP);OZ(up,"GetLoadingStatus",puP);
function puP(){
return up.fx3;
}










OZ(up,"MnZ",luX);OZ(up,"GetPlayingState",luX);
function luX(){
return up.OjE;
}










OZ(up,"Wid",dMh);OZ(up,"GetBitrateKbps",dMh);
function dMh(){
return up.UQZ.M9;
}







OZ(up,"Iom",w71);OZ(up,"GetCdnName",w71);
function w71(){
return up.UQZ._fK;
}










OZ(up,"RhM",D_q);OZ(up,"GetResource",D_q);
function D_q(){
return up.UQZ.w6;
}













OZ(up,"M8p",Afd);OZ(up,"GetStream",Afd);
function Afd(){
return up.UQZ;
}






OZ(up,"t_t",oJI);OZ(up,"GetStreams",oJI);
function oJI(){
return null;
}









OZ(up,"dcl",TsV);OZ(up,"GetLastError",TsV);
function TsV(){
return up.sOE;
}







OZ(up,"tUW",ACM);OZ(up,"GetLastMetadata",ACM);
function ACM(){
return up.fDr;
}









OZ(up,"br4",STP);OZ(up,"GetDisplayWidth",STP);
function STP(){
return up.BaK;
}





OZ(up,"cMt",hhq);OZ(up,"GetDisplayHeight",hhq);
function hhq(){
return up.H7g;
}



















OZ(up,"HS_",IJz);OZ(up,"GetStreamerType",IJz);
function IJz(){
return null;
}











OZ(up,"nOK",kNr);OZ(up,"GetLoadedBytes",kNr);
function kNr(){
return-1;
}










OZ(up,"cQi",kXO);OZ(up,"SetStream",kXO);
function kXO(Wiw){

if(Wiw.M9<=-2)Wiw.M9=up.UQZ.M9;
if(Wiw._fK==null)Wiw._fK=up.UQZ._fK;
if(Wiw.w6==null)Wiw.w6=up.UQZ.w6;
if(!up.UQZ.j5q(Wiw)){
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.xUm,Wiw);
}
}
up.UQZ=Wiw;
}












OZ(up,"rpy",qEn);OZ(up,"SetError",qEn);
function qEn(lNq){

up.sOE=lNq;

if(lNq!=null){
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.vHz,lNq);
}
}else{
up.FvG.Yb(lNq);
}
}

















OZ(up,"Pmd",ZSP);OZ(up,"SetMetadata",ZSP);
function ZSP(UFr){

up.fDr=UFr;

if(UFr!=null){
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.GMW,UFr);
}
}

}








OZ(up,"WWf",u6d);OZ(up,"Log",u6d);
function u6d(message){
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.x1x,message);
}

}







OZ(up,"_fW",_bK);OZ(up,"LogError",_bK);
function _bK(message){
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.x1x,"ERROR:"+message);
}
}

















OZ(up,"wFZ",Ss1);OZ(up,"GetTotalLoadedBytes",Ss1);
function Ss1(){
return null;
}










OZ(up,"oed",yFG);OZ(up,"SetLoadingStatus",yFG);
function yFG(Nch){
up.fx3=Nch;
if(Nch!=null){
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.kTl,Nch);
}
}
}








OZ(up,"eco",apd);OZ(up,"SetPlayingState",apd);
function apd(Uy){

if(Uy!=up.OjE){
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.yeD,Uy);
}
}
up.OjE=Uy;
}








OZ(up,"sgt",ECq);OZ(up,"SetBitrateKbps",ECq);
function ECq(TC8){
var Wiw=new StreamInfo(TC8,null,null,-1,-1,-1);
up.cQi(Wiw);
}








OZ(up,"txs",o3O);OZ(up,"SetCdnName",o3O);
function o3O(TRS){
var Wiw=new StreamInfo(-2,TRS,null,-1,-1,-1);
up.cQi(Wiw);
}









OZ(up,"xQX",Iwf);OZ(up,"SetResource",Iwf);
function Iwf(eoy){
var Wiw=new StreamInfo(-2,null,eoy,-1,-1,-1);
up.cQi(Wiw);
}






OZ(up,"fJw",U28);OZ(up,"SetDisplaySize",U28);
function U28(_4W,dZo){

if(_4W!=-1&&dZo!=-1){

up.BaK=_4W;
up.H7g=dZo;

var Gm8=new Yw();
Gm8.Yb(_4W);
Gm8.Yb(dZo);
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.zJ2,Gm8);
}
}
}








OZ(up,"UCH",pBL);OZ(up,"SetAvailableStreams",pBL);
function pBL(Xdp){
up.Hfe=Xdp;
if(Xdp!=null){
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.pOB,Xdp);
}
}
}





OZ(up,"ydC",uhs);OZ(up,"SetProxyInitialized",uhs);
function uhs(){
if(!up.BW7){
up.BW7=true;
var bC=up.Mqe.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var VW8=bC[Jo];

up.j9V(VW8,ConvivaStreamerProxy.U6H,null);
}
}
}

LK(up,"j9V",IgC);
function IgC(VW8,TQ,SVh){
try{
VW8(TQ,SVh);
}catch(fe){
VW8(ConvivaStreamerProxy.x1x,"ConvivaStreamerProxy : uncaught exception "+fe);
}
}

dN(up,ConvivaStreamerProxy,"API_VERSION",ConvivaStreamerProxy.aM_);
dN(up,ConvivaStreamerProxy,"BOOLEAN_FALSE",ConvivaStreamerProxy.BvQ);
dN(up,ConvivaStreamerProxy,"BOOLEAN_TRUE",ConvivaStreamerProxy.Khw);
dN(up,ConvivaStreamerProxy,"BOOLEAN_UNAVAILABLE",ConvivaStreamerProxy.ObP);
dN(up,ConvivaStreamerProxy,"BUFFERING",ConvivaStreamerProxy.vkg);
dN(up,ConvivaStreamerProxy,"CAPABILITY_BITRATE_METRICS",ConvivaStreamerProxy.IrL);
dN(up,ConvivaStreamerProxy,"CAPABILITY_BITRATE_SWITCHING",ConvivaStreamerProxy._Ci);
dN(up,ConvivaStreamerProxy,"CAPABILITY_CDN_SWITCHING",ConvivaStreamerProxy.b7N);
dN(up,ConvivaStreamerProxy,"CAPABILITY_QUALITY_METRICS",ConvivaStreamerProxy.rgw);
dN(up,ConvivaStreamerProxy,"CAPABILITY_SOURCE_CHUNKED",ConvivaStreamerProxy.RXQ);
dN(up,ConvivaStreamerProxy,"CAPABILITY_SOURCE_HTTP",ConvivaStreamerProxy.asA);
dN(up,ConvivaStreamerProxy,"CAPABILITY_SOURCE_STREAMING",ConvivaStreamerProxy.pbC);
dN(up,ConvivaStreamerProxy,"CAPABILITY_VIDEO",ConvivaStreamerProxy.JV);
dN(up,ConvivaStreamerProxy,"ERROR",ConvivaStreamerProxy.FVe);
dN(up,ConvivaStreamerProxy,"METADATA_DURATION",ConvivaStreamerProxy.biP);
dN(up,ConvivaStreamerProxy,"METADATA_ENCODED_FRAMERATE",ConvivaStreamerProxy.zoT);
dN(up,ConvivaStreamerProxy,"NOT_MONITORED",ConvivaStreamerProxy.fFX);
dN(up,ConvivaStreamerProxy,"PAUSED",ConvivaStreamerProxy.yzw);
dN(up,ConvivaStreamerProxy,"PLAYING",ConvivaStreamerProxy.tAQ);
dN(up,ConvivaStreamerProxy,"REASON_AVAILABLESTREAMINFOCHANGE",ConvivaStreamerProxy.pOB);
dN(up,ConvivaStreamerProxy,"REASON_DISPLAYSIZECHANGE",ConvivaStreamerProxy.zJ2);
dN(up,ConvivaStreamerProxy,"REASON_ERRORCHANGE",ConvivaStreamerProxy.vHz);
dN(up,ConvivaStreamerProxy,"REASON_LOADINGSTATUSCHANGE",ConvivaStreamerProxy.kTl);
dN(up,ConvivaStreamerProxy,"REASON_LOG",ConvivaStreamerProxy.x1x);
dN(up,ConvivaStreamerProxy,"REASON_METADATACHANGE",ConvivaStreamerProxy.GMW);
dN(up,ConvivaStreamerProxy,"REASON_PLAYINGSTATECHANGE",ConvivaStreamerProxy.yeD);
dN(up,ConvivaStreamerProxy,"REASON_PROXYINITIALIZED",ConvivaStreamerProxy.U6H);
dN(up,ConvivaStreamerProxy,"REASON_STREAMINFOCHANGE",ConvivaStreamerProxy.xUm);
dN(up,ConvivaStreamerProxy,"STOPPED",ConvivaStreamerProxy.HwE);
dN(up,ConvivaStreamerProxy,"UNKNOWN",ConvivaStreamerProxy.dHj);
if(up!=fj)ju.apply(up,arguments);
}
Bg(ConvivaStreamerProxy,"ConvivaStreamerProxy");







function IcH(){
var up=this;

if(up==fj)IcH.X1E=null;

if(up==fj)IcH.DDV=null;




if(up==fj)IcH.me9="UNINITIALIZED";
if(up==fj)IcH.Lnh=0;

if(up==fj)IcH.tAQ="PLAYING";
if(up==fj)IcH.CS=3;
if(up==fj)IcH.HwE="STOPPED";
if(up==fj)IcH.cz=1;
if(up==fj)IcH.yzw="PAUSED";
if(up==fj)IcH.Qh=12;
if(up==fj)IcH.vkg="BUFFERING";
if(up==fj)IcH.DV=6;
if(up==fj)IcH.fFX="NOT_MONITORED";
if(up==fj)IcH.nw=98;
if(up==fj)IcH.dHj="UNKNOWN";
if(up==fj)IcH.ft=100;

dN(up,IcH,"Ns",D3);
function D3(){
IcH.X1E=new EW();
IcH.DDV=new EW();
IcH.X1E.a9(IcH.me9,IcH.Lnh);IcH.DDV.a9(IcH.Lnh,IcH.me9);
IcH.X1E.a9(IcH.tAQ,IcH.CS);IcH.DDV.a9(IcH.CS,IcH.tAQ);
IcH.X1E.a9(IcH.HwE,IcH.cz);IcH.DDV.a9(IcH.cz,IcH.HwE);
IcH.X1E.a9(IcH.yzw,IcH.Qh);IcH.DDV.a9(IcH.Qh,IcH.yzw);
IcH.X1E.a9(IcH.vkg,IcH.DV);IcH.DDV.a9(IcH.DV,IcH.vkg);
IcH.X1E.a9(IcH.fFX,IcH.nw);IcH.DDV.a9(IcH.nw,IcH.fFX);
IcH.X1E.a9(IcH.dHj,IcH.ft);IcH.DDV.a9(IcH.ft,IcH.dHj);
}

dN(up,IcH,"NXs",YVG);
function YVG(iQc){
if(IcH.X1E==null){
IcH.Ns();
}
if(IcH.X1E.Wt(iQc)){
return IcH.X1E.tc(iQc);
}else{
return IcH.ft;
}
}

dN(up,IcH,"i2y",Rws);
function Rws(ahR){
if(IcH.DDV==null){
IcH.Ns();
}
if(IcH.DDV.Wt(ahR)){
return IcH.DDV.tc(ahR);
}else{
return IcH.dHj;
}
}

dN(up,IcH,"ox",gv);
function gv(){
IcH.X1E=null;
}














}
Bg(IcH,"IcH");







function D4u(){
var up=this;
function ju(AR,eMe,f5,PE){
up.gtM=AR;
up.m5j=eMe;
up.IyF=f5;
up.n3c=PE;
}



if(up!=fj)up.gtM=undefined;
OZ(up,"O2s",e0B);
function e0B(){return up.gtM;}




if(up!=fj)up.m5j=undefined;
OZ(up,"Bwl",M2J);
function M2J(){return up.m5j;}




if(up!=fj)up.IyF=undefined;







OZ(up,"rlg",MSA);
function MSA(){
return oL.R5(up.IyF);
}

if(up!=fj)up.n3c=undefined;
OZ(up,"pEj",tF4);
function tF4(){return up.n3c;}
if(up!=fj)ju.apply(up,arguments);
}
Bg(D4u,"D4u");


















function StreamInfo(){
var up=this;







if(up==fj)StreamInfo.dHj=-1;


if(up==fj)StreamInfo.ct3=0;


if(up==fj)StreamInfo.qme=1;


if(up==fj)StreamInfo.ZIc=2;


if(up==fj)StreamInfo.yhl=3;





if(up!=fj)up.fm=StreamInfo.dHj;






if(up!=fj)up.mR7=-1;





if(up!=fj)up.vuw=-1;





if(up!=fj)up.M9=-1;



if(up!=fj)up.w6=null;



if(up!=fj)up._fK=null;








function ju(Jq,LUA,go,fm,Grb,GFW){
up.M9=Jq;
up._fK=LUA;
up.w6=go;
up.fm=fm;
up.mR7=GFW;
up.vuw=Grb;
}


OZ(up,"Wid",dMh);OZ(up,"GetBitrateKbps",dMh);
function dMh(){
return up.M9;
}


OZ(up,"Iom",w71);OZ(up,"GetCdnName",w71);
function w71(){
return up._fK;
}


OZ(up,"RhM",D_q);OZ(up,"GetResource",D_q);
function D_q(){
return up.w6;
}

OZ(up,"j5q",YQT);OZ(up,"equals",YQT);
function YQT(jy){
if(jy==null)return false;

return up._fK==jy._fK&&up.w6==jy.w6&&up.M9==jy.M9&&up.fm==jy.fm
&&up.mR7==jy.mR7&&up.vuw==jy.vuw;
}






dN(up,StreamInfo,"QKZ",UuP);dN(up,StreamInfo,"ConstructClone",UuP);
function UuP(lGX){
if(lGX==null)return null;

return lGX;
}

OZ(up,"vo",Ua);OZ(up,"ToStr",Ua);
function Ua(){
var Fe2=null;

switch(up.fm){
case StreamInfo.dHj:
Fe2="UNKNOWN";
break;
case StreamInfo.qme:
Fe2="VIDEO";
break;
case StreamInfo.ct3:
Fe2="AUDIO";
break;
case StreamInfo.ZIc:
Fe2="TEXT";
break;
case StreamInfo.yhl:
Fe2="RESOURCE";
break;
default:
throw new Error("Unknown stream type "+up.fm);
}

return "type="+Fe2+
", bitrateKbps="+up.M9+
", resource="+(up.w6!=null?up.w6:"null")+
", cdnName="+(up._fK!=null?up._fK:"null")+
", sourceHeightPixels="+up.mR7+
", sourceWidthPixels="+up.vuw;
}
dN(up,StreamInfo,"AUDIO",StreamInfo.ct3);
dN(up,StreamInfo,"RESOURCE",StreamInfo.yhl);
dN(up,StreamInfo,"TEXT",StreamInfo.ZIc);
dN(up,StreamInfo,"UNKNOWN",StreamInfo.dHj);
dN(up,StreamInfo,"VIDEO",StreamInfo.qme);
if(up!=fj)ju.apply(up,arguments);
}
Bg(StreamInfo,"StreamInfo");
























function StreamSwitch(){
var up=this;

if(up==fj)StreamSwitch.YF9=0;






if(up==fj)StreamSwitch.nsi="PENDING";



if(up==fj)StreamSwitch.QuW="IN_PROGRESS";




if(up==fj)StreamSwitch.TgA="SUCCEEDED";



if(up==fj)StreamSwitch.D4J="FAILED";




if(up==fj)StreamSwitch.UhJ="INTERRUPTED";








dN(up,StreamSwitch,"LII",p_4);dN(up,StreamSwitch,"MakeSwitch",p_4);
function p_4(bWS,wo1,f1){
return new StreamSwitch(StreamSwitch.BEz(false),bWS,wo1,-1,null,f1);
}














dN(up,StreamSwitch,"QpG",AhB);dN(up,StreamSwitch,"MakeSwitchToStream",AhB);
function AhB(wo1,f1){
return new StreamSwitch(StreamSwitch.BEz(false),null,wo1,-1,null,f1);
}




OZ(up,"wy",NQ);OZ(up,"Cleanup",NQ);
function NQ(){
}









if(up!=fj)up._cW=undefined;
ED(up,"kQ",d4);ED(up,"id",d4);
function d4(){return up._cW;}
Wf(up,"kQ",J_F);Wf(up,"id",J_F);
function J_F(nQ){up._cW=nQ;}









if(up!=fj)up.ILK=undefined;
ED(up,"n8",uAU);ED(up,"timeoutMs",uAU);
function uAU(){return up.ILK;}
Wf(up,"n8",g7B);Wf(up,"timeoutMs",g7B);
function g7B(nQ){up.ILK=nQ;}








if(up!=fj)up.w25=undefined;
ED(up,"Pkb",yrY);ED(up,"sourceStream",yrY);
function yrY(){return up.w25;}
Wf(up,"Pkb",z_O);Wf(up,"sourceStream",z_O);
function z_O(nQ){up.w25=nQ;}















if(up!=fj)up.cH3=undefined;
ED(up,"lfw",psL);ED(up,"targetStream",psL);
function psL(){return up.cH3;}
Wf(up,"lfw",MDi);Wf(up,"targetStream",MDi);
function MDi(nQ){up.cH3=nQ;}













if(up!=fj)up.IO1=undefined;
ED(up,"TMB",K4M);ED(up,"mode",K4M);
function K4M(){return up.IO1;}
Wf(up,"TMB",_jy);Wf(up,"mode",_jy);
function _jy(nQ){up.IO1=nQ;}









if(up!=fj)up.Ie6=undefined;
ED(up,"f1",GQk);ED(up,"status",GQk);
function GQk(){return up.Ie6;}
Wf(up,"f1",SMG);Wf(up,"status",SMG);
function SMG(nQ){up.Ie6=nQ;}







function ju(kQ,Pkb,lfw,n8,TMB,f1){
up.kQ=kQ;
up.Pkb=Pkb;
up.lfw=lfw;
up.n8=n8;
up.TMB=TMB;
up.f1=f1;
}










OZ(up,"OeK",PdJ);OZ(up,"CheckValidity",PdJ);
function PdJ(){
if(up.kQ==null){
return "StreamSwitch.id is null (and must be non-null)";
}

if(up.kQ!=null&&!((typeof up.kQ==="string"))){
return "StreamSwitch.id is not a string";
}else if(up.n8!=null&&!((typeof up.n8==="number"))){
return "StreamSwitch.timeoutMs is not an int";
}else if(up.TMB!=null&&!((typeof up.TMB==="string"))){
return "StreamSwitch.mode is not a string";
}else if(up.f1!=null&&!((typeof up.f1==="string"))){
return "StreamSwitch.status is not a string";
}else if(up.Pkb!=null&&!(up.Pkb instanceof CandidateStream)){
return "StreamSwitch.sourceStream is not a CandidateStream";
}else if(up.lfw!=null&&!(up.lfw instanceof CandidateStream)){
return "StreamSwitch.targetStream is not a CandidateStream";
}
var Oah=(up.Pkb!=null?up.Pkb.OeK():null);
if(Oah!=null){
return Oah;
}
var sUL=(up.lfw!=null?up.lfw.OeK():null);
if(sUL!=null){
return sUL;
}
return null;
}





dN(up,StreamSwitch,"QKZ",UuP);dN(up,StreamSwitch,"ConstructClone",UuP);
function UuP(lGX){

return lGX;
}



dN(up,StreamSwitch,"rL",ry);dN(up,StreamSwitch,"StaticInit",ry);
function ry(){
StreamSwitch.YF9=0;
}


dN(up,StreamSwitch,"bx",vl);dN(up,StreamSwitch,"StaticCleanup",vl);
function vl(){
StreamSwitch.YF9=0;
}

dN(up,StreamSwitch,"BEz",PYD);dN(up,StreamSwitch,"GetNextId",PYD);
function PYD(u2I){
var kQ=StreamSwitch.YF9;
StreamSwitch.YF9+=1;
if(u2I){
return "c3."+oL.fg(kQ);
}else{
return oL.fg(kQ);
}
}
dN(up,StreamSwitch,"FAILED",StreamSwitch.D4J);
dN(up,StreamSwitch,"INTERRUPTED",StreamSwitch.UhJ);
dN(up,StreamSwitch,"IN_PROGRESS",StreamSwitch.QuW);
dN(up,StreamSwitch,"PENDING",StreamSwitch.nsi);
dN(up,StreamSwitch,"SUCCEEDED",StreamSwitch.TgA);
if(up!=fj)ju.apply(up,arguments);
}
Bg(StreamSwitch,"StreamSwitch");
















function StreamerError(){
var up=this;




if(up==fj)StreamerError.UQc=0;

if(up==fj)StreamerError.Abc=1;

if(up==fj)StreamerError.e9E=2;

if(up==fj)StreamerError.kOK=3;






if(up==fj)StreamerError.y4_=0;



if(up==fj)StreamerError.VuW=1;


if(up!=fj)up.k3=undefined;


if(up!=fj)up.XfB=undefined;


if(up!=fj)up.slO=undefined;


if(up!=fj)up.dHu=undefined;


if(up!=fj)up.cmo=undefined;







dN(up,StreamerError,"kHO",Imp);dN(up,StreamerError,"makeUnscopedError",Imp);
function Imp(k3,XfB){
return new StreamerError(k3,null,-1,XfB,StreamerError.UQc);
}








dN(up,StreamerError,"YKZ",ZRP);dN(up,StreamerError,"makeStreamError",ZRP);
function ZRP(KU,XfB,slO){
return new StreamerError(KU,slO,-1,XfB,StreamerError.e9E);
}








dN(up,StreamerError,"VR0",Q6_);dN(up,StreamerError,"makeResourceError",Q6_);
function Q6_(KU,XfB,slO){
return new StreamerError(KU,slO,-1,XfB,StreamerError.kOK);
}










dN(up,StreamerError,"qM6",ftG);dN(up,StreamerError,"makeStreamSegmentError",ftG);
function ftG(KU,XfB,slO,Ny){
return new StreamerError(KU,slO,Ny,XfB,StreamerError.Abc);
}

function ju(F4Y,UQZ,BwZ,MaJ,gBT){
up.k3=F4Y;

up.slO=UQZ;
up.dHu=BwZ;
up.XfB=MaJ;
up.cmo=gBT;
}






dN(up,StreamerError,"QKZ",UuP);dN(up,StreamerError,"ConstructClone",UuP);
function UuP(lGX){
if(lGX==null)return null;

return lGX;
}

OZ(up,"vo",Ua);OZ(up,"ToStr",Ua);
function Ua(){
return "errorCode="+(up.k3!=null?up.k3:"null")+
", index="+up.dHu+
", severity="+up.XfB+
", scope="+up.cmo+
", stream=("+up.slO.vo()+")";
}








if(up!=fj)ju.apply(up,arguments);
}
Bg(StreamerError,"StreamerError");





function jh(){
var up=this;

if(up!=fj)up.E0=undefined;
function ju(bk){
if(bk==undefined)bk=0;
up.E0=new Array(bk);
}

dN(up,jh,"oIy",yzx);
function yzx(){
var S5=new jh();
var cn=[];


for(var co=0;co<arguments.length;co++){
cn.push(arguments[co]);
}
S5.E0=cn;
return S5;
}

dN(up,jh,"Y_",gE);
function gE(l4){
if(l4==null)return null;
var S5=new jh();
S5.E0=l4;
return S5;
}

OZ(up,"UX",h0);
function h0(){
return up.E0;
}


ED(up,"ug",OH);
function OH(){
return up.E0.length;
}


OZ(up,"tc",JJ);
function JJ(Ny){
if(Ny>=up.E0.length){
throw new Error("Index out of bounds: "+Ny+" (length "+up.E0.length+")");
}else if(Ny<0){
throw new Error("Index out of bounds: "+Ny);
}
return up.E0[Ny];
}

OZ(up,"a9",Ut);
function Ut(Ny,Sk){
if(Ny>=up.E0.length){
throw new Error("Index out of bounds: "+Ny+" (length "+up.E0.length+")");
}else if(Ny<0){
throw new Error("Index out of bounds: "+Ny);
}
up.E0[Ny]=Sk;
}

ED(up,"YC",jO);
function jO(){
return up.E0;
}

if(up!=fj)ju.apply(this,arguments);

}
Bg(jh,"jh");







function qu(){
var up=this;
if(up==fj){



qu.ua6=parseInt(Math.floor(Math.random()*10000),16);
qu.jK=null;

qu.JCl="convivaLoadingElement_"+qu.ua6;

qu.D_=0;
qu.gK={};
}


dN(up,qu,"bx",vl);
function vl(){
if(qu.jK){
qu.jK.innerHTML="";
}
BW(qu.gK,function(cn){cn.wy();});
qu.gK={};
}

OZ(up,"wy",NQ);
function NQ(){
up.qA=null;
delete qu.gK[up.D_];
if(up.Ss){
Lt.Te(up.Ss);
up.Ss=null;
}
}




function _d(H1,qA,n8){
up.H1=H1;
up.qA=qA;

if(n8!=undefined&&n8>0){
up.Ss=Lt.b_(Yd,n8);
}else{
up.Ss=null;
}
up.D_=qu.D_++;
qu.gK[up.D_]=up;


qu.jK=document.getElementById(qu.JCl);
if(!qu.jK){
qu.jK=qu.bA();
}
up.DY=false;

{
up.zm=document.createElement("div");
qu.jK.appendChild(up.zm);

var lA=document.createElement("script");
lA.src=up.H1;
lA.type="text/javascript";
lA.onload=(function(){
O9.Yv(function(){GM(up.D_);},
"ClassLoader.onload");
});
lA.onerror=(function(rU){
up.Error=new Error("Error loading module");
if(up.qA)up.qA(up.Error,up);
up.wy();
return false;
});
up.zm.appendChild(lA);













}

};


function GM(D_){
var g7=qu.gK[D_];
if(g7==null||g7==undefined)return;
g7.DY=true;
if(g7.qA)g7.qA(null,g7);
g7.wy();
}

function Yd(){

var fh=qu.gK[up.D_];
if(!fh){
return;
}
up.Error=new Error("ClassLoader timeout");
if(up.qA)up.qA(up.Error,up);
up.wy();
}



OZ(up,"Eg",J8);
function J8(l5){
return sHq(l5);
}



dN(up,qu,"bA",Uu);
function Uu(){







var ev=document.createElement("div");
document.body.appendChild(ev);


ev.style.position='absolute';
ev.style.margin="0pt 0pt 0pt 0pt";
ev.style.left="0px";
ev.style.top="0px";
ev.style.visibility="hidden";
ev.style.width="0px";
ev.style.height="0px";

ev.className="convivaLoadElement";

ev.id=qu.JCl;

return ev;
}

if(up!=fj)_d.apply(up,arguments);
}
Bg(qu,"qu");


















function Gnw(){
var up=this;



dN(up,Gnw,"jMw",HJf);
function HJf(LQ){
if(LQ==null){
return "null";
}
var aq=oL.PX(LQ);
var cN="{";
var bC=aq.VO;
for(var Jo=0;Jo<bC.length;Jo++){
var Tj=bC[Jo];

cN+=Tj+":\""+aq.tc(Tj)+"\" ";
}
cN+="}";
return cN;
}




dN(up,Gnw,"hJD",zGT);
function zGT(cn){
if(cn==null){
return "null";
}
var cN="[";
for(var Ny=0;Ny<cn.Bt;Ny++){
cN+=cn.tc(Ny);
if(Ny<cn.Bt-1){
cN+=", ";
}
}
cN+="]";
return cN;
}
}
Bg(Gnw,"Gnw");








function UF(){
var up=this;


if(up!=fj)up._W=null;



dN(up,UF,"Y_",gE);
function gE(nI){
var S5=new UF();
S5._W=nI;
return S5;
}

OZ(up,"UX",h0);
function h0(){
return up._W;
}


dN(up,UF,"BU",S2);
function S2(f8){
if(f8==null)return null;
var E0=null;
if(f8.hasOwnProperty("UX"))
E0=f8.UX();
else
E0=f8;
var _W="";
for(var co=0;co<E0.length;co++){
_W+=String.fromCharCode(E0[co]);
}
var S5=new UF();
S5._W=_W;
return S5;
}



dN(up,UF,"UA",Zd);
function Zd(_W){
return S2(va(_W));
}



dN(up,UF,"qi",eG);
function eG(_W){
var S5=new UF();
var rU="";
for(var co=0;co<_W.length;co++){
rU+=" "+_W.charCodeAt(co);
}
S5._W=_W;
return S5;
}


OZ(up,"v_",ES);
function ES(){
var S5=new Array();
var nI=up._W;
for(var co=0;co<nI.length;co++){
S5.push(nI.charCodeAt(co)&0xFF);
}
return jh.Y_(S5);
}


OZ(up,"vo",Ua);
function Ua(){
return up._W;
}




OZ(up,"fW",zq);
function zq(){
return oL.FP(up._W);
};


}
Bg(UF,"UF");






function nD(){
var up=this;
if(up==fj){


nD.nL=function(sq,_F,Tv){
if(sq.addEventListener){
sq.addEventListener(_F,Tv,false);
}else{
sq.attachEvent('on'+_F,Tv);
}
};

nD.Pi5=function(sq,_F,Tv){
if(sq.removeEventListener){
sq.removeEventListener(_F,Tv,false);
}else{
sq.detachEvent('on'+_F,Tv);
}
};


nD.nL(window,"message",B7);
nD.ZHR={};



nD.ua6=parseInt(Math.floor(Math.random()*10000),16);
nD.jK=null;

nD.JCl="convivaCommunicationElement_"+nD.ua6;

nD.Eo=0;
nD.SY={};


nD.li8=null;

}

if(up!=fj)up.m1A=null;



if(up!=fj)up.ja_=true;


dN(up,nD,"bx",vl);
function vl(){
if(nD.jK){
nD.jK.innerHTML="";
nD.ZHR={};
}
BW(nD.SY,function(cn){cn.wy();});
nD.SY={};
}

OZ(up,"wy",NQ);
function NQ(){
up.qA=null;
delete nD.SY[up.Eo];
if(up.Ss){
Lt.Te(up.Ss);
up.Ss=null;
}
if(up.m1A!=null){
up.m1A.wy();
up.m1A=null;
}
}

ED(up,"mV",bH);
function bH(){
if(up.jV==null)return null;
if(up.o_==null){

try{
up.o_=NM(up.jV);
}catch(fe){
up.o_=null;
}
}
return up.o_;
};





function _d(H1,qA,jm,ym){
up.oJW=Lt.VB();
up.H1=H1;
up.qA=qA;
if(!ym){
ym=new EW();
}else if(!ym.hasOwnProperty("ToObject")){

ym=oL.PX(ym);
}
up.ym=ym;
up.tM="GET";
up.ar="text/plain";

up.jm=jm;

up.jJd=null;
up.X5=null;
up.T8T=0;

up.Mr3=null;


if(nD.li8!=null){

up.m1A=nD.li8(up);
if(up.m1A!=null){
return;
}
}

up.Mi_(H1,qA,jm,ym);
}

OZ(up,"Mi_",Wv3);
function Wv3(H1,qA,jm,ym){

var xF=H1.match(/^(https?:\/\/[^\/]*)(\/.*)$/);
if(!xF){
O9.FW("DataLoader: cannot parse url: "+H1);
return;
}
up.kK=xF[1];
if(up.kK.indexOf("localhost")<0){
var ci=5;
}
var ID=xF[2];

up.tM="GET";
var jV="";
up.ar="text/plain";
if(jm){
jV=jm.vo();
up.ar="application/octet-stream; charset=x-user-defined";
up.tM="POST";
}

if(ym.Wt('contentType')){
up.ar=ym.tc('contentType');
}
up.setTimeout(ym);
up.Eo=nD.ua6+"_"+(nD.Eo++);
nD.SY[up.Eo]=up;

if(/octet-stream/.test(up.ar)&&!Vz.hp&&jV.substr(0,2)=="pb"){
jV=Vz.Em(jV);
}
if(/octet-stream/.test(up.ar)&&_vt){
jV=xHs(jV);
}

up.rU="id="+up.Eo+
",method="+up.tM+
",url="+ID+
",contentType="+up.ar+
","+jV;


DX();
}

OZ(up,"setTimeout",WQD);
function WQD(ym){
if(ym.Wt('timeoutMs')){
up.Ss=Lt.b_(Yd,parseInt(ym.tc('timeoutMs')));
}else{
up.Ss=null;
}
}

function Yd(){

var fh=nD.SY[up.Eo];
if(!fh){
return;
}
up.jJd=new Error("DataLoader timeout");
fh.T8T=Lt.VB()-fh.oJW;
if(up.qA)up.qA(up.jJd,up);
up.wy();
}



function wV8(){

nD.jK=document.getElementById(nD.JCl);
if(!nD.jK){
nD.jK=nD.bA();
nD.ZHR={};
}
};


function DX(){
var Gy=up.kK;

var rK=document.getElementById(nD.ZHR[Gy]);
if(rK&&rK.eB){


Be(rK);
}else if(rK&&!rK.eB){


setTimeout(function(){DX();},500);
}else{


wV8();
rK=document.createElement("iframe");
rK.height=30;
rK.src=Gy+"/ConvivaCommunicationProxy.html";
rK.eB=false;
rK.onload=function(){

rK.eB=true;
Be(rK);
};
rK.id=nD.F3k(Gy);
nD.jK.appendChild(rK);

nD.ZHR[Gy]=rK.id;

}

};



dN(up,nD,"F3k",nc5);
function nc5(Gy){
return "_convivaRemoteFrame_"+Gy.replace(/\//g,"").replace(/\:/g,"")+"_"+nD.ua6;
};


function Be(NJ){
if(NJ&&NJ.contentWindow){
NJ.contentWindow.postMessage(up.rU,up.ja_?"*":up.kK);
}else{
ct.qF("DataLoader","could not send message through communication frame, expect DataLoader timeout");
}
};

var _vt=(typeof(window.ActiveXObject)!='undefined');
var X5I=new RegExp(String.fromCharCode(1)+".","g");
var b7K=String.fromCharCode(1)+String.fromCharCode(1);
var Cn5=String.fromCharCode(1)+String.fromCharCode(2);
function PjU(rU){


rU=rU.replace(X5I,
function(match){return(match==b7K?String.fromCharCode(0)
:String.fromCharCode(1));});
return rU;
}
var TjV=new RegExp("["+String.fromCharCode(0)+String.fromCharCode(1)+"]","g");
function xHs(nI){
return nI.replace(TjV,
function(match){return(match==String.fromCharCode(0)?b7K:Cn5);});
}


function B7(fe){
var data=fe.data.toString();




var xF=data.match(/^id=([^,]+),([^,]+),/);
if(!xF){
return;
}
var e1r=xF[0].length;
var Mu9=null;
if(e1r==data.length){
Mu9="";
}else{
Mu9=data.substring(e1r);
}

var fh=nD.SY[xF[1]];
if(!fh){
if(0&&window.console){
window.console.log("  "+nD.ua6+" ignoring message");
}
return;
}

if(!fh.ja_&&fe.origin!=fh.kK){
return;
}
if(_vt&&/octet-stream/.test(fh.ar)){
Mu9=PjU(Mu9);
}
if(0&&window.console){
var kz=(new Date().getTime()/1000).toFixed(3);
var iM="["+kz+"] "+nD.ua6+": Got incoming: (len="+Mu9.length+"): ";
if(0){
iM+=Mu9;
}else{
for(var co=0;co<Mu9.length;co++){
iM+=Mu9.charCodeAt(co).toString(16)+",";
}
}
window.console.log(iM);
}

if(xF[2]=="ok"){
fh.jJd=null;;
}else{
fh.jJd=new Error(Mu9);
}
fh.X5=UF.qi(Mu9);
fh.T8T=Lt.VB()-fh.oJW;
if(fh.qA)fh.qA(fh.jJd,fh);
fh.wy();
};



dN(up,nD,"bA",Uu);
function Uu(){







var ev=document.createElement("div");
document.body.appendChild(ev);


ev.style.position='absolute';
ev.style.margin="0pt 0pt 0pt 0pt";
ev.style.left="0px";
ev.style.top="0px";
ev.style.visibility="hidden";
ev.style.width="0px";
ev.style.height="0px";




ev.className="convivaCommunicationElement";

ev.id=nD.JCl;

return ev;
}

if(up!=fj)_d.apply(up,arguments);
}
Bg(nD,"nD");





function EW(){
var up=this;

if(up!=fj)up.sq=undefined;
function ju(bk){
up.sq={};
}

dN(up,EW,"Y_",gE);
function gE(LQ){
if(LQ==null)return null;
if(LQ instanceof EW){
return LQ;
}
if(LQ.hasOwnProperty("A6")){
LQ=LQ.A6();
}
var S5=new EW();

Zi(LQ,function(G2){
S5.sq[G2]=LQ[G2];
});
return S5;
}

OZ(up,"A6",IP);
function IP(){
return up.sq;
}


dN(up,EW,"oIy",yzx);
function yzx(){
var S5=new EW();
for(var co=0;co+1<arguments.length;co+=2){
S5.sq[arguments[co]]=arguments[co+1];
}
return S5;
};


OZ(up,"Rr",HI);
function HI(sq){
Zi(up.sq,function(G2){
sq[G2]=up.sq[G2];
});
}

OZ(up,"tc",JJ);
function JJ(Tj){
return up.sq[Tj];
}

OZ(up,"a9",Ut);
function Ut(Tj,Sk){
up.sq[Tj]=Sk;
}


OZ(up,"m3",kG);
function kG(){
Zi(up.sq,function(Ti){
delete up.sq[Ti];
});
}


OZ(up,"Wt",jf);
function jf(Tj){
return(up.sq[Tj]!==undefined);
}

OZ(up,"OC",pi);
function pi(Tj){
return Wt(Tj);
}


ED(up,"VO",gu);
function gu(){
var S5=new Array();
Zi(up.sq,function(Ti){
S5.push(Ti);
});
return S5;
}


ED(up,"YC",jO);
function jO(){
var S5=new Array();
Zi(up.sq,function(Ti){
S5.push(up.sq[Ti]);
});
return S5;
}


ED(up,"GA",z8);
function z8(){
var S5=new Array();
Zi(up.sq,function(Ti){
S5.push(new sk(Ti,up.sq[Ti]));
});
return S5;
}


ED(up,"Bt",il);
function il(){
var S5=0;
Zi(up.sq,function(Ti){
S5++;
});
return S5;
}



OZ(up,"Yb",Ut);


OZ(up,"gS",ZQ);
function ZQ(Tj){
if(up.Wt(Tj)){
delete up.sq[Tj];
return true;
}else
return false;
}

if(up!=fj)ju.apply(this,arguments);

}
Bg(EW,"EW");









function M8(){
var up=this;
if(up!=fj)up.Lo=undefined;

function ju(){
up.Lo=new Yw();
}




OZ(up,"wy",NQ);
function NQ(){
up.Lo=new Yw();
}

OZ(up,"q_",t0);
function t0(z0){
up.Lo.Yb(z0);
}

OZ(up,"X8",ds);
function ds(z0){
up.Lo.gS(z0);
}

OZ(up,"_o",ce);
function ce(){
var bC=up.Lo.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var oW=bC[Jo];

oW();
}
}
if(up!=fj)ju.apply(up,arguments);
}
Bg(M8,"M8");




function Inferrer(){
var up=this;

if(up==fj)Inferrer.MOVING_STATE="MOVING";
if(up==fj)Inferrer.NOT_MOVING_STATE="NOT_MOVING";

if(up==fj)Inferrer._Yn=4;
if(up==fj)Inferrer.EyE=4;

if(up==fj)Inferrer.Wkw=1;
if(up==fj)Inferrer.Mwe=0.25;

if(up==fj)Inferrer.XDG=0;

if(up!=fj)up.QO=undefined;
if(up!=fj)up.gR=0;
if(up!=fj)up.xK=0;

if(up!=fj)up.T8Y=Inferrer._Yn;
if(up!=fj)up.ZRS=Inferrer.EyE;

if(up!=fj)up.Lnr=Inferrer.Wkw;
if(up!=fj)up.qr7=Inferrer.Mwe;
if(up!=fj)up.W9b=Inferrer.XDG;

if(up!=fj)up.lAz=null;

function ju(ym){
up.lAz=oL.PX(ym);
up.k2K();
up.bV();
}

OZ(up,"Update",TXC);
function TXC(V_){
var kz=Lt.VB();
if(up.xK>0&&kz>up.xK){
up.QO.gX(0,(oL.fFw(V_-up.gR))/(kz-up.xK));
}
up.xK=kz;
up.gR=oL.fFw(V_);
if(up.QO.Bt>Math.max(up.T8Y,up.ZRS)){
up.QO.DU(up.QO.Bt-1);
}
return up.tSo();
}

LK(up,"tSo",_iv);
function _iv(){
var KL4=null;
var SZx=up.QO.Bt;
if(SZx>=Math.min(up.T8Y,up.ZRS)){
var qZE=0.0;
var bC=up.QO.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var ok=bC[Jo];

qZE+=ok;
}
qZE/=SZx;

if(SZx>=up.T8Y
&&Math.abs(qZE-up.Lnr)<up.qr7){
KL4=Inferrer.MOVING_STATE;
}
if(SZx>=up.ZRS
&&qZE==up.W9b){
KL4=Inferrer.NOT_MOVING_STATE;
}
}





return KL4;
}

LK(up,"bV",AH);
function AH(){
up.QO=new Yw();
up.gR=-1;
up.xK=0;
}

LK(up,"k2K",mHr);
function mHr(){
if(up.lAz){
if(up.lAz.Wt("movingMinimumSamples")){
up.T8Y=up.lAz.tc("movingMinimumSamples");
}
if(up.lAz.Wt("notMovingMinimumSamples")){
up.ZRS=up.lAz.tc("notMovingMinimumSamples");
}
if(up.lAz.Wt("movingExpectedSpeed")){
up.Lnr=up.lAz.tc("movingExpectedSpeed");
}
if(up.lAz.Wt("notMovingExpectedSpeed")){
up.W9b=up.lAz.tc("notMovingExpectedSpeed");
}
if(up.lAz.Wt("movingSpeedTolerance")){
up.qr7=up.lAz.tc("movingSpeedTolerance");
}
}
}







if(up!=fj)ju.apply(up,arguments);
}
Bg(Inferrer,"Inferrer");




function sk(){
var up=this;

if(up!=fj)up.Tj=undefined;
if(up!=fj)up.vN=undefined;
function ju(Tj,vN){
up.Tj=Tj;
up.vN=vN;
}


ED(up,"Zw",kt);
function kt(){
return up.Tj;
}

ED(up,"GU",LL);
function LL(){
return up.vN;
}
if(up!=fj)ju.apply(this,arguments);

}
Bg(sk,"sk");





function oL(){
oL.w_=function(HW,ab){
return HW.indexOf(ab);
};

oL.JA=function(HW,ab){
return(0==HW.indexOf(ab));
};

oL.Pe=function(HW,ab){
return(0<=HW.indexOf(ab));
};

oL.tF=function(nI,IO){
if(IO<0||IO>=nI.length){
throw new Error("ArgumentOutOfRange");
}
return nI[IO];
};

oL.n0=function(_W,Bx,Mf){
if(Bx<0||Bx>=_W.length||(Mf!=undefined&&(Mf<0||Bx+Mf>_W.length))){
throw new Error("ArgumentOutOfRange");
}
if(Mf==undefined){
return _W.substr(Bx);
}else{
return _W.substr(Bx,Mf);
}
};

oL.Nq=function(HW,qf){
var S5=HW.split(qf);
return jh.Y_(S5);
};

oL.mT=function(nI){
return nI.split("");
};

oL.hI=function(ZR,Mp){
if(ZR==null){
if(Mp==null)return 0;
return-1;
}
if(Mp==null)return 1;

if(ZR<Mp)return-1;
if(ZR==Mp)return 0;
return 1;
};

oL.xO=function(nI){
return nI.replace(/^\s*/,"").replace(/\s*$/,"");
};

oL.ZLT=function(ZR,Q3s,I40){
if(Q3s==null||Q3s==""||I40==null){
throw new Error("InvalidArgument");
}

var ecJ=ZR.indexOf(Q3s);
if(ecJ>=0){
var AS=Q3s.length;

return ZR.substr(0,ecJ)+I40+oL.ZLT(ZR.substr(ecJ+AS),Q3s,I40);
}else{
return ZR;
}
};

oL._Tg=function(ZR,Q3s){
if(Q3s==null||Q3s==""){
throw new Error("InvalidArgument");
}
return ZR.lastIndexOf(Q3s);
}

oL.js=function(l4){
return Yw.Y_(l4);
};


oL.zF=function(zr){
return jh.Y_(zr);
};




oL.LS=function(l4){
if(l4==null)return null;
return l4.UX();
};

oL.js=function(zr){
return Yw.Y_(zr);
};


oL.R5=function(cn){
if(cn==null)return null;
return cn.UX();
};


oL.PX=function(zr){
var mP=EW.Y_(zr);

return mP;
};


oL.uM=function(aq){
if(aq==null)return null;
if(aq.hasOwnProperty("A6")){
return aq.A6();
}else{
return aq;
}
};

oL.Jp=function(aq,sq){
if(aq==null)return;
aq.Rr(sq);
};





oL._TN=function(nI){
return nI;
}


oL.FP=function(_W){
try{
if(window.DOMParser){
var p3=(new DOMParser()).parseFromString(_W,"text/xml");
return p3.documentElement;
}else{

var p3=new i8("Microsoft.XMLDOM");
p3.async="false";
p3.loadXML(_W);
return p3.documentElement;
}
}catch(fe){
return null;
}
};

oL.EP=function(Vl){
try{
if(window.XMLSerializer){
return(new XMLSerializer()).serializeToString(Vl);
}else{
return Vl.xml;
}
}catch(fe){
return null;
}
};

oL.fg=function(LQ){

if(typeof(LQ.fg)=="function"){
return LQ.fg();
}else{
return LQ.toString();
}
}

oL.WOY=function(nI){
return nI.toLowerCase();
}

oL.t2u=function(nI){
return parseInt(nI);
}

oL.fFw=function(Sk){
if(Sk instanceof L_){
return Sk.mg;
}else if(Sk instanceof Q2){
return Sk.mg;
}else{
return Number(Sk);
}
}


oL.N1V=new RegExp("^([+-]?[0-9]*\\.?[0-9]+)((e|E)[+-]?[0-9]+)?$");
oL.iGL=function(Sk){


oL.qlz(Sk,oL.N1V,"double");
return parseFloat(Sk);
}

oL.qlz=function(nI,IgG,jE){
if(!IgG.test(nI)){
throw new Error("Invalid string for "+jE+": "+nI);
}
}
}
Bg(oL,"oL");





function Yw(){
var up=this;

if(up!=fj)up.E0=undefined;
function ju(){

if(arguments.length>1){
SH.z2("Error: Instantiate ListCS with too many arguments");
}else if(arguments.length==0){
jh.call(up,0);
}else if(arguments[0]instanceof Number){
jh.call(up,arguments[0]);
}else if(arguments[0]instanceof Array){
jh.call(up,arguments[0].length);
up.E0=arguments[0];
}else if(arguments[0]instanceof jh){
jh.call(up,arguments[0].length);
up.E0=arguments[0].E0;
}else{
SH.z2("Error: Instantiate ListCS with inappropriate arguments");
}
}

dN(up,Yw,"oIy",yzx);
function yzx(l4){
var S5=new Yw();
for(var co=0;co<arguments.length;co++){
S5.E0.push(arguments[co]);
}
return S5;
}

dN(up,Yw,"Y_",gE);
function gE(l4){
if(l4==null){
return l4;
}
if(l4 instanceof Yw){
return l4;
}
var S5=new Yw();
S5.E0=l4;
return S5;
}

OZ(up,"UX",h0);
function h0(){
return up.E0;
}

ED(up,"Bt",il);
function il(){
return up.E0.length;
}

ED(up,"YC",jO);
function jO(){
return up.E0;
}

OZ(up,"Yb",hV);
function hV(fe){
up.E0.push(fe);
}

OZ(up,"m3",kG);
function kG(fe){
up.E0.length=0;
}

OZ(up,"Hh",pU);
function pU(fe){
var Bx=arguments[1];
if(Bx==null){
Bx=0;
}else if(Bx<0||Bx>=up.E0.length){
throw new Error("Starting index out of bound");
}

for(var co=Bx;co<up.E0.length;co++){
if(up.E0[co]==fe)return co;
}
return-1;
}

OZ(up,"OC",pi);
function pi(fe){
return up.Hh(fe)>=0;
}

OZ(up,"gX",KO);
function KO(Ny,fe){
up.E0.splice(Ny,0,fe);
}

OZ(up,"gS",ZQ);
function ZQ(fe){
var Ny=up.Hh(fe);
if(Ny<0)return false;
up.DU(Ny);
return true;
}

OZ(up,"JI",yp);
function yp(IO,Mf){
up.E0.splice(IO,Mf);
}

OZ(up,"DU",eA);
function eA(IO){
up.E0.splice(IO,1);
}

OZ(up,"g2",Al);
function Al(aw,Wp){
var S5=new Yw();
S5.E0=up.E0.slice(aw,aw+Wp);
return S5;
}

OZ(up,"HC",JE);
function JE(){
up.E0.sort.apply(up.E0,arguments);
}

OZ(up,"jR",tt);
function tt(){
return jh.Y_(up.E0.slice());
}

if(up!=fj)ju.apply(this,arguments);

}
Bg(Yw,"Yw");


































function CO(){
var up=this;
if(up!=fj)up.Xv=undefined;
if(up==fj)CO.fa=300;
if(up!=fj)up.lY=CO.fa*1000;

ED(up,"kw",En);
function En(){return up.lY;}


if(up==fj)CO.ku=20000;
if(up!=fj)up.ZE=CO.ku;

ED(up,"Yo",DR);
function DR(){return up.ZE;}

if(up==fj)CO.g0t=-1;
if(up!=fj)up.dp=CO.g0t;

ED(up,"W1",bJ);
function bJ(){
if(up.dp<=0){
return up.ZE;
}
return up.dp;
}

if(up==fj)CO.WC=2000;
if(up!=fj)up.wV=CO.WC;

ED(up,"bD",aU);
function aU(){return up.wV;}

if(up!=fj)up.__=CO.WC;

ED(up,"aa",W4);
function W4(){return up.__;}

if(up==fj)CO.wmb=-1;
if(up!=fj)up.PQ=CO.wmb;

ED(up,"Mi",SL);
function SL(){
if(up.PQ<=0){
return up.ZE;
}
return up.PQ;
}

if(up==fj)CO.LP=800;
if(up!=fj)up.Jy=CO.LP;

ED(up,"O2",TZ);
function TZ(){return up.Jy;}

if(up==fj)CO.ql=40000;
if(up!=fj)up.c2=CO.ql;

ED(up,"C3",we);
function we(){return up.c2;}

if(up==fj)CO.e3="0";
if(up!=fj)up.Tl=CO.e3;

ED(up,"PH",oX);
function oX(){return up.Tl;}

if(up==fj)CO.uF=600;
if(up==fj)CO._B=CO.uF*1000;

Va(up,CO,"dy",HY);
function HY(){return CO._B;}

if(up==fj)CO.N0=300;
if(up==fj)CO.Wh=CO.N0*1000;

Va(up,CO,"E6",yx);
function yx(){return CO.Wh;}



if(up==fj)CO.zS=30;
if(up==fj)CO.OU=CO.zS*1000;

Va(up,CO,"fN",LR);
function LR(){return CO.OU;}

if(up==fj)CO.vg=30;
if(up==fj)CO.HQ=CO.vg*1000;

Va(up,CO,"xn",yh);
function yh(){return CO.HQ;}





if(up==fj)CO.Kox="";
if(up==fj)CO.zHg=CO.Kox;

Va(up,CO,"kd9",k1U);
function k1U(){return CO.zHg;}









if(up==fj)CO.K84=110;
if(up==fj)CO.Qpn=CO.K84/100.0;

Va(up,CO,"xCh",njI);
function njI(){return CO.Qpn;}





if(up==fj)CO.eKR=true;

Va(up,CO,"n4C",Rd1);
function Rd1(){return CO.eKR;}




if(up==fj)CO.GAq=true;

Va(up,CO,"Cwv",NZM);
function NZM(){return CO.GAq;}




if(up==fj)CO.cgp=10;
if(up==fj)CO.ukx=CO.cgp;

Va(up,CO,"vOF",rhL);
function rhL(){return CO.ukx;}



if(up==fj)CO.Fn5=-1;
if(up==fj)CO.nup=CO.Fn5;

Va(up,CO,"ayU",hN_);
function hN_(){return CO.nup;}



if(up==fj)CO.Ou_=-1;
if(up==fj)CO.WVf=CO.Ou_;

Va(up,CO,"N0l",VVN);
function VVN(){return CO.WVf;}



if(up==fj)CO.S4t=60;
if(up==fj)CO.B33=CO.S4t;

Va(up,CO,"wq2",_zv);
function _zv(){return CO.B33;}

if(up==fj)CO.J2=1200;
if(up==fj)CO.ln=CO.J2*1000;

Va(up,CO,"Qw",Py);
function Py(){return CO.ln;}



if(up==fj)CO.Ix=true;

Va(up,CO,"U5",v2);
function v2(){return CO.Ix;}


if(up==fj)CO.Ze=true;

Va(up,CO,"ex",gY);
function gY(){return CO.Ze;}




if(up==fj)CO.MjM="true";
if(up==fj)CO.NUU=(CO.MjM=="true");

Va(up,CO,"Qn3",CDn);
function CDn(){return CO.NUU;}


if(up==fj)CO.jUw=false;

Va(up,CO,"rbL",UhK);
function UhK(){return CO.jUw;}


if(up==fj)CO.fZ=false;

Va(up,CO,"pY",ey);
function ey(){return CO.fZ;}

if(up==fj)CO.a3=false;

Va(up,CO,"wh",yb);
function yb(){return CO.a3;}




if(up==fj)CO.MPH=true;

Va(up,CO,"Ugx",OIE);
function OIE(){return CO.MPH;}




if(up!=fj)up.PMc=true;

ED(up,"uKm",iuu);
function iuu(){return up.PMc;}





if(up==fj)CO.Nee=8*1000;
if(up==fj)CO.sNi=CO.Nee;

Va(up,CO,"CGD",CzB);
function CzB(){return CO.sNi;}




if(up==fj)CO.L9P=5*60;
if(up==fj)CO.Srw=CO.L9P;

Va(up,CO,"v9u",WTW);
function WTW(){return CO.Srw;}




if(up==fj)CO.wg9=7*1000;
if(up==fj)CO._CK=CO.wg9;

Va(up,CO,"Wj1",VB0);
function VB0(){return CO._CK;}





if(up==fj)CO.VpK=500;
if(up==fj)CO.MOU=CO.VpK;

Va(up,CO,"Bwj",iyO);
function iyO(){return CO.MOU;}




if(up==fj)CO.dVe=10;
if(up==fj)CO.OVq=CO.dVe;

Va(up,CO,"cWG",Gun);
function Gun(){return CO.OVq;}





if(up==fj)CO.Uid=4*1000;
if(up==fj)CO.LxP=CO.Uid;

Va(up,CO,"tOm",Pt5);
function Pt5(){return CO.LxP;}





if(up==fj)CO.rN9=15*1000;
if(up==fj)CO.JLC=CO.rN9;

Va(up,CO,"FnO",tiJ);
function tiJ(){return CO.JLC;}





if(up==fj)CO.Q1C=15*1000;
if(up==fj)CO.haG=CO.Q1C;

Va(up,CO,"ZWb",din);
function din(){return CO.haG;}



if(up==fj)CO.emZ=4*1000;
if(up==fj)CO.Vfx=CO.emZ;

Va(up,CO,"T90",UFW);
function UFW(){return CO.Vfx;}



if(up==fj)CO.iXJ=2;
if(up==fj)CO.q5j=CO.iXJ;

Va(up,CO,"SJR",Twp);
function Twp(){return CO.q5j;}



if(up==fj)CO.q_u=120*1000;
if(up==fj)CO.OHC=CO.q_u;

Va(up,CO,"HVc",K5v);
function K5v(){return CO.OHC;}



if(up==fj)CO.UAc=600*1000;
if(up==fj)CO.pJ4=CO.UAc;

Va(up,CO,"Vv0",Wdl);
function Wdl(){return CO.pJ4;}



if(up==fj)CO.ZLI=2;
if(up==fj)CO.hJk=CO.ZLI;

Va(up,CO,"BBC",ikc);
function ikc(){return CO.hJk;}



if(up==fj)CO.MSg=15*1000;
if(up==fj)CO.uzA=CO.MSg;

Va(up,CO,"qCz",LPq);
function LPq(){return CO.uzA;}






if(up==fj)CO.h4A=0;
if(up==fj)CO.kxN=CO.h4A/100.0;

Va(up,CO,"tGr",qly);
function qly(){return CO.kxN;}

if(up==fj)CO.Mu=600;
if(up==fj)CO.mz=CO.Mu*1000;

Va(up,CO,"n_",kM);
function kM(){return CO.mz;}

if(up==fj)CO.rH=15;
if(up==fj)CO.OQ=CO.rH;

Va(up,CO,"p7",WO);
function WO(){return CO.OQ;}

if(up==fj)CO.Oi=30*1000;
if(up==fj)CO.D2=CO.Oi;

Va(up,CO,"VU",hh);
function hh(){return CO.D2;}

if(up==fj)CO.qz0=7*1000;
if(up==fj)CO.xb=CO.qz0;

Va(up,CO,"vS",Yk);
function Yk(){return CO.xb;}

if(up==fj)CO.NYf=11*1000;
if(up==fj)CO.ur=CO.NYf;

Va(up,CO,"vJ",EE);
function EE(){return CO.ur;}


if(up==fj)CO.eD8=3*1000;
if(up==fj)CO.CPP=CO.eD8;

Va(up,CO,"ib5",jEo);
function jEo(){return CO.CPP;}


if(up==fj)CO.F5Q=10*1000;
if(up==fj)CO.MOY=CO.F5Q;

Va(up,CO,"tE1",xR4);
function xR4(){return CO.MOY;}


if(up==fj)CO.Z1d=1*1000;
if(up==fj)CO.YrT=CO.Z1d;

Va(up,CO,"kev",D7H);
function D7H(){return CO.YrT;}

if(up==fj)CO.qS=10;
if(up==fj)CO.nP=CO.qS*1000;

Va(up,CO,"yK",zC);
function zC(){return CO.nP;}

if(up==fj)CO.ao=5;
if(up==fj)CO.MW=CO.ao;

Va(up,CO,"j8",_P);
function _P(){return CO.MW;}

if(up==fj)CO.CH=30;
if(up==fj)CO.P5=CO.CH*1000;

Va(up,CO,"Lc",tD);
function tD(){return CO.P5;}

if(up==fj)CO.rA=25;
if(up==fj)CO.uI=CO.rA;

Va(up,CO,"LY",U8);
function U8(){return CO.uI;}

if(up==fj)CO.o7=7;
if(up==fj)CO.tz=CO.o7;

Va(up,CO,"vC",Lu);
function Lu(){return CO.tz;}

if(up==fj)CO.dj=false;

Va(up,CO,"D5",jH);
function jH(){return CO.dj;}

if(up==fj)CO.gk=20000;
if(up!=fj)up.dB=CO.gk;

ED(up,"hJ",Ct);
function Ct(){return up.dB;}

if(up!=fj)up.nT6=true;

ED(up,"mSk",Ure);
function Ure(){return up.nT6;}

if(up==fj)CO.PGP=200;
if(up!=fj)up.D2N=CO.PGP;

ED(up,"ZVW",kD1);
function kD1(){return up.D2N;}

if(up!=fj)up.ZOv=false;

ED(up,"xz0",b0i);
function b0i(){return up.ZOv;}



if(up==fj)CO.M76=90*1000;
if(up==fj)CO.r0D=CO.M76;

Va(up,CO,"eon",ubb);
function ubb(){return CO.r0D;}




if(up==fj)CO.UUz=6;
if(up==fj)CO._PF=CO.UUz;

Va(up,CO,"PmJ",kLE);
function kLE(){return CO._PF;}





if(up==fj)CO.BPn=3;
if(up==fj)CO.Eii=CO.BPn;

Va(up,CO,"uE8",B8N);
function B8N(){return CO.Eii;}











if(up==fj)CO.IhZ="llnwd\\.net";
if(up==fj)CO.AKr=CO.IhZ;

Va(up,CO,"k48",qQ_);
function qQ_(){return CO.AKr;}




if(up==fj)CO.pul=true;
if(up==fj)CO.bgo=CO.pul;

Va(up,CO,"hIA",zDl);
function zDl(){return CO.bgo;}





if(up==fj)CO.xXe=true;
if(up==fj)CO.JLS=CO.xXe;

Va(up,CO,"G1r",z83);
function z83(){return CO.JLS;}












if(up==fj)CO.you=true;
if(up==fj)CO.wuk=CO.you;

Va(up,CO,"N7P",z2O);
function z2O(){return CO.wuk;}








if(up==fj)CO.Nbp=false;
if(up==fj)CO.CVM=CO.Nbp;

Va(up,CO,"lHo",IL_);
function IL_(){return CO.CVM;}







if(up==fj)CO.GfX=50;
if(up==fj)CO.Gc2=CO.GfX;

Va(up,CO,"xIL",ObS);
function ObS(){return CO.Gc2;}





if(up==fj)CO.DNM=true;
if(up==fj)CO.ti_=CO.DNM;

Va(up,CO,"fJ8",hcj);
function hcj(){return CO.ti_;}





if(up==fj)CO.LbH=true;
if(up==fj)CO.GOJ=CO.LbH;

Va(up,CO,"RwB",PhU);
function PhU(){return CO.GOJ;}






if(up==fj)CO.avT=3;
if(up==fj)CO.QPE=CO.avT;

Va(up,CO,"JdX",haS);
function haS(){return CO.QPE;}




if(up==fj)CO.gOO=100;
if(up==fj)CO.mAR=CO.gOO;

Va(up,CO,"IB2",nC4);
function nC4(){return CO.mAR;}





if(up==fj)CO.xTh=1;
if(up==fj)CO.Gwm=CO.xTh;

Va(up,CO,"Ojc",EIk);
function EIk(){return CO.Gwm;}





if(up==fj)CO.pvY=true;
if(up==fj)CO.Zgo=CO.pvY;

Va(up,CO,"scs",dAh);
function dAh(){return CO.Zgo;}




if(up==fj)CO.GO1=5000;
if(up==fj)CO.L4E=CO.GO1;

Va(up,CO,"tTS",eGj);
function eGj(){return CO.L4E;}




if(up==fj)CO.cEQ=false;
if(up==fj)CO.rET=CO.cEQ;

Va(up,CO,"RY2",ugd);
function ugd(){return CO.rET;}




if(up==fj)CO.XMl=4000;
if(up==fj)CO.X8Z=CO.XMl;

Va(up,CO,"UA7",B_8);
function B_8(){return CO.X8Z;}




if(up==fj)CO.mNd=6000;
if(up==fj)CO.VaV=CO.mNd;

Va(up,CO,"J95",VQw);
function VQw(){return CO.VaV;}






if(up==fj)CO.fiE=false;
if(up==fj)CO.vI5=CO.fiE;

Va(up,CO,"xOJ",vOg);
function vOg(){return CO.vI5;}





if(up==fj)CO.BQT=120;
if(up==fj)CO.Vmh=CO.BQT/100.0;

Va(up,CO,"Tbb",aHg);
function aHg(){return CO.Vmh;}





if(up==fj)CO.kYV=100;
if(up==fj)CO.d3v=CO.kYV;

Va(up,CO,"wuv",J7g);
function J7g(){return CO.d3v;}





if(up==fj)CO.gt3=100;
if(up==fj)CO.oAN=CO.gt3;

Va(up,CO,"xKE",mFv);
function mFv(){return CO.oAN;}





if(up==fj)CO.URj=60*1000;
if(up==fj)CO.uLH=CO.URj;

Va(up,CO,"P_m",ATK);
function ATK(){return CO.uLH;}





if(up==fj)CO.ke0=120;
if(up==fj)CO.kTO=CO.ke0*1000;

Va(up,CO,"DVd",cOo);
function cOo(){return CO.kTO;}





if(up==fj)CO.Jh6=2;
if(up==fj)CO.qvB=CO.Jh6;

Va(up,CO,"lWO",zt0);
function zt0(){return CO.qvB;}





if(up==fj)CO.P1j=1000;
if(up==fj)CO.IjK=CO.P1j;

Va(up,CO,"NYy",hxn);
function hxn(){return CO.IjK;}





if(up==fj)CO.nkp=5;
if(up==fj)CO.jf9=CO.nkp;

Va(up,CO,"t0Z",ZVF);
function ZVF(){return CO.jf9;}





if(up==fj)CO.KRY=30;
if(up==fj)CO.zAe=CO.KRY;

Va(up,CO,"max",bAT);
function bAT(){return CO.zAe;}


if(up==fj)CO.Ve0=0;
if(up==fj)CO.S9x=CO.Ve0;

Va(up,CO,"z4C",IoJ);
function IoJ(){return CO.S9x;}



if(up==fj)CO.apT=1;
if(up==fj)CO.gpk=CO.apT;

Va(up,CO,"Yqi",ELi);
function ELi(){return CO.gpk;}






if(up==fj)CO.IJd=0;
if(up==fj)CO.Zzu=CO.IJd;

Va(up,CO,"kF8",Hci);
function Hci(){return CO.Zzu;}



if(up==fj)CO.MMn="";
if(up==fj)CO.LhM=CO.MMn;

Va(up,CO,"XHx",kSs);
function kSs(){return CO.LhM;}



if(up==fj)CO.fi6=7000;
if(up==fj)CO.y7X=CO.fi6;

Va(up,CO,"pbv",MtV);
function MtV(){return CO.y7X;}


if(up==fj)CO.ort=false;
if(up==fj)CO.CVl=CO.ort;

Va(up,CO,"Bpl",pdx);
function pdx(){return CO.CVl;}


if(up!=fj)up.lQv=undefined;

if(up!=fj)up.G4D=undefined;


if(up!=fj)up.f9=undefined;
if(up!=fj)up.Tko=undefined;

if(up!=fj)up.h5=undefined;
if(up!=fj)up.WQ=undefined;
if(up!=fj)up.I8=undefined;

if(up==fj)CO.BX="livePassConfig.xml";
if(up==fj)CO.wA=CO.BX;



if(up!=fj)up.bB=undefined;

ED(up,"iw",k0);
function k0(){return up.bB;}

if(up!=fj)up.KS=undefined;

ED(up,"Fm",zM);
function zM(){return up.KS;}


if(up==fj)CO.U0n=100;
if(up==fj)CO.K45=CO.U0n;

Va(up,CO,"_Uf",Og_);
function Og_(){return CO.K45;}


if(up==fj)CO.Ozf=false;
if(up==fj)CO.LDt=CO.Ozf;

Va(up,CO,"jfb",lOX);
function lOX(){return CO.LDt;}


if(up!=fj)up.gF=undefined;





if(up==fj)CO.POc=null;




if(up!=fj)up.sHG=false;


if(up!=fj)up.KQN=undefined;




if(up==fj)CO.JAS=5000;
if(up==fj)CO.knV=CO.JAS;




if(up==fj)CO.XyF=false;



if(up==fj)CO.yG3="livepassdl.conviva.com/module/";
if(up==fj)CO.LUW=undefined;












function ju(sz,n1,ME,sC,YA,bWC){
up.h5=sz;
up.WQ=n1;
up.I8=sC;
up.sHG=bWC;
up.Xv=new M8();
if(ME!=null){
up.Xv.q_(ME);
}

up.lQv=null;
up.G4D=null;
up.bB=null;
up.KQN=0;
if(YA!=null){

up.lQv=uPJ.orQ(YA);
up.A1();
if(bWC){
if(CO.Ykc(up.lQv)){

up.Tb();
}else{
up.L9(up.lY);
}
}
}else{

up.gF=null;

up.Tb();
}
}


LK(up,"D_d",D82);
function D82(){
up.I8=null;
up.Xv=new M8();
up.L9(up.lY);
up.sHG=true;
}











dN(up,CO,"mR",Za);
function Za(sz,n1,ME,sC){


if(CO.POc!=null){
CO.POc.wy();
CO.POc=null;
}
CO.POc=new CO(sz,n1,ME,sC,null,false);
return CO.POc;
}











dN(up,CO,"xWy",qbO);
function qbO(sz,n1,ME,sC){
O9.y8(CO.LUW!=null,"_fake_config_xml needs to be set");
if(CO.POc!=null){
CO.POc.wy();
CO.POc=null;
}

CO.POc=new CO(sz,n1,ME,sC,CO.LUW,false);
return CO.POc;
}









dN(up,CO,"Z2",Si);
function Si(sz,n1,YA,QZF){
if(QZF&&CO.POc!=null){


CO.POc.D_d();
}else{
O9.y8(CO.POc==null||!CO.POc.sHG,"createPeriodicLoader.instance");
O9.y8(YA!=null,"createPeriodicLoader.config");

CO.POc=new CO(sz,n1,null,null,YA,true);
}
return CO.POc;
}




dN(up,CO,"el",aY);
function aY(){

return CO.POc;
}


dN(up,CO,"bx",vl);
function vl(){
if(CO.POc!=null){
CO.POc.wy();
CO.POc=null;
}
CO.b9=null;
CO.dgH=false;

CO.ng=null;
CO.wA=CO.BX;
CO.XyF=false;
CO.grL=null;
CO.U7B=null;
CO.knV=CO.JAS;
}


OZ(up,"wy",NQ);
function NQ(){
if(up.f9!=null){
up.f9.wy();
up.f9=null;
}
if(up.Tko!=null){
up.Tko.wy();
up.Tko=null;
}
if(up.gF!=null){
up.gF.wy();
up.gF=null;
}
if(up.Xv!=null){
up.Xv.wy();
up.Xv=null;
}
up.G4D=null;
up.lQv=null;
up.h5=null;
}



LK(up,"bF",St);
function St(eCx){







var nI=undefined;
if(eCx){
nI=CO.Tsf(up.h5);
}else{
nI=CO.C4c(up.h5);
}
nI+="/lpconfig/cfg/";
nI+=CO.GK("c3.customerName",up.WQ);
var tis=undefined;
tis="JS";
nI+="&"+CO.GK("c3.platform",tis);
var wY=false;
wY=(up.gF!=null);

if(wY){

nI+="&"+CO.GK("c3.dver",Uj.Re);
}else{

nI+="&"+CO.GK("c3.sver",Uj.Re);
}
nI+="?random="+O9.bG();



nI+="&uuid="+gV.eSE();
return nI;
}

dN(up,CO,"GK",DR3);
function DR3(Tj,nQ){
return(O9.oN(Tj)+"="+O9.oN(nQ));
}






dN(up,CO,"Tsf",b5U);
function b5U(sz){
var Y_F=oL.Nq(sz,'/');
if(Y_F.ug>=3){
var pq=Y_F.tc(2);
if(CO.grL!=null){
return oL.ZLT(sz,pq,CO.grL);
}else if(pq=="livepass.conviva.com"){
return oL.ZLT(sz,pq,"livepassdl.conviva.com");
}else if(pq=="livepass.staging.conviva.com"){
return oL.ZLT(sz,pq,"livepassdl.staging.conviva.com");
}
}
return sz;
}

dN(up,CO,"C4c",J0F);
function J0F(sz){
var Y_F=oL.Nq(sz,'/');
if(Y_F.ug>=3){
var pq=Y_F.tc(2);
if(CO.U7B!=null){
return oL.ZLT(sz,pq,CO.U7B);
}else if(pq=="livepass.conviva.com"){
return oL.ZLT(sz,pq,"livepassdl2.conviva.com");
}else if(pq=="livepass.staging.conviva.com"){
return oL.ZLT(sz,pq,"livepassdl2.staging.conviva.com");
}
}
return sz;
}


ED(up,"OP",dt);
function dt(){
if(up.G4D!=null){
return up.G4D.vo();
}
return null;
}

OZ(up,"Tb",Tm);
function Tm(){
if(CO.b9!=null){
up.lQv=uPJ.orQ(CO.b9);
up.A1();
return;
}
var H1=up.bF(true);
ct.qF("LivePassConfigLoader","loading primary config "+H1);
var ym=null;

if(!up.sHG){

ym=new EW();
if(CO.knV!=0){
ym.Yb("timeoutMs",oL.fg(CO.knV));
}
}
up.f9=new nD(H1,
function(EU,fh){
up.we6(EU,fh);
},null,ym);
}

LK(up,"we6",DrT);
function DrT(EU,fh){
O9.Yv(
function(){
if(up.sHG){
if(EU==null){
up.oy5(fh.X5);
}else{
if(up.I8!=null)up.I8(EU);
}
}else{
if(EU==null){
CO.XyF=false;
ct.qF("LivePassConfigLoader","use the primary config");
up.oy5(fh.X5);
}else{

var cR5=up.bF(false);
if(cR5==fh.H1){
if(up.I8!=null)up.I8(EU);
}else{
ct.qF("LivePassConfigLoader","loading backup config "+cR5);
var ym=new EW();
if(CO.knV!=0){
ym.Yb("timeoutMs",oL.fg(CO.knV));
}
up.Tko=new nD(cR5,
function(fe,E7){
up.XBL(fe,E7);
},null,ym);
}
}
}
},"ProcessPrimaryResponse");
}

LK(up,"XBL",AuZ);
function AuZ(EU,fh){
O9.Yv(
function(){
if(EU==null){
CO.XyF=true;
up.oy5(fh.X5);
}else{

ct.qF("LivePassConfigLoader","loading the backup configuration failed");
if(up.I8!=null)up.I8(EU);
}
},"ProcessBackupResponse");
}

LK(up,"oy5",Sh7);
function Sh7(JEp){
ct.qF("LivePassConfigLoader","updateConfigXml");
var FaA=undefined;
FaA=JEp.fW();
if(FaA!=null){
up.lQv=uPJ.Y_(FaA);
O9.Yv(up.A1,"LPConfigLoader.HandleNewConfig");
}else{
SH.z2("LivePassConfigLoader newConfigDoc==null");
}
}



LK(up,"A1",s4);
function s4(){
if(up.lQv==null)return;
var Ykc=CO.Ykc(up.lQv);
if(!up.qun(up.lQv)){
var rU="LivePassConfigLoader.HandleNewConfig:"+
"Got an invalid livePassConfig ";

if(O9.rV()<0.001){



rU+=("doc="+up.lQv.vo());
rU=oL.n0(rU,0,1000);
}
O9.Ep(rU,true);
return;
}

if(Ykc){
up.G4D=up.lQv;
}else{
if(up.bB==null){

up.bB=up.hc();
}
up.G4D=up.lQv.Im(up.bB);
var osi=up.G4D.ixD("alternative").Bt;
if(osi!=1){



var MVp="LivePassConfigLoader.processConfigXML:"
+"Did not find exactly one alternative. alt="+oL.fg(up.bB)+", "
+"count="+osi;
if(O9.rV()<0.001){



MVp+=("doc="+up.lQv.vo());
MVp=oL.n0(MVp,0,1000);
}
O9.Ep(MVp,true);
}
}


if(!CO.dgH&&!Ykc){
up.KS=0;
try{
var BH="";
BH=up.G4D.SX("ccsWallTimeSec");
if(BH!=null){
up.KS=1000.0*oL.iGL(BH);
}
}catch(fe){
}
}else{


up.KS=0;
}


if(!Ykc){

try{
CO.S9x=VW.tn(up.G4D.SX("serviceConfigVersion"));
}catch(fe){

}
}


if(CO.U7B!=null){
CO.czI=up.G4D.SX("service");
}
up.KQN=0;
up.DJT(VW.Je);
}



LK(up,"DJT",l5d);
function l5d(p4b){

CO.K45=up.Gv("pingRatePercent","value",CO.U0n);



CO.LDt=up.UZr("lzmaEnabled","value",CO.Ozf);

var fYT=false;
while(p4b>0&&!fYT){
switch(up.KQN){

case 0:

up.ZE=up.Gv("heartbeatIntervalMs","value",
CO.ku);

up.ZE=up.Gv("heartbeatInterval","msecs",
up.ZE);






up.dp=up.Gv("heartbeatIntervalLightMs","value",
up.ZE);

up.wV=up.Gv("heartbeatIntervalUrgentFullMs","value",
CO.WC);

up.__=up.Gv("heartbeatIntervalUrgentLightMs","value",
CO.WC);



up.PQ=up.Gv("heartbeatIntervalPauseMs","value",
up.ZE);


up.Jy=up.Gv("initSelectResourceIntervalMs","value",
CO.LP);


up.c2=up.Gv("selectResourceIntervalMs","value",
CO.ql);

up.c2=up.Gv("selectResourceInterval","msecs",
up.c2);


up.Tl=up.SX("protocolVersion","value",
CO.e3);


up.Tl=up.SX("protocolVersion","ver",
up.Tl);


var D0=up.Gv("refreshIntervalSec","value",
CO.fa);

D0=up.Gv("refreshInterval","secs",
D0);

if(D0*1000!=up.lY){
up.lY=D0*1000;
if(up.gF!=null){
up.gF.wy();
up.L9(up.lY);
}
}


var Vo=up.Gv("maxPlayerPausedIntervalSec","value",
CO.uF);

Vo=up.Gv("maxPlayerPausedIntervalSec","secs",
Vo);
CO._B=1000*Vo;
break;
case 1:

var SR=up.Gv("maxPlayerBufferingIntervalSec","value",
CO.N0);

SR=up.Gv("maxPlayerBufferingIntervalSec","secs",
SR);
CO.Wh=1000*SR;

var iE=up.Gv("maxPlayerStoppedIntervalSec","value",
CO.zS);

iE=up.Gv("maxPlayerStoppedIntervalSec","secs",
iE);
CO.OU=1000*iE;

var tS1=up.Gv("minStartStopIntervalMs",
"value",CO.Ou_);
CO.WVf=tS1;



var AZ9=up.Gv("phtStopThresholdSec",
"value",CO.cgp);
CO.ukx=AZ9;

var Yxn=up.Gv("urlGeneratorTimeoutSec",
"value",CO.Fn5);
CO.nup=Yxn;


var gTJ=up.Gv("traceOnSwitchDurationThresholdSec",
"value",CO.S4t);
CO.B33=gTJ;


var tG=up.Gv("maxSwitchLockTimeSec","value",CO.J2);
CO.ln=tG*1000;


CO.fZ=(up.SX("fastSwitchUpEnabled","value","false")=="true");


CO.jUw=(up.SX("play2Disabled","value","false")=="true");

CO.Ze=(up.SX("unpublishNotifyEnabled","value","true")=="true");


CO.NUU=(up.SX("playStopEnabled","value",CO.MjM)=="true");

CO.Ix=(up.SX("recoverForByteRuleEnabled","value","true")=="true");
break;
case 2:

CO.a3=(up.SX("lazySwitchLockEnabled","value","false")=="true");

CO.MPH=(up.SX("bytesLoadedSwitchDownEnabled","value","true")=="true");

up.PMc=up.UZr("syncModeEnabled","value",true);

CO.sNi=up.lHg("livePlay2StartTimeoutSec","value",CO.Nee);

CO.Srw=up.Gv("resourceSwitchDampenIntervalSec","value",CO.L9P);

CO._CK=up.Gv("bytesLoadedWindowMs","value",CO.wg9);

CO.MOU=up.Gv("switchControlCheckIntervalMs","value",CO.VpK);

CO.OVq=up.Gv("frameDropRatioMediumThreshold","value",CO.dVe);

CO.LxP=up.Gv("liveLazySwitchBufferLengthThresholdMs","value",CO.Uid);

CO.JLC=up.Gv("vodLazySwitchBufferLengthThresholdMs","value",CO.rN9);

CO.haG=up.Gv("zeriLazySwitchBufferLengthThresholdMs","value",CO.Q1C);

CO.Vfx=up.Gv("baseRecoveryWaitTimeMs","value",CO.emZ);

CO.q5j=up.Gv("recoveryWaitBackoffRate","value",CO.iXJ);

CO.OHC=up.Gv("maxRecoveryWaitTimeMs","value",CO.q_u);

CO.pJ4=up.Gv("baseSwitchLockTimeMs","value",CO.UAc);
break;
case 3:
CO.hJk=up.Gv("switchLockBackoffRate","value",CO.ZLI);

CO.uzA=up.Gv("bufferTrendExtrapolationTimeMs","value",CO.MSg);

var lnq=(up.Gv("flashAccessPingPct","value",
CO.h4A));
CO.kxN=lnq/100.0;


var yz=up.Gv("maxPlayerErrorIntervalSec","value",
CO.vg);

yz=up.Gv("maxPlayerErrorIntervalSec","secs",
yz);
CO.HQ=1000*yz;


CO.zHg=up.SX("abName","value",CO.Kox);


var Ym9=up.Gv("bandwidthMbrSafetyPct","value",
CO.K84);
CO.Qpn=Ym9/100.0;


CO.eKR=(up.SX("flashAccessModuleDownloadEnabled","value","true")=="true");


CO.GAq=(up.SX("fakeBufferFullEnabled","value","true")=="true");

var yF=up.Gv("maxPlayerNotMonitoredIntervalSec","value",
CO.Mu);

yF=up.Gv("maxPlayerNotMonitoredIntervalSec","secs",
yF);

CO.mz=1000*yF;


CO.OQ=up.Gv("maxLiveControlBufferEvent","value",
CO.rH);

CO.D2=up.lHg("maxLiveControlDurationSec","value",
CO.Oi);

CO.xb=up.lHg("maxLiveControlBufferingTimeSec","value",
CO.qz0);

CO.ur=up.lHg("maxLiveControlInitBufferingTimeSec","value",
CO.NYf);
break;
case 4:
CO.CPP=up.lHg("maxVodControlBufferingTimeSec","value",
CO.eD8);

CO.MOY=up.lHg("maxVodControlInitBufferingTimeSec","value",
CO.F5Q);

CO.YrT=up.lHg("maxZeriControlBufferingTimeSec","value",
CO.Z1d);

CO.MW=up.Gv("liveBufferTimeSec","value",
CO.ao);

CO.nP=1000*up.Gv("bwUpdateIntervalSec","value",
CO.qS);


CO.P5=1000*up.Gv("tokenExpiredIntervalSec","value",
CO.CH);


CO.dj=(up.SX("connectionRacerMemoryEnabled","value","false")=="true");


CO.tz=up.Gv("ConfigLoadIgnoreSec","value",
CO.o7);

CO.uI=up.Gv("frameDropRatioThreshold","value",
CO.rA);


up.dB=up.Gv("joinHbDelayMs","value",
CO.gk);


up.nT6=(up.SX("keepAliveEnabled","value","true")=="true");
up.D2N=up.Gv("keepAliveSecs","value",CO.PGP);


up.ZOv=up.UZr("nominalBitrateOverrideEnabled","value",false);


CO.r0D=up.Gv("maxRepeatedEventAgeMs","value",
CO.M76);

CO._PF=up.Gv("maxRepeatedEventsPerSessInfo","value",
CO.UUz);

CO.Eii=up.Gv("fcSubscribeTryTimes","value",
CO.BPn);


CO.AKr=up.SX("fcSubscribeUrlRegex","value",
CO.IhZ);



CO.JLS=up.UZr("inferPlayingStateFromFramerate","value",
CO.xXe);

CO.wuk=up.UZr("netStreamInfoEnabled","value",
CO.you);

CO.CVM=up.UZr("convivaBwMeasurementEnabled","value",
CO.Nbp);

CO.QPE=up.Gv("chunkRecoveryCheckIntervalSec","value",
CO.avT);
CO.ti_=up.UZr("disableFullChunkReport","value",CO.DNM);
CO.GOJ=up.UZr("isCachedChunkCountedForBwMeasurement","value",CO.LbH);
CO.Gc2=up.Gv("chunkRecoveryBufferLengthProvisionPercent","value",
CO.GfX);
CO.mAR=up.Gv("vodBufferTimeMs","value",CO.gOO);


CO.Gwm=up.Gv("switchControlImpl","value",CO.xTh);

CO.Zgo=up.UZr("useHistBwAtStart","value",CO.pvY);
CO.L4E=up.Gv("manifestTimeoutMs","value",CO.GO1);
CO.rET=up.UZr("precisionServerEnabled","value",CO.cEQ);

CO.X8Z=up.Gv("hdsAudioChunkFailureLockTimeMs","value",CO.XMl);
CO.VaV=up.Gv("hdsAudioChunkDownloadTimeoutMs","value",CO.mNd);

CO.vI5=up.UZr("stageVideoEnabled","value",CO.fiE);

var iHQ=up.Gv("httpBandwidthMbrSafetyPct","value",CO.BQT);
CO.Vmh=iHQ/100.0;

CO.d3v=up.Gv("httpFrameDropRatioThreshold","value",CO.kYV);

CO.oAN=up.Gv("httpFrameDropRatioMediumThreshold","value",CO.gt3);

CO.uLH=up.Gv("httpBaseSwitchLockTimeMs","value",CO.URj);

var pQz=up.Gv("httpMaxSwitchLockTimeSec","value",CO.ke0);
CO.kTO=pQz*1000;

CO.qvB=up.Gv("httpSwitchLockBackoffRate","value",CO.Jh6);

CO.IjK=up.Gv("httpVodBufferTimeMs","value",CO.P1j);

CO.jf9=up.Gv("httpLiveBufferTimeSec","value",CO.nkp);

CO.zAe=up.Gv("httpLiveMinimumOffsetSec","value",CO.KRY);

CO.bgo=up.UZr("disableMonStats","value",CO.pul);

CO.gpk=up.Gv("stitchingMinKeyFrames","value",CO.apT);

CO.Zzu=up.Gv("stitchingOverlapMs","value",CO.IJd);

CO.LhM=up.SX("hlsKeyToken","value",CO.MMn);

CO.y7X=up.Gv("switchUpMinBufferLengthMs","value",CO.fi6);

CO.CVl=up.UZr("alignedHls","value",CO.ort);

break;
default:

fYT=true;
break;
}
p4b--;
up.KQN++;
}
if(!fYT)return;


up.KQN=0;
if(up.Xv!=null){
up.Xv._o();
}
}





OZ(up,"Cv",s0);
function s0(){
var PR=null;
PR="jsModulePath";
var S5=new Yw();
if(up.G4D==null)return S5;

var VR=up.G4D.ixD(PR);
var bC=VR.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var gZ=bC[Jo];

var H1=undefined;
if(gZ.XTo.textContent!=""){
H1=gZ.XTo.textContent;
}else if(gZ.tc()){
H1=gZ.tc();
}
S5.Yb(H1);
}
return S5;
}

OZ(up,"hZ",dR);
function dR(){
return up.G4D.ixD("gateway");
}




LK(up,"hc",IR);
function IR(){
var tA=new Yw();
var ZA=undefined;
var FLr=up.lQv.ixD("alternative");
var bC=FLr.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var tE=bC[Jo];

ZA=new Uf();
ZA.bL=tE.I8_();
ZA.ns=tE.deB();

if(ZA.ns>0){
tA.Yb(ZA);
}
}


var v7=0.0;
var HH=tA.YC;
for(var wt=0;wt<HH.length;wt++){
var q8w=HH[wt];

v7+=q8w.ns;
}

var yE=O9.rV()*v7;
var jk=tA.YC;
for(var jB=0;jB<jk.length;jB++){
var Df=jk[jB];

yE=yE-Df.ns;
if(yE<=0){
return Df.bL;
}
}
return null;
}






OZ(up,"SX",N3);
function N3(PR,Vj,lF){
if(CO.ng!=null&&CO.ng.Wt(PR)){
return CO.ng.tc(PR);
}
var cN=lF;

try{
var fO=up.G4D.ixD(PR);
if(fO!=null&&fO.Bt>0){
var Heq=fO.tc(0);
cN=Heq.SX(Vj);
if(cN==null){
cN=lF;
}
}
}catch(fe){
}
return cN;
}

OZ(up,"Gv",SO);
function SO(PR,Vj,lF){
var S5=lF;
try{
S5=VW.tn(up.SX(PR,Vj,oL.fg(lF)));
}catch(fe){

}
return S5;
}











OZ(up,"UZr",Yb1);
function Yb1(PR,Vj,lF){
var CsT=up.SX(PR,Vj,null);
if(CsT==null){
return lF;
}else if(CsT=="true"){
return true;
}else{
return false;
}
}



OZ(up,"Zu",q3);
function q3(z0){
up.Xv.q_(z0);
}

OZ(up,"fA",El);
function El(z0){
if(up.Xv!=null){
up.Xv.X8(z0);
}
}

LK(up,"L9",c3);
function c3(G1){
if(up.gF!=null){
up.gF.wy();
}
up.gF=new Lt(G1,up.Tb,"LivePassConfigLoader.LoadConfigGeneral");
}

OZ(up,"J7",Hw);
function Hw(PR,Vj,lF){
var S5=lF;
try{
S5=n4.tn(up.SX(PR,Vj,oL.fg(lF)));
}catch(fe){

}
return S5;
}
















OZ(up,"lHg",T98);
function T98(PR,Vj,HgO){
var S5=HgO;
try{
var pd4=up.SX(PR,Vj,null);
if(pd4!=null){
S5=1000*VW.tn(pd4);
}
}catch(fe){

}
return S5;
}


OZ(up,"YY",Os);
function Os(IO){
var LN=up.bF(true);

if(oL.w_(LN,'?')>=0){
LN=oL.n0(LN,0,oL.w_(LN,'?'));
}
IO.a9("LivePass.configUrl",LN);
IO.a9("LivePass.uuid",oL.fg(SH.Jl));
}

LK(up,"qun",bM3);
function bM3(mV){
if(mV!=null){
if(mV.u_j()=="livePassConfig"){
return true;
}
}
return false;
}

dN(up,CO,"fUC",j6U);
function j6U(nI,Wp){
var cN=undefined;
for(cN=nI;cN.length<Wp;cN='0'+cN);
return cN;
}

dN(up,CO,"ngj",Dwz);
function Dwz(sz,n1,XP8,Tcp){
var tA4="";
var hC="";
var kYz="";
var kf6=undefined;
var zWA=undefined;

var rk=(oL.w_(sz,"https")==0)?"https://":"http://";

if(XP8!=null&&XP8!=""){
tA4=XP8+"/";
}else{
tA4+=rk;
tA4+=CO.yG3;
tA4+=n1+"/";
}

if(Tcp=="telemetry"){
kYz=tA4+"LivePassModuleMain_telemetry.swf";
}else if(Tcp=="osmf"){
kYz=tA4+"LivePassModuleMain_osmf.swf";
}else if(Tcp=="osmf_aes"){
kYz=tA4+"LivePassModuleMain_osmf_aes.swf";
}else{
kYz=tA4+"LivePassModuleMain.swf";
}

kf6=tA4+"LivePassModuleMain.dll";
zWA=tA4+"LivePassModuleMain.js";

for(var co=0;co<100;co++){
hC+="\t\t\t<gateway protocolVersision='0' loadState='available'>"+rk+"gw"+
CO.fUC(oL.fg(co),3)+".lphbs.com</gateway>\n";
}
CO.LUW="<livePassConfig isFakeConfig='true'>\n\
                              \t<common>\n\
                              \t\t<gateways>\n"+hC+"</gateways>\n"+
"\t\t<modulePath>"+kYz+"</modulePath>\n"+
"\t\t<silverModulePath>"+kf6+"</silverModulePath>\n"+
"\t\t<jsModulePath>"+zWA+"</jsModulePath>"+
"\t</common>\n\
                             </livePassConfig>";
}

dN(up,CO,"Ykc",Upo);
function Upo(mV){
return(mV.SX("isFakeConfig")=="true");
}




if(up==fj)CO.b9=null;



dN(up,CO,"TNW",EVJ);
function EVJ(){
var _W=undefined;
if(CO.b9!=null){
_W=CO.b9;
}else{
_W="<livePassConfig><common></common><alternative name='base' affinity='100'></alternative></livePassConfig>";
CO.b9=_W;
}
CO.Z2("","",_W,false);
}

OZ(up,"Rl",ro);
function ro(){
up.bB=null;
CO.dgH=true;
up.A1();
CO.dgH=false;
}

OZ(up,"PuR",J5n);
function J5n(){
return up.bF(true);
}

OZ(up,"IlM",act);
function act(){
return up.bF(false);
}

OZ(up,"ec",rR);
function rR(){
return up.sHG;
}
OZ(up,"CD",QJ);
function QJ(){
return up.bB;
}

OZ(up,"dw",Yx);
function Yx(Ib){
CO.b9=Ib;
up.lQv=uPJ.orQ(Ib);
up.A1();
}


dN(up,CO,"xf",Tk);
function Tk(){
if(CO.POc!=null&&CO.POc.ec()){
CO.POc.Tb();
}
}

if(up==fj)CO.ng=null;

dN(up,CO,"S6",_S);
function _S(){
CO.ng=null;
CO.dgH=true;
if(CO.POc!=null&&CO.POc.ec()){
CO.POc.A1();
}
CO.dgH=false;
}
dN(up,CO,"kr",zo);
function zo(PR,nQ){
if(CO.ng==null){
CO.ng=new EW();
}
CO.ng.a9(PR,nQ);
CO.dgH=true;
if(CO.POc!=null&&CO.POc.ec()){
CO.POc.A1();
}
CO.dgH=false;
}


Z0(up,CO,"yP",gx);
function gx(nQ){
CO.wA=nQ;
}

if(up==fj)CO.dgH=false;


Z0(up,CO,"Up3",gXr);
function gXr(nQ){
CO.knV=nQ;
}


Va(up,CO,"G_F",RpM);
function RpM(){
return CO.XyF;
}


if(up==fj)CO.grL=undefined;

if(up==fj)CO.U7B=undefined;




if(up==fj)CO.czI=undefined;

























































































if(up!=fj)ju.apply(up,arguments);
}
Bg(CO,"CO");


function Uf(){
var up=this;
if(up!=fj)up.bL=undefined;
if(up!=fj)up.ns=undefined;
}
Bg(Uf,"Uf");








function Uj(){
var up=this;

function ju(){
O9.FW("LivePassVersion: is an all-static class");
}


if(up==fj)Uj.BI=2;
if(up==fj)Uj.tj=78;
if(up==fj)Uj.pb=0;
if(up==fj)Uj.ya=16704;


if(up==fj)Uj.bxs="Conviva LivePass version 2.78.0.16704";





Va(up,Uj,"Re",Vc);
function Vc(){
return Uj.bN+"."+Uj.ya;
}





Va(up,Uj,"bN",vI);
function vI(){
return Uj.BI+"."+Uj.tj+"."+Uj.pb;
}



Va(up,Uj,"vWs",M7k);
function M7k(){
return Uj.bxs;
}





if(up!=fj)ju.apply(up,arguments);
}
Bg(Uj,"Uj");










function FFQ(){
var up=this;





dN(up,FFQ,"dJN",v57);
function v57(aq,Tj,nQ){
aq[Tj]=nQ;
}

dN(up,FFQ,"f1Z",BuY);
function BuY(){
return[];
}

dN(up,FFQ,"GGV",xnL);
function xnL(voR){
return voR.length;
}

dN(up,FFQ,"MIl",Jvv);
function Jvv(xBP,nQ){
xBP.push(nQ);
}

dN(up,FFQ,"zHL",fVr);
function fVr(xBP,Ny){
xBP.splice(Ny,1);
}

dN(up,FFQ,"Vf",ktu);
function ktu(SF,sq){
}

dN(up,FFQ,"b3k",W6J);
function W6J(SF,sq){
var LQ=FFQ.Vf(SF,sq);
}
}
Bg(FFQ,"FFQ");







function UU(){
var up=this;
var Xk="convivaPersistent";



UU.zB=null;

UU.nr=function(Tci,qA){


qA()
}

UU.wy=function(){
J6();
UU.zB=null;
}

function Ij(){
if(UU.zB!=null&&UU.zB!=undefined)return;

UU.zB={};
try{
var jV=localStorage.getItem(Xk);
if(jV!=undefined&&jV!=null){
UU.zB=eval("("+jV+")");
}
}catch(fe){}
};

UU.hB=function(bp){
Ij();
return UU.zB.hasOwnProperty(bp);
};

UU.JQ=function(bp){
Ij();
return UU.zB[bp];
};

UU.jZ=function(bp,t1){
Ij();
if(UU.zB.hasOwnProperty(bp)){
return UU.zB[bp];
}else{
return t1;
}
};



UU.yL=function(bp,n7){
Ij();
UU.zB[bp]=n7;
return J6();
};

UU.Pp=function(bp){
Ij();
if(UU.zB.hasOwnProperty(bp)){
delete UU.zB[bp];
J6();
return true;
}else{
return false;
}
};

UU.FD=function(){
Ij();
var Vp=[];
for(var G2 in UU.zB){
if(UU.zB.hasOwnProperty(G2))
Vp.push(G2);
}
return oL.js(Vp);
};

function J6(){

var _W="";
var xQ=UU.FD();
BW(xQ.YC,function(Sk){
if(_W!="")_W+=",";
_W+="'"+Sk+"':'"+UU.zB[Sk].toString()+"'";
});
_W="{"+_W+"}";
try{
localStorage.setItem(Xk,_W);
}catch(fe){

return false;
}
return true;
}

UU.EoO=function(){
return false;
};

UU.TNW=function(){
UU.nr(true,
function(){});
};

}
Bg(UU,"UU");




















function SH(){
var up=this;





if(up==fj)SH.Bo=null;

if(up==fj)SH.r33="https://pings.conviva.com";
if(up==fj)SH.Dm=SH.r33;




if(up==fj)SH.L0=0;



if(up==fj)SH.Q4=false;


if(up==fj)SH.ZD=undefined;
if(up==fj)SH.qQW=undefined;



if(up==fj)SH.cV=undefined;


if(up==fj)SH.LZ=undefined;


if(up==fj)SH.JO=undefined;


if(up==fj)SH.ba=1000;
if(up==fj)SH.R9=SH.ba;


if(up==fj)SH.oa=60000;
if(up==fj)SH.QS=SH.oa;


if(up==fj)SH.cd=3;
if(up==fj)SH.AT=SH.cd;


if(up==fj)SH.WF=10;


if(up==fj)SH.EL=undefined;




if(up==fj)SH.a6="pingid";

function ju(){
O9.FW("Ping: is an all-static class");
}











dN(up,SH,"nr",kP);
function kP(l7,c4){
SH.L0=l7;
SH.Bo=SH.Dm;
SH.qQW=new EW();
SH.ZD=c4;

SH.Q4=false;



SH.LZ=new Yw();
SH.JO=new Yw();
SH.EL=null;

var Jg=oL.w_(SH.Bo,'?');
if(Jg<0){

SH.Bo=SH.Bo+"/ping.ping?";
}else{
SH.Bo=oL.n0(SH.Bo,0,Jg)+"/ping.ping?"+oL.n0(SH.Bo,Jg+1)+"&";
}

if(SH.L0==0){

try{
var uL=UU.jZ(SH.a6,SH.L0);
var Cc=undefined;
Cc=uL;
SH.L0=n4.tn(Cc);
}catch(fe){
}
}
if(SH.L0==0){

SH.L0=O9.bG();
if(SH.L0==0){
SH.L0++;
}
}
UU.yL(SH.a6,oL.fg(SH.L0));



SH.Bo+="uuid="+oL.fg(SH.L0);
SH.Bo+="&ver="+Uj.Re;
SH.Bo+="&comp=js";
if(SH.ZD!=null){
var bC=SH.ZD.VO;
for(var Jo=0;Jo<bC.length;Jo++){
var KE=bC[Jo];

SH.Bo+="&"+O9.oN(KE)+"="+O9.oN(SH.ZD.tc(KE));
}
}

}


dN(up,SH,"wy",NQ);
function NQ(){
if(SH.EL!=null){
Lt.Te(SH.EL);
SH.EL=null;
}
SH.Bo=null;

if(SH.cV){
SH.cV.wy();
SH.cV=null;
}
SH.LZ=null;
SH.JO=null;
SH.Dm=SH.r33;
}








dN(up,SH,"z2",lZ);
function lZ(rU){
SH.Nxj(rU,100);
}







dN(up,SH,"Nxj",KAi);
function KAi(rU,Pcx){

if(Pcx<0)Pcx=0;
if(Pcx>100)Pcx=100;

if(SH.Bo==null){


var q2="";
q2=window.location.rk+"//"+window.location.pq;
if(oL.Pe(q2,"localhost:8888")){
SH.Dm=q2;
}
SH.nr(0,null);
}
ct.Error("Ping",rU);

rU="d="+O9.oN(rU);

if(SH.qQW.Wt(rU)){
return;
}
if(O9.rV()*100>=(Pcx*CO._Uf/100.0)){
SH.qQW.a9(rU,true);
return;
}
if(SH.qQW.Bt>2000){
var aZe="d="+O9.oN("2000 PING!!!!");
if(!SH.qQW.Wt(aZe)){
SH.qQW.a9(aZe,true);
SH.Cd(aZe);
}
return;
}
SH.qQW.a9(rU,true);
SH.Cd(rU);
}




dN(up,SH,"Cd",Lx);
function Lx(rU){

var bC=SH.LZ.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var dl=bC[Jo];

if(dl==rU){

return;
}
}
if(SH.Q4)return;


if(SH.LZ.Bt<SH.WF){
SH.LZ.Yb(rU);
}else{
return;
}



if(SH.EL==null)SH.QK();
}



dN(up,SH,"QK",dG);
function dG(){
if(SH.LZ==null||SH.JO==null)return;

if(SH.LZ.Bt>0){
var kz=Lt.fp();
var rU=SH.LZ.tc(0);
SH.LZ.DU(0);
SH.Q4=true;

if(SH.cV){
SH.cV.wy();
}
SH.cV=new nD(SH.Bo+"&"+rU,
function(EU,fh){
if(EU!=null){
ct.Error("Ping","Failed to send ping: "+oL.fg(EU));
}
},null,null);

SH.JO.Yb(kz);
var d2=0;
if(SH.JO.Bt>=SH.AT){





SH.JO.JI(0,SH.JO.Bt-SH.AT+1);
d2=VW.z_(SH.JO.tc(0)+SH.QS-kz);

}

if(d2<=SH.R9)d2=SH.R9;


SH.EL=Lt.b_(SH.QK,d2,"Ping.wait");
}else{

SH.EL=null;
}
SH.Q4=false;
}




Va(up,SH,"Jl",uh);
function uh(){return SH.L0;}
Z0(up,SH,"Jl",Ej);
function Ej(nQ){
SH.L0=nQ;
try{
UU.yL(SH.a6,oL.fg(nQ));
}catch(fe){
}
}





Va(up,SH,"H5",hn);
function hn(){return SH.LZ;}



Va(up,SH,"m4",LA);
function LA(){return SH.R9;}
Z0(up,SH,"m4",pp);
function pp(nQ){SH.R9=nQ;}



Va(up,SH,"hY",KR);
function KR(){return SH.QS;}
Z0(up,SH,"hY",u0);
function u0(nQ){SH.QS=nQ;}



Va(up,SH,"ZM",Gp);
function Gp(){return SH.AT;}
Z0(up,SH,"ZM",JH);
function JH(nQ){SH.AT=nQ;}


Z0(up,SH,"hUw",eei);
function eei(nQ){SH.Dm=nQ;}






if(up!=fj)ju.apply(up,arguments);
}
Bg(SH,"SH");
























function QnM(){
var up=this;









dN(up,QnM,"THC",ett);
function ett(nI,OWO,OeN){
if(nI==null){
return null;
}
var OuY=true;
nI=QnM.r3G(nI,OWO);
var aq=new EW();

var and=oL.Nq(nI,OWO);
var bC=and.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var Ti=bC[Jo];

var zoC=oL.Nq(Ti,OeN);
if(zoC==null||zoC.ug!=2){
OuY=false;
break;
}else{
aq.Yb(zoC.tc(0),zoC.tc(1));
}
}
if(OuY){
return aq;
}else{
return null;
}
}







dN(up,QnM,"HuF",N6x);
function N6x(nI,Vfl){
if(nI==null){
return null;
}
nI=QnM.r3G(nI,Vfl);
var cN=oL.Nq(nI,Vfl);
if(cN.ug==1&&cN.tc(0)==""){
return null;
}else{
return cN;
}
}


dN(up,QnM,"r3G",EmC);
function EmC(nI,JU){
if(nI==null)return "";
while(nI.length>0&&oL._Tg(nI,JU)==nI.length-1){
nI=oL.n0(nI,0,nI.length-1);
}
return nI;
}


dN(up,QnM,"cDw",mvs);
function mvs(nI,JU){
return oL.ZLT(nI,JU,"");
}
}
Bg(QnM,"QnM");






function Lt(){
var up=this;


if(up!=fj)this.MG=null;


if(up!=fj)this.uV=null;

if(up!=fj)this.AB=0;
if(up!=fj)this.bWC=true;
if(up!=fj)this.iM="";
if(up!=fj)this.qA=null;

if(up==fj){
Lt.clU=null;
Lt.vym=0;
}else{
this.yMq=null;
}

function ju(AB,qA,iM){
up.MG=null;
up.iM=iM;
up.AB=AB;
up.qA=qA;
up.bWC=true;

if(Lt.clU!=null){
up.yMq=Lt.clU(up);
if(up.yMq!=null){

return;
}
}
up.rs0();
}


OZ(up,"wy",NQ);
function NQ(){
if(up.yMq!=null){
up.yMq.wy();
}
if(up.MG){
up.e5B();
}
up.MG=null;
up.qA=null;
}

OZ(up,"rs0",tMg);
function tMg(){
if(up.yMq!=null){
up.yMq.rs0();
return;
}
up.MG=setInterval(hz,up.AB);
}


OZ(up,"e5B",zqP);
function zqP(){
if(up.yMq!=null){
up.yMq.e5B();
return;
}
if(up.MG!=null)clearInterval(up.MG);
}


OZ(up,"sLy",GT5);
function GT5(){
if(up.yMq!=null){
return up.yMq.sLy();
}
return(up.MG!=null);
}



OZ(up,"eO",M_);
function M_(){
if(up.yMq!=null){
up.yMq.eO();
return;
}
up.e5B();
up.rs0();
}



function hz(){
if(!up.bWC){
up.e5B();
}
O9.Hy();
O9.Yv(up.qA,(up.iM?up.iM:"ProtectedTimer.Tick"));
}


OZ(up,"mE",wv);
function wv(oT){
up.AB=oT;
up.eO();
}


dN(up,Lt,"b_",_i);
function _i(Kq,px,rU){
var KuA=new Lt(px,Kq,rU);
KuA.bWC=false;
return KuA;
}

dN(up,Lt,"Te",oC);
function oC(io){
if(io!=null)io.wy();
}

Lt.fp=function(){
return(new Date()).getTime()+Lt.vym;
};





Lt.PgU=0;
Lt.ZAw=function(IxR){
Lt.PgU+=IxR;
};

Lt.VB=function(){
return Lt.PgU+Lt.fp();
}


Lt.mu=function(Jb){
return(new Date(Jb)).toString();
};

if(up!=fj)ju.apply(up,arguments);
}
Bg(Lt,"Lt");






function Dh(){

Dh.iA=function(x7){

if(arguments.length==1){
return new x7();
}else if(arguments.length==2){
return new x7(arguments[1]);
}else if(arguments.length==3){
return new x7(arguments[1],arguments[2]);
}else if(arguments.length==4){
return new x7(arguments[1],arguments[2],arguments[3]);
}else if(arguments.length==5){
return new x7(arguments[1],arguments[2],arguments[3],arguments[4]);
}else if(arguments.length==6){
return new x7(arguments[1],arguments[2],arguments[3],arguments[4],arguments[5]);
}else if(arguments.length==7){
return new x7(arguments[1],arguments[2],arguments[3],arguments[4],arguments[5],arguments[6]);
}else{
SH.z2("Error:CreateInstance too many args: "+x7.toString());
return null;
}
};


Dh.Eg=function(sq){
if(sq==null)return null;
return sq.constructor;
};






Dh.un=function(l5){
return sHq(l5);
};

Dh.eE=function(sq){
if(sq==null)return null;
O9.FW("GetClassName not implemented");
return null;
};



function cS(o8,sq,vL){
try{
var i0=M3(o8,sq);
if(i0==null||i0==undefined||!i0 instanceof Function){
var fL=new Error("1069: Missing property "+o8);
O9.Q_("InvokeMethod: "+o8,fL);
return null;
}
return i0.apply(sq,vL);
}catch(fe){
var wc=fe;
if(fe.n3==1069){
O9.Q_("InvokeMethod: "+o8,fe);
return null;
}





















O9.Q_("InvokeMethod:"+o8,fe);
}
return null;
};


Dh.YB=function(o8,sq){
return cS(o8,sq,Ap(arguments,2));
};

Dh.Qz=function(o8,sq){
return cS(o8,sq,Ap(arguments,2));
};



Dh.hB=function(SF,sq){
try{
return(sq!=null&&sq.hasOwnProperty(SF));
}catch(fe){
if(fe.n3==1069){

return false;
}
throw fe;
}
return false;
};

Dh.JQ=M3;

Dh.TOy=M3;

Dh.xI=Dh.hB;


Dh.eF=Dh.hB;


Dh.yL=function(SF,sq,vN){
sq[SF]=vN;
};



Dh.G6=Dh.hB;

Dh.Vf=Dh.JQ;

Dh.HM=Dh.yL;


function M3(HE,sq){
var Sk=sq[HE];
return sq[HE];
};




Dh.GO=function(e_){
return e_;
};

function Ap(vL,Iq){
var S5=new Array();
for(var co=Iq;co<vL.length;co++){
S5.push(vL[co]);
}
return S5;
}
}
Bg(Dh,"Dh");
function ct(){
ct.Lz=false;


ct.XU=true;



ct.Xp=null;







ct.nr=function(){
};

ct.cE=[];
ct.rP=function(Kq){
ct.cE.push(Kq);
};

ct.biV=function(Kq){
for(var co=0;co<ct.cE.length;co++){
if(ct.cE[co]==Kq){
ct.cE.splice(co,1);
return;
}
}
};

ct.wW=function(){
ct.cE=[];
};

function r0P(NAR){
var co,Fn,w4,aJD=document.cookie.split(";");
for(co=0;co<aJD.length;co++){
Fn=aJD[co].substr(0,aJD[co].indexOf("="));
w4=aJD[co].substr(aJD[co].indexOf("=")+1);
Fn=Fn.replace(/^\s+|\s+$/g,"");
if(Fn==NAR){
return unescape(w4);
}
}
return null;
}

function VC8(zkY){
var _W=null;
try{
_W=location.search;







}catch(fe){}
if(_W!=null&&_W.length>1){
var rjS=O9.rx(_W.substr(1));
return rjS.tc(zkY);
}else{
return null;
}
}

function nAt(){
if(ct.Xp==null){

var cn=VC8("toggleTraces");
if(cn!=null&&cn.toString()=="true"){
ct.Xp=true;
}else{
ct.Xp=false;
}
}

return(ct.XU||r0P("toggleTraces")!=null||ct.Xp);
}

function a7(rU){
for(var co=0;co<ct.cE.length;co++){
try{
ct.cE[co](rU);
}catch(fe){}
}
}

function Lr5(){
return "["+((new Date()).getTime()/1000).toFixed(3)+"] ";
}

ct.qF=function(yM,rU){
if(rU!=undefined){
rU=yM+": "+rU;
}else{
rU=yM+": ";
}
rU=Lr5()+rU;
if(nAt()&&window.console&&window.console.log)
window.console.log(rU);
a7(rU);
};

ct.M3x=function(yM,rU){
rU=Lr5()+yM+": "+rU;
if(nAt()&&window.console&&window.console.warn)
window.console.warn(rU);
a7(rU);
};

ct.Error=function(yM,rU){
rU=Lr5()+yM+": "+rU;
if(nAt()&&window.console&&window.console.error)
window.console.error(rU);
a7(rU);
};

ct.EM=function(){
};


ct.Eoy=function(){
return nAt();
}

}
Bg(ct,"ct");




function O9(){
O9.V6="ERROR_CONNECTION_FAILURE";
O9.NO="ERROR_STREAMING_FAILURE";


O9.rV=Math.random;
O9.bG=function(){
return Math.floor(Math.random()*n4.Je);
};

O9.dU=[];
O9.MU=function(dM){
O9.dU.push(dM);
};

O9.Hy=function(){
O9.dU=[];
};






O9.r1=function(Kq,rU,h9){
var zb=O9.dU.length;
var lb=h9;
try{
if(O9.H6){
try{
lb=Kq();
}catch(fe){
O9.Q_(rU?rU:"RunProtected",fe);
}
}else{
lb=Kq();
}
}finally{

if(O9.dU.length>zb){
O9.dU=O9.dU.slice(0,zb);
}
}
return lb;
};
O9.Yv=O9.r1;


O9.y8=function(Sk,rU){
if(!Sk){
O9.FW("Assertion: "+rU);
}
};

O9.Ep=function(rU,QK){
if(O9.ON!=null&&
O9.ON(rU,null)){
return false;
}
if(QK){
SH.z2("Error:"+rU);
}else{
ct.Error("Utils",rU);
}
return true;
};

O9.FW=function(rU){
if(O9.Ep(rU,true)){
throw new Error("Error: "+rU);
}
};


O9.Q_=function(rU,EU){
var Hjy="Uncaught exception ";
var evZ="";
var tP8=rU.indexOf("IGNORE");
if(tP8<0){
Hjy+=rU;
}else{
Hjy+=rU.substr(0,tP8);
evZ=rU.substr(tP8);
}
if(O9.dU.length>0){
Hjy+="(crumbs: "+O9.dU.toString()+")";
}
if(O9.ON!=null&&
O9.ON(Hjy,EU)){
return;
}


if(!O9.H6){
throw EU;
}else{


var stack=O9.zJ(EU);
if(stack!=null){
Hjy+=", stack:"+stack;
}else{
Hjy+=", exc:"+EU.toString();
}
if(evZ!=""){
Hjy+=" "+evZ;
}
SH.z2(Hjy);
}
};

O9.zJ=function(fe,H1,xE){
var q0=[];
var XD=false;
if(fe.stack){
var xF;
var Z3y=32;
while((xF= /^\s*(.*)@(.+):(\d+)\s*$/gm.exec(fe.stack))){
if(Z3y--<=0)break;
q0.push(xF[1]+" @ "+xF[2]+":"+xF[3]);
}
XD=true;
}else if(false&&window.opera&&fe.message){
var uw=fe.message.split("\n");
for(var co=0,Wp=uw.length;co<Wp;co++){
if(uw[co].match(/^\s*[A-Za-z0-9\-_\$]+\(/)){
var B3=uw[co];

if(uw[co+1]){
B3+=" at "+uw[co+1];
co++;
}
q0.push(B3);
}
}
q0.shift();
XD=true;
}
if(!XD){
var U_=arguments.callee.caller;
while(U_){
var aO=U_.toString();
var xF= /\s*function\s*(\S+)\s*\( ?/gm.exec(aO);
var HE;
if(xF){
HE="  *"+xF[1];
}else{


HE="  *"+aO.substring(0,64)+".....";
}


if(q0.join("\n").indexOf(HE)>=0){
q0.push(HE);
q0.push("...recursion...");
break;
}else{
q0.push(HE);
}
U_=U_.caller;
}
}
if(q0){
q0=q0.join("\n");
}
var HW=fe.toString();
if(HW.match(/^\[object/)){
return fe.Sj+" "+fe.hq+"\nTrace:\n"+q0.toString();
}else{
return HW+"\nTrace:\n"+q0.toString();
}
};


O9.GK=function(Ot){

Ot=oL.uM(Ot);
var S5="";
Zi(Ot,function(G2){
var T9=encodeURIComponent(G2)+"="+encodeURIComponent(Ot[G2]);
if(S5)S5+="&";
S5+=T9;
});
return S5;
};

O9.oN=function(_W){
return encodeURIComponent(_W);
};

O9.mC8=function(_W){
return decodeURIComponent(_W);
}

O9.rx=function(_W){
var sp=_W.split("&");
var S5={};
BW(sp,function(T9){
var Nx=T9.split("=");
if(Nx.length<1)return;
var Tj=Nx[0];
var vN="";
if(Nx.length>=1){
vN=Nx[1];
}
S5[decodeURIComponent(Tj)]=decodeURIComponent(vN);
});
return EW.Y_(S5);
};

O9.pw="xx";

O9.pD="bb";

O9.Mul="c3.global";


O9.H6=true;


O9.id=function(_W){
return _W;
};




O9.ON=null;


O9.ZLd=Math.pow(2,15)-1;
}
Bg(O9,"O9");














function gV(){
var up=this;




if(up==fj)gV.c6="uuid";




if(up==fj)gV.nF=null;

function ju(){
throw new Error("Uuid is a static class");
}



dN(up,gV,"nr",kP);
function kP(){
var du=UU.jZ(gV.c6,null);
try{

if(du==null){
gV.nF=null;
}else{
gV.nF=new jh(4);
var Ny=0;
var bC=oL.Nq(du,",").YC;
for(var Jo=0;Jo<bC.length;Jo++){
var JU=bC[Jo];

gV.nF.a9(Ny,parseInt(JU));
if(gV.nF.tc(Ny)<0){
gV.nF.a9(Ny,gV.nF.tc(Ny)+4294967296);
}
Ny++;
}
}
}catch(fe){
gV.nF=null;
}
if(gV.nF!=null&&gV.nF.ug!=4){
gV.nF=null;
}
if(gV.nF==null){

gV.nF=
jh.oIy(0,0,0,0);
}
}




dN(up,gV,"fd",R3);
function R3(id){
O9.y8(id.ug==4,"id must have 4 ints");
gV.nF=id;

var _W=oL.fg(gV.nF.tc(0))+","+oL.fg(gV.nF.tc(1))+","+oL.fg(gV.nF.tc(2))+","+oL.fg(gV.nF.tc(3));
UU.yL(gV.c6,_W);
SH.Jl=n4.z_(gV.nF.tc(0));
}




dN(up,gV,"eSE",Xtj);
function Xtj(){
if(gV.nF==null){
return "0.0.0.0";
}
return(""+gV.nF.tc(0)+"."+gV.nF.tc(1)+"."+gV.nF.tc(2)+"."+gV.nF.tc(3));
}

dN(up,gV,"wy",NQ);
function NQ(){
gV.nF=null;
}



Va(up,gV,"Kn",Ye);
function Ye(){
O9.y8(gV.nF!=null,"Uuid");
return gV.nF;
}







dN(up,gV,"bz",nN);
function nN(){
UU.Pp(gV.c6);
gV.nF=
jh.oIy(0,0,0,0);
}





dN(up,gV,"ffk",UcW);
function UcW(){
gV.nF=
jh.oIy(0,0,0,0);
}



Va(up,gV,"eP",XW);
function XW(){
return gV.nF;
}




dN(up,gV,"fMy",zb4);
function zb4(jy){
if(jy.ug!=gV.nF.ug)return false;
for(var co=0;co<gV.nF.ug;co++){
if(gV.nF.tc(co)!=jy.tc(co))return false;
}
return true;
}




dN(up,gV,"zH",MM);
function MM(id){
return gV.fMy(oL.zF(id));
}


if(up!=fj)ju.apply(up,arguments);
}
Bg(gV,"gV");















function uPJ(){
var up=this;

if(up!=fj)up.Ut8=undefined;

ED(up,"XTo",Q5Y);
function Q5Y(){return up.Ut8;}


dN(up,uPJ,"orQ",LRx);
function LRx(nI){
if(nI==null)return null;
return uPJ.Y_(oL.FP(nI));
}


dN(up,uPJ,"Y_",gE);
function gE(nQ){
if(nQ==null)return null;
var cN=new uPJ();
cN.Ut8=nQ;
return cN;
}

OZ(up,"vo",Ua);
function Ua(){
return oL.EP(up.Ut8);
}


OZ(up,"ixD",WSY);
function WSY(Sj){
var cN=new Yw();
var fO=up.Ut8.getElementsByTagName(Sj);
for(var Ny=0;Ny<fO.length;Ny++){
cN.Yb(uPJ.Y_(fO[Ny]));
}
return cN;
}

OZ(up,"u_j",ywJ);
function ywJ(){
if(up.Ut8==null){
return null;
}
var cN=null;
cN=up.Ut8.nodeName;
return cN;
}


OZ(up,"SX",N3);
function N3(Sj){
if(up.Ut8==null){
return null;
}
var cN=null;
cN=up.Ut8.getAttribute(Sj);
if(cN){
cN=cN.toString();
}
return cN;
}


OZ(up,"tc",JJ);
function JJ(){
var cN=null;
if(up.Ut8!=null){
if(up.Ut8.textContent!=""){
cN=up.Ut8.textContent;
}else if(up.Ut8.getAttribute("value")){
cN=(up.Ut8.getAttribute("value"));
}
}

return cN;
}


OZ(up,"deB",YZs);
function YZs(){
var S5=0;
try{
S5=Math.max(0,VW.tn(up.SX("affinity")));
}catch(fe){

}
return S5;
}

OZ(up,"I8_",LW4);
function LW4(){
var S5=null;
try{
S5=up.SX("name");
}catch(fe){

}
return S5;
}


OZ(up,"Im",CW);
function CW(PBm){




var Ejb=up.Ut8.cloneNode(true);
var Cb=Ejb.childNodes;
for(var Ny=0;Ny<Cb.length;Ny++){
var mB=Cb[Ny];
if(mB.nodeName=="alternative"&&mB.getAttribute("name").toString()!=PBm){
Ejb.removeChild(Cb[Ny]);
Ny--;
}
}
return uPJ.Y_(Ejb);
}
}
Bg(uPJ,"uPJ");












function hF(){
var up=this;

if(up!=fj)up.fV=undefined;
if(up==fj)hF.ap=0;


if(up!=fj)up.dK=null;

function ju(){
up.fV=hF.ap;
hF.ap++;
up.dK=new jo(up.fV);
}



ED(up,"id",d4);
function d4(){return up.fV;}


ED(up,"pO",TH);
function TH(){return up.dK;}


OZ(up,"ox",gv);
function gv(){
up.dK=null;
O9.Yv(
function(){
uf.kp("Cleanup",
function(){
var yM=uf.UK;
if(yM==null)return;
Dh.YB("jt",yM,up.fV);
});
},"ConvivaGenericSession.cleanup IGNORE "+up.fV);
}

if(up!=fj)ju.apply(up,arguments);
}
Bg(hF,"hF");













function ConvivaLightSession(){
var up=this;
if(up!=fj)up.Ay=undefined;
if(up!=fj)up.Du=undefined;




if(up==fj)ConvivaLightSession.V6=O9.V6;
if(up==fj)ConvivaLightSession.NO=O9.NO;





function ju(KC){hF.call(up);
O9.Yv(
function(){
up.Ay=KC;
up.Du=null;
uf.kp("ConvivaLightSession",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"new ConvivaLightSession and LivePass not ready");
Dh.YB("UL",yM,up.fV,up.Ay);
});
},"ConvivaLightSession.ctor IGNORE "+up.fV);
}






OZ(up,"DC",hG);OZ(up,"reportError",hG);
function hG(b8){
var DIe=Lt.fp();
uf.kp("reportError",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"LivePass not ready");
Dh.YB("iwg",
yM,up.fV,b8,DIe);
});
}



OZ(up,"uvO",b2H);OZ(up,"setCurrentResource",b2H);
function b2H(w6){
O9.Yv(
function(){
uf.kp("setCurrentResource",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"setCurrentResource called when LivePass not ready");
Dh.YB("uvO",yM,up.fV,w6);
});
},"ConvivaLightSession.setCurrentResource IGNORE "+up.fV);
}








OZ(up,"tlE",VJG);OZ(up,"setCurrentBitrate",VJG);
function VJG(M9){
O9.Yv(
function(){
uf.kp("setCurrentBitrate",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"setCurrentBitrate called when LivePass not ready");
Dh.YB("tlE",yM,up.fV,M9);
});
},"ConvivaLightSession.setCurrentBitrate IGNORE "+up.fV);
}









OZ(up,"iVI",W3h);OZ(up,"setContentLength",W3h);
function W3h(cX){
O9.Yv(
function(){
uf.kp("setContentLength",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"setContentLength called when LivePass not ready");
Dh.YB("iVI",yM,up.fV,cX);
});
},"ConvivaLightSession.setContentLength IGNORE "+oL.fg(up.fV));
}













OZ(up,"eN",f_);OZ(up,"selectResource",f_);
function f_(qA){
O9.Yv(
function(){
uf.kp("SelectResource",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"selectResource called when LivePass not ready");
Dh.YB("eN",yM,up.fV,up.Ay,qA);
});
},"ConvivaLightSession.selectResource IGNORE "+up.fV);
}























OZ(up,"Wd",be);OZ(up,"startMonitor",be);
function be(G_,w6){
var vu=null;
ConvivaLightSession.TDT(up.Ay);
up.dlF(G_);

O9.Yv(
function(){
uf.kp("startMonitor",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"startMonitor called before the LivePass is ready");




Dh.YB("Wd",yM,up.fV,up.Ay,G_,w6,vu);
});
},"ConvivaLightSession.startMonitor IGNORE "+up.fV);
}





OZ(up,"sT",oD);OZ(up,"attachStreamer",oD);
function oD(G_){
O9.Yv(
function(){
up.dlF(G_);
uf.kp("attachStreamer",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"attachStreamer called before the LivePass is ready");
var w6=null;
Dh.YB("Wd",yM,up.fV,up.Ay,G_,w6,null);
});
},"ConvivaLightsession.attachStreamer "+up.fV);
}





OZ(up,"KV",md);OZ(up,"pauseMonitor",md);
function md(){
O9.Yv(
function(){
uf.kp("pauseMonitor",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"pauseMonitor called when LivePass not ready");
Dh.YB("KV",yM,up.fV);
});
up.ob2();
},"ConvivaLightSession.pauseMonitor IGNORE "+up.fV);
}




OZ(up,"oP",wT);OZ(up,"stopMonitor",wT);
function wT(){
O9.Yv(
function(){
uf.kp("stopMonitor",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"stopMonitor called when LivePass not ready");
Dh.YB("oP",yM,up.fV);
});

up.ob2();
},"ConvivaLightSession.stopMonitor IGNORE "+up.fV);
}








dN(up,ConvivaLightSession,"TDT",Jez);dN(up,ConvivaLightSession,"SetJoinStartTimeContentInfo",Jez);
function Jez(LC){
var gi=oL.PX(LC.N_);


if(gi==null){
gi=new EW();
}
if(!gi.Wt("joinStartTime")){
gi.a9("joinStartTime",oL.fg((Lt.fp())));
}else{
}

LC.N_=gi;
}




OZ(up,"gxz",Jqk);OZ(up,"adStart",Jqk);
function Jqk(){
O9.Yv(
function(){
uf.kp("adStart",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"adStart called when LivePass is not ready");
Dh.YB("gxz",yM,up.fV);
});
},"ConvivaLightSession.adStart IGNORE "+up.fV);
}




OZ(up,"KSn",Awm);OZ(up,"adEnd",Awm);
function Awm(){
O9.Yv(
function(){
uf.kp("adEnd",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"adEnd called when LivePass is not ready");
Dh.YB("KSn",yM,up.fV);
});
},"ConvivaLightSession.adEnd IGNORE "+up.fV);
}




OZ(up,"euf",i8S);OZ(up,"reportAdError",i8S);
function i8S(){
O9.Yv(
function(){
uf.kp("reportAdError",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"reportAdError() called when LivePass is not ready");
Dh.YB("euf",yM,up.fV);
});
},"ConvivaLightSession.reportAdError IGNORE "+up.fV);
}




OZ(up,"ox",gv);OZ(up,"cleanup",gv);
function gv(){
up.Phox();

up.ob2();
}

LK(up,"dlF",ZZQ);
function ZZQ(nI){
if(up.Du!=null){
ct.Error("ConvivaLightSession","session "+up.fV+" is already monitoring a streamer");
}else if(nI!=null){
up.Du=nI;

LivePass.V0(nI,up);
}
}

LK(up,"ob2",UAh);
function UAh(){
if(up.Du!=null){

LivePass.FS(up.Du);
up.Du=null;
}
}
dN(up,ConvivaLightSession,"ERROR_CONNECTION_FAILURE",ConvivaLightSession.V6);
dN(up,ConvivaLightSession,"ERROR_STREAMING_FAILURE",ConvivaLightSession.NO);
if(up!=fj)ju.apply(up,arguments);
}
Bg(ConvivaLightSession,"ConvivaLightSession");












function ConvivaMetricsSession(){
var up=this;
function ju(jz){hF.call(up);
O9.Yv(
function(){
uf.kp("ConvivaMetricsSession",
function(){
var yM=uf.UK;
O9.y8(yM!=null,"new ConvivaMetricsSession and LivePass not ready");
Dh.YB("Pn",yM,up.fV,jz);
});
},"ConvivaMetricsSession.ctor."+up.fV);
}
if(up!=fj)ju.apply(up,arguments);
}
Bg(ConvivaMetricsSession,"ConvivaMetricsSession");




















function LivePass(){
var up=this;




if(up==fj)LivePass.X9=LivePass.P2;


if(up==fj)LivePass.PC=undefined;

if(up==fj)LivePass.fw=undefined;




if(up==fj)LivePass.hb=new EW();









dN(up,LivePass,"Ns",D3);dN(up,LivePass,"init",D3);
function D3(sz,TY,qA){
if(LivePass.CHy!=null&&LivePass.CHy!=""){
sz=LivePass.CHy;
}
ct.qF("LivePass","Initializing "+Uj.vWs);
O9.Yv(
function(){

if(LivePass.X9==LivePass.df||
LivePass.X9==LivePass.J9){
return;
}
ct.Ka="LivePass";
LivePass.fw=qA;
LivePass.X9=LivePass.df;
LivePass.PC=null;

var n8=0;
uf.nr(sz,TY,LivePass.SV,n8);

},"LivePass.init");
}












dN(up,LivePass,"Tr4",jh1);dN(up,LivePass,"setupQuickInit",jh1);
function jh1(sz,TY,XP8,Tcp){
uf.n34=true;
CO.ngj(sz,TY,XP8,Tcp);
}

dN(up,LivePass,"ox",gv);dN(up,LivePass,"cleanup",gv);
function gv(){
O9.Yv(
function(){
var bC=LivePass.hb.VO;
for(var Jo=0;Jo<bC.length;Jo++){
var G2=bC[Jo];

var GX=LivePass.hb.tc(G2);
GX.ox();
LivePass.hb.gS(G2);
}

LivePass.hb.m3();
if(LivePass.PC!=null){
LivePass.PC.wy();
LivePass.PC=null;
}
LivePass.fw=null;
uf.wy();
LivePass.X9=LivePass.P2;
},"LivePass.cleanup");
}













dN(up,LivePass,"K6",Da);dN(up,LivePass,"createMonitoringSession",Da);
function Da(G_,d9,hA,ym){
return O9.r1(
function(){

var LC=new ConvivaContentInfo(d9,new Yw(),hA);
LC.N_=ym;
return LivePass.M2V(G_,LC);
},"LivePass.createMonitoringSession",null);
}












dN(up,LivePass,"M2C",onY);dN(up,LivePass,"createSession",onY);
function onY(G_,KC){



KC.K_y=true;
return LivePass.M2V(G_,KC);
}

dN(up,LivePass,"M2V",flL);dN(up,LivePass,"createSessionWithCci",flL);
function flL(G_,LC){
return O9.r1(
function(){
if(G_!=null&&LivePass.tf(G_)!=null){
ct.Error("LivePass","createSessionWithCci called twice for the same streamer");
return null;
}


ConvivaLightSession.TDT(LC);
var GX=new ConvivaLightSession(LC);
GX.Wd(G_,null);
return GX;
},"LivePass.createSessionWithCci",null);
}








dN(up,LivePass,"tf",Fy);dN(up,LivePass,"getMonitoringSession",Fy);
function Fy(G_){
var Tj=LivePass.lV(G_,false,0);

if(LivePass.hb.Wt(Tj)){
return LivePass.hb.tc(Tj);
}else{
return null;
}
}







dN(up,LivePass,"yX",mF);dN(up,LivePass,"cleanupMonitoringSession",mF);
function mF(G_){
O9.Yv(
function(){
if(G_==null||LivePass.hb==null){
ct.Error("LivePass","cleanupMonitoringSession called before createMonitoringSession");
return;
}
var GX=LivePass.tf(G_);
if(GX!=null){
GX.ox();
}
},"LivePass.cleanupMonitoringSession");
}


dN(up,LivePass,"SV",My);dN(up,LivePass,"ourInitHandler",My);
function My(jv){
if(jv.iv==ConvivaNotification.g9){

LivePass.X9=LivePass.J9;


LivePass.PC=null;
}else if(jv.iv==ConvivaNotification.bQ||
jv.iv==ConvivaNotification.vU){
LivePass.X9=LivePass.Ir;
}
if(LivePass.fw!=null){
O9.Yv(
function(){LivePass.fw(jv);},
"LivePass notifier");
}
}





Va(up,LivePass,"ZY",Xw);Va(up,LivePass,"ready",Xw);
function Xw(){return LivePass.X9==LivePass.J9;}

dN(up,LivePass,"vK",Ie);dN(up,LivePass,"setReady",Ie);
function Ie(){
LivePass.X9=LivePass.J9;
}





Va(up,LivePass,"Zn",SE);Va(up,LivePass,"pending",SE);
function SE(){return LivePass.X9==LivePass.df;}


Va(up,LivePass,"hK",Rx);Va(up,LivePass,"version",Rx);
function Rx(){
return Uj.Re;
}



Va(up,LivePass,"po",h_);Va(up,LivePass,"stats",h_);
function h_(){
var po=new EW();
po.a9("LivePass.version",Uj.Re);
uf.YY(po);
return oL.uM(po);
}






Va(up,LivePass,"pO",TH);Va(up,LivePass,"metrics",TH);
function TH(){
return uf.b2();
}


dN(up,LivePass,"Xp",AQ);dN(up,LivePass,"toggleTraces",AQ);
function AQ(jl){
O9.Yv(
function(){

ct.XU=jl;
uf.kp("toggleTraces",
function(){
var yM=uf.UK;
Dh.YB("Xp",yM,jl);
});
},"LivePass.toggleTraces");
}







dN(up,LivePass,"lV",jg);dN(up,LivePass,"getStreamerKey",jg);
function jg(G_,SA,Dp){
if(SA){
var Tj=oL.fg(Dp);
G_.rD=Tj;
return Tj;
}else{
var Tj=G_.rD;
if(!Tj)return null;
return Tj;
}
}

dN(up,LivePass,"V0",Sa);dN(up,LivePass,"addStreamer",Sa);
function Sa(G_,FC){
var Tj=LivePass.lV(G_,true,FC.id);
LivePass.hb.a9(Tj,FC);
}

dN(up,LivePass,"FS",qO);dN(up,LivePass,"removeStreamer",qO);
function qO(G_){
var Tj=LivePass.lV(G_,false,0);
LivePass.hb.gS(Tj);
delete G_.rD;
}


if(up==fj)LivePass.bZH=false;




if(up==fj)LivePass.J9=0;




if(up==fj)LivePass.P2=-1;




if(up==fj)LivePass.df=-2;




if(up==fj)LivePass.Ir=-3;


if(up==fj)LivePass.CHy=null;
dN(up,LivePass,"STATE_ERROR",LivePass.Ir);
dN(up,LivePass,"STATE_INIT_PENDING",LivePass.df);
dN(up,LivePass,"STATE_NOT_INITIALIZED",LivePass.P2);
dN(up,LivePass,"STATE_READY",LivePass.J9);
}
Bg(LivePass,"LivePass");





























function uf(){
var up=this;





if(up==fj)uf.iF=null;




if(up==fj)uf.f9=null;




if(up==fj)uf.h5=null;





if(up==fj)uf.tW=null;




if(up==fj)uf.X7=undefined;




if(up==fj)uf.WV=null;



if(up==fj)uf.zN=undefined;


if(up==fj)uf.BbC=undefined;




if(up==fj)uf.uQ=undefined;


if(up==fj)uf.XI="lastSwfUrls";

if(up==fj)uf.FT=undefined;
if(up==fj)uf.Obu=null;


if(up==fj)uf.Fe=undefined;




if(up==fj)uf.RcP=false;



if(up==fj)uf.e6C=null;


if(up==fj)uf.Aq=null;


if(up==fj)uf.m90=false;



if(up==fj)uf.gfr=false;


if(up==fj)uf.A6L=0;

if(up==fj)uf.CMV=0;

if(up==fj)uf.l58=0;

if(up==fj)uf.n34=false;











if(up==fj)uf.DZn=undefined;

function ju(){
O9.FW("LivePassInit: is a static class");
}






dN(up,uf,"nr",kP);
function kP(sz,TY,Z5,n8){

uf.A6L=Lt.VB();

uf.h5=sz;
uf.X7=TY;
uf.tW=Z5;
uf.FT=null;
uf.Fe=null;
uf.WV=null;
uf.Aq=null;
uf.zN=new Yw();
uf.BbC=new Yw();
uf.uQ=new EW();
uf.DZn=new EW();

if(n8!=0){
uf.KvI(n8);
}

StreamSwitch.rL();

UU.nr(uf.RcP,
function(){
uf.UcI(sz,TY,Z5);
});
}

dN(up,uf,"KvI",ywY);
function ywY(n8){
Lt.b_(
function(){
if(LivePass.Zn){
uf.Mhb(ConvivaNotification.HBx,
"LivePass init times out");

SH.z2("LivePass init timesout in "+n8+" ms");
uf.wy();
}
},n8,"LivePass.init times out");
}

dN(up,uf,"UcI",CIz);
function CIz(sz,TY,Z5){
gV.nr();
var g5=
EW.oIy("cust",TY);
SH.nr(n4.z_(gV.Kn.tc(0)),g5);

ct.nr();
ct.qF("LivePassInit","serviceUrl="+sz);


uf.l58=Lt.VB();


if(uf.n34){
uf.f9=CO.xWy(sz,TY,null,
function(fe){
uf.xvY("y",VW.z_(Lt.VB()-uf.l58));
uf.Mhb(ConvivaNotification.bQ,
"Error loading config: "+oL.fg(fe));
});
uf.f0();
}else{
uf.f9=CO.mR(sz,TY,uf.f0,
function(fe){
uf.xvY("y",VW.z_(Lt.VB()-uf.l58));
uf.Mhb(ConvivaNotification.bQ,
"Error loading config: "+oL.fg(fe));
});
}
uf.f9.YY(uf.uQ);


var Rs=null;



if(uf.m90){
uf.xd();
return;
}

if(uf.iF==null&&Rs!=null&&Rs.Wt(sz)){

var I3=Rs.tc(sz);
uf.lO(I3);
uf.Aq=I3;
}
}




dN(up,uf,"wy",NQ);
function NQ(){




if(uf.Fe!=null){
uf.Fe.ox();
uf.Fe=null;
}
if(uf.FT!=null){
Dh.YB("ox",uf.FT);
uf.FT=null;
ConvivaPrivateModule={};
}
if(uf.iF!=null){
uf.iF.wy();
uf.iF=null;
}
if(uf.f9!=null){
uf.f9.wy();
uf.f9=null;
}
uf.tW=null;
uf.h5=null;
uf.WV=null;
uf.zN=null;
uf.BbC=null;
uf.Aq=null;
StreamSwitch.bx();
SH.wy();
UU.wy();
uf.RcP=false;
uf.Obu=null;
}




dN(up,uf,"lO",QZ);
function QZ(I3){
uf.uQ.a9("LivePass.moduleUrl",I3);
ct.qF("LivePassInit","loading module "+I3);

uf.CMV=Lt.VB();
uf.iF=new qu(I3,uf.km,0);
}

dN(up,uf,"km",L5);
function L5(EU,g7){
uf.xvY("x",VW.z_(Lt.VB()-uf.CMV));



if(uf.iF==null){
uf.iF=g7;
}
if(EU==null){


uf.xd();
}else{
uf.Obu=oL.fg(EU);


if(uf.WV!=null){
uf.Kx2();
uf.wy();
}
}
}

dN(up,uf,"f0",wK);
function wK(){

if(uf.WV!=null){
return;
}
uf.xvY("y",VW.z_(Lt.VB()-uf.l58));

uf.WV=uf.f9.OP;

if(uf.m90){
ct.qF("LivePassInit","Not loading module; using built-in");
uf.xd();
return;
}

if(uf.iF!=null){
if(uf.TW(uf.iF.Vi)){
uf.xd();
return;
}else{


uf.iF.wy();
uf.iF=null;
uf.CMV=0;
}
}


uf.lO(uf.s_());
}



dN(up,uf,"s_",ix);
function ix(){
var VH=uf.f9.Cv();
O9.y8(VH.Bt>0,"have module url");
return VH.tc(0);
}





dN(up,uf,"TW",v3);
function v3(I3){
var VH=uf.f9.Cv();
O9.y8(VH.Bt>0,"have module url");
return VH.Hh(I3)>=0;
}





dN(up,uf,"xd",aW);
function aW(){
if(uf.WV==null)return;

if(!uf.m90){
if(uf.iF==null){
return;
}else if(uf.Obu!=null){


uf.Kx2();
uf.wy();
return;
}else if(!uf.iF.DY){
return;
}
}



uf.MA("ct","ConvivaLivePass","com.conviva.utils").cE=ct.cE;

uf.FT=Dh.iA(uf.MA("eI",
"ConvivaLivePass",""));

var wI=new EW();
wI.a9("loaderVersionMajor",Uj.BI);
wI.a9("loaderVersionMinor",Uj.tj);
wI.a9("loaderVersionRelease",Uj.pb);
wI.a9("loaderVersionSvn",Uj.ya);
wI.a9("traceSenderId",ct.t3);

wI.a9("traceToConsole",ct.XU);
wI.a9("disablePersistentStorage",uf.RcP);
if(uf.gfr){
wI.a9("TESTAPI_Testing",uf.gfr);
}
if(uf.e6C!=null){
wI.a9("playerTypeOverride",uf.e6C);
}
if(uf.O19){
wI.a9("noModuleLoading",uf.O19);
}
wI.a9("loaderNotificationCallback",uf.US);

Dh.YB("Yc",uf.FT,oL.uM(wI));
Dh.YB("Ns",uf.FT,uf.h5,uf.X7,uf.WV);


ct.qF("LivePassInit","complete");


uf.b2();

uf.vx();
}

dN(up,uf,"b2",Ia);
function Ia(){
if(uf.Fe!=null){
return uf.Fe.pO;
}

uf.Fe=new ConvivaMetricsSession(O9.Mul);
return uf.Fe.pO;
}



dN(up,uf,"US",Tq);
function Tq(jv){
if(uf.tW==null||jv==null)return;


var o1=jv;
if(o1==null)return;
if(uf.tW!=null){
uf.tW(o1);
}
}






dN(up,uf,"gcw",wu7);
function wu7(jE,yQ){
if(!LivePass.ZY){
if(!LivePass.Zn){

O9.Yv(yQ,"InvokeWhenFail."+jE);
}else if(uf.BbC!=null){
uf.BbC.Yb(yQ);
}
}
}






dN(up,uf,"kp",Nb);
function Nb(jE,yQ){
if(LivePass.ZY){
O9.Yv(yQ,"InvokeWhenReady."+jE);
}else if(uf.zN!=null){
uf.zN.Yb(yQ);
}
}





dN(up,uf,"Ab",vk);
function vk(jE,qA,n8){

if(!LivePass.Zn){
qA();
return;
}

uf.zN.Yb(qA);


if(n8>0){
Lt.b_(
function(){uf.Ki(qA);},
n8,
"LPInit.invokeWhenReady");
}
}



dN(up,uf,"Ki",w1);
function w1(qA){
if(uf.zN==null)return;
var Ny=uf.zN.Hh(qA);
if(Ny>=0){

uf.zN.DU(Ny);
qA();
}
}


dN(up,uf,"vx",RM);
function RM(){
LivePass.vK();
if(uf.zN!=null&&uf.zN.Bt>0){

var Ol=uf.zN;
uf.zN=new Yw();
var bC=Ol.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var Vh=bC[Jo];

O9.Yv(Vh,"LivePassInit.NotifyLivePassReady");
}
}
uf.zoM(ConvivaNotification.g9,
"Conviva LivePass is ready",null);
}

dN(up,uf,"Mhb",hWO);
function hWO(k3,RED){
if(uf.BbC!=null&&uf.BbC.Bt>0){

var Ol=uf.BbC;
uf.BbC=new Yw();
var bC=Ol.YC;
for(var Jo=0;Jo<bC.length;Jo++){
var Vh=bC[Jo];

O9.Yv(Vh,"LivePassInit.NotifyLivePassFail");
}
}
uf.zoM(k3,RED,null);
}

dN(up,uf,"zoM",pfl);
function pfl(iv,message,jz){
uf.xvY("v",iv);
uf.xvY("w",VW.z_(Lt.VB()-uf.A6L));
uf.xvY("z",(CO.G_F?1:0));

LivePass.pO.sf("eInitTime",null,null,uf.DZn);

if(uf.tW!=null){
uf.tW(new ConvivaNotification(iv,message,jz));
}
}



Va(up,uf,"UK",Pi);
function Pi(){
return uf.FT;
}

dN(up,uf,"xvY",TUH);
function TUH(Tj,vN){
if(uf.DZn==null){
uf.DZn=new EW();
}
uf.DZn.a9(Tj,vN);
}




dN(up,uf,"Kx2",Uqf);
function Uqf(){
uf.uQ.a9("LivePass.moduleLoadError",uf.Obu);
O9.Ep("Error: failed to load the module "+uf.iF.Vi,true);
uf.Mhb(ConvivaNotification.vU,
"Error loading module: "+uf.Obu);
}









dN(up,uf,"MA",Lj);
function Lj(l5,Yj,TU){


if(!uf.m90){
if(uf.iF==null||!uf.iF.DY||uf.WV==null)return null;
}
if(uf.m90){
return sHq(l5);
}

return uf.iF.Eg(l5);
}


Va(up,uf,"il1",zO2);
function zO2(){
return uf.m90;
}
Z0(up,uf,"il1",V1g);
function V1g(nQ){
uf.m90=nQ;
}

Va(up,uf,"O19",klk);
function klk(){
return uf.m90;
}

dN(up,uf,"oj",ho);
function ho(){
return uf.Aq;
}


Va(up,uf,"Nb0",dEy);
function dEy(){
return uf.DZn;
}

dN(up,uf,"YY",Os);
function Os(po){

if(uf.f9!=null){
uf.f9.YY(po);
}
if(uf.FT!=null){

var kN=po;
Dh.YB("Hg",uf.FT,kN);
}
}


Va(up,uf,"dRH",yky);
function yky(){return uf.A6L;}












































if(up!=fj)ju.apply(up,arguments);
}
Bg(uf,"uf");
















function jo(){
var up=this;

if(up!=fj)up.fV=undefined;



function ju(_w){
up.fV=_w;
}

OZ(up,"F9",BL);OZ(up,"sendEvent",BL);
function BL(Sj,qY){
O9.Yv(
function(){
uf.kp("metricsSendEvent",
function(){
var yM=uf.UK;
Dh.YB("Xr",yM,up.fV,Sj,qY);
});
},"MetricsProxy.sendEvent");
}

OZ(up,"sf",RK);OZ(up,"sendEvent2",RK);
function RK(Sj,Ml,kB,PV){
O9.Yv(
function(){
uf.kp("metricsSendEvent2",
function(){
var yM=uf.UK;
Dh.YB("Fq",yM,up.fV,Sj,Ml,kB,PV);
});
},"MetricsProxy.sendEvent2");
}

OZ(up,"tx",lk);OZ(up,"sendMeasurement",lk);
function lk(Sj,qY,nQ){
O9.Yv(
function(){
uf.kp("sendMeasurement",
function(){
var yM=uf.UK;
Dh.YB("Cy",yM,up.fV,Sj,qY,nQ);
});
},"MetricsProxy.sendMeasurement");
}

OZ(up,"_1",Xu);OZ(up,"setState",Xu);
function Xu(HN){
O9.Yv(
function(){
var yM=uf.UK;
O9.y8(yM!=null,"SetState and LivePass not ready");
Dh.YB("LF",yM,up.fV,HN);
},"MetricsProxy.setState");
}


OZ(up,"wN",ov);OZ(up,"numPending",ov);
function ov(){



var S5=0;
try{
var yM=uf.UK;
O9.y8(yM!=null,"NumPending and LivePass not ready");
var oU8=Dh.YB("TE",yM,up.fV);
S5=VW.z_(oU8);
}catch(fe){
O9.Q_("NumPending",fe);
}
return S5;
}

if(up!=fj)ju.apply(up,arguments);
}
Bg(jo,"jo");


})();
function Conviva(){};
{
Conviva.LivePass=ConvivaPrivateLoader.LivePass;
Conviva.ConvivaContentInfo=ConvivaPrivateLoader.ConvivaContentInfo;
Conviva.ConvivaNotification=ConvivaPrivateLoader.ConvivaNotification;
Conviva.ConvivaLightSession=ConvivaPrivateLoader.ConvivaLightSession;
Conviva.ConvivaStreamerProxy=ConvivaPrivateLoader.ConvivaStreamerProxy;
Conviva.StreamSwitch=ConvivaPrivateLoader.StreamSwitch;
Conviva.CandidateStream=ConvivaPrivateLoader.CandidateStream;
Conviva.StreamerStateManager=ConvivaPrivateLoader.StreamerStateManager;
Conviva.StreamerError=ConvivaPrivateLoader.StreamerError;
Conviva.StreamInfo=ConvivaPrivateLoader.StreamInfo;
Conviva.Inferrer=ConvivaPrivateLoader.Inferrer;
}

Conviva["Trace"]=ConvivaPrivateLoader.ct;
Conviva["Trace"]["AddTracer"]=ConvivaPrivateLoader.ct.rP;

Conviva["Trace"]["Info"]=ConvivaPrivateLoader.ct.qF;
Conviva["Trace"]["Warning"]=ConvivaPrivateLoader.ct.M3x;

