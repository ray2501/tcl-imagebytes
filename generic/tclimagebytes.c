/*
 * For C++ compilers, use extern "C"
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <tcl.h>
#include <tk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * Only the _Init function is exported.
 */

extern DLLEXPORT int	Imagebytes_Init(Tcl_Interp * interp);

/*
 * end block for C++
 */

#ifdef __cplusplus
}
#endif


static int BytesToPhoto(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv) {
    Tk_PhotoHandle target;
    Tk_PhotoImageBlock output;
    unsigned char *binary = NULL;
    char *photo = NULL;
    int len = 0, length = 0;
    int width = 0, height = 0, channel = 0;
    int n = 0;
    unsigned char *outptr = NULL;

	if (objc != 6) {
		Tcl_WrongNumArgs(interp, 1, objv, "bytearray photo width height channel");
		return TCL_ERROR;
	}

    binary = Tcl_GetByteArrayFromObj(objv[1], (int *) &length);
    if( !binary || length < 1 ){
        return TCL_ERROR;
    }

    photo = Tcl_GetStringFromObj(objv[2], &len);
    if( !photo || len < 1 ){
        return TCL_ERROR;
    }

    if(Tcl_GetIntFromObj(interp, objv[3], (int *) &width) != TCL_OK) {
        return TCL_ERROR;
    }

    if(Tcl_GetIntFromObj(interp, objv[4], (int *) &height) != TCL_OK) {
        return TCL_ERROR;
    }

    if(Tcl_GetIntFromObj(interp, objv[5], (int *) &channel) != TCL_OK) {
        return TCL_ERROR;
    }

    if (channel != 3 && channel != 4) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "Wrong channel number",
                                    (char *)NULL );
        }

        return TCL_ERROR;
    }

    if (length != width * height * channel * sizeof(unsigned char)) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "Wrong byte array",
                                    (char *)NULL );
        }

        return TCL_ERROR;
    }

    target = Tk_FindPhoto (interp, photo);
	if (target==NULL) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "Not find image",
                                    (char *)NULL );
        }

		return TCL_ERROR;
	}

	output.width=width;
	output.height=height;
	output.pitch=width*channel;
	output.pixelSize=channel;

    output.pixelPtr = ckalloc(length);

    for (n=0; n<4; n++) {
        output.offset[n]=n;
    }

    if (channel == 3) {
        output.offset[3] = 0;
    }

    outptr = output.pixelPtr;
    memcpy(outptr, binary, length);

	Tk_PhotoSetSize(interp, target, width, height);
	Tk_PhotoPutBlock(interp, target, &output, 0, 0,
                     width, height, TK_PHOTO_COMPOSITE_SET);

	ckfree(output.pixelPtr);

    return TCL_OK;
}


static int BytesFromPhoto(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv) {
    Tk_PhotoHandle source;
    Tk_PhotoImageBlock input;
    unsigned char *binary = NULL;
    unsigned char *inptr = NULL;
    char *photo = NULL;
    int len = 0, length = 0;
    int width = 0, height = 0, channel = 0;
    Tcl_Obj *result;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "photo");
        return TCL_ERROR;
    }
    
    photo = Tcl_GetStringFromObj(objv[1], &len);
    if( !photo || len < 1 ){
        return TCL_ERROR;
    }

    source = Tk_FindPhoto (interp, photo);
    if (source==NULL) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "Not find image",
                                    (char *)NULL );
        }

        return TCL_ERROR;
    }
    Tk_PhotoGetImage(source, &input);
	width = input.width;
	height = input.height;
	channel = input.pixelSize;  /* I think it is RGBA color model */
	
	if (channel != 4) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "Wrong channel number",
                                    (char *)NULL );
        }

        return TCL_ERROR;
        
    }    

    length = width * height * channel * sizeof(unsigned char);
    binary = (unsigned char *) ckalloc (length);
    if(!binary) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "Malloc memory failed",
                                    (char *)NULL );
        }

        return TCL_ERROR;
    }

    /* Copy data from Tk photo image */
    inptr = input.pixelPtr;
    memcpy(binary, inptr, length);

    result = Tcl_NewDictObj();
    Tcl_DictObjPut(interp, result, Tcl_NewStringObj( "width", -1 ), Tcl_NewIntObj( width ));
    Tcl_DictObjPut(interp, result, Tcl_NewStringObj( "height", -1 ), Tcl_NewIntObj( height ));
    Tcl_DictObjPut(interp, result, Tcl_NewStringObj( "channel", -1 ), Tcl_NewIntObj( channel ));
    Tcl_DictObjPut(interp, result, Tcl_NewStringObj( "data", -1 ), Tcl_NewByteArrayObj( binary, length));

    Tcl_SetObjResult(interp, result);
    if (binary) ckfree(binary);

    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * Imagebytes_Init --
 *
 *	Initialize the new package.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	The Imagebytes package is created.
 *
 *----------------------------------------------------------------------
 */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
DLLEXPORT int
Imagebytes_Init(Tcl_Interp *interp)
{

    if (Tcl_InitStubs(interp, "8.6", 0) == NULL) {
	return TCL_ERROR;
    }

	if (Tk_InitStubs(interp, "8.6", 0) == NULL) {
		return TCL_ERROR;
	}

    if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }

	Tcl_CreateObjCommand(interp, "bytesToPhoto", BytesToPhoto, NULL, NULL);
	Tcl_CreateObjCommand(interp, "bytesFromPhoto", BytesFromPhoto, NULL, NULL);

    return TCL_OK;
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */
