#!/bin/bash
#
#  This file is part of BRAT.
#
#    BRAT is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    BRAT is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with BRAT.  If not, see <http://www.gnu.org/licenses/>.
#

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
#echo "Checking ia32-libs-gtk..."
#out=$(dpkg -s ia32-libs-gtk)
#if [ $? -ne 0 ]; then
#	echo "  ERROR: ia32-libs-gtk is not installed!"
#	echo " "
#	echo "Installation Aborted"
#	exit 1
#fi

#------------------------------------------
# Run installer
#------------------------------------------
./brat-3.3.0-x86_64-installer.run



