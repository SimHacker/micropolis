#!/usr/bin/perl
use strict;
use warnings;
use Getopt::Long;

my $mapfile = "map";
my $outputfile = "out.png";
GetOptions(
	"map=s" => \$mapfile,
	"output=s" => \$outputfile,
	) or exit 2;

my @u;
my @v;
my @x;
my @y;
my %args;
open MAP, "<", $mapfile
	or die "$mapfile: $!\n";
while (<MAP>)
{
	chomp;
	if (/^\s*(\d+)\s*,\s*(\d+)\s*=>\s*(\d+)\s*,\s*(\d+)\s*$/)
	{
		my ($u, $v, $x, $y) = ($1, $2, $3, $4);
		push @u, $u;
		push @v, $v;
		push @x, $x;
		push @y, $y;
	}
	elsif (/^\s*%(\S+)\s+(.*)$/)
	{
		$args{$1} = $2;
	}
	elsif (!/^\s*$/) {
		die "$mapfile: invalid format\n";
	}
}
close MAP
	or die "$mapfile: $!\n";

if ($v[0] == $v[1]) {
	push @u, (shift @u);
	push @v, (shift @v);
	push @x, (shift @x);
	push @y, (shift @y);
}

my $a = (($x[0]-$x[1])*($v[0]-$v[2])-($x[0]-$x[2])*($v[0]-$v[1]))
	/(($u[0]-$u[1])*($v[0]-$v[2])-($u[0]-$u[2])*($v[0]-$v[1]));
my $b = (($x[0]-$x[1])-$a*($u[0]-$u[1]))/($v[0]-$v[1]);
my $c = $x[0] - $a*$u[0] - $b*$v[0];

my $d = (($y[0]-$y[1])*($v[0]-$v[2])-($y[0]-$y[2])*($v[0]-$v[1]))
	/(($u[0]-$u[1])*($v[0]-$v[2])-($u[0]-$u[2])*($v[0]-$v[1]));
my $e = (($y[0]-$y[1])-$d*($u[0]-$u[1]))/($v[0]-$v[1]);
my $f = $y[0] - $d*$u[0] - $e*$v[0];

my $sx = $a;
my $ry = $b;
my $tx = $c;
my $rx = $d;
my $sy = $e;
my $ty = $f;

for (my $i = 0; $i < 3; $i++) {
	my $x = $u[$i]*$sx + $v[$i]*$ry + $tx;
	my $y = $u[$i]*$rx + $v[$i]*$sy + $ty;
	print "$u[$i],$v[$i] => $x,$y\n";
}

foreach my $file (@ARGV)
{
	system "convert", $file, "-matte", "-virtual-pixel", "Transparent",
			"-affine", "$sx,$rx,$ry,$sy,$tx,$ty",
			"-transform",
			"-crop", $args{crop} || "96x96+0+0",
			$outputfile;
}
