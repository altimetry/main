#!/usr/bin/env perl

# Updates the XML definitions of InstallBuilder to add some
# definitions which cannot be set statically.

use File::Basename;

my $SourceFile		= $ARGV[0];
my $DestFile		= $ARGV[1];
my $BuildDirectory	= $ARGV[2];
my $Version		= $ARGV[3];
my $WithSource		= $ARGV[4];
my %SourceDirs = (
			cdcover			=> "Art for cd labels",
			CMakeModules		=> "Cmake modules",
			'CMakeModules/Platform'	=> "Platform specific cmake modules",
			data			=> "Data files",
			display			=> "BratDisplay source files",
			doc			=> "Documentation source files",
			gui			=> "BratGui source files",
			libbrathl		=> "Brat high level library source files",
			'libgeotiff.build'	=> "Original geotiff library and automake=>cmake conversion",
            'libnetcdf.build'	=> "Original netcdf library and automake=>cmake conversion",
 			'libproj.build'		=> "Original proj library and automake=>cmake conversion",
			'libudunits.build'	=> "Original udunits library and automake=>cmake conversion",
			'coda.build'	=> "Original low level product library",
			packager		=> "Definition and tools to make install packages",
			process			=> "Sources for console/command line programs",
			scheduler		=> "BratScheduler source files",
		);

my %DesktopShortcuts = (
			BratCreateYFX		=> "BratIcon",
			BratCreateZFXY		=> "BratIcon",
			BratexportAscii		=> "BratIcon",
			BratListFieldNames	=> "BratIcon",
			BratShowInternalFiles	=> "BratIcon",
		);

my @GarbageFiles	= (
		'CMakeCache\.txt',
		'cmake\.check',
		'CMake.*Compiler\.cmake',
		'CMakeTmp',
		'CMakeFiles',
		'cmake\.depends',
		'.*\.log$',
		'cmake_install\.cmake',
		'CMakeSystem\.cmake',
		'Makefile',
		'install_manifest\.txt',
		'.*.bck$',
		'.*.bak$',
		'.*.backup$',
		'[^-A-Z.0-9_]');

my $GarbageFilesRE	= join("|", map("(?:$_)", @GarbageFiles));

my @XMLText;
my $XMLLineForSource		= undef;
my $XMLLineForDoc		= undef;
my $XMLLineForDesktopShortcuts	= undef;
my $XMLLineForDirParam		= undef;



sub GetFilesInDir ($)
{
  my @Result;
  foreach (glob "$_[0]/*")
  {
    next if -d;
    $_	= basename($_);
    next if m/$GarbageFilesRE/oi;
    push @Result, $_;
  }
  return \@Result;
}


sub WriteSourceDirInfo($$$)
{
  my ($Dir, $Desc, $Content)	= @_;

  my $Name	= lc($Dir)."source";
  $Name	=~ s/\///g;
  $Name =~ s/\./_/g;
  $Dir	= "/$Dir"	if $Dir;
  my $Source	= "..$Dir";

  $XMLText[$XMLLineForSource]	= [] unless ref($XMLText[$XMLLineForSource]) eq 'ARRAY';

  push @{$XMLText[$XMLLineForSource]}, <<"EOF";
                <folder>
                    <description>$Desc</description>
                    <destination>\${sourcedir}$Dir</destination>
                    <name>$Name</name>
                    <platforms>all</platforms>
                    <distributionFileList>
EOF

  foreach (@$Content)
  {
    push @{$XMLText[$XMLLineForSource]}, <<"EOF";
                        <distributionFile>
                            <origin>$Source/$_</origin>
                        </distributionFile>
EOF
  }

  push @{$XMLText[$XMLLineForSource]}, <<"EOF";
                    </distributionFileList>
                </folder>
EOF
  $XMLText[$XMLLineForDirParam]	= [] unless ref($XMLText[$XMLLineForDirParam]) eq 'ARRAY';
}

sub ReadXML()
{
  my $InSourceModule	= 0;
  my $InDefaultModule	= 0;
  open XML, "<", $SourceFile
		  or die "Error opening $SourceFile: $!\n";
  while (<XML>)
  {
    $InSourceModule	= 1 if m/\<name\>development\<\/name\>/;
    $InDefaultModule	= 1 if m/\<name\>default\<\/name\>/;
    if (m/\<\/component\>/)
    {
      $InDefaultModule	= 0;
      if ($InSourceModule && (! defined $XMLLineForSource))
      {
	push @XMLText	, $_, [];
	$XMLLineForSource	= $#XMLText;
        $InSourceModule	= 0;
	next;
      }
      $InSourceModule	= 0;
    }
    if (m/\<\/directoryParameter\>/)
    {
      if (! defined $XMLLineForDirParam)
      {
	push @XMLText, $_, [];
	$XMLLineForDirParam	= $#XMLText;
	next;
      }
    }
    if ($InDefaultModule)
    {
      if (m/\<\/desktopShortcutList\>/ && (! defined $XMLLineForDesktopShortcuts))
      {
        push @XMLText	, [], $_;
        $XMLLineForDesktopShortcuts	= $#XMLText -1;
        next;
      }
    }
    if (m/\<name\>documentfiles\<\/name\>/ && (! defined $XMLLineForDoc))
    {
      push @XMLText, $_, [];
      $XMLLineForDoc	= $#XMLText;
      next;
    }
    push @XMLText, $_;
  }
  close XML;
  die "Source component not found in $SourceFile\n" unless defined $XMLLineForSource;
  die "Default component not found in $SourceFile\n" unless defined $XMLLineForDesktopShortcuts;
}

