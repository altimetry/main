#!/bin/bash
#=================================================================================
#                                 CheckInstall_Brat.sh
#=================================================================================
# Project   : S3-ALTB
# Company   : Deimos Engenharia
# Component : BRAT
# Language  : bash
#---------------------------------------------------------------------------------
#
# Scope : Script to check multiarch compatibility and install BRAT-3.3.1-x86_64
#
#=================================================================================
# $Revision:  $:
# $LastChangedBy:  $:
# $LastChangedDate:  $:
#=================================================================================


# Discarding stderr output
exec 2>/dev/null

#------------------------
# Checking dpkg allowed foreign architectures
#------------------------
echo " "
echo "Checking allowed foreign architectures..."
architectures=$(dpkg --print-foreign-architectures)

if [ $? -ne 0 ]; then
	echo "  ERROR: allowed foreign architectures not found!"
	echo "         Check if multiarch support is present on current dpkg version."
	echo " "
	echo "Installation Aborted"
	exit 1
fi

if  [[ $architectures != *"i386"* ]]
then
	echo "  ERROR: architecture i386 is not supported!"
	echo "         For installing multiarch binaries, run as sudo:"
	echo "             dpkg --add-architecture i386 && apt-get update"
	echo "         And install 'ia32-libs' and 'ia32-libs-gtk' libraries."
	echo " "
	echo "Installation Aborted"
	exit 1
fi

echo "  supported foreign architectures: $architectures."

#----------------------
# Checking 'ia32-libs'
#----------------------
echo "Checking ia32-libs..."
out=$(dpkg -s ia32-libs)
if [ $? -ne 0 ]; then
	echo "  ERROR: ia32-libs is not installed!"
	echo " "
	echo "Installation Aborted"
	exit 1
fi

#------------------------------------------
# Checking 'ia32-libs-gtk'
#------------------------------------------
echo "Checking ia32-libs-gtk..."
out=$(dpkg -s ia32-libs-gtk)
if [ $? -ne 0 ]; then
	echo "  ERROR: ia32-libs-gtk is not installed!"
	echo " "
	echo "Installation Aborted"
	exit 1
fi

#------------------------------------------
# Run installer
#------------------------------------------
./brat-3.3.1-x86_64-installer.run



