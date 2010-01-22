using System;
using System.Drawing;
using System.Runtime.InteropServices;

namespace FreeImageAPI
{
    public enum FIA_BITDEPTH
    {
        BIT8,
        BIT16,
        BIT24,
        BIT32
    }

    [Serializable, StructLayout(LayoutKind.Sequential)]
	public struct FIA_Matrix : IEquatable<FIA_Matrix>
	{
        private IntPtr data;

        /// <summary>
        /// A read-only field that represents a handle that has been initialized to zero.
        /// </summary>
        public static readonly FIA_Matrix Zero;

        /// <returns>
		/// <b>true</b> if the two <see cref="FIA_Matrix"/> structures are equal; otherwise, <b>false</b>.
		/// </returns>
		public static bool operator ==(FIA_Matrix left, FIA_Matrix right)
		{
			return (left.data == right.data);
		}

        /// <summary>
		/// Tests whether two specified <see cref="FIA_Matrix"/> structures are different.
		/// </summary>
		/// <param name="left">The <see cref="FIA_Matrix"/> that is to the left of the inequality operator.</param>
		/// <param name="right">The <see cref="FIA_Matrix"/> that is to the right of the inequality operator.</param>
		/// <returns>
		/// <b>true</b> if the two <see cref="FIA_Matrix"/> structures are different; otherwise, <b>false</b>.
		/// </returns>
		public static bool operator !=(FIA_Matrix left, FIA_Matrix right)
		{
			return (left.data != right.data);
		}

        /// <summary>
		/// Converts the numeric value of the <see cref="FIA_Matrix"/> object
		/// to its equivalent string representation.
		/// </summary>
		/// <returns>The string representation of the value of this instance.</returns>
		public override string ToString()
		{
			return data.ToString();
		}

		/// <summary>
		/// Returns a hash code for this <see cref="FIA_Matrix"/> structure.
		/// </summary>
		/// <returns>An integer value that specifies the hash code for this <see cref="FIA_Matrix"/>.</returns>
		public override int GetHashCode()
		{
			return data.GetHashCode();
		}

        /// <summary>
        /// Determines whether the specified <see cref="Object"/> is equal to the current <see cref="Object"/>.
        /// </summary>
        /// <param name="obj">The <see cref="Object"/> to compare with the current <see cref="Object"/>.</param>
        /// <returns><b>true</b> if the specified <see cref="Object"/> is equal to the current <see cref="Object"/>; otherwise, <b>false</b>.</returns>
        public override bool Equals(object obj)
        {
            return ((obj is FIA_Matrix) && (this == ((FIA_Matrix)obj)));
        }

        /// <summary>
        /// Indicates whether the current object is equal to another object of the same type.
        /// </summary>
        /// <param name="other">An object to compare with this object.</param>
        /// <returns><b>true</b> if the current object is equal to the other parameter; otherwise, <b>false</b>.</returns>
        public bool Equals(FIA_Matrix other)
        {
            return (this == other);
        }
    }

    public enum FIA_MatrixOrder
    {
        MatrixOrderPrepend = 0,
        MatrixOrderAppend
    }

    public partial class FreeImage
    {
        private const string FreeImageAlgorithmsLibrary = "FreeImageAlgorithms.dll";

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_IsGreyScale")]	
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool IsGreyScale(FIBITMAP dib);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_LoadFIBFromFile")]
        internal static extern uint LoadFIBFromFile(string filepath);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_SaveFIBToFile")]
        internal static extern int SaveFIBToFile(FIBITMAP dib, string filepath, FIA_BITDEPTH bitDepth);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_HistEq")]
        internal static extern uint FreeImage_HistEq(FIBITMAP dib);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_GetDibSection")]
        internal static extern IntPtr GetDibSection(FIBITMAP dib, IntPtr hdc, int left, int top, int right, int bottom);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_FibToHBitmap")]
        internal static extern IntPtr ToHBitmap(FIBITMAP dib);  

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_HBitmapToFIB")]
        internal static extern uint HBitmapToFIB(IntPtr hdc, IntPtr hbitmap);  

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_FreeHBitmap")]
        internal static extern void FreeHBitmap(IntPtr hbitmap);  

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_SetGreyLevelPalette")]
        internal static extern int SetGreyLevelPalette(FIBITMAP dib);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_FindMinMax")]
        internal static extern void FindMinMax(FIBITMAP dib, out double min, out double max);
        
        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_ConvertInt16ToUInt16")]
        internal static extern FIBITMAP ConvertInt16ToUInt16(FIBITMAP dib);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_GetMaxPosibleValueForFib")]
        internal static extern void GetMaxPosibleValueForFib(FIBITMAP dib, out double max);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_GetMinPosibleValueForFib")]
        internal static extern void GetMinPosibleValueForFib(FIBITMAP dib, out double min);

