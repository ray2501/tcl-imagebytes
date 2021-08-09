# -*- tcl -*-
# Tcl package index file, version 1.1
#
if {[package vsatisfies [package provide Tcl] 9.0-]} {
    package ifneeded imagebytes 0.2 \
	    [list load [file join $dir libtcl9imagebytes0.2.so] [string totitle imagebytes]]
} else {
    package ifneeded imagebytes 0.2 \
	    [list load [file join $dir libimagebytes0.2.so] [string totitle imagebytes]]
}
