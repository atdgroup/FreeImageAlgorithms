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

#include "FreeImageAlgorithms.h"
#include "FreeImageAlgorithms_IO.h"
#include "FreeImageAlgorithms_Logic.h"
#include "FreeImageAlgorithms_Drawing.h"
#include "FreeImageAlgorithms_Palettes.h"
#include "FreeImageAlgorithms_Utilities.h"
#include "FreeImageAlgorithms_Arithmetic.h"

#include "FreeImageAlgorithms_Utils.h"

#include "Constants.h"

#include "profile.h"

#include <iostream>
#include <math.h>

#define ROOT2 1.4142f

// Class that templates functions so that they work on all image types.
template < class Tsrc > class TemplateImageFunctionClass
{
  public:

	int GradientBlendMosaicPaste (FIBITMAP* dst, FIBITMAP* src, int x, int y);
};

static TemplateImageFunctionClass < unsigned char > UCharImage;
static TemplateImageFunctionClass < unsigned short > UShortImage;
static TemplateImageFunctionClass < short > ShortImage;
static TemplateImageFunctionClass < unsigned long > ULongImage;
static TemplateImageFunctionClass < long > LongImage;
static TemplateImageFunctionClass < float > FloatImage;
static TemplateImageFunctionClass < double > DoubleImage;

static inline int RoundRealToNearestInteger(float value)
{
  return (int) (value + 0.5f);
}

static inline FIBITMAP*
FIA_DistanceMap (int width, int height)
{
	FIBITMAP *image = FreeImage_AllocateT(FIT_FLOAT, width, height, 32, 0, 0, 0);

	float *bits = NULL;

	float center_x = (float)(width / 2.0f + 0.5f);
	float center_y = (float)(height / 2.0f + 0.5f);

	float current_min;

	for(int y = 0; y < height; y++)
	{
		bits = (float *) FIA_GetScanLineFromTop(image, y);

		for(int x = 0; x < width; x++) {

			if (x <= center_x)
				current_min = (float) x;
			else
				current_min = (float) (width - x);

#ifdef WIN32
			if (y <= center_y)
				bits[x] = min(current_min, (float) y);
			else
				bits[x] = min(current_min, (float) (height - y));
#else
			if (y <= center_y)
				bits[x] = std::min(current_min, (float) y);
			else
				bits[x] = std::min(current_min, (float) (height - y));
#endif
		}
	}

	return image;
}


