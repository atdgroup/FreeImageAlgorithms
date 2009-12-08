using System;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace FreeImageAPI
{
    public class FreeImageException : ApplicationException
    {
        public FreeImageException(string message)
            : base(message)
        {
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct FIARECT
    {
        public int left;
        public int top;
        public int right;
        public int bottom;

        public FIARECT(int left, int top, int right, int bottom)
        {
            this.left = left;
            this.top = top;
            this.right = right;
            this.bottom = bottom;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct FIAPOINT
    {
        public int x;
        public int y;

        public FIAPOINT(int x, int y)
        {
            this.x = x;
            this.y = y;
        }    
    }

    public class FreeImageAlgorithmsBitmap : FreeImageBitmap
    {
        // We need this to keep a ref so the are not garbage collected.
        // As they are passed to unmanaged code.
        private static OutputMessageFunction errorCallback;
        private bool isThumbnail;

        /*
        static FreeImageAlgorithmsBitmap()
        {
            FreeImageBitmap.errorCallback = FreeImageBitmap.FreeImageErrorOccurred;   
            FreeImage.SetOutputMessage(FreeImageBitmap.errorCallback);
        }
        */

        public FreeImageAlgorithmsBitmap() : base () {}

        public FreeImageAlgorithmsBitmap(FreeImageAlgorithmsBitmap fib) : base(fib) { }

        public FreeImageAlgorithmsBitmap(string filename) : base(filename) { }

        public FreeImageAlgorithmsBitmap(FIBITMAP dib) : base(dib) { }

        public FreeImageAlgorithmsBitmap(int width, int height, PixelFormat pixelFormat)
            : base(width, height, pixelFormat) {}

        public FreeImageAlgorithmsBitmap(Bitmap bitmap) : base(bitmap) {}

        public FreeImageAlgorithmsBitmap(Stream stream) : base(stream) { }

        public FreeImageAlgorithmsBitmap(Stream stream, FREE_IMAGE_FORMAT format) : base(stream, format) { }

        public FreeImageAlgorithmsBitmap(int width, int height, int colorDepth)
        {
            FIBITMAP dib = FreeImage.Allocate(width, height, colorDepth,
                FreeImage.FI_RGBA_RED_MASK,
                FreeImage.FI_RGBA_GREEN_MASK,
                FreeImage.FI_RGBA_BLUE_MASK);

            if (dib.IsNull)
            {
                throw new Exception("Unable to create bitmap.");
            }

            this.ReplaceDib(dib);
        }

        public FreeImageAlgorithmsBitmap(int width, int height, FREE_IMAGE_TYPE type, int colorDepth)
        {
            FIBITMAP dib = FreeImage.AllocateT(type, width, height, colorDepth,
                FreeImage.FI_RGBA_RED_MASK,
                FreeImage.FI_RGBA_GREEN_MASK,
                FreeImage.FI_RGBA_BLUE_MASK);

            if (dib.IsNull)
            {
                throw new Exception("Unable to create bitmap.");
            }

            this.ReplaceDib(dib);
        }

        private static void FreeImageErrorOccurred(FREE_IMAGE_FORMAT format, string msg)
        {
            throw new FreeImageException(msg);
        }

        public void SaveToFile(string filePath)
        {
            if(this.IsGreyScale)
                FreeImage.SaveFIBToFile(this.Dib, filePath, FIA_BITDEPTH.BIT8);
            else
                FreeImage.SaveFIBToFile(this.Dib, filePath, FIA_BITDEPTH.BIT24);
        }

        public void SaveToFile(string filePath, FIA_BITDEPTH depth)
        {
            FreeImage.SaveFIBToFile(this.Dib, filePath, depth);
        }

        ~FreeImageAlgorithmsBitmap()
        {
            // call Dispose with false.  Since we're in the
            // destructor call, the managed resources will be
            // disposed of anyways.
            Dispose(false);	
        }

        /*
        protected virtual void Dispose( bool disposing )
        {
            if (this.IsThumbnail)
            {
                Console.WriteLine("Here");
            }

            base.Dispose(disposing);
        }

        public void Dispose()
        {
            Dispose(true);

            // Tell the GC that the Finalize process no longer needs
            // to be run for this object.
            GC.SuppressFinalize(this);
        }
        */

        public bool IsGreyScale
        {
            get
            {
                if(FreeImage.IsGreyScale(this.Dib))
                    return true;

                return false;
            }
        }

        public void ConvertToStandardType()
        {
            FIBITMAP tmp_dib = FreeImage.ConvertToStandardType(this.Dib, true);

            this.ReplaceDib(tmp_dib);
        }
 
         public void ConvertToType(FREE_IMAGE_TYPE type)
         {
             FIBITMAP tmp_dib = FreeImage.ConvertToType(this.Dib, type, true);

             this.ReplaceDib(tmp_dib);
        }

        public void ConvertToGreyscale()
        {
            FIBITMAP tmp_dib = FreeImage.ConvertToGreyscale(this.Dib);

            this.ReplaceDib(tmp_dib);
        }

        public void VerticalFlip()
        {
            FreeImage.FlipVertical(this.Dib);
        }

        public void ConvertInt16ToUInt16()
        {
            FIBITMAP tmp_dib = FreeImage.ConvertInt16ToUInt16(this.Dib);

            this.ReplaceDib(tmp_dib);
        }
        

        public void LinearScaleToStandardType(double min, double max)
        {
            double minFound, maxFound;

            FIBITMAP tmp_dib = FreeImage.LinearScaleToStandardType(this.Dib, min, max,
                out minFound, out maxFound);

            ReplaceDib(tmp_dib);
        }

        public void ConvertTo8Bits()
        {
            FIBITMAP tmp_dib = FreeImage.ConvertTo8Bits(this.Dib);

            this.ReplaceDib(tmp_dib);
        }

        public void ConvertTo24Bits()
        {
            FIBITMAP tmp_dib = FreeImage.ConvertTo24Bits(this.Dib);

            this.ReplaceDib(tmp_dib);
        }
        
        public void StretchImageToType(FREE_IMAGE_TYPE type, double max)
        {
            FIBITMAP tmp_dib = FreeImage.StretchImageToType(this.Dib, type, max);

            this.ReplaceDib(tmp_dib);
        }

        public void Threshold(byte value)
        {
            FIBITMAP tmp_dib = FreeImage.Threshold(this.Dib, value);

            this.ReplaceDib(tmp_dib);
        }

        public void FindMinMaxIntensity(out double min, out double max)
        { 
            FreeImage.FindMinMax(this.Dib, out min, out max);
        }

        public int MemorySizeInBytes
        {
            get
            {
                return (this.ColorDepth * this.Pitch * this.Height) / 8;
            }
        }

        public void SetGreyLevelPalette()
        {
            FreeImage.SetGreyLevelPalette(this.Dib);
        }

        public double MaximumPossibleIntensityValue
        {
            get
            {
                double max = 0.0;

                FreeImage.GetMaxPosibleValueForFib(this.Dib, out max);

                return max;
            }
        }

        public bool GetGreyLevelHistogram(int number_of_bins, out ulong[] hist)
        {
            double max = this.MaximumPossibleIntensityValue;
  
            hist = new ulong[number_of_bins];

            return FreeImage.Histogram(this.Dib, 0, max, number_of_bins, hist);
        }

        /*
        public bool GetGreyLevelHistogram(int number_of_bins, out ulong[] hist, out double range_per_bin)
        {            
            double max = 0.0;

            FreeImage.GetMaxPosibleValueForFib(this.Dib, out max);

            FreeImageType type = FreeImage.GetImageType(this.Dib);
	
	        bool isFloat = (type == FreeImageType.Float || type == FreeImageType.Double);

            int number_of_bins = (int) Math.Ceiling(max) + 1;

            range_per_bin = 1.0;
	        if(isFloat || number_of_bins > 255)
	        {
		        number_of_bins = 256;
		        range_per_bin = max / 256.0;   
	        }
	        
            double max = this.MaximumPossibleIntensityValue;
            range_per_bin = max / 256.0;

            hist = new ulong[number_of_bins];

            return FreeImage.Histogram(this.Dib, 0, max, number_of_bins, hist);
        }
        */
        
        /*
        public bool AdjustGamma(double gamma)
        {
            return FreeImage.AdjustGamma(this.Dib, gamma);
        }

        public bool AdjustBrightness(double brightness)
        {
            return FreeImage.AdjustBrightness(this.Dib, brightness);
        }

        public bool AdjustContrast(double contrast)
        {
            return FreeImage.AdjustContrast(this.Dib, contrast);
        }

        public bool Invert()
        {
            return FreeImage.Invert(this.Dib);
        }

        public bool Paste(FreeImageBitmap src, int left, int top, int alpha)
        {
            return FreeImage.Paste(this.Dib, src.dib, left, top, alpha);
        }

        public bool Paste(FreeImageBitmap src, Point location, int alpha)
        {
            return this.Paste(src, location.X, location.Y, alpha);
        }
        */

        public new FreeImageAlgorithmsBitmap Copy(int left, int top, int right, int bottom)
        {
            EnsureNotDisposed();
            FreeImageAlgorithmsBitmap result = null;
            FIBITMAP newDib = FreeImage.Copy(this.Dib, left, top, right, bottom);
            if (!newDib.IsNull)
            {
                result = new FreeImageAlgorithmsBitmap(newDib);
            }
            return result;
        }

        public bool SimplePaste(FreeImageBitmap src, Point location)
        {
            return FreeImage.SimplePaste(this.Dib, src.Dib, location.X, location.Y);
        }

        public bool SimplePasteFromTopLeft(FreeImageBitmap src, Point location)
        {
            return FreeImage.SimplePasteFromTopLeft(this.Dib, src.Dib, location.X, location.Y);
        }

        /*
        public void EqualizeHistogram()
        {
            uint tmp_dib = FreeImage.FreeImage_HistEq(this.Dib);

            FreeImage.Unload(this.Dib);

            this.Dib = tmp_dib;
            
        }
        */

        public FIAPOINT Correlate(FIARECT rect1, FreeImageBitmap src2, FIARECT rect2, out double max)
        {
            FIAPOINT pt = new FIAPOINT();

            FreeImage.CorrelateImageRegions(this.Dib, rect1,
                src2.Dib, rect2, out pt, out max);

            return pt;
        }

        public FIAPOINT CorrelateImageAlongRightEdge(FreeImageBitmap src2, uint thickness, out double max)
        {
            FIAPOINT pt = new FIAPOINT();

            FreeImage.CorrelateImagesAlongRightEdge(this.Dib,
                src2.Dib, thickness, out pt, out max);

            return pt;
        }

        public FIAPOINT CorrelateImageAlongBottomEdge(FreeImageBitmap src2, uint thickness, out double max)
        {
            FIAPOINT pt = new FIAPOINT();

            FreeImage.CorrelateImagesAlongBottomEdge(this.Dib,
                src2.Dib, thickness, out pt, out max);

            return pt;
        }
    }
}
