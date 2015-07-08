#!/usr/bin/env perl

# Builds the list of source files and write it on standard output
# as InstallBuilder xml format

use File::Basename;
use File::Path;
use File::Spec;
use File::Copy "cp";

my $SourceDir		= shift @ARGV;
my $BuiltSourceDir	= shift @ARGV;
my $BuiltDirName	= shift @ARGV;

my @SourceDirs = qw(
		CMakeModules
		CMakeModules/Platform
		data
		display
		doc
		gui
		libbrathl
        externals/
        externals/libgeotiff.brat
                externals/netcdf-c.brat
		externals/proj.brat
                externals/SZip.brat
                externals/udunits.brat
                externals/hdf5.brat
                externals/curl.brat
                externals/coda.brat
		packager
		process
		scheduler
        vtkzlib
        vtkjpeg
        vtktiff
	);

my @GarbageFiles	= (
		'\.\.?',
		'SVN',
		'CVS',
		'RCS',
		'SCCS',
		'.svn',
		'CMakeCache\.txt',
		'cmake\.check',
		'CMake.*Compiler\.cmake',
		'CMakeTmp',
		'CMakeFiles',
		'cmake\.depends',
		'.*\.log',
		'cmake_install\.cmake',
		'CMakeSystem\.cmake',
		'Makefile',
        'RELEASE',
        'TODO',
		'install_manifest\.txt',
		'\*.dir',
		'.*\.make',
		'~.*',
		'.*\.bck',
		'.*\.bak',
		'.*\.backup',
		'.#.*',
        '.*~',
        'proj',
        'libgeotiff',
		'netcdf',
		'udunits',
		'coda',
		'[^-A-Z.0-9_]');

my $GarbageFilesRE	= join("|", map("(?:$_)", @GarbageFiles));

my @FilesToSave;

sub IsNameOk ($)
{
  return (m/^(?:$GarbageFilesRE)$/oi ? 0 : 1);
}

sub CheckDir ($);
sub CheckDir ($)
{
  local $_;
  my $Dir	= shift;
  my @Files;
  opendir DIR, $Dir or die "Error opening directory $Dir: $!\n";
  @Files	= readdir DIR;
  closedir DIR;
  foreach (@Files)
  {
    next unless IsNameOk $_;
    if (-d File::Spec->catdir($Dir, $_))
    {
      CheckDir(File::Spec->catdir($Dir, $_));
      next;
    }
    push @FilesToSave, File::Spec->catfile($Dir, $_);
  }
}


chdir $SourceDir	or die "Error changing to directory $SourceDir: $!\n";

CheckDir $_	foreach @SourceDirs;

foreach (glob('*'))
{
  next if -d;
  push @FilesToSave, $_	if IsNameOk($_);
}

my $CreatedPath	= File::Spec->catdir($BuiltSourceDir, $BuiltDirName);

if (-d $CreatedPath)
{
  rmtree $CreatedPath, 0, 1;
}
die "Error existing and not removable directory $CreatedPath\n"
	if -d $CreatedPath;

mkdir $CreatedPath
	or die "Error creating directory $CreatedPath: $!\n";

foreach (@FilesToSave)
{
  my $Dir	= dirname($_);
  my $Name	= basename($_);
  my $FinalDir	= File::Spec->catdir($CreatedPath, $Dir);
  my $FinalFile	= File::Spec->catfile($FinalDir, $Name);
  mkpath $FinalDir, 0, 0755;
  cp $_, $FinalFile
	or die "Error copying $_ to $FinalFile: $!\n";
}

chdir $BuiltSourceDir
	or die "Error changing to directory $BuiltSourceDir: $!\n";