static inline float*
FIA_GeneratePMap (FIBITMAP *mask)
{
	int width = FreeImage_GetWidth(mask);
	int height = FreeImage_GetHeight(mask);
	int size = width * height;

	// Set up the image where the 'alpha' values will be stored
	float* pMatrix = (float*) malloc(sizeof(float)*height*width);

	// Initialise with 'high' values
#ifdef WIN32
	float max_val = (float) max((height+1),(width+1));
#else
	float max_val = (float) std::max((height+1),(width+1));
#endif

	// Initialise with 'high' values
	for (int i=0; i<size; i++)
		pMatrix[i] = (float) max_val;


	BYTE *pCentre=NULL, *pLeft=NULL, *pTop=NULL, *pTopLeft=NULL, *pTopRight=NULL;
	BYTE *pRight=NULL, *pBottom=NULL, *pBottomLeft=NULL, *pBottomRight=NULL;
	float *pCentreFM=NULL, *pLeftFM=NULL, *pTopFM=NULL, *pTopLeftFM=NULL, *pTopRightFM=NULL;
	float *pCentreBM=NULL, *pRightBM=NULL, *pBottomBM=NULL, *pBottomLeftBM=NULL, *pBottomRightBM=NULL;
	float LT, BLTL, RB, BRTR, currMin;

	for (register int Y = 1; Y< height; Y++)
	{			
		pCentre = FIA_GetScanLineFromTop(mask, Y) + 1;

		pLeft = 		pCentre -1;
		pTop =          pCentre + FreeImage_GetPitch(mask);  // underlying FreeImage is upside down

		pTopLeft = 		pTop-1;
		pTopRight =		pTop+1;

		pCentreFM =     &pMatrix[Y*width+1];		
		pLeftFM = 		pCentreFM -1;
		pTopFM = 		pCentreFM - width;
		pTopLeftFM = 	pTopFM-1;
		pTopRightFM =	pTopFM+1;

		for (register int X = 1; X< width-1; X++)
		{
			// Look left for an edge
			if (*pCentre == 1)
			{
				if (*pLeft == 0 || *pTop == 0)
					*pCentreFM = 1;
				else if ( *pTopLeft == 0 || *pTopRight == 0)
					*pCentreFM = ROOT2;
				else
				{
						// We have found an edge on this scan line
						#ifdef WIN32
						LT = min(*pLeftFM+1.0f, *pTopFM+1.0f);
						BLTL = min(*pTopRightFM+ROOT2, *pTopLeftFM+ROOT2);
						*pCentreFM = min(LT, BLTL);
						#else
						LT = std::min(*pLeftFM+1.0f, *pTopFM+1.0f);
                                                BLTL = std::min(*pTopRightFM+ROOT2, *pTopLeftFM+ROOT2);
                                                *pCentreFM = std::min(LT, BLTL);
						#endif
				}
			}

			pCentre ++;		
			pLeft ++; 		
			pTop ++; 			
			pTopLeft ++; 		
			pTopRight ++;

			pCentreFM ++;		
			pLeftFM ++; 		
			pTopFM ++; 			
			pTopLeftFM ++; 		
			pTopRightFM ++;

		}  
	}

	for (register int Y = height-2; Y >= 0; Y--)
	{
		//pCentre =       maskATinfo.firstPixelAddress.Pix8_Ptr+Y*maskATinfo.rawPixels+maskATimWidth-2;
		pCentre = FIA_GetScanLineFromTop(mask, Y) + FreeImage_GetWidth(mask) - 2;

		pRight = 		pCentre +1;
		//pBottom = 		pCentre+info.rawPixels;
		pBottom = 		pCentre - FreeImage_GetPitch(mask);   // underlying FreeImage is upside down
		pBottomRight =	pBottom+1;
		pBottomLeft =	pBottom-1;

		pCentreBM =     &pMatrix[Y*width+width-2];		
		pRightBM = 		pCentreBM +1;
		pBottomBM = 	pCentreBM+width; 
		pBottomRightBM= pBottomBM+1;
		pBottomLeftBM =	pBottomBM-1;

		for (register int X = width-2; X > 0; X--)
		{
			// Look right for an edge
			if (*pCentre == 1)
			{
				if (*pRight == 0 || *pBottom == 0)
					*pCentreBM = 1;
				else if ((*pBottomLeft == 0 || *pBottomRight == 0) && *pCentreBM != 1)
					*pCentreBM = ROOT2;
				else
				{
						// We have found an edge on this scan line
						#ifdef WIN32
						RB = min(*pRightBM+1, *pBottomBM+1);
						BRTR = min(*pBottomRightBM+ROOT2, *pBottomLeftBM+ROOT2 );
						currMin = min(RB, BRTR);
						*pCentreBM = min(currMin, *pCentreBM);
						#else
						RB = std::min(*pRightBM+1, *pBottomBM+1);
                                                BRTR = std::min(*pBottomRightBM+ROOT2, *pBottomLeftBM+ROOT2 );
                                                currMin = std::min(RB, BRTR);
                                                *pCentreBM = std::min(currMin, *pCentreBM);
						#endif
				}
			}

			pCentre--;	
			pRight--;
			pBottom--;
			pBottomRight--;
			pBottomLeft--;

			pCentreBM--;		
			pRightBM--;
			pBottomBM--;
			pBottomLeftBM--;
			pBottomRightBM--;

		}				  
	}

	return pMatrix;
}

static FIARECT SetRectRelativeToPoint(FIARECT rect, FIAPOINT pt)
{
        FIARECT r;

        r.left = rect.left - pt.x;
        r.right = rect.right - pt.x;
        r.top = rect.top - pt.y;
        r.bottom = rect.bottom - pt.y;

        return r;
}