        [DllImport("FreeImageAlgorithms_d.dll", EntryPoint="FIA_Histogram")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool Histogram(FIBITMAP dib, double min, double max, int number_of_bins,
            [In, Out] ulong[] values);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_LinearScaleToStandardType")]
        internal static extern FIBITMAP LinearScaleToStandardType(FIBITMAP dib, double min, double max,
            out double min_within_image, out double max_within_image);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_GetGreyScalePixelValuesForLine")]
        internal static extern int GetGreyScalePixelValuesForLine(FIBITMAP dib, FIAPOINT pt1, FIAPOINT pt2,
            out short[] values);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_GetGreyScalePixelValuesAsDoublesForLine")]
        internal static extern int GetGreyScalePixelValuesAsDoublesForLine(FIBITMAP dib,
            FIAPOINT pt1, FIAPOINT pt2, out double[] values);    

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_SimplePaste")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool SimplePaste(FIBITMAP dst, FIBITMAP src, int left, int bottom);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint="FIA_SimplePasteFromTopLeft")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool SimplePasteFromTopLeft(FIBITMAP dst, FIBITMAP src, int left, int top);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_StretchImageToType")]
        internal static extern FIBITMAP StretchImageToType(FIBITMAP src, FREE_IMAGE_TYPE type, double max);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_Is16BitReally12BitImage")]
        internal static extern bool Is16BitReally12BitImage(FIBITMAP src);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_CorrelateImageRegions")]
        internal static extern bool CorrelateImageRegions(FIBITMAP src1, FIARECT rect1, FIBITMAP src2, FIARECT rect2, out FIAPOINT pt, out double max);
    
        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_CorrelateImagesAlongRightEdge")]
        internal static extern bool CorrelateImagesAlongRightEdge(FIBITMAP src1, FIBITMAP src2, uint thickness, out FIAPOINT pt, out double max);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_CorrelateImagesAlongBottomEdge")]
        internal static extern bool CorrelateImagesAlongBottomEdge(FIBITMAP src1, FIBITMAP src2, uint thickness, out FIAPOINT pt, out double max);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_GetGradientBlendAlphaImage")]
        internal static extern FIBITMAP GetGradientBlendAlphaImage(FIBITMAP src2, FIARECT rect1, FIARECT rect2, out FIARECT intersect_rect);
    
        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_GradientBlendPasteFromTopLeft")]
        internal static extern bool GradientBlendPasteFromTopLeft(FIBITMAP dst, FIBITMAP src, int left, int top, FIBITMAP mask);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_GradientBlendedIntersectionImage")]
        internal static extern FIBITMAP GradientBlendedIntersectionImage(FIBITMAP src1, FIARECT rect1, FIBITMAP src2, FIARECT rect2, FIBITMAP mask, out FIARECT intersect_rect);
   
        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_DrawSolidGreyscaleRect")]
        internal static extern bool DrawSolidRectangle(FIBITMAP src, FIARECT rect, double value);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_DrawColourRect")]
        internal static extern bool DrawColourRect(FIBITMAP src, FIARECT rect,
            RGBQUAD colour, int line_width);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_MatrixNew")]
        internal static extern FIA_Matrix MatrixNew();

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_MatrixDestroy")]
        internal static extern bool MatrixDestroy(FIA_Matrix matrix);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_MatrixScale")]
        internal static extern bool MatrixScale(FIA_Matrix matrix, double x, double y, FIA_MatrixOrder order);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_MatrixRotate")]
        internal static extern bool MatrixRotate(FIA_Matrix matrix, double a, FIA_MatrixOrder order);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_MatrixTranslate")]
        internal static extern bool MatrixTranslate(FIA_Matrix matrix, double x, double y, FIA_MatrixOrder order);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_MatrixInvert")]
        internal static extern bool MatrixInvert(FIA_Matrix matrix);

        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_AffineTransform")]
        internal static extern FIBITMAP AffineTransform(FIBITMAP src, int image_dst_width, int image_dst_height, FIA_Matrix matrix, RGBQUAD colour);
   
        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_DrawImageFromSrcToDst")]
        internal static extern bool DrawImageFromSrcToDst(FIBITMAP dst, FIBITMAP src, FIA_Matrix matrix,
            int dstLeft, int dstTop, int dstWidth, int dstHeight,
            int srcLeft, int srcTop, int srcWidth, int srcHeight, RGBQUAD colour);
   
        [DllImport(FreeImageAlgorithmsLibrary, EntryPoint = "FIA_DrawImageToDst")]
        internal static extern bool DrawImageToDst(FIBITMAP dst, FIBITMAP src, FIA_Matrix matrix,
            int dstLeft, int dstTop, int dstWidth, int dstHeight, RGBQUAD colour);            
    }
}
