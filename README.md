tcl-imagebytes
=====

This package is to and from byte array and Tk photo image.

It is about an idea, byte array and Tk photo image.
When users load/decode an image data to a byte array, can users pass
the byte array to Tk photo image and show the image? When Tk photo
image has image data, can users get the image data to a byte array?
This package is to test it.


Implement commands
=====

bytesToPhoto bytearray photo width height channel  
bytesFromPhoto photo  


UNIX BUILD
=====

Building under most UNIX systems is easy, just run the configure script
and then run make. For more information about the build process, see
the tcl/unix/README file in the Tcl src dist. The following minimal
example will install the extension in the /opt/tcl directory.

    $ cd tcl-imagebytes
    $ ./configure --prefix=/opt/tcl
    $ make
    $ make install

If you need setup directory containing tcl configuration (tclConfig.sh)
and tk configuration (tkConfig.sh), below is an example:

    $ cd tcl-imagebytes
    $ ./configure --with-tcl=/usr/lib64 --with-tk=/usr/lib64
    $ make
    $ make install


Example
=====

	package require Tk
	package require stbimage
	package require imagebytes

	image create photo imgobj
	set d [::stbimage::load test.jpg]
	set width [dict get $d width]
	set height [dict get $d height]
	set channel [dict get $d channel]
	set data [dict get $d data]

	if {$channel != 3 && $channel != 4} {
		puts "Channel $channel - not supported image."
		exit
	}
    # For test to photo function
	bytesToPhoto $data imgobj $width $height $channel

	label .label
	.label configure -image imgobj
	pack .label

	# For test from photo function
	set d2 [bytesFromPhoto imgobj]
	set width [dict get $d2 width]
	set height [dict get $d2 height]
	set channel [dict get $d2 channel]
	set data [dict get $d2 data]
	if {$channel != 4} {
		puts "channel is not 4."
	} else {
		::stbimage::write tga output.tga $width $height $channel $data
	}

