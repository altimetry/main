#!/usr/bin/env perl

# Builds the list of source files and write it on standard output
# as InstallBuilder xml format

use FindBin;
use File::Basename;
use File::Spec;
use File::Path;
use File::Copy;

use strict;

my $ScriptLocation	= $FindBin::Bin;
my $DestDir		= $ARGV[0];
my $Version		= $ARGV[1];
my $MD5Command		= $ARGV[2];
my $SHA1Command	 	= $ARGV[3];

die "PARAMETER ERROR: launch this script via cmake\n"
	unless (@ARGV == 4);

$MD5Command	= ""	if $MD5Command =~ m/NOTFOUND/i;
$SHA1Command	= ""	if $SHA1Command =~ m/NOTFOUND/i;

my $SrcDir = File::Spec->catdir($ScriptLocation, File::Spec->updir());
my $BinDir = File::Spec->catdir($DestDir, File::Spec->updir());

my $DataSrcDir       = File::Spec->catdir($SrcDir, "data");
my $DocSrcDir       = File::Spec->catdir($SrcDir, "doc");
my $DocBinDir       = File::Spec->catdir($BinDir, "doc");
my $DocDestDir	    = File::Spec->catdir($DestDir, "doc");
my $CodaDocDir	    = File::Spec->catdir(File::Spec->catdir($SrcDir, "externals", "coda"), "doc");
my $SourcesDestDir	= File::Spec->catdir($DestDir, "sources");

my %DistribFiles = (
                "brat-$Version-linux-installer.run"	=> [ $BinDir,	$DestDir	],
		"brat-$Version-windows-installer.exe"	=> [ $BinDir,	$DestDir	],
		"brat-$Version-macosx-i386.dmg"	=> [ $BinDir,	$DestDir	],
		"brat-$Version-macosx-ppc.dmg"	=> [ $BinDir,	$DestDir	],
		"brat-$Version.tar.gz"			=> [ $BinDir,	$SourcesDestDir	],
		"BratIcon.ico"				=> [ $DataSrcDir,	$DestDir	],
		"brat_user_manual_$Version.pdf"		=> [ $DocBinDir, $DocDestDir	],
		"html"				=> [ $CodaDocDir, $DocDestDir, "coda"		],
		"codadef"				=> [ $DocBinDir, $DocDestDir		],
		"README"				=> [ $SrcDir,	$DestDir,	"README.txt", 1 ],
		"INSTALL"				=> [ $SrcDir,	$SourcesDestDir,	"INSTALL.txt", 1 ],
		"README.dvd.doc.txt"			=> [ $ScriptLocation,
									$DocDestDir,	"README.txt", 1 ],
		);

#
# Files which must be checked with MD5 or SHA1
#
my @CheckSumFiles	 = (
                "brat-$Version-linux-installer.run",
		"brat-$Version-windows-installer.exe",
		"brat-$Version-macosx-i386.dmg",
		"brat-$Version-macosx-ppc.dmg",
		"brat-$Version.tar.gz",
		);

sub MkDir ($)
{
  my $Dir	= shift;
#  print "-- Creating directory $Dir\n";
  mkdir $Dir or die "Error creating directory '$Dir': $!\n";
}

sub ChDir ($)
{
  my $Dir	= shift;
  print "-- Changing current directory to $Dir\n";
  chdir $Dir or die "Error setiing current directory '$Dir': $!\n";
}

sub CopyTree ($$)
{
  my @ToDo	= [ @_ ];
  while (@ToDo)
  {
    my ($Source, $Dest)	= @{ shift @ToDo };
    if (-d $Source)
    {
      if (! -d $Dest)
      {
#	print "-- Creating directory $Dest\n";
	mkpath $Dest, 0, 0755;
      }
	
      FILE:
      foreach (glob("$Source/*"))
      {
	my $Name	= basename($_);
        next FILE if $Name =~ m/^(Makefile|master\.mk)/i;
	push @ToDo, [ $_, "$Dest/$Name" ];
      }
    }
    else
    {
#      print "-- Copying $Source to $Dest\n";
      copy $Source, $Dest	or die "Problem copying file: $!\n";
    }
  }
}

sub WriteFile ($$$)
{
  local $_;
  my $WindowsEOL	= shift;
  my $File		= shift;
  my $Text		= shift;
  my $EOL		= ($WindowsEOL ? chr(13).chr(10) : chr(10));
  if (ref($Text) ne 'ARRAY')
  {
#    print "-- Reading file '$Text'\n";
    {
      local $/	= "\n";	# Ensure unix-like line termination
      open INFILE, "<", $Text	or die "Error opening the file: $!\n";
      $Text	= [ <INFILE> ];
      close INFILE;
    }
  }
#  print "-- Creating file '$File'\n";
  open OUTFILE, ">", $File	or die "Error creating the file: $!\n";
  foreach (@$Text)
  {
    s/[\r\n\s]+$//;
    print OUTFILE $_,$EOL;
  }
  close OUTFILE;
}

sub Run($)
{
  my $Command	= shift;
  print "-- Running command \"$Command\"\n";
  system $Command and die "Error running command\n";
}



if ( -e $DestDir )
{
  print "-- Removing directory $DestDir\n";
  rmtree $DestDir, 0, 0
	or die "Error suppressiong old directory '$DestDir'\n";
}


MkDir $DestDir;
#MkDir $DataDir;
MkDir $DocDestDir;
MkDir $SourcesDestDir;

#MkDir(File::Spec->catfile($DataDir, $_))	foreach @DataSampleDirs;


my @FilesChecked;

foreach my $File (keys %DistribFiles)
{
  my $Source	= File::Spec->catfile($DistribFiles{$File}[0],
				      $File);
  my $Dest	= File::Spec->catfile($DistribFiles{$File}[1],
				      ($DistribFiles{$File}[2] ?
					$DistribFiles{$File}[2] :
					$File));
  warn("\nWARNING: $Source does not exist\n\n"),next unless -r $Source;
  if ($DistribFiles{$File}[3])
  {
    WriteFile 1, $Dest, $Source;
  }
  else
  {
    CopyTree $Source, $Dest;
  }
  if (grep $File eq $_, @CheckSumFiles)
  {
    $Dest	= substr($Dest, length($DestDir)+1)	if substr($Dest, 0, length($DestDir)) eq $DestDir;
    push @FilesChecked, $Dest;
  }
}

WriteFile 1, File::Spec->catfile($DestDir, "autorun.inf"),
	[
	"[autorun]",
	"icon=BratIcon.ico",
	"label=Brat $Version",
	];

ChDir $DestDir;

if (@FilesChecked)
{
  my $HashFile;
  if ($MD5Command)
  {
    $HashFile	= File::Spec->catfile($DestDir, "brat-$Version-installers.md5");
    print "-- Calculating MD5 Hashes on @FilesChecked\n";
    Run "$MD5Command @FilesChecked >$HashFile";
  }
  if ($SHA1Command)
  {
    $HashFile	= File::Spec->catfile($DestDir, "brat-$Version-installers.sha1");
    print "-- Calculating SHA1 Hashes on @FilesChecked\n";
    Run "$SHA1Command @FilesChecked >$HashFile";
  }
}