template < typename Tsrc > int TemplateImageFunctionClass <
    Tsrc >::GradientBlendMosaicPaste(FIBITMAP* dst, FIBITMAP* src, int x, int y)
{
	int xc=0, yc=0, x1=0, y1=0, intersect_width, intersect_height, bytespp; 
	FIBITMAP *dstRegion=NULL, *dstRegionMask=NULL, *dstRegionMaskInverted=NULL;
    FIBITMAP *maskedSrc=NULL, *invertedMaskedSrc=NULL, *blended_section=NULL, *distMapEdges = NULL, *srcRegion = NULL;
    float *pCentreFM=NULL, *pLeftFM=NULL, *pTopFM=NULL, *pTopLeftFM=NULL;
    float **pTopRightFM=NULL,*distMapEdgesBits = NULL, *pMatrix = NULL;
    double val, max_possoble_value; 
	BYTE *pCentre=NULL, *pLeft=NULL, *pTop=NULL, *pTopLeft=NULL, *pTopRight=NULL, *pCentreF = NULL;
	FIARECT dstRect, srcRect;
    bool greyscale_image = true;

	if(dst == NULL || src == NULL)
	    goto CLEANUP;
	    
    if(FreeImage_GetImageType(src) == FIT_BITMAP && FreeImage_GetBPP(src) > 8) {
	    greyscale_image = false;
    }
    
	PROFILE_START("FIA_GradientBlendMosaicPaste");
	
	FIARECT src_intersection_rect, intersect_rect;

	dstRect = FIAImageRect(dst);
	srcRect = MakeFIARect(x, y, x + FreeImage_GetWidth(src) - 1, y + FreeImage_GetHeight(src) - 1);

    if(FIA_IntersectingRect(dstRect, srcRect, &intersect_rect) == 0) {
        		
        PROFILE_STOP("FIA_GradientBlendMosaicPaste");
        
		FreeImage_OutputMessageProc (FIF_UNKNOWN, "Image dst (Left, Top, Right, Bottom) (%d, %d, %d, %d)"
												  " and image src (%d, %d, %d, %d) do not intersec." ,
									 dstRect.left, dstRect.top, dstRect.right, dstRect.bottom,
		                             srcRect.left, srcRect.top, srcRect.right, srcRect.bottom);

		goto CLEANUP;
    }

    src_intersection_rect = SetRectRelativeToPoint(intersect_rect, MakeFIAPoint(x, y));

    intersect_width = intersect_rect.right - intersect_rect.left + 1;
    intersect_height = intersect_rect.bottom - intersect_rect.top + 1;
	
    // Copy the part of dst out that corresponds to the placement of the src image. 
	dstRegion = FIA_CopyLeftTopWidthHeight (dst, intersect_rect.left, intersect_rect.top, intersect_width, intersect_height);
	
	srcRegion = FIA_CopyLeftTopWidthHeight (src, src_intersection_rect.left, src_intersection_rect.top,
	                                        intersect_width, intersect_height);
	
	// Check that the width & height is what was specified
	if(FreeImage_GetWidth(srcRegion) != intersect_width || FreeImage_GetHeight(srcRegion) != intersect_height) {
	
		PROFILE_STOP("FIA_GradientBlendMosaicPaste");
		
	    FreeImage_OutputMessageProc (FIF_UNKNOWN, "Image src (%d, %d) is not the requested size (%d, %d)." ,
		                             FreeImage_GetWidth(srcRegion), FreeImage_GetHeight(srcRegion),
		                             intersect_width, intersect_height);

		goto CLEANUP;
	}
	
	if(FIA_CheckSizesAreSame(dstRegion, srcRegion) == 0) {

		PROFILE_STOP("FIA_GradientBlendMosaicPaste");
		
		FreeImage_OutputMessageProc (FIF_UNKNOWN, "Image dst (%d, %d) and image src (%d, %d) are not the same size." ,
		                             FreeImage_GetWidth(dstRegion), FreeImage_GetHeight(dstRegion),
		                             FreeImage_GetWidth(srcRegion), FreeImage_GetHeight(srcRegion));

		goto CLEANUP;
	}
	
	blended_section = FIA_CloneImageType(srcRegion, intersect_width, intersect_height);	
	
	FIA_GetMaxPosibleValueForGreyScaleType (FreeImage_GetImageType(dst), &max_possoble_value);

	dstRegionMask = FIA_Threshold(dstRegion, 1.0, max_possoble_value, 1.0);
	FIA_InPlaceConvertToStandardType(&dstRegionMask, 0);	
	FIA_InPlaceConvertTo8Bit(&dstRegionMask);
		
	maskedSrc = FreeImage_Clone(srcRegion);       
	FIA_MaskImage(maskedSrc, dstRegionMask);
		
	dstRegionMaskInverted = FreeImage_Clone(dstRegionMask);
	FIA_ReverseMaskImage (dstRegionMaskInverted, 1);
		
	invertedMaskedSrc = FreeImage_Clone(srcRegion);
    FIA_MaskImage(invertedMaskedSrc, dstRegionMaskInverted);
        
	PROFILE_START("FIA_GradientBlendMosaicPaste - DistanceMap");
		
	distMapEdges = FIA_DistanceMap (intersect_width, intersect_height);

	PROFILE_STOP("FIA_GradientBlendMosaicPaste - DistanceMap");
		
	PROFILE_START("FIA_GradientBlendMosaicPaste - PMap");
		
	pMatrix = FIA_GeneratePMap(dstRegionMask);
	
	PROFILE_STOP("FIA_GradientBlendMosaicPaste - PMap");
	
	pCentreFM = pMatrix;
	
	PROFILE_START("FIA_GradientBlendMosaicPaste - Blend");
	
	bytespp = FreeImage_GetLine (srcRegion) / FreeImage_GetWidth (srcRegion);
    distMapEdgesBits = NULL;
    	
    if(  greyscale_image) {
      
      Tsrc *blended_section_ptr=NULL, *dst_region_ptr=NULL, *mask_src_ptr=NULL;
      
	  for (y1=0;y1<intersect_height;y1++) 
	  {
          pCentreF = FIA_GetScanLineFromTop(dstRegionMask, y1);
          distMapEdgesBits = (float*)FIA_GetScanLineFromTop(distMapEdges, y1);
		  blended_section_ptr = (Tsrc*) FIA_GetScanLineFromTop(blended_section, y1);
		  dst_region_ptr = (Tsrc*) FIA_GetScanLineFromTop(dstRegion, y1);
		  mask_src_ptr = (Tsrc*) FIA_GetScanLineFromTop(maskedSrc, y1);
	
		  for (x1=0;x1<intersect_width;x1++) 
		  {
		      if (*pCentreF==1)
		      {   
		          val = *pCentreFM / (*pCentreFM+distMapEdgesBits[x1]);

				  *blended_section_ptr = (Tsrc) RoundRealToNearestInteger ((float)((*dst_region_ptr * val) + (*mask_src_ptr * (1.0f-val))));
  		  		
			  }
			  else {
				  *blended_section_ptr=0;
			  }
  			
			  pCentreF++;
			  pCentreFM++;
  			
			  blended_section_ptr++;
  			
			  dst_region_ptr++;
			  mask_src_ptr++;

		   }
	  }
	}
	else {
	
	  BYTE *blended_section_ptr=NULL, *dst_region_ptr=NULL, *mask_src_ptr=NULL;
	  
	  int bytespp = FreeImage_GetLine (srcRegion) / FreeImage_GetWidth (srcRegion);
	  
	  for (y1=0;y1<intersect_height;y1++) 
	  {
           pCentreF = FIA_GetScanLineFromTop(dstRegionMask, y1);
           distMapEdgesBits = (float*)FIA_GetScanLineFromTop(distMapEdges, y1);
		   blended_section_ptr = FIA_GetScanLineFromTop(blended_section, y1);
		   dst_region_ptr = FIA_GetScanLineFromTop(dstRegion, y1);
		   mask_src_ptr = FIA_GetScanLineFromTop(maskedSrc, y1);
  				
		  for (x1=0;x1<intersect_width ;x1++) 
		  {
		      if (pCentreF[x1] > 0)
		      {
		    	  val = *pCentreFM / (*pCentreFM+distMapEdgesBits[x1]);
                  
				  blended_section_ptr[FI_RGBA_RED] = RoundRealToNearestInteger ((float)((dst_region_ptr[FI_RGBA_RED] * val) + (mask_src_ptr[FI_RGBA_RED] * (1-val))));
				  blended_section_ptr[FI_RGBA_GREEN] = RoundRealToNearestInteger ((float)((dst_region_ptr[FI_RGBA_GREEN] * val) + (mask_src_ptr[FI_RGBA_GREEN] * (1-val))));
				  blended_section_ptr[FI_RGBA_BLUE] = RoundRealToNearestInteger ((float)((dst_region_ptr[FI_RGBA_BLUE] * val) + (mask_src_ptr[FI_RGBA_BLUE] * (1-val))));	  		
			  }
			  else {
				  blended_section_ptr[FI_RGBA_RED]	= 0;
				  blended_section_ptr[FI_RGBA_GREEN] = 0;
				  blended_section_ptr[FI_RGBA_BLUE]	= 0;			  
  			  }
  			
  			  blended_section_ptr += bytespp;
  			  dst_region_ptr += bytespp;
  			  mask_src_ptr += bytespp;
  			  
			  pCentreFM++;
		   }
	  }
	}

    if(FIA_CheckSizesAreSame(invertedMaskedSrc, dstRegionMaskInverted) == 0) {

		PROFILE_STOP("FIA_GradientBlendMosaicPaste");

		FreeImage_OutputMessageProc (FIF_UNKNOWN, "Foreground src (%d, %d) and mask image (%d, %d) are not the same size." ,
		                             FreeImage_GetWidth(invertedMaskedSrc), FreeImage_GetHeight(invertedMaskedSrc),
		                             FreeImage_GetWidth(dstRegionMaskInverted), FreeImage_GetHeight(dstRegionMaskInverted));
		goto CLEANUP;
	}

	FIA_Combine(blended_section, invertedMaskedSrc, dstRegionMaskInverted);
	
	PROFILE_STOP("FIA_GradientBlendMosaicPaste - Blend");
	
	FIA_PasteFromTopLeft(dst, blended_section, intersect_rect.left, intersect_rect.top);
	
	if(blended_section != NULL)
		FreeImage_Unload(blended_section);

	if(dstRegion != NULL)
		FreeImage_Unload(dstRegion);
	
	if(srcRegion != NULL)
		FreeImage_Unload(srcRegion);
	
	if(dstRegionMask != NULL)
		FreeImage_Unload(dstRegionMask);
	
	if(dstRegionMaskInverted != NULL)
		FreeImage_Unload(dstRegionMaskInverted);
	
	if(maskedSrc != NULL)
		FreeImage_Unload(maskedSrc);
	
	if(invertedMaskedSrc != NULL)
		FreeImage_Unload(invertedMaskedSrc);
    
	if(distMapEdges != NULL)
		FreeImage_Unload(distMapEdges);

	if(pMatrix != NULL)
		free(pMatrix);
	
	PROFILE_STOP("FIA_GradientBlendMosaicPaste");
	
	return FIA_SUCCESS;

CLEANUP:

	if(blended_section != NULL)
		FreeImage_Unload(blended_section);

	if(dstRegion != NULL)
		FreeImage_Unload(dstRegion);
	
	if(srcRegion != NULL)
		FreeImage_Unload(srcRegion);
	
	if(dstRegionMask != NULL)
		FreeImage_Unload(dstRegionMask);
	
	if(dstRegionMaskInverted != NULL)
		FreeImage_Unload(dstRegionMaskInverted);
	
	if(maskedSrc != NULL)
		FreeImage_Unload(maskedSrc);
	
	if(invertedMaskedSrc != NULL)
		FreeImage_Unload(invertedMaskedSrc);
    
	if(distMapEdges != NULL)
		FreeImage_Unload(distMapEdges);

	if(pMatrix != NULL)
		free(pMatrix);

	return FIA_ERROR;
}


