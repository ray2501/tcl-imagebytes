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
    int len = 0, length = 0, width = 0, height = 0, channels = 0;

    if (objc != 6) {
        Tcl_WrongNumArgs(interp, 1, objv, "bytearray photo width height channels");
        return TCL_ERROR;
    }

    binary = Tcl_GetByteArrayFromObj(objv[1], (int *) &length);
    if ( !binary || length < 1 ){
        Tcl_SetResult(interp, "invalid byte array", TCL_STATIC);
        return TCL_ERROR;
    }

    photo = Tcl_GetStringFromObj(objv[2], &len);
    if ( !photo || len < 1 ){
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[3], &width) != TCL_OK) {
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[4], &height) != TCL_OK) {
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[5], &channels) != TCL_OK) {
        return TCL_ERROR;
    }

    if (channels < 1 || channels > 4) {
        Tcl_SetResult(interp, "invalid number of channels", TCL_STATIC);
        return TCL_ERROR;
    }

    if (length != width * height * channels * sizeof(unsigned char)) {
        Tcl_SetResult(interp, "invalid byte array length", TCL_STATIC);
        return TCL_ERROR;
    }

    target = Tk_FindPhoto(interp, Tcl_GetString(objv[2]));
    if (target == NULL) {
        Tcl_SetResult(interp, "photo not found", TCL_STATIC);
        return TCL_ERROR;
    }

    memset(&output, 0, sizeof(output));
    output.width = width;
    output.height = height;
    output.pitch = width * channels;
    output.pixelSize = channels;
    output.pixelPtr = binary;

    switch (channels) {
    case 1: /* grey */
        output.offset[0] = 0;
        output.offset[1] = 1;
        output.offset[2] = 1;
        output.offset[3] = 1;
        break;
    case 2: /* grey with alpha */
        output.offset[0] = 0;
        output.offset[1] = 2;
        output.offset[2] = 2;
        output.offset[3] = 1;
        break;
    case 3: /* RGB */
    case 4: /* RGB with alpha */
        output.offset[0] = 0;
        output.offset[1] = 1;
        output.offset[2] = 2;
        output.offset[3] = 3;
        break;
    }
    if (Tk_PhotoExpand(interp, target, width, height) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Tk_PhotoPutBlock(interp, target, &output, 0, 0, width, height, TK_PHOTO_COMPOSITE_SET) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}


static int BytesFromPhoto(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv) {
    Tk_PhotoHandle source;
    Tk_PhotoImageBlock input;
    unsigned char *binary = NULL;
    char *photo = NULL;
    int len = 0, length = 0, width = 0, height = 0, channels = 0;
    Tcl_Obj *result;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "photo");
        return TCL_ERROR;
    }

    photo = Tcl_GetStringFromObj(objv[1], &len);
    if( !photo || len < 1 ){
        return TCL_ERROR;
    }

    source = Tk_FindPhoto(interp, Tcl_GetString(objv[1]));
    if (source == NULL) {
        Tcl_SetResult(interp, "photo not found", TCL_STATIC);
        return TCL_ERROR;
    }

    Tk_PhotoGetImage(source, &input);
    width = input.width;
    height = input.height;
    channels = input.pixelSize;

    if (channels < 1 || channels > 4) {
        Tcl_SetResult(interp, "unsupported number of channels", TCL_STATIC);
        return TCL_ERROR;
    }

    length = width * height * channels * sizeof(unsigned char);
    if (channels == input.pixelSize &&
        input.pitch == channels * width &&
        input.offset[0] == 0 &&
        (channels < 2 || input.offset[1] == 1) &&
        (channels < 3 || input.offset[2] == 2) &&
        (channels < 4 || input.offset[3] == 3)) {
        /* input.pixelPtr is in proper channel order */
        binary = input.pixelPtr;
    } else {
        int x, y, tmp;
        unsigned char *input_pixel, *output_pixel;

        binary = (unsigned char *) attemptckalloc(length);
        if (binary == NULL) {
            Tcl_SetResult(interp, "out of memory", TCL_STATIC);
            return TCL_ERROR;
        }
        output_pixel = binary;

        switch (channels) {
        case 1: /* grey */
            for (y = 0; y < height; y++) {
                input_pixel = input.pixelPtr + y * input.pitch;
                for (x = 0; x < width; x++) {
                    tmp  = 19518 * input_pixel[input.offset[0]];
                    tmp += 38319 * input_pixel[input.offset[1]];
                    tmp +=  7442 * input_pixel[input.offset[2]];
                    tmp = tmp >> 16;
                    if (tmp > 255) {
                        tmp = 255;
                    }
                    output_pixel[0] = tmp;
                    output_pixel++;
                    input_pixel += input.pixelSize;
                }
            }
            break;
        case 2: /* grey with alpha */
            for (y = 0; y < height; y++) {
                input_pixel = input.pixelPtr + y * input.pitch;
                for (x = 0; x < width; x++) {
                    tmp  = 19518 * input_pixel[input.offset[0]];
                    tmp += 38319 * input_pixel[input.offset[1]];
                    tmp +=  7442 * input_pixel[input.offset[2]];
                    tmp = tmp >> 16;
                    if (tmp > 255) {
                        tmp = 255;
                    }
                    output_pixel[0] = tmp;
                    output_pixel[1] = input_pixel[input.offset[3]];
                    output_pixel += 2;
                    input_pixel += input.pixelSize;
                }
            }
            break;
        case 3: /* RGB */
            for (y = 0; y < height; y++) {
                input_pixel = input.pixelPtr + y * input.pitch;
                for (x = 0; x < width; x++) {
                    output_pixel[0] = input_pixel[input.offset[0]];
                    output_pixel[1] = input_pixel[input.offset[1]];
                    output_pixel[2] = input_pixel[input.offset[2]];
                    output_pixel += 3;
                    input_pixel += input.pixelSize;
                }
            }
            break;
        case 4: /* RGB with alpha */
            for (y = 0; y < height; y++) {
                input_pixel = input.pixelPtr + y * input.pitch;
                for (x = 0; x < width; x++) {
                    output_pixel[0] = input_pixel[input.offset[0]];
                    output_pixel[1] = input_pixel[input.offset[1]];
                    output_pixel[2] = input_pixel[input.offset[2]];
                    output_pixel[3] = input_pixel[input.offset[3]];
                    output_pixel += 4;
                    input_pixel += input.pixelSize;
                }
            }
            break;
        }
    }

    result = Tcl_NewDictObj();
    Tcl_DictObjPut(interp, result, Tcl_NewStringObj("width", -1), Tcl_NewIntObj(width));
    Tcl_DictObjPut(interp, result, Tcl_NewStringObj("height", -1), Tcl_NewIntObj(height));
    Tcl_DictObjPut(interp, result, Tcl_NewStringObj("channels", -1), Tcl_NewIntObj(channels));
    Tcl_DictObjPut(interp, result, Tcl_NewStringObj("data", -1), Tcl_NewByteArrayObj(binary, length));

    Tcl_SetObjResult(interp, result);

    if (binary != input.pixelPtr) {
        ckfree(binary);
    }

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

