

rpath_hell()
{
	# changes references in each lib to its dpendencies

	for bin in *.dylib; do
	  otool -L $bin | tr -d '\t' | sed 's/ .*//' | grep "^$lib_path_to_rm" |
	  while read A; do install_name_tool -change $A @rpath/`basename $A` $bin; done
	done
	
	# chagnes each lib load path
	
	for bin in *.dylib; do
	  otool -D $bin | grep "^$lib_path_to_rm" |
	  while read B; do install_name_tool -id @rpath/`basename $B` $bin; done
	done
}

lib_path_to_rm=/usr/local/lib
rpath_hell lib_path_to_rm

lib_path_to_rm=/usr/local/Cellar/geos/3.4.2/lib
rpath_hell lib_path_to_rm

lib_path_to_rm=/usr/local/opt
rpath_hell lib_path_to_rm

