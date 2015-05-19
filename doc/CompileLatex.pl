#!/usr/bin/env perl

# Compile the latex documentation

use strict;
use Cwd;
use File::Copy;

# No check on parameters. Should only be called from makefile

my $OriginalDir		= getcwd;

die "Invalid number of arguments\n" if @ARGV != 5;

my $LatexCompiler		= $ARGV[0];
my $LatexDirectory		= $ARGV[1];
my $LatexMainFile		= $ARGV[2];
my $PdfFile			= $ARGV[3];
my $DestinationFile		= $ARGV[4];


chdir $LatexDirectory or die "Error changing to dir $LatexDirectory: $!\n";

foreach my $Index (1..3)
{
  system($LatexCompiler, $LatexMainFile)
	and die "\n\nLatex compilation error\n\n";
}

move $PdfFile, $DestinationFile
	or die "Error renaming $PdfFile to $DestinationFile: $!\n";

END {
  chdir $OriginalDir;
}
