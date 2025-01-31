# -*- tcl -*-
# Tcl package index file, version 1.1
#
if {[package vsatisfies [package provide Tcl] 9.0-]} {
    package ifneeded imagebytes 0.4 \
	    [list load [file join $dir libtcl9imagebytes0.4.so] [string totitle imagebytes]]
} else {
    package ifneeded imagebytes 0.4 \
	    [list load [file join $dir libimagebytes0.4.so] [string totitle imagebytes]]
}
