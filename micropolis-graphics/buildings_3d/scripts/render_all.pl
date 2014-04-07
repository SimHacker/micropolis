#!/usr/bin/perl

use strict;
use warnings;

my @zones = qw(
	house01     1x1
	house02     1x1
	house03     1x1
	house04     1x1
	house05     1x1
	house06     1x1
	house07     1x1
	house08     1x1
	house09     1x1
	house10     1x1
	house11     1x1
	house12     1x1
	firestation 3x3
	police      3x3
	hospital    3x3
	church      3x3
	coal        4x4
	com01       3x3
	com02       3x3
	com03       3x3
	com05       3x3
	com06       3x3
	com11       3x3
	res2        3x3
	res3        3x3
	res4        3x3
	res09       3x3
	);

unless (-d "raw_renders") {
	mkdir "raw_renders";
}

for (my $i = 0; $i < @zones; $i += 2)
{
	my $zone = $zones[$i];
	my $zonetype = $zones[$i+1];

	do_render("raw_renders/$zone.png", $zone);

	do_transform(
		"${zone}_16x16.png",
		"raw_renders/$zone.png",
		"${zonetype}_16x16.map"
		);
	do_transform(
		"${zone}_32x32.png",
		"raw_renders/$zone.png",
		"${zonetype}_32x32.map"
		);
}

sub do_transform
{
	my ($outfile, $infile, $mapfile) = @_;

	return if not newer_than($infile, $outfile);
	print "$outfile\n";
	system "scripts/bend.pl","--map=scripts/$mapfile",
		"--output=$outfile", $infile;
	exit 1 if $? != 0;
}

sub do_render
{
	my ($outfile, $infile) = @_;

	# check if any of the input files are newer than the output
	# file
	my $any_newer;
	foreach my $f (all_files_in($infile)) {
		$any_newer ||= newer_than($f, $outfile);
	}
	return if not $any_newer;

	print "$outfile\n";
	system "scripts/render.sh", "$infile/$infile.blend", "raw";
	exit 1 if $? != 0;
	rename "raw0000.png", $outfile
		or die "$outfile: $!\n";
}

sub all_files_in
{
	my ($path) = @_;

	my @rv;
	opendir my $dh, $path
		or die "$path: $!\n";
	foreach my $f (readdir $dh) {
		next if $f =~ /^\./;
		if (-d "$path/$f") {
			push @rv, all_files_in("$path/$f");
		}
		else {
			push @rv, "$path/$f";
		}
	}
	closedir $dh
		or die "$path: $!\n";

	return @rv;
}

sub newer_than
{
	my ($afile, $ref_file) = @_;

	use File::stat;
	my $a = stat($afile);
	my $b = stat($ref_file);

	if (!$a or !$b) { return 1; }
	return $a->mtime > $b->mtime;
}
