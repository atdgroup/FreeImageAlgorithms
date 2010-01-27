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

static inline int RoundRealToNearestInteger(float value)
{
  return (int) (value + 0.5f);
}

FIBITMAP* DLL_CALLCONV
FIA_GradientBlendMosaicPaste (FIBITMAP* dst, FIBITMAP* src, int x, int y)
{
	#define ROOT2 1.4142
	int i;
	float currMin;
	int xc=0, yc=0, x1=0, y1=0; 
	FIBITMAP *maskA=NULL, *maskB=NULL, *maskAT=NULL, *maskATI=NULL, *im2=NULL, *im4=NULL, *im5=NULL;
	//IPIImageRef maskA=0, maskB=0, maskAT=0, maskATI=0, im2=0, im4=0, im5=0;
	int err;
	//IPIImageInfo info, maskAinfo, maskATinfo, im2info, im4info, im5info;
	double val;
	BYTE *pCentre=NULL, *pLeft=NULL, *pTop=NULL, *pTopLeft=NULL, *pTopRight=NULL;
	float *pCentreFM=NULL, *pLeftFM=NULL, *pTopFM=NULL, *pTopLeftFM=NULL, *pTopRightFM=NULL;
	BYTE *pRight=NULL, *pBottom=NULL, *pBottomLeft=NULL, *pBottomRight=NULL, *pCentreF;
	float *pCentreBM=NULL, *pRightBM=NULL, *pBottomBM=NULL, *pBottomLeftBM=NULL, *pBottomRightBM=NULL;
	BYTE *pOutImage=NULL, *pmaskA=NULL, *pim2=NULL;
	int X, Y, maxWidthHeight;
	float   *pMatrix, LT, BLTL, RB, BRTR, *pDistMapEdges;
	static float *distMapEdges=NULL;
	static int distMapWidth=-1, distMapHeight=-1;
	
	bool greyscale_image = true;

    if(FreeImage_GetImageType(src) == FIT_BITMAP && FreeImage_GetBPP(src) > 8) {
	    greyscale_image = false;
    }
    
	PROFILE_START("FIA_GradientBlendMosaicPaste");
	
		//imWidth, imHeight
		int imWidth = FreeImage_GetWidth(src);
		int imHeight = FreeImage_GetHeight(src);
		
	PROFILE_START("FIA_GradientBlendMosaicPaste - Cloning");
	
	/*
		maskA = FIA_CloneImageType(src, imWidth, imHeight);
		maskB = FIA_CloneImageType(src, imWidth, imHeight);
		maskAT = FIA_CloneImageType(src, imWidth, imHeight);
		maskATI = FIA_CloneImageType(src, imWidth, imHeight);
		im2 = FIA_CloneImageType(src, imWidth, imHeight);
		im4 = FIA_CloneImageType(src, imWidth, imHeight);
	*/
		im5 = FIA_CloneImageType(src, imWidth, imHeight);
	
		
	PROFILE_STOP("FIA_GradientBlendMosaicPaste - Cloning");
		
		PROFILE_START("FIA_GradientBlendMosaicPaste - Masks");
		
//		IPI_Extract (dst, maskA, 1, 1, MakeRect (y, x, imHeight, imWidth));
		maskA = FIA_Copy(dst, x, y, x+imWidth-1, y+imHeight-1);   // use -1 for FIA definition of bottom and right
		
		#ifdef GENERATE_DEBUG_IMAGES
		FIA_SaveFIBToFile(maskA, DEBUG_DATA_DIR "maskA.bmp", BIT24);
		#endif
		 
//		IPI_Copy (maskA, maskB);
//		IPI_Cast (maskB, IPI_PIXEL_U8);
		//maskB = FIA_Copy(dst, x, y, x+imWidth-1, y+imHeight-1);   // use -1 for FIA definition of bottom and right
		maskB = FreeImage_Clone(maskA);	
		FIA_InPlaceConvertTo8Bit(&maskB);
		
		//IPI_Threshold (maskB, maskAT, 1.0, 255.0, 1.0, 1);
		maskAT = FIA_Threshold(maskB, 1.0, 255.0, 1.0);	
		
		#ifdef GENERATE_DEBUG_IMAGES
	    FIA_SaveFIBToFile(maskAT, DEBUG_DATA_DIR "maskAT.bmp", BIT24);
		#endif
			
		//IPI_Mask (src, maskAT, im2);
		im2 = FreeImage_Clone(src);
		
		#ifdef GENERATE_DEBUG_IMAGES
		FIA_SaveFIBToFile(im2, DEBUG_DATA_DIR "im2-before_mask.bmp", BIT24);
        #endif	
        
		FIA_MaskImage(im2, maskAT);
		
		#ifdef GENERATE_DEBUG_IMAGES
		FIA_SaveFIBToFile(im2, DEBUG_DATA_DIR "im2.bmp", BIT24);
        #endif	
		
		//IPI_Threshold (maskAT, maskATI, 0.0, 0.0, 1.0, 1);
		// Inverts black / white
		//maskATI = FIA_Threshold(maskAT, 0.0, 0.0, 1.0);	
		
		maskATI = FreeImage_Clone(maskAT);
		FIA_ReverseMaskImage (maskATI, 1);
		
		//FreeImage_Invert(maskATI);
		
		#ifdef GENERATE_DEBUG_IMAGES
		FIA_SaveFIBToFile(maskATI, DEBUG_DATA_DIR "maskAT-reversed.bmp", BIT24);
        #endif	
		
		//IPI_Mask (src, maskATI, im4);
		im4 = FreeImage_Clone(src);
        FIA_MaskImage(im4, maskATI);
        
        #ifdef GENERATE_DEBUG_IMAGES
        FIA_SaveFIBToFile(im4, DEBUG_DATA_DIR "im4.bmp", BIT24);
	    #endif
	
	PROFILE_STOP("FIA_GradientBlendMosaicPaste - Masks");
	
	xc= RoundRealToNearestInteger((float)imWidth/2.0f);
	yc= RoundRealToNearestInteger((float)imHeight/2.0f);
	
	PROFILE_START("FIA_GradientBlendMosaicPaste - DistanceMap");
	
	if (distMapEdges == NULL || distMapWidth != imWidth || distMapHeight != imHeight)
	{
		if (distMapEdges) free(distMapEdges);
		distMapEdges = (float*) malloc(imHeight*imWidth*sizeof(float));
		for (Y = 0; Y< imHeight; Y++)
		{
			for (X = 0; X< imWidth; X++)
			{
				if (X<=xc)
					currMin = (float) X;
				else
					currMin = (float)(imWidth - X);
				if (Y<=yc)
					distMapEdges[Y*imWidth+X]= (float) min(currMin,Y);
				else
					distMapEdges[Y*imWidth+X]= (float) min(currMin,imHeight - Y);
			}
		}

		distMapHeight = imHeight;
		distMapWidth = imWidth;
	}
		
	PROFILE_STOP("FIA_GradientBlendMosaicPaste - DistanceMap");
		
	PROFILE_START("FIA_GradientBlendMosaicPaste - PMap");
		
	// Set up the matrix where the 'alpha' values will be stored
	pMatrix = (float*) malloc(sizeof(float)*imHeight*imWidth);
	maxWidthHeight = max((imHeight+1),(imWidth+1));

	// Initialise with 'high' values
	for (i=0; i<imHeight*imWidth; i++)
		pMatrix[i] = (float) maxWidthHeight;
			
	for (Y = 1; Y< imHeight; Y++)
	{
	    
		//pCentre =       maskATinfo.firstPixelAddress.Pix8_Ptr+Y*maskATinfo.rawPixels+1;			
		pCentre = FIA_GetScanLineFromTop(maskAT, Y) + 1;
			
		pLeft = 		pCentre -1;
		//pTop = 			pCentre - maskATinfo.rawPixels; 	
		pTop =          pCentre + FreeImage_GetPitch(maskAT);  // underlying FreeImage is upside down
		
		pTopLeft = 		pTop-1;
		pTopRight =		pTop+1;
		
		pCentreFM =     &pMatrix[Y*imWidth+1];		
		pLeftFM = 		pCentreFM -1;
		pTopFM = 		pCentreFM - imWidth;
		pTopLeftFM = 	pTopFM-1;
		pTopRightFM =	pTopFM+1;
		
		for (X = 1; X< imWidth-1; X++)
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
						LT = min(*pLeftFM+1, *pTopFM+1);
						BLTL = min(*pTopRightFM+ROOT2, *pTopLeftFM+ROOT2);
						*pCentreFM = min(LT, BLTL);
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
	
	#ifdef GENERATE_DEBUG_IMAGES
	FIBITMAP *pMap2 = FIA_LoadGreyScaleFIBFromArrayData ((BYTE*)pMatrix, 32, imWidth, imHeight,
                                   FIT_FLOAT, 0, 0);
	
	FIA_SaveFIBToFile(pMap2, DEBUG_DATA_DIR "pmap_inter.bmp", BIT8);
	#endif
	
	for (Y = imHeight-2; Y >= 0; Y--)
	{
		//pCentre =       maskATinfo.firstPixelAddress.Pix8_Ptr+Y*maskATinfo.rawPixels+maskATimWidth-2;
		pCentre = FIA_GetScanLineFromTop(maskAT, Y) + FreeImage_GetWidth(maskAT) - 2;
				
		pRight = 		pCentre +1;
		//pBottom = 		pCentre+info.rawPixels;
		pBottom = 		pCentre - FreeImage_GetPitch(maskAT);   // underlying FreeImage is upside down
		pBottomRight =	pBottom+1;
		pBottomLeft =	pBottom-1;
		
		pCentreBM =     &pMatrix[Y*imWidth+imWidth-2];		
		pRightBM = 		pCentreBM +1;
		pBottomBM = 	pCentreBM+imWidth; 
		pBottomRightBM= pBottomBM+1;
		pBottomLeftBM =	pBottomBM-1;
		
		for (X = imWidth-2; X > 0; X--)
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
						RB = min(*pRightBM+1, *pBottomBM+1);
						BRTR = min(*pBottomRightBM+ROOT2, *pBottomLeftBM+ROOT2 );
						currMin = min(RB, BRTR);
						*pCentreBM = min(currMin, *pCentreBM);
					}
				}
			
		pCentre	--;	
		pRight  --;
		pBottom --;
		pBottomRight --;
		pBottomLeft --;
		
		pCentreBM --;		
		pRightBM  --;
		pBottomBM  --;
		pBottomLeftBM --;
		pBottomRightBM --;
			
		}				  
	}
	
	PROFILE_STOP("FIA_GradientBlendMosaicPaste - PMap");
	
	#ifdef GENERATE_DEBUG_IMAGES
	FIBITMAP *pMap = FIA_LoadGreyScaleFIBFromArrayData ((BYTE*)pMatrix, 32, imWidth, imHeight,
                                   FIT_FLOAT, 0, 0);
	
	FIA_SaveFIBToFile(pMap, DEBUG_DATA_DIR "pmap.bmp", BIT8);
	
	
	FIBITMAP *pDist = FIA_LoadGreyScaleFIBFromArrayData ((BYTE*)distMapEdges, 32, imWidth, imHeight,
                                   FIT_FLOAT, 0, 0);
	
	FIA_SaveFIBToFile(pDist, DEBUG_DATA_DIR "pdist.bmp", BIT8);
	#endif
	
	pCentreFM = pMatrix;
	pDistMapEdges=distMapEdges;
	
	PROFILE_START("FIA_GradientBlendMosaicPaste - Blend");
	
	int bytespp = FreeImage_GetLine (src) / FreeImage_GetWidth (src);
	
	#ifdef GENERATE_DEBUG_IMAGES
	FIA_SaveFIBToFile(dst, DEBUG_DATA_DIR "dst.bmp", BIT24);
	#endif
    
    //float* alpha_values = (float*) malloc(sizeof(float)*imHeight*imWidth);
    //int index=0;
    
    if(  greyscale_image) {
      
	  for (y1=0;y1<imHeight;y1++) 
	  {
          pCentreF = FIA_GetScanLineFromTop(maskAT, y1);
		  pOutImage = FIA_GetScanLineFromTop(im5, y1);
		  pmaskA = FIA_GetScanLineFromTop(maskA, y1);
		  pim2 = FIA_GetScanLineFromTop(im2, y1);
  				
		  for (x1=0;x1<imWidth;x1++) 
		  {
		      if (*pCentreF==1)
		      {   
		          val = *pCentreFM / (*pCentreFM+*pDistMapEdges);
		    	  //alpha_values[index++] = 255.0*val;
                  
				  *pOutImage	= RoundRealToNearestInteger ((*pmaskA * val) + (*pim2 * (1-val)));
  		  		
			  }
			  else {
				  *pOutImage=0;
				  //alpha_values[index++] = 0.0;
			  }
  			
			  pCentreF++;
			  pCentreFM++;
  			
			  pOutImage++;
  			
			  pmaskA++;
			  pim2++;
			  pDistMapEdges++;

		   }
	  }
	}
	else {
	
	  int bytespp = FreeImage_GetLine (src) / FreeImage_GetWidth (src);
	  
	  for (y1=0;y1<imHeight;y1++) 
	  {
           pCentreF = FIA_GetScanLineFromTop(maskAT, y1);
		   pOutImage = FIA_GetScanLineFromTop(im5, y1);
		   pmaskA = FIA_GetScanLineFromTop(maskA, y1);
		  pim2 = FIA_GetScanLineFromTop(im2, y1);
  				
		  for (x1=0;x1<imWidth;x1++) 
		  {
		      if (*pCentreF==1)
		      {
		    	  val = *pCentreFM / (*pCentreFM+*pDistMapEdges);
                  
				  pOutImage[FI_RGBA_RED] = RoundRealToNearestInteger ((pmaskA[FI_RGBA_RED] * val) + (pim2[FI_RGBA_RED] * (1-val)));
				  pOutImage[FI_RGBA_GREEN] = RoundRealToNearestInteger ((pmaskA[FI_RGBA_GREEN] * val) + (pim2[FI_RGBA_GREEN] * (1-val)));
				  pOutImage[FI_RGBA_BLUE] = RoundRealToNearestInteger ((pmaskA[FI_RGBA_BLUE] * val) + (pim2[FI_RGBA_BLUE] * (1-val)));	  		
			  }
			  else {
				  pOutImage[FI_RGBA_RED]	= 0;
				  pOutImage[FI_RGBA_GREEN]	= 0;
				  pOutImage[FI_RGBA_BLUE]	= 0;
				  
  			  }
  			
  			  pOutImage += bytespp;
  			  pmaskA += bytespp;
  			  pim2 += bytespp;
  			  
			  pCentreF++;
			  pCentreFM++;

			  pDistMapEdges++;

		   }
	  }
	}
	
	#ifdef GENERATE_DEBUG_IMAGES
	FIBITMAP *alpha_values_im = FIA_LoadGreyScaleFIBFromArrayData ((BYTE*)alpha_values, 32, imWidth, imHeight,
                                   FIT_FLOAT, 0, 0);
	
	FIA_SaveFIBToFile(alpha_values_im, DEBUG_DATA_DIR "alpha_values.bmp", BIT8);
    #endif

	FIA_Combine(im5, im4, maskATI);
	
	PROFILE_STOP("FIA_GradientBlendMosaicPaste - Blend");
	
	#ifdef GENERATE_DEBUG_IMAGES
	FIA_SaveFIBToFile(im5, DEBUG_DATA_DIR "im5.bmp", BIT24);
	#endif
	
	//err = IPI_Copy (im4, imBlended); if (err) return err; 
	FIBITMAP *blended = FreeImage_ConvertToStandardType(im5);

	FreeImage_Unload(maskA);
	FreeImage_Unload(maskB);
	FreeImage_Unload(maskAT);
	FreeImage_Unload(maskATI);
	FreeImage_Unload(im2);
	FreeImage_Unload(im4);
	FreeImage_Unload(im5);

	free(pMatrix);
	
	PROFILE_STOP("FIA_GradientBlendMosaicPaste");
	
	return blended;
}