sub AddSourceFiles ()
{
  push @{$XMLText[$XMLLineForSource]}, <<"EOF";
        <component>
            <name>source</name>
            <description>Source files</description>
            <canBeEdited>1</canBeEdited>
            <detailedDescription>The source files needed to build Brat from scratch</detailedDescription>
            <selected>0</selected>
            <show>1</show>
            <desktopShortcutList/>
            <folderList>
EOF
  WriteSourceDirInfo "", "Top level source directory", GetFilesInDir('..');
  foreach my $Dir (sort keys %SourceDirs)
  {
    WriteSourceDirInfo $Dir, $SourceDirs{$Dir}, GetFilesInDir("../$Dir");
  }
  push @{$XMLText[$XMLLineForSource]}, <<"EOF";
            </folderList>
            <postInstallationActionList/>
            <startMenuShortcutList/>
        </component>
EOF
}


sub AddDirParams ()
{
  if ($WithSource)
  {
    push @{$XMLText[$XMLLineForDirParam]}, <<'EOF';
        <directoryParameter>
            <name>sourcedir</name>
            <title>Directory for source files</title>
            <description>Root of source tree</description>
            <explanation>Directory where to write the source files</explanation>
            <value>${installdir}/src</value>
            <default>${installdir}/src</default>
            <allowEmptyValue>0</allowEmptyValue>
            <ask>yes</ask>
            <cliOptionName>source</cliOptionName>
            <leftImage></leftImage>
            <mustBeWritable>yes</mustBeWritable>
            <mustExist>0</mustExist>
            <width>30</width>
        </directoryParameter>
EOF
  }
}

sub AddDesktopShortcuts ()
{
  my $Name;

  foreach $Name (sort keys %DesktopShortcuts)
  {
    my $Icon	= $DesktopShortcuts{$Name};
    push @{$XMLText[$XMLLineForDesktopShortcuts]}, <<"EOF";
                <shortcut>
                    <comment>$Name command line program</comment>
                    <exec>\${installdir}/bin/$Name</exec>
                    <icon>\${installdir}/data/$Icon.png</icon>
                    <name>$Name program</name>
                    <path></path>
                    <platforms>all</platforms>
                    <windowsExec>\${installdir}/bin/$Name.exe</windowsExec>
                    <windowsExecArgs>"%1"</windowsExecArgs>
                    <windowsIcon>\${installdir}/data/$Icon.ico</windowsIcon>
                    <windowsPath></windowsPath>
                    <ruleList>
                        <compareText>
                            <logic>equals</logic>
                            <nocase>0</nocase>
                            <text>\${cliprograms}</text>
                            <value>1</value>
                        </compareText>
                    </ruleList>
                </shortcut>
EOF
  }
}

sub AddDoc ()
{
  push @{$XMLText[$XMLLineForDoc]}, <<"EOF";
                    <distributionFileList>
                        <distributionFile>
                            <origin>$BuildDirectory/doc/brat_user_manual_\${product_version}.pdf</origin>
                        </distributionFile>
EOF
  push @{$XMLText[$XMLLineForDoc]}, <<"EOF"
                        <distributionFile>
                            <origin>$BuildDirectory/doc/brathl_reference_manual_\${product_version}.pdf</origin>
                        </distributionFile>
EOF
	if -r "$BuildDirectory/doc/brathl_reference_manual_$Version.pdf";
  push @{$XMLText[$XMLLineForDoc]}, <<"EOF";
                        <distributionDirectory>
                            <origin>$BuildDirectory/doc/coda-html</origin>
                        </distributionDirectory>
EOF
  push @{$XMLText[$XMLLineForDoc]}, <<"EOF";
                        <distributionDirectory>
                            <origin>$BuildDirectory/doc/data-dictionary-html</origin>
                        </distributionDirectory>
                    </distributionFileList>
EOF
}

#
#========================================================
#

die basename($0).": Invalid number or arguments: ",join(", ",map( "\"$_\"", @ARGV)),"\n"
								if @ARGV != 5;
die basename($0).": Invalid source flag '$WithSource'\n"	unless uc($WithSource) =~ m/^ON|OFF/;
die basename($0).": Invalid build dir '$BuildDirectory'\n"	unless -d $BuildDirectory;
$WithSource	= uc($WithSource) eq 'ON';

ReadXML;

AddSourceFiles		if $WithSource;
AddDirParams;
AddDesktopShortcuts;
AddDoc;

open XML, ">", $DestFile	or die "Error creating $DestFile: $!\n";
foreach (@XMLText)
{
  print XML (ref($_) eq 'ARRAY' ? @$_ : $_);
}
close XML;
