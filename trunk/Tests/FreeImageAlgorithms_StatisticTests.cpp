#include "CuTest.h"

#include "Constants.h"
#include "FreeImageAlgorithms.h"
#include "FreeImageAlgorithms_IO.h"
#include "FreeImageAlgorithms_Statistics.h"
#include "FreeImageAlgorithms_Utilities.h"
#include "FreeImageAlgorithms_Arithmetic.h"

#include "FreeImageAlgorithms_Testing.h"

#include <iostream>
#include <math.h>

static void TestFIA_HistogramTest(CuTest* tc)
{
//    const char *file= TEST_DATA_DIR "drone-bee-greyscale.jpg";
    const char *file= TEST_DATA_DIR "test.bmp";
    
    FIBITMAP *dib = FIA_LoadFIBFromFile(file);
    
    CuAssertTrue(tc, dib != NULL);
    
    unsigned long hist[256];
    
    PROFILE_START("FreeImageAlgorithms_Histogram");
    
//    if (FIA_Histogram(dib, 0, 255, 2, hist) == FIA_ERROR) {
    if (FIA_Histogram(dib, -10.999, 255.0, 256, hist) == FIA_ERROR) {
        CuFail(tc, "Failed");
    }
    
    PROFILE_STOP("FreeImageAlgorithms_Histogram");
    
    FreeImage_Unload(dib);
}

static void TestFIA_StatisticsTest(CuTest* tc)
{
	// To check in matlab use: I=imread('<filename>'); median(double(I(:))) or prctile(double(I(:)), [25 50 75])
//    const char *file= TEST_DATA_DIR "fibres_greyscale.jpg"; double median_target=118.0;  // Target checked in MATLAB LQ=85, median=118, UQ=151
    const char *file= TEST_DATA_DIR "drone-bee-greyscale.jpg"; double median_target=188.0;  // Target checked in MATLAB LQ=110, median=188, UQ=202
//    const char *file= TEST_DATA_DIR "test.bmp"; double median_target=0.0;  // Target checked in MATLAB LQ=0, median=0, UQ=168
	double median=0;
    
	FIBITMAP *mask=NULL;
    FIBITMAP *dib = FIA_LoadFIBFromFile(file);

    CuAssertTrue(tc, dib != NULL);
    
    StatisticReport report;
    
	memset(&report, 0, sizeof(StatisticReport));

	printf("%s\n", file);

	// run as 8 bit
	PROFILE_START("FIA_StatisticReport");
    
    if (FIA_StatisticReport(dib, &report) == FIA_ERROR) {
        CuFail(tc, "Failed");
    }
    
    PROFILE_STOP("FIA_StatisticReport");

	printf("area\t%d\nmin\t%f\nmax\t%f\nmean\t%f\nstdDev\t%f\nSkew\t%f\nKurt\t%f\n", report.area, report.minValue, report.maxValue, 
		report.mean, report.stdDeviation, report.skewness, report.kurtosis);


	// run median test
	PROFILE_START("FIA_GetMedianFromImage");
    
	median = FIA_GetMedianFromImage(dib);
    
    PROFILE_STOP("FIA_GetMedianFromImage");

	printf("\n");
	printf("median\t%f\n", median);

	if (median != median_target) {
        CuFail(tc, "Failed");
    }

	// run as float, with a mask
	mask = FIA_Threshold(dib, 250.0, 255.0, 1.0);
	FIA_InPlaceConvertToGreyscaleFloatType(&dib, FIT_FLOAT);
//	FIA_AddGreyLevelImageConstant(dib, 1000.0);

	memset(&report, 0, sizeof(StatisticReport));
    
    PROFILE_START("FIA_StatisticReportWithMask");
    
//    if (FIA_StatisticReport(dib, &report) == FIA_ERROR) {
    if (FIA_StatisticReportWithMask(dib, mask, &report) == FIA_ERROR) {
        CuFail(tc, "Failed");
    }
    
    PROFILE_STOP("FIA_StatisticReportWithMask");

	printf("\n");
	printf("area\t%d\nmin\t%f\nmax\t%f\nmean\t%f\nstdDev\t%f\nSkew\t%f\nKurt\t%f\n", report.area, report.minValue, report.maxValue, 
		report.mean, report.stdDeviation, report.skewness, report.kurtosis);
    
    FreeImage_Unload(dib);
}

static void TestFIA_CentroidTest(CuTest* tc)
{
    const char *file= TEST_DATA_DIR "drone-bee-greyscale.jpg";
    
    FIBITMAP *dib = FIA_LoadFIBFromFile(file);
    
    CuAssertTrue(tc, dib != NULL);
    
    PROFILE_START("FreeImageAlgorithms_StatisticReport");
    
    float x_centroid, y_centroid;
    
    if (FIA_Centroid(dib, &x_centroid, &y_centroid) == FIA_ERROR) {
        CuFail(tc, "Failed");
    }
    
    PROFILE_STOP("FreeImageAlgorithms_StatisticReport");
    
    FreeImage_Unload(dib);
}

/*
 static void
 TestFIA_MonoAreaTest(CuTest* tc)
 {
 double white_area, black_area;

 const char *file = TEST_DATA_DIR "drone-bee-greyscale.jpg";

 FIBITMAP *dib = FIA_LoadFIBFromFile(file);
 
 if(FIA_MonoImageFindWhiteFraction(dib, &white_area, &black_area) == FREEIMAGE_ALGORITHMS_ERROR)
 CuFail(tc, "Failed");

 FreeImage_Unload(dib);

 // float white_area = 0.540436;
 // white_area * 100 = 54.0436
 // 54.0436 + 0.5 = 54.5436
 // floor(54.5436) = 54
 // 54 / 100 = 0.54
 double x = floor(white_area*100+.05)/100;

 CuAssertTrue(tc, x == 0.54);
 CuAssertTrue(tc, white_area + black_area == 1.0);
 }

 static void
 TestFIA_MonoComparisonTest(CuTest* tc)
 {
 const char *file1 = IMAGE_DIR "\\texture.bmp";
 const char *file2 = IMAGE_DIR "\\mask.bmp";

 FIBITMAP *dib1 = FIA_LoadFIBFromFile(file1);
 FIBITMAP *dib2 = FIA_LoadFIBFromFile(file2);
 
 int tp, tn, fp, fn;

 int error = FIA_MonoTrueFalsePositiveComparison(dib1, dib2,
 &tp, &tn, &fp, &fn);

 if(error == FREEIMAGE_ALGORITHMS_ERROR)
 CuFail(tc, "Failed");

 FreeImage_Unload(dib1);
 FreeImage_Unload(dib2);

 //std::cout << "True Positive: " << tp
 //	<< "\nTrue Negative: " << tn
 //	<< "\nFalse Positive: " << fp
 //	<< "\nFalse Negative: " << fn << std::endl;
 
 CuAssertTrue(tc, tp == 35400);
 CuAssertTrue(tc, tn == 889);
 CuAssertTrue(tc, fp == 18);
 CuAssertTrue(tc, fn == 29229);
 }
 */

CuSuite* DLL_CALLCONV
CuGetFreeImageAlgorithmsStatisticSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    //SUITE_ADD_TEST(suite, TestFIA_MonoAreaTest);
    //SUITE_ADD_TEST(suite, TestFIA_MonoComparisonTest);
    //SUITE_ADD_TEST(suite, TestFIA_HistogramTest);
    SUITE_ADD_TEST(suite, TestFIA_StatisticsTest);
    //SUITE_ADD_TEST(suite, TestFIA_CentroidTest);

    return suite;
}