int DLL_CALLCONV
FIA_GradientBlendMosaicPaste (FIBITMAP* dst, FIBITMAP* src, int x, int y)
{
    if (dst == NULL && src == NULL)
        return NULL;

    FREE_IMAGE_TYPE src_type = FreeImage_GetImageType (src);

    switch (src_type)
    {
        case FIT_BITMAP:       // standard image: 1-, 4-, 8-, 16-, 24-, 32-bit
        {
            return UCharImage.GradientBlendMosaicPaste (dst, src, x, y);
        }

        case FIT_UINT16:       // array of unsigned short: unsigned 16-bit
        {
            return UShortImage.GradientBlendMosaicPaste (dst, src, x, y);
        }

        case FIT_INT16:        // array of short: signed 16-bit
        {
            return ShortImage.GradientBlendMosaicPaste (dst, src, x, y);
        }

        case FIT_UINT32:       // array of unsigned long: unsigned 32-bit
        {
            return ULongImage.GradientBlendMosaicPaste (dst, src, x, y);
        }

        case FIT_INT32:        // array of long: signed 32-bit
        {
            return LongImage.GradientBlendMosaicPaste (dst, src, x, y);
        }

        case FIT_FLOAT:        // array of float: 32-bit
        {
            return FloatImage.GradientBlendMosaicPaste (dst, src, x, y);
        }

        case FIT_DOUBLE:       // array of double: 64-bit
        {
            return DoubleImage.GradientBlendMosaicPaste (dst, src, x, y);
        }

        case FIT_COMPLEX:      // array of FICOMPLEX: 2 x 64-bit
        {
            break;
        }

        default:
        {
            break;
        }
    }

    FreeImage_OutputMessageProc (FIF_UNKNOWN,
          "Unable to perform GradientBlend on image type %d.", src_type);

    return FIA_ERROR;
}
