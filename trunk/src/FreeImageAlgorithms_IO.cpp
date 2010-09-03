/*
 * Copyright 2007-2010 Glenn Pierce, Paul Barber,
 * Oxford University (Gray Institute for Radiation Oncology and Biology) 
 *
 * This file is part of FreeImageAlgorithms.
 *
 * FreeImageAlgorithms is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FreeImageAlgorithms is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with FreeImageAlgorithms.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "FreeImageAlgorithms_IO.h"
#include "FreeImageAlgorithms_Utilities.h"
#include "FreeImageAlgorithms_Palettes.h"
#include "FreeImageAlgorithms_Utils.h"

#include <iostream>
#include <assert.h>

static void
CopyGreyScaleBytesToFIBitmap (FIBITMAP * src, BYTE * data, int padded, int vertical_flip)
{
    int data_line_length, height = FreeImage_GetHeight (src);

    if (padded)
    {
        data_line_length = FreeImage_GetPitch (src);
    }
    else
    {
        data_line_length = FreeImage_GetLine (src);
    }

    BYTE *bits;
    BYTE *data_row;

    int line;

    for(register int y = 0; y < height; y++)
    {
        if (vertical_flip)
        {
            line = height - y - 1;
        }
        else
        {
            line = y;
        }

        bits = (BYTE *) FreeImage_GetScanLine (src, line);

        data_row = data + (height - y - 1) * data_line_length;

        memcpy (bits, data_row, data_line_length);
    }
}

void DLL_CALLCONV
FIA_CopyColourBytesToFIBitmap (FIBITMAP * src, BYTE * data, int padded, int vertical_flip,
                           COLOUR_ORDER order)
{
    int data_line_length;
    int height = FreeImage_GetHeight (src);
    int width = FreeImage_GetWidth (src);

    if (padded)
    {
        data_line_length = FreeImage_GetPitch (src);
    }
    else
    {
        data_line_length = FreeImage_GetLine (src);
    }

    // Calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit) 
    int bytespp = FreeImage_GetLine (src) / width;

    BYTE *bits, *data_row;

    int line;

	if(order == COLOUR_ORDER_RGB) {

		for(register int y = 0; y < height; y++)
		{
			if (vertical_flip) {
				line = height - y - 1;
			}
			else {
				line = y;
			}

			bits = (BYTE *) FreeImage_GetScanLine (src, line);

			data_row = (BYTE *) data + (height - y - 1) * data_line_length;

			memcpy(bits, data_row, data_line_length);
		}
	}
	else {

		for(register int y = 0; y < height; y++)
		{
			if (vertical_flip)
				line = height - y - 1;
			else
				line = y;

			bits = (BYTE *) FreeImage_GetScanLine (src, line);

			data_row = (BYTE *) data + (height - y - 1) * data_line_length;

			memcpy(bits, data_row, data_line_length);

			// Switch R and B pixels
			for(register int x = 0; x < data_line_length; x+=bytespp) {

				SWAP(bits[x], bits[x+2]);
			}
		}
	}
}

int DLL_CALLCONV
FIA_CopyColourBytesTo8BitFIBitmap (FIBITMAP * src, BYTE * data, int data_bpp, int channel, int padded, int vertical_flip)
{
    int data_line_length;
    int height = FreeImage_GetHeight (src);
    int width = FreeImage_GetWidth (src);

	if(FreeImage_GetImageType(src) != FIT_BITMAP) {
	
		FreeImage_OutputMessageProc (FIF_UNKNOWN,
			                         "Destination image must be of type FIT_BITMAP");
		return FIA_ERROR;
	}

	if(data_bpp != 24 && data_bpp != 32) {

		FreeImage_OutputMessageProc (FIF_UNKNOWN,
			                         "Array data must be 24 or 32 bits");
		return FIA_ERROR;
	}

	int bpp = FreeImage_GetBPP(src);

	if(bpp != 8) {

		FreeImage_OutputMessageProc (FIF_UNKNOWN,
			                         "Destination image must be 8 bits");
		return FIA_ERROR;
	}

	 // Calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit) 
    int bytespp = data_bpp / 8; //FreeImage_GetLine (src) / width;

	if(channel != FI_RGBA_RED && channel != FI_RGBA_GREEN &&
		channel != FI_RGBA_BLUE) {

		FreeImage_OutputMessageProc (FIF_UNKNOWN,
			                         "Invalid channel specified");

		return FIA_ERROR;
	}	

    if (padded)
    {
        data_line_length = bytespp * width;
    }
    else
    {
        data_line_length = bytespp * width;
    }

    BYTE *bits, *data_row;

    int line;
	BYTE increment = bytespp;
	register BYTE *ptr = NULL;

	// Slightly faster taking the if out of the loop
	if (vertical_flip) {

		for(register int y = 0; y < height; y++)
		{		
			line = height - y - 1;

			bits = (BYTE *) FreeImage_GetScanLine (src, line);

			data_row = (BYTE *) data + line * data_line_length;

			ptr = data_row + channel;

			for(register int x = 0; x < width; ptr+=increment, x++) {

				bits[x] = *ptr;
			}
		}
	}
	else {
		for(register int y = 0; y < height; y++)
		{		
			bits = (BYTE *) FreeImage_GetScanLine (src, y);

			data_row = (BYTE *) data + (height - y - 1) * data_line_length;

			ptr = data_row + channel;

			for(register int x = 0; x < width; ptr+=increment, x++) {

				bits[x] = *ptr;
			}
		}
	}

	return FIA_SUCCESS;
}

void DLL_CALLCONV
FIA_CopyBytesToFBitmap (FIBITMAP * src, BYTE * data, int padded, int vertical_flip,
                        COLOUR_ORDER order)
{
    FREE_IMAGE_TYPE type = FreeImage_GetImageType (src);
    int bpp = FreeImage_GetBPP (src);

    switch (bpp)
    {
        case 8:
        case 16:
        {
            CopyGreyScaleBytesToFIBitmap (src, data, padded, vertical_flip);
            break;
        }

        case 24:
        {
            FIA_CopyColourBytesToFIBitmap (src, data, padded, vertical_flip, order);
            break;
        }

        case 32:
        {
            if (type == FIT_BITMAP)
            {
                FIA_CopyColourBytesToFIBitmap (src, data, padded, vertical_flip, order);
            }

            if (type == FIT_FLOAT)
            {
                CopyGreyScaleBytesToFIBitmap (src, data, padded, vertical_flip);
            }

            break;
        }
    }
}

FIBITMAP *DLL_CALLCONV
FIA_LoadFIBFromFile (const char *pathname)
{
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

    // check the file signature and deduce its format 
    // (the second argument is currently not used by FreeImage) 
    fif = FreeImage_GetFileType (pathname, 0);

    if (fif == FIF_UNKNOWN)
    {
        // no signature ? // try to guess the file format from the file extension

        fif = FreeImage_GetFIFFromFilename (pathname);
    }                           // check that the plugin has reading capabilities ... 

    if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading (fif))
    {
        // load the file
        return FreeImage_Load (fif, pathname, 0);
    }

    return NULL;
}

FIBITMAP *DLL_CALLCONV
FIA_LoadGreyScaleFIBFromArrayData (BYTE * data, int bpp, int width, int height,
                                   FREE_IMAGE_TYPE data_type, int padded, int vertical_flip)
{
    FIBITMAP *dib = NULL;
    BYTE *data_ptr = NULL;

    // Check the data type and convert to bpp
    if (bpp < 0)
    {
        return NULL;
    }

    data_ptr = data;

    switch (bpp)
    {
        case 8:

            // Allocate a new dib
            if ((dib = FreeImage_Allocate (width, height, 8, 0, 0, 0)) == NULL)
            {
                return NULL;
            }

            FIA_SetGreyLevelPalette (dib);

            break;

        case 16:

            if (data_type == FIT_UINT16)
            {

                if ((dib = FreeImage_AllocateT (FIT_UINT16, width, height, 16, 0, 0, 0)) == NULL)
                {
                    return NULL;
                }
            }

            if (data_type == FIT_INT16)
            {

                if ((dib = FreeImage_AllocateT (FIT_INT16, width, height, 16, 0, 0, 0)) == NULL)
                {
                    return NULL;
                }
            }

            break;

        case 32:

            if (data_type == FIT_FLOAT)
            {

                if ((dib = FreeImage_AllocateT (FIT_FLOAT, width, height, 32, 0, 0, 0)) == NULL)
                {
                    return NULL;
                }
            }
            else
            {

                return NULL;
            }

            break;
    }

    if (dib != NULL)
        FIA_CopyBytesToFBitmap (dib, data, padded, vertical_flip, COLOUR_ORDER_RGB);

    return dib;
}

FIBITMAP *DLL_CALLCONV
FIA_LoadColourFIBFromArrayData (BYTE * data, int bpp, int width, int height, int padded,
                                int vertical_flip, COLOUR_ORDER colour_order)
{
    FIBITMAP *dib = NULL;
    BYTE *data_ptr = NULL;

    // Check the data type and convert to bpp
    if (bpp < 0)
        return NULL;

    data_ptr = data;

    switch (bpp)
    {
        case 8:
        case 16:
        {
            return NULL;
        }

        case 24:
        {
            // Allocate a new dib
            if ((dib = FreeImage_AllocateT (FIT_BITMAP, width, height, 24, 0, 0, 0)) == NULL)
            {
                return NULL;
            }

            break;
        }

        case 32:
        {
            if ((dib = FreeImage_Allocate (width, height, 32, 0, 0, 0)) == NULL)
            {
                return NULL;
            }

            break;
        }
    }

    if (dib != NULL)
    {
        FIA_CopyBytesToFBitmap (dib, data, padded, vertical_flip, colour_order);
    }

    return dib;
}

int DLL_CALLCONV
FIA_SimpleSaveFIBToFile (FIBITMAP *src, const char *filepath)
{
	// Pass a filepath to be saved
	// Determine the file type from the extension
	// Try to save in the highest bpp possible for that plugin.
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

    if (src == NULL)
        return FIA_ERROR;

    // Try to guess the file format from the file extension
    fif = FreeImage_GetFIFFromFilename (filepath);

	if(fif == FIF_UNKNOWN) {
		FreeImage_OutputMessageProc (FIF_UNKNOWN,
                                     "Error Saving File! Unknown FIF.");	
		return FIA_ERROR;
	}

	// Check that the plugin has writing capabilities ... 
	if(FreeImage_FIFSupportsWriting(fif) == 0)
	{
		FreeImage_OutputMessageProc (FIF_UNKNOWN,
                                     "Error Saving File! Image Plugin does not support writing.");
		return FIA_ERROR;
	}

	FIBITMAP* dib = FreeImage_Clone(src);

	FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(dib);
	BOOL can_save = false;

	if(image_type == FIT_BITMAP) {

		// standard bitmap type
		// check that the plugin has sufficient writing
		// and export capabilities ...
		unsigned int bpp = FreeImage_GetBPP(dib);
		can_save = (FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp));
	} 
	else {

		// Png can onlty save FIT_UINT16 so lets convert it first
		if (fif == FIF_PNG && image_type == FIT_INT16)
        {
           FIA_InPlaceConvertInt16ToUInt16(&dib);
		   image_type = FIT_UINT16;
		}

		// special bitmap type
		// check that the plugin has sufficient export capabilities
		can_save = FreeImage_FIFSupportsExportType(fif, image_type);
	}

	// Error Saving File! Image type can not save with the desired bpp or type;
	// Try converting to a standard type.
	if(can_save == false)
		FIA_InPlaceConvertToStandardType(&dib, 1);

	if (!FreeImage_Save (fif, dib, filepath, 0))
    {
		FreeImage_Unload (dib);

		FreeImage_OutputMessageProc (FIF_UNKNOWN,
                                     "Unknown Error Saving File! FreeImage_Save Failed");
        return FIA_ERROR;
    }

	FreeImage_Unload (dib);

	return FIA_SUCCESS;
}

int DLL_CALLCONV
FIA_SaveFIBToFile (FIBITMAP *src, const char *filepath,
                   FREEIMAGE_ALGORITHMS_SAVE_BITDEPTH bit_depth)
{
	FIBITMAP *tmp = FreeImage_ConvertToStandardType (src, 0);
	FIBITMAP *dib = NULL;
	
    if (bit_depth == BIT24)
		dib = FreeImage_ConvertTo24Bits (tmp);
	//else if (bit_depth == BIT16)
    //    dib = FreeImage_ConvertTo16Bits555 (tmp);
    else if (bit_depth == BIT32)
        dib = FreeImage_ConvertTo32Bits (tmp);
    else
        dib = FreeImage_ConvertTo8Bits (tmp);

	if(FreeImage_GetBPP(src) == 8) {
		FIA_CopyPalette(src, dib);
    }
	
	int err = FIA_SimpleSaveFIBToFile (dib, filepath);

	FreeImage_Unload(tmp);
	FreeImage_Unload(dib);

	if(err == FIA_ERROR) {
		FreeImage_OutputMessageProc (FIF_UNKNOWN, "Error Saving File!");
        return FIA_ERROR;
	}

	return err;
}