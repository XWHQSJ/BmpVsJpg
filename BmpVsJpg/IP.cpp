#include "stdafx.h"
#include <math.h>

#include "dibapi.h"

// Definitions required for convolution image filtering
#define KERNELCOLS 3
#define KERNELROWS 3
#define KERNELELEMENTS (KERNELCOLS * KERNELROWS)

// struct for convolute kernel 
typedef struct 
{
  int Element[KERNELELEMENTS];
  int Divisor;
} KERNEL;

// The following kernel definitions are for convolution filtering.
// Kernel entries are specified with a divisor to get around the
// requirement for floating point numbers in the low pass filters. 

KERNEL HP1 = {                    // HP filter #1
  {-1, -1, -1,
   -1,  9, -1,
   -1, -1, -1},
    1                             // Divisor = 1
};

KERNEL HP2 = {                    // HP filter #2
  { 0, -1,  0,
   -1,  5, -1,
    0, -1,  0},
    1                             // Divisor = 1
};

KERNEL HP3 = {                    // HP filter #3
  { 1, -2,  1,
   -2,  5, -2,
    1, -2,  1},
    1                             // Divisor = 1
};

KERNEL LP1 = {                    // LP filter #1
  { 1,  1,  1,
    1,  1,  1,
    1,  1,  1},
    9                             // Divisor = 9
};

KERNEL LP2 = {                    // LP filter #2
  { 1,  1,  1,
    1,  2,  1,
    1,  1,  1},
    10                            // Divisor = 10
};

KERNEL LP3 = {                    // LP filter #3
  { 1,  2,  1,
    2,  4,  2,
    1,  2,  1},
    16                            // Divisor = 16
};

KERNEL VertEdge = {              // Vertical edge
  { 0,  0,  0,
    -1, 1,  0,
    0,  0,  0},
    1                             // Divisor = 1
};

KERNEL HorzEdge = {              // Horizontal edge
  { 0,  -1,  0,
    0,  1,  0,
    0,  0,  0},
    1                             // Divisor = 1
};

KERNEL VertHorzEdge = {           // Vertical Horizontal edge
  { -1, 0,  0,
    0,  1,  0,
    0,  0,  0},
    1                             // Divisor = 1
};

KERNEL EdgeNorth = {              // North gradient
  { 1,  1,  1,
    1, -2,  1,
   -1, -1, -1},
    1                             // Divisor = 1
};

KERNEL EdgeNorthEast = {          // North East gradient
  { 1,  1,  1,
   -1, -2,  1,
   -1, -1,  1},
    1                             // Divisor = 1
};

KERNEL EdgeEast = {               // East gradient
  {-1,  1,  1,
   -1, -2,  1,
   -1,  1,  1},
    1                             // Divisor = 1
};

KERNEL EdgeSouthEast = {          // South East gradient
  {-1, -1,  1,
   -1, -2,  1,
    1,  1,  1},
    1                             // Divisor = 1
};

KERNEL EdgeSouth = {              // South gadient
  {-1, -1, -1,
    1, -2,  1,
    1,  1,  1},
    1                             // Divisor = 1
};

KERNEL EdgeSouthWest = {          // South West gradient
  { 1, -1, -1,
    1, -2, -1,
    1,  1,  1},
    1                             // Divisor = 1
};

KERNEL EdgeWest = {               // West gradient
  { 1,  1, -1,
    1, -2, -1,
    1,  1, -1},
    1                             // Divisor = 1
};

KERNEL EdgeNorthWest = {          // North West gradient
  { 1,  1,  1,
    1, -2, -1,
    1, -1, -1},
    1                             // Divisor = 1
};

KERNEL Lap1 = {					  // Laplace filter 1
  { 0,  1,  0,
    1, -4,  1,
    0,  1,  0},
    1                             // Divisor = 1
};

KERNEL Lap2 = {					  // Laplace filter 2
  { -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1},
    1                             // Divisor = 1
};

KERNEL Lap3 = {					  // Laplace filter 3
  { -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1},
    1                             // Divisor = 1
};

KERNEL Lap4 = {					  // Laplace filter 4
  { 1, -2, 1,
    -2, 4, -2,
    1, -2, 1},
    1                             // Divisor = 1
};

KERNEL Sobel[4] = {
	{                    // Sobel1
		{-1, 0, 1,
		 -2, 0, 2,
		 -1, 0, 1},
		1                             // Divisor = 1
	},
	{                    // Sobel2
		{-1, -2, -1,
		  0,  0,  0,
		  1,  2,  1},
		1                             // Divisor = 1
	},
	{                    // Sobel3
		{-2, -1, 0,
		 -1,  0, 1,
		  0,  1, 2},
		1                             // Divisor = 1
	},
	{                    // Sobel4
		{0, -1, -2,
		 1,  0, -1,
		 2,  1, 0},
		1                             // Divisor = 1
	}
};

KERNEL Hough[4] = {
	{                    // Hough1
		{-1, 0, 1,
		 -1, 0, 1,
		 -1, 0, 1},
		1                             // Divisor = 1
	},
	{                    // Hough2
		{-1, -1, 0,
		 -1,  0, 1,
		  0,  1, 1},
		1                             // Divisor = 1
	},
	{                    // Hough3
		{-1, -1, -1,
		  0,  0, 0,
		  1,  1, 1},
		1                             // Divisor = 1
	},
	{                    // Hough4
		{0, -1, -1,
		 1,  0, -1,
		 1,  1, 0},
		1                             // Divisor = 1
	}
};

// local use macro
#define PIXEL_OFFSET(i, j, nWidthBytes)	\
		(LONG)((LONG)(i)*(LONG)(nWidthBytes) + (LONG)(j)*3)

// local function prototype
int compare(const void *e1, const void *e2);
void DoMedianFilterDIB(int *red, int *green, int *blue, int i, int j, 
			WORD wBytesPerLine, LPBYTE lpDIBits);
void DoConvoluteDIB(int *red, int *green, int *blue, int i, int j, 
			WORD wBytesPerLine, LPBYTE lpDIBits, KERNEL *lpKernel);
BOOL ConvoluteDIB(HDIB hDib, KERNEL *lpKernel, int Strength, int nKernelNum=1);


// function body

/************************************************************************* 
 * 
 * HighPassDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * int nAlgorithm   - specify the filter to use
 * int Strength     - operation strength set to the convolute
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * High pass filtering to sharp DIB
 * 
 ************************************************************************/ 
BOOL HighPassDIB(HDIB hDib, int Strength, int nAlgorithm) 
{
	switch (nAlgorithm)
	{
	case FILTER1:
		return ConvoluteDIB(hDib, &HP1, Strength);
	case FILTER2:
		return ConvoluteDIB(hDib, &HP2, Strength);
	case FILTER3:
		return ConvoluteDIB(hDib, &HP3, Strength);
	}

	return FALSE;
}

/************************************************************************* 
 * 
 * LowPassDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * int nAlgorithm   - specify the filter to use
 * int Strength     - operation strength set to the convolute
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * Low pass filtering to blur DIB
 * 
 ************************************************************************/ 
BOOL LowPassDIB(HDIB hDib, int Strength, int nAlgorithm) 
{
	switch (nAlgorithm)
	{
	case FILTER1:
		return ConvoluteDIB(hDib, &LP1, Strength);
	case FILTER2:
		return ConvoluteDIB(hDib, &LP2, Strength);
	case FILTER3:
		return ConvoluteDIB(hDib, &LP3, Strength);
	}

	return FALSE;
}

/************************************************************************* 
 * 
 * EdgeEnhanceDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * int nAlgorithm   - specify the filter to use
 * int Strength     - operation strength set to the convolute
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * Edge enhance DIB
 * 
 ************************************************************************/ 
BOOL EdgeEnhanceDIB(HDIB hDib, int Strength, int nAlgorithm)
{
	switch (nAlgorithm)
	{
	case VERT:
		return ConvoluteDIB(hDib, &VertEdge, Strength);
	case HORZ:
		return ConvoluteDIB(hDib, &HorzEdge, Strength);
	case VERTHORZ:
		return ConvoluteDIB(hDib, &VertHorzEdge, Strength);
	case NORTH:
		return ConvoluteDIB(hDib, &EdgeNorth, Strength);
	case NORTHEAST:
		return ConvoluteDIB(hDib, &EdgeNorthEast, Strength);
	case EAST:
		return ConvoluteDIB(hDib, &EdgeEast, Strength);
	case SOUTH:
		return ConvoluteDIB(hDib, &EdgeSouth, Strength);
	case SOUTHEAST:
		return ConvoluteDIB(hDib, &EdgeSouthEast, Strength);
	case SOUTHWEST:
		return ConvoluteDIB(hDib, &EdgeSouthWest, Strength);
	case WEST:
		return ConvoluteDIB(hDib, &EdgeWest, Strength);
	case NORTHWEST:
		return ConvoluteDIB(hDib, &EdgeNorthWest, Strength);
	case LAP1:
		return ConvoluteDIB(hDib, &Lap1, Strength);
	case LAP2:
		return ConvoluteDIB(hDib, &Lap2, Strength);
	case LAP3:
		return ConvoluteDIB(hDib, &Lap3, Strength);
	case LAP4:
		return ConvoluteDIB(hDib, &Lap4, Strength);
	case SOBEL:
		return ConvoluteDIB(hDib, Sobel, Strength, 4);
	case HOUGH:
		return ConvoluteDIB(hDib, Hough, Strength, 4);
	}

	return FALSE;
}

/************************************************************************* 
 * 
 * MedianFilterDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * This is the media filtering function to DIB
 * 
 ************************************************************************/ 
BOOL MedianFilterDIB(HDIB hDib) 
{
	WaitCursorBegin();

	HDIB hNewDib = NULL;
	// we only convolute 24bpp DIB, so first convert DIB to 24bpp
	WORD wBitCount = DIBBitCount(hDib);
	if (wBitCount != 24)
		hNewDib = ConvertDIBFormat(hDib, 24, NULL);
	else
		hNewDib = CopyHandle(hDib);

	if (! hNewDib)
	{
		WaitCursorEnd();
		return FALSE;
	}

	// new DIB attributes
	WORD wDIBWidth = (WORD)DIBWidth(hNewDib);
	WORD wDIBHeight = (WORD)DIBHeight(hNewDib);
	WORD wBytesPerLine = (WORD)BytesPerLine(hNewDib);
	DWORD dwImageSize = wBytesPerLine * wDIBHeight;

	// Allocate and lock memory for filtered image data
	HGLOBAL hFilteredBits = GlobalAlloc(GHND, dwImageSize);
	if (!hFilteredBits) 
	{
		WaitCursorEnd();
		return FALSE;
	}
	LPBYTE lpDestImage = (LPBYTE)GlobalLock(hFilteredBits);

	// get bits address in DIB
	LPBYTE lpDIB = (LPBYTE)GlobalLock(hNewDib);
	LPBYTE lpDIBits = FindDIBBits(lpDIB);

	// convolute...
	for (int i=1; i<wDIBHeight-1; i++) 
		for (int j=1; j<wDIBWidth-1; j++) 
		{
			int  red=0, green=0, blue=0; 
			DoMedianFilterDIB(&red, &green, &blue, i, j, wBytesPerLine, lpDIBits);

			LONG lOffset= PIXEL_OFFSET(i,j, wBytesPerLine);
			*(lpDestImage + lOffset++) = BOUND(blue, 0, 255);
			*(lpDestImage + lOffset++) = BOUND(green, 0, 255);
			*(lpDestImage + lOffset)   = BOUND(red, 0, 255);
		}

	// a filtered image is available in lpDestImage
	// copy it to DIB bits
	memcpy(lpDIBits, lpDestImage, dwImageSize);

	// cleanup temp buffers
	GlobalUnlock(hFilteredBits);
	GlobalFree(hFilteredBits);
	GlobalUnlock(hNewDib);

	// rebuild hDib
	HDIB hTmp = NULL;
	if (wBitCount != 24)
		hTmp = ConvertDIBFormat(hNewDib, wBitCount, NULL);
	else
		hTmp = CopyHandle(hNewDib);
	GlobalFree(hNewDib);
	DWORD dwSize = GlobalSize(hTmp);
	memcpy((LPBYTE)GlobalLock(hDib), (LPBYTE)GlobalLock(hTmp), dwSize);
	GlobalUnlock(hTmp);
	GlobalFree(hTmp);
	GlobalUnlock(hDib);
	WaitCursorEnd();

	return TRUE;
}

/************************************************************************* 
 * 
 * ConvoluteDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * KERNEL *lpKernel - pointer of kernel used to convolute with DIB
 * int Strength     - operation strength set to the convolute
 * int nKernelNum   - kernel number used to convolute
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * This is the generic convolute function to DIB
 * 
 ************************************************************************/ 
BOOL ConvoluteDIB(HDIB hDib, KERNEL *lpKernel, int Strength, int nKernelNum) 
{
	WaitCursorBegin();

	HDIB hNewDib = NULL;
	// we only convolute 24bpp DIB, so first convert DIB to 24bpp
	WORD wBitCount = DIBBitCount(hDib);
	if (wBitCount != 24)
		hNewDib = ConvertDIBFormat(hDib, 24, NULL);
	else
		hNewDib = CopyHandle(hDib);

	if (! hNewDib)
	{
		WaitCursorEnd();
		return FALSE;
	}

	// new DIB attributes
	WORD wDIBWidth = (WORD)DIBWidth(hNewDib);
	WORD wDIBHeight = (WORD)DIBHeight(hNewDib);
	WORD wBytesPerLine = (WORD)BytesPerLine(hNewDib);
	DWORD dwImageSize = wBytesPerLine * wDIBHeight;

	// Allocate and lock memory for filtered image data
	HGLOBAL hFilteredBits = GlobalAlloc(GHND, dwImageSize);
	if (!hFilteredBits) 
	{
		WaitCursorEnd();
		return FALSE;
	}
	LPBYTE lpDestImage = (LPBYTE)GlobalLock(hFilteredBits);

	// get bits address in DIB
	LPBYTE lpDIB = (LPBYTE)GlobalLock(hNewDib);
	LPBYTE lpDIBits = FindDIBBits(lpDIB);

	// convolute...
	for (int i=1; i<wDIBHeight-1; i++) 
		for (int j=1; j<wDIBWidth-1; j++) 
		{
			int  red=0, green=0, blue=0; 

			for (int k=0; k<nKernelNum; ++k)
			{
				int r=0, g=0, b=0; 
				DoConvoluteDIB(&r, &g, &b, i, j, 
					wBytesPerLine, lpDIBits, lpKernel+k);
				if (r > red)
					red = r;
				if (g > green)
					green = g;
				if (b > blue)
					blue = b;
				//red += r; green += g; blue += b;
			}

			// original RGB value in center pixel  (j, i)
			LONG lOffset= PIXEL_OFFSET(i,j, wBytesPerLine);
			BYTE OldB = *(lpDIBits + lOffset++);
			BYTE OldG = *(lpDIBits + lOffset++);
			BYTE OldR = *(lpDIBits + lOffset);
			// When we get here, red, green and blue have the new RGB value.
			if (Strength != 10) 
			{
				// Interpolate pixel data
				red   = OldR + (((red - OldR) * Strength) / 10);
				green = OldG + (((green - OldG) * Strength) / 10);
				blue  = OldB + (((blue - OldB) * Strength) / 10);
			}

			lOffset= PIXEL_OFFSET(i,j, wBytesPerLine);
			*(lpDestImage + lOffset++) = BOUND(blue, 0, 255);
			*(lpDestImage + lOffset++) = BOUND(green, 0, 255);
			*(lpDestImage + lOffset)   = BOUND(red, 0, 255);
		}

	// a filtered image is available in lpDestImage
	// copy it to DIB bits
	memcpy(lpDIBits, lpDestImage, dwImageSize);

	// cleanup temp buffers
	GlobalUnlock(hFilteredBits);
	GlobalFree(hFilteredBits);
	GlobalUnlock(hNewDib);

	// rebuild hDib
	HDIB hTmp = NULL;
	if (wBitCount != 24)
		hTmp = ConvertDIBFormat(hNewDib, wBitCount, NULL);
	else
		hTmp = CopyHandle(hNewDib);
	GlobalFree(hNewDib);
	DWORD dwSize = GlobalSize(hTmp);
	memcpy((LPBYTE)GlobalLock(hDib), (LPBYTE)GlobalLock(hTmp), dwSize);
	GlobalUnlock(hTmp);
	GlobalFree(hTmp);
	GlobalUnlock(hDib);
	WaitCursorEnd();

	return TRUE;
}

// local function: perform convolution to DIB with a kernel
void DoConvoluteDIB(int *red, int *green, int *blue, int i, int j, 
			WORD wBytesPerLine, LPBYTE lpDIBits, KERNEL *lpKernel)
{
	BYTE b[9], g[9], r[9];
	LONG lOffset;
	
	lOffset= PIXEL_OFFSET(i-1,j-1, wBytesPerLine);
	b[0] = *(lpDIBits + lOffset++);
	g[0] = *(lpDIBits + lOffset++);
	r[0] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i-1,j, wBytesPerLine);
	b[1] = *(lpDIBits + lOffset++);
	g[1] = *(lpDIBits + lOffset++);
	r[1] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i-1,j+1, wBytesPerLine);
	b[2] = *(lpDIBits + lOffset++);
	g[2] = *(lpDIBits + lOffset++);
	r[2] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i,j-1, wBytesPerLine);
	b[3] = *(lpDIBits + lOffset++);
	g[3] = *(lpDIBits + lOffset++);
	r[3] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i,j, wBytesPerLine);
	b[4] = *(lpDIBits + lOffset++);
	g[4] = *(lpDIBits + lOffset++);
	r[4] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i,j+1, wBytesPerLine);
	b[5] = *(lpDIBits + lOffset++);
	g[5] = *(lpDIBits + lOffset++);
	r[5] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i+1,j-1, wBytesPerLine);
	b[6] = *(lpDIBits + lOffset++);
	g[6] = *(lpDIBits + lOffset++);
	r[6] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i+1,j, wBytesPerLine);
	b[7] = *(lpDIBits + lOffset++);
	g[7] = *(lpDIBits + lOffset++);
	r[7] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i+1,j+1, wBytesPerLine);
	b[8] = *(lpDIBits + lOffset++);
	g[8] = *(lpDIBits + lOffset++);
	r[8] = *(lpDIBits + lOffset);

	*red = *green = *blue = 0;
	for (int k=0; k<8; ++k)
	{
		*red   += lpKernel->Element[k]*r[k];
		*green += lpKernel->Element[k]*g[k];
		*blue  += lpKernel->Element[k]*b[k];
	}

	if (lpKernel->Divisor != 1) 
	{
		*red   /= lpKernel->Divisor;
		*green /= lpKernel->Divisor;
		*blue  /= lpKernel->Divisor;
	}

	// getoff opposite
	*red   = abs(*red);
	*green = abs(*green);
	*blue  = abs(*blue);
}

// local function: perform median filter to DIB
void DoMedianFilterDIB(int *red, int *green, int *blue, int i, int j, 
			WORD wBytesPerLine, LPBYTE lpDIBits)
{
	BYTE b[9], g[9], r[9];
	LONG lOffset;
	
	lOffset= PIXEL_OFFSET(i-1,j-1, wBytesPerLine);
	b[0] = *(lpDIBits + lOffset++);
	g[0] = *(lpDIBits + lOffset++);
	r[0] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i-1,j, wBytesPerLine);
	b[1] = *(lpDIBits + lOffset++);
	g[1] = *(lpDIBits + lOffset++);
	r[1] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i-1,j+1, wBytesPerLine);
	b[2] = *(lpDIBits + lOffset++);
	g[2] = *(lpDIBits + lOffset++);
	r[2] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i,j-1, wBytesPerLine);
	b[3] = *(lpDIBits + lOffset++);
	g[3] = *(lpDIBits + lOffset++);
	r[3] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i,j, wBytesPerLine);
	b[4] = *(lpDIBits + lOffset++);
	g[4] = *(lpDIBits + lOffset++);
	r[4] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i,j+1, wBytesPerLine);
	b[5] = *(lpDIBits + lOffset++);
	g[5] = *(lpDIBits + lOffset++);
	r[5] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i+1,j-1, wBytesPerLine);
	b[6] = *(lpDIBits + lOffset++);
	g[6] = *(lpDIBits + lOffset++);
	r[6] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i+1,j, wBytesPerLine);
	b[7] = *(lpDIBits + lOffset++);
	g[7] = *(lpDIBits + lOffset++);
	r[7] = *(lpDIBits + lOffset);

	lOffset= PIXEL_OFFSET(i+1,j+1, wBytesPerLine);
	b[8] = *(lpDIBits + lOffset++);
	g[8] = *(lpDIBits + lOffset++);
	r[8] = *(lpDIBits + lOffset);

    qsort(r, 9, 1, compare);
    qsort(g, 9, 1, compare);
    qsort(b, 9, 1, compare);

	*red   = r[0];
	*green = g[0];
	*blue  = b[0];
}

// function used to sort in the call of qsort
int compare(const void *e1, const void *e2)
{
	if (*(BYTE *)e1 < *(BYTE *)e2)
		return -1;
	if (*(BYTE *)e1 > *(BYTE *)e2)
		return 1;

	return 0;
}

/************************************************************************* 
 * 
 * ReverseDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * This function reverse DIB
 * 
 ************************************************************************/ 
BOOL ReverseDIB(HDIB hDib) 
{
	WaitCursorBegin();

	HDIB hNewDib = NULL;
	// only support 256 grayscale image
	WORD wBitCount = DIBBitCount(hDib);
	if (wBitCount != 8)
	{
		WaitCursorEnd();
		return FALSE;
	}

	// the maxium pixel value
	int  nMaxValue = 256;

	// source pixel data
    LPBITMAPINFO lpSrcDIB = (LPBITMAPINFO)GlobalLock(hDib);
	if (! lpSrcDIB)
	{
		WaitCursorBegin();
		return FALSE;
	}

	// new DIB attributes
	LPSTR lpPtr;
	LONG lHeight = DIBHeight(lpSrcDIB);
	LONG lWidth = DIBWidth(lpSrcDIB);
	DWORD dwBufferSize = GlobalSize(lpSrcDIB);
	int nLineBytes = BytesPerLine(lpSrcDIB);

	// convolute...
	for (long i=0; i<lHeight; i++) 
		for (long j=0; j<lWidth; j++) 
		{
			lpPtr=(char *)lpSrcDIB+(dwBufferSize-nLineBytes-i*nLineBytes)+j;
			*lpPtr = nMaxValue - *lpPtr;
		}

	// cleanup
	GlobalUnlock(hDib);
	WaitCursorEnd();

	return TRUE;
}

/************************************************************************* 
 * 
 * ErosionDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * BOOL bHori	    - erosion direction
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * This function do erosion with the specified direction
 * 
 ************************************************************************/ 
BOOL ErosionDIB(HDIB hDib, BOOL bHori)
{
	// start wait cursor
	WaitCursorBegin();

    // Old DIB buffer
	if (hDib == NULL)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// only support 256 color image
	WORD wBitCount = DIBBitCount(hDib);
	if (wBitCount != 8)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// new DIB
	HDIB hNewDIB = CopyHandle(hDib);
	if (! hNewDIB)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// source dib buffer
    LPBITMAPINFO lpSrcDIB = (LPBITMAPINFO)GlobalLock(hDib);
	if (! lpSrcDIB)
	{
		WaitCursorBegin();
		return FALSE;
	}
    // New DIB buffer
    LPBITMAPINFO lpbmi = (LPBITMAPINFO)GlobalLock(hNewDIB);
	if (! lpbmi)
	{
		WaitCursorBegin();
		return FALSE;
	}

	// start erosion...
	LPSTR lpPtr;
	LPSTR lpTempPtr;
	LONG  x,y;
	BYTE  num, num0;
	int   i;
	LONG lHeight = DIBHeight(lpSrcDIB);
	LONG lWidth = DIBWidth(lpSrcDIB);
	DWORD dwBufferSize = GlobalSize(lpSrcDIB);
	int nLineBytes = BytesPerLine(lpSrcDIB);
	if(bHori)
	{
		for (y=0; y<lHeight; y++)
		{
			lpPtr=(char *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes)+1;
			lpTempPtr=(char *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes)+1;
			for (x=1; x<lWidth-1; x++)
			{
				num0 = num = 0 ;
				for(i=0;i<3;i++)
				{
					num=(unsigned char)*(lpPtr+i-1);
					if(num > num0)
						num0 = num;
				}
				*lpTempPtr=(unsigned char)num0;
				/*
				num=(unsigned char)*lpPtr;
				if (num==0)
				{
					*lpTempPtr=(unsigned char)0;
					for(i=0;i<3;i++)
					{
						num=(unsigned char)*(lpPtr+i-1);
						if(num==255)
						{
							*lpTempPtr=(unsigned char)255;
							break;
						}
					}
				}
				else 
					*lpTempPtr=(unsigned char)255;
				*/
				lpPtr++;
				lpTempPtr++;
			}
		}
	}
	else		// Vertical
	{
		for (y=1; y<lHeight-1; y++)
		{
			lpPtr=(char *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes);
			lpTempPtr=(char *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes);
			for (x=0; x<lWidth; x++)
			{
				num0 = num = 0 ;
				for(i=0;i<3;i++)
				{
					num=(unsigned char)*(lpPtr+i-1);
					if(num > num0)
						num0 = num;
				}
				*lpTempPtr=(unsigned char)num0;
				/*
				num=(unsigned char)*lpPtr;
				if (num==0)
				{
					*lpTempPtr=(unsigned char)0;
					for(i=0;i<3;i++)
					{
						num=(unsigned char)*(lpPtr+(i-1)*nLineBytes);
						if(num==255)
						{
							*lpTempPtr=(unsigned char)255;
							break;
						}
					}
				}
				else 
					*lpTempPtr=(unsigned char)255;
				*/
				lpPtr++;
				lpTempPtr++;
			}
		}
	}

	// cleanup
	GlobalUnlock(hDib);
	GlobalUnlock(hNewDIB);
	GlobalFree(hNewDIB);
	WaitCursorEnd();

	return TRUE;
}

/************************************************************************* 
 * 
 * DilationDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * BOOL bHori	    - dilation direction
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * This function do dilation with the specified direction
 * 
 ************************************************************************/ 
BOOL DilationDIB(HDIB hDib, BOOL bHori)
{
	// start wait cursor
	WaitCursorBegin();

    // Old DIB buffer
	if (hDib == NULL)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// only support 256 color image
	WORD wBitCount = DIBBitCount(hDib);
	if (wBitCount != 8)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// new DIB
	HDIB hNewDIB = CopyHandle(hDib);
	if (! hNewDIB)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// source dib buffer
    LPBITMAPINFO lpSrcDIB = (LPBITMAPINFO)GlobalLock(hDib);
	if (! lpSrcDIB)
	{
		WaitCursorBegin();
		return FALSE;
	}
    // New DIB buffer
    LPBITMAPINFO lpbmi = (LPBITMAPINFO)GlobalLock(hNewDIB);
	if (! lpbmi)
	{
		WaitCursorBegin();
		return FALSE;
	}

	// start erosion...
	LPSTR lpPtr;
	LPSTR lpTempPtr;
	LONG  x,y;
	BYTE  num, num0;
	int   i;
	LONG lHeight = DIBHeight(lpSrcDIB);
	LONG lWidth = DIBWidth(lpSrcDIB);
	DWORD dwBufferSize = GlobalSize(lpSrcDIB);
	int nLineBytes = BytesPerLine(lpSrcDIB);
	if(bHori)
	{
		for(y=0;y<lHeight;y++)
		{
			lpPtr=(char *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes)+1;
			lpTempPtr=(char *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes)+1;
			for(x=1;x<lWidth-1;x++)
			{
				num0 = num = 255;
				for(i=0;i<3;i++)
				{
					num=(unsigned char)*(lpPtr+i-1);
					if(num < num0)
						num0 = num;
				}
				*lpTempPtr=(unsigned char)num0;
				/*
				num=(unsigned char)*lpPtr;
				if (num==255)
				{
					*lpTempPtr=(unsigned char)255;
					for(i=0;i<3;i++)
					{
						num=(unsigned char)*(lpPtr+i-1);
						if(num==0)
						{
							*lpTempPtr=(unsigned char)0;
							break;
						}
					}
				}
				else 
					*lpTempPtr=(unsigned char)0;
				*/
				lpPtr++;
				lpTempPtr++;
			}
		}
	}
	else
	{
		for(y=1;y<lHeight-1;y++)
		{
			lpPtr=(char *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes);
			lpTempPtr=(char *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes);
			for(x=0;x<lWidth;x++)
			{
				num0 = num = 255;
				for(i=0;i<3;i++)
				{
					num=(unsigned char)*(lpPtr+i-1);
					if(num < num0)
						num0 = num;
				}
				*lpTempPtr=(unsigned char)num0;
				/*
				num=(unsigned char)*lpPtr;
				if (num==255)
				{
					*lpTempPtr=(unsigned char)255;
					for(i=0;i<3;i++)
					{
						num=(unsigned char)*(lpPtr+(i-1)*nLineBytes);
						if(num==0)
						{
							*lpTempPtr=(unsigned char)0;
							break;
						}
					}
				}
				else 
					*lpTempPtr=(unsigned char)0;
				*/
				lpPtr++;
				lpTempPtr++;
			}
		}
	}

	// cleanup
	GlobalUnlock(hDib);
	GlobalUnlock(hNewDIB);
	GlobalFree(hNewDIB);
	WaitCursorEnd();

	return TRUE;
}

/************************************************************************* 
 * 
 * MorphOpenDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * BOOL bHori	    - open direction
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * This function do open with the specified direction
 * 
 ************************************************************************/ 
BOOL MorphOpenDIB(HDIB hDib, BOOL bHori)
{
	// Step 1: erosion
	if (! ErosionDIB(hDib, bHori))
		return FALSE;

	// Step 2: dilation	
	if (! DilationDIB(hDib, bHori))
		return FALSE;

	return TRUE;
}

/************************************************************************* 
 * 
 * MorphCloseDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * BOOL bHori	    - close direction
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * This function do close with the specified direction
 * 
 ************************************************************************/ 
BOOL MorphCloseDIB(HDIB hDib, BOOL bHori)
{
	// Step 1: dilation	
	if (! DilationDIB(hDib, bHori))
		return FALSE;

	// Step 2: erosion
	if (! ErosionDIB(hDib, bHori))
		return FALSE;

	return TRUE;
}

/************************************************************************* 
 * 
 * ContourDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * BOOL bHori	    - open direction
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * This function contour DIB with the specified direction
 * 
 ************************************************************************/ 
BOOL ContourDIB(HDIB hDib, BOOL bHori)
{
	// start wait cursor
	WaitCursorBegin();

    // Old DIB buffer
	if (hDib == NULL)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// only support 256 color image
	WORD wBitCount = DIBBitCount(hDib);
	if (wBitCount != 8)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// new DIB
	HDIB hNewDIB = CopyHandle(hDib);
	if (! hNewDIB)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// source dib buffer
    LPBITMAPINFO lpSrcDIB = (LPBITMAPINFO)GlobalLock(hDib);
	if (! lpSrcDIB)
	{
		WaitCursorBegin();
		return FALSE;
	}
    // New DIB buffer
    LPBITMAPINFO lpbmi = (LPBITMAPINFO)GlobalLock(hNewDIB);
	if (! lpbmi)
	{
		WaitCursorBegin();
		return FALSE;
	}

	// start erosion...
	LPBYTE lpPtr;
	LPBYTE lpTempPtr;
	LONG  x,y;
	BYTE  num, num0;
	int   i;
	LONG lHeight = DIBHeight(lpSrcDIB);
	LONG lWidth = DIBWidth(lpSrcDIB);
	DWORD dwBufferSize = GlobalSize(lpSrcDIB);
	int nLineBytes = BytesPerLine(lpSrcDIB);

	// Step 1: erosion
	if(bHori)
	{
		for (y=0; y<lHeight; y++)
		{
			lpPtr=(BYTE *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes)+1;
			lpTempPtr=(BYTE *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes)+1;
			for (x=1; x<lWidth-1; x++)
			{
				num0 = num = 0 ;
				for(i=0;i<3;i++)
				{
					num=(unsigned char)*(lpPtr+i-1);
					if(num > num0)
						num0 = num;
				}
				*lpTempPtr=(unsigned char)num0;
				/*
				num=(unsigned char)*lpPtr;
				if (num==0)
				{
					*lpTempPtr=(unsigned char)0;
					for(i=0;i<3;i++)
					{
						num=(unsigned char)*(lpPtr+i-1);
						if(num==255)
						{
							*lpTempPtr=(unsigned char)255;
							break;
						}
					}
				}
				else 
					*lpTempPtr=(unsigned char)255;
				*/
				lpPtr++;
				lpTempPtr++;
			}
		}
	}
	else		// Vertical
	{
		for (y=1; y<lHeight-1; y++)
		{
			lpPtr=(BYTE *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes);
			lpTempPtr=(BYTE *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes);
			for (x=0; x<lWidth; x++)
			{
				num0 = num = 0 ;
				for(i=0;i<3;i++)
				{
					num=(unsigned char)*(lpPtr+i-1);
					if(num > num0)
						num0 = num;
				}
				*lpTempPtr=(unsigned char)num0;
				/*
				num=(unsigned char)*lpPtr;
				if (num==0)
				{
					*lpTempPtr=(unsigned char)0;
					for(i=0;i<3;i++)
					{
						num=(unsigned char)*(lpPtr+(i-1)*nLineBytes);
						if(num==255)
						{
							*lpTempPtr=(unsigned char)255;
							break;
						}
					}
				}
				else 
					*lpTempPtr=(unsigned char)255;
				*/
				lpPtr++;
				lpTempPtr++;
			}
		}
	}
	
	// Step 2: original image minues dilation image
	if(bHori)
	{
		for(y=0;y<lHeight;y++)
		{
			lpPtr=(BYTE *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes)+1;
			lpTempPtr=(BYTE *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes)+1;
			for(x=1;x<lWidth-1;x++)
			{
				if (*lpTempPtr == *lpPtr)
					*lpTempPtr = (BYTE)255;
				else
					*lpTempPtr = *lpTempPtr - *lpPtr;

				lpPtr++;
				lpTempPtr++;
			}
		}
	}
	else
	{
		for(y=1;y<lHeight-1;y++)
		{
			lpPtr=(BYTE *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes);
			lpTempPtr=(BYTE *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes);
			for(x=0;x<lWidth;x++)
			{
				if (*lpTempPtr == *lpPtr)
					*lpTempPtr = (BYTE)255;
				else
					*lpTempPtr = *lpTempPtr - *lpPtr;

				lpPtr++;
				lpTempPtr++;
			}
		}
	}

	// cleanup
	GlobalUnlock(hDib);
	GlobalUnlock(hNewDIB);
	GlobalFree(hNewDIB);

	return TRUE;
}

/************************************************************************* 
 * 
 * ThinningDIB() 
 * 
 * Parameters: 
 * 
 * HDIB hDib        - objective DIB handle
 * 
 * Return Value: 
 * 
 * BOOL             - True is success, else False
 * 
 * Description: 
 * 
 * This function thins a DIB
 * 
 ************************************************************************/ 
BOOL ThinningDIB(HDIB hDib)
{
	static int erasetable[256]=
	{
		0,0,1,1,0,0,1,1,
		1,1,0,1,1,1,0,1,
		1,1,0,0,1,1,1,1,
		0,0,0,0,0,0,0,1,

		0,0,1,1,0,0,1,1,
		1,1,0,1,1,1,0,1,
		1,1,0,0,1,1,1,1,
		0,0,0,0,0,0,0,1,

		1,1,0,0,1,1,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,

		1,1,0,0,1,1,0,0,
		1,1,0,1,1,1,0,1,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,

		0,0,1,1,0,0,1,1,
		1,1,0,1,1,1,0,1,
		1,1,0,0,1,1,1,1,
		0,0,0,0,0,0,0,1,

		0,0,1,1,0,0,1,1,
		1,1,0,1,1,1,0,1,
		1,1,0,0,1,1,1,1,
		0,0,0,0,0,0,0,0,

		1,1,0,0,1,1,0,0,
		0,0,0,0,0,0,0,0,
		1,1,0,0,1,1,1,1,
		0,0,0,0,0,0,0,0,

		1,1,0,0,1,1,0,0,
		1,1,0,1,1,1,0,0,
		1,1,0,0,1,1,1,0,
		1,1,0,0,1,0,0,0
	};

	// start wait cursor
	WaitCursorBegin();

    // Old DIB buffer
	if (hDib == NULL)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// only support 256 color image
	WORD wBitCount = DIBBitCount(hDib);
	if (wBitCount != 8)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// new DIB
	HDIB hNewDIB = CopyHandle(hDib);
	if (! hNewDIB)
	{
		WaitCursorEnd();
        return FALSE;
	}

	// source dib buffer
    LPBITMAPINFO lpSrcDIB = (LPBITMAPINFO)GlobalLock(hDib);
	if (! lpSrcDIB)
	{
		WaitCursorBegin();
		return FALSE;
	}
    // New DIB buffer
    LPBITMAPINFO lpbmi = (LPBITMAPINFO)GlobalLock(hNewDIB);
	if (! lpbmi)
	{
		WaitCursorBegin();
		return FALSE;
	}

	// start erosion...
	LPSTR lpPtr;
	LPSTR lpTempPtr;
	LONG  x,y;
	BYTE  num;
	LONG lHeight = DIBHeight(lpSrcDIB);
	LONG lWidth = DIBWidth(lpSrcDIB);
	DWORD dwBufferSize = GlobalSize(lpSrcDIB);
	int nLineBytes = BytesPerLine(lpSrcDIB);
	int nw,n,ne,w,e,sw,s,se;

	BOOL Finished=FALSE;
    while(!Finished)
	{
    	Finished=TRUE;
		for (y=1;y<lHeight-1;y++)
		{ 
			lpPtr=(char *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes);
			lpTempPtr=(char *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes);
			x=1; 
			while(x<lWidth-1)
			{
				if(*(lpPtr+x)==0)
				{
					w=(unsigned char)*(lpPtr+x-1);
					e=(unsigned char)*(lpPtr+x+1);
					if( (w==255)|| (e==255))
					{
						nw=(unsigned char)*(lpPtr+x+nLineBytes-1);
						n=(unsigned char)*(lpPtr+x+nLineBytes);
						ne=(unsigned char)*(lpPtr+x+nLineBytes+1);
						sw=(unsigned char)*(lpPtr+x-nLineBytes-1);
						s=(unsigned char)*(lpPtr+x-nLineBytes);
						se=(unsigned char)*(lpPtr+x-nLineBytes+1);
						num=nw/255+n/255*2+ne/255*4+w/255*8+e/255*16+sw/255*32+s/255*64+se/255*128;
						if(erasetable[num]==1)
						{
							*(lpPtr+x)=(BYTE)255;
							*(lpTempPtr+x)=(BYTE)255;
							Finished=FALSE;
							x++;
						}
					}
				}
				x++;
			}
		}
	
		for (x=1;x<lWidth-1;x++)
		{ 
			y=1;
			while(y<lHeight-1)
			{
				lpPtr=(char *)lpbmi+(dwBufferSize-nLineBytes-y*nLineBytes);
				lpTempPtr=(char *)lpSrcDIB+(dwBufferSize-nLineBytes-y*nLineBytes);
				if(*(lpPtr+x)==0)
				{
					n=(unsigned char)*(lpPtr+x+nLineBytes);
					s=(unsigned char)*(lpPtr+x-nLineBytes);
					if( (n==255)|| (s==255))
					{
						nw=(unsigned char)*(lpPtr+x+nLineBytes-1);
						ne=(unsigned char)*(lpPtr+x+nLineBytes+1);
						w=(unsigned char)*(lpPtr+x-1);
						e=(unsigned char)*(lpPtr+x+1);
						sw=(unsigned char)*(lpPtr+x-nLineBytes-1);
						se=(unsigned char)*(lpPtr+x-nLineBytes+1);
						num=nw/255+n/255*2+ne/255*4+w/255*8+e/255*16+sw/255*32+s/255*64+se/255*128;
						if(erasetable[num]==1)
						{
							*(lpPtr+x)=(BYTE)255;
							*(lpTempPtr+x)=(BYTE)255;
							Finished=FALSE;
							y++;
						}
					}
				}
				y++;
			}
		} 
	}

	// cleanup
	GlobalUnlock(hDib);
	GlobalUnlock(hNewDIB);
	GlobalFree(hNewDIB);

	return TRUE;
}

//////////////////////////////////////////////////////////
// internal definitions

#define PI (double)3.14159265359

/*complex number*/
typedef struct
{
	double re;
	double im;
}COMPLEX;

/*complex add*/
COMPLEX Add(COMPLEX c1, COMPLEX c2)
{
	COMPLEX c;
	c.re=c1.re+c2.re;
	c.im=c1.im+c2.im;
	return c;
}

/*complex substract*/
COMPLEX Sub(COMPLEX c1, COMPLEX c2)
{
	COMPLEX c;
	c.re=c1.re-c2.re;
	c.im=c1.im-c2.im;
	return c;
}

/*complex multiple*/
COMPLEX Mul(COMPLEX c1, COMPLEX c2)
{
	COMPLEX c;
	c.re=c1.re*c2.re-c1.im*c2.im;
	c.im=c1.re*c2.im+c2.re*c1.im;
	return c;
}
//////////////////////////////////////////////////////////

/*
void FFT(COMPLEX * TD, COMPLEX * FD, int power);
void IFFT(COMPLEX * FD, COMPLEX * TD, int power);
void DCT(double *f, double *F, int power);
void IDCT(double *F, double *f, int power);
void WALh(double *f, double *W, int power);
void IWALh(double *W, double *f, int power);
*/

/****************************************************
	FFT()

	参数：

		TD为时域值
		FD为频域值
		power为2的幂数

	返回值：

		无

	说明：

		本函数实现快速傅立叶变换
****************************************************/
void FFT(COMPLEX * TD, COMPLEX * FD, int power)
{
	int count;
	int i,j,k,bfsize,p;
	double angle;
	COMPLEX *W,*X1,*X2,*X;

	/*计算傅立叶变换点数*/
	count=1<<power;
	
	/*分配运算所需存储器*/
	W=(COMPLEX *)malloc(sizeof(COMPLEX)*count/2);
	X1=(COMPLEX *)malloc(sizeof(COMPLEX)*count);
	X2=(COMPLEX *)malloc(sizeof(COMPLEX)*count);
	
	/*计算加权系数*/
	for(i=0;i<count/2;i++)
	{
		angle=-i*PI*2/count;
		W[i].re=cos(angle);
		W[i].im=sin(angle);
	}
	
	/*将时域点写入存储器*/
	memcpy(X1,TD,sizeof(COMPLEX)*count);
	
	/*蝶形运算*/
	for(k=0;k<power;k++)
	{
		for(j=0;j<1<<k;j++)
		{
			bfsize=1<<(power-k);
			for(i=0;i<bfsize/2;i++)
			{
				p=j*bfsize;
				X2[i+p]=Add(X1[i+p],X1[i+p+bfsize/2]);
				X2[i+p+bfsize/2]=Mul(Sub(X1[i+p],X1[i+p+bfsize/2]),W[i*(1<<k)]);
			}
		}
		X=X1;
		X1=X2;
		X2=X;
	}
	
	/*重新排序*/
	for(j=0;j<count;j++)
	{
		p=0;
		for(i=0;i<power;i++)
		{
			if (j&(1<<i)) p+=1<<(power-i-1);
		}
		FD[j]=X1[p];
	}
	
	/*释放存储器*/
	free(W);
	free(X1);
	free(X2);
}

/****************************************************
	IFFT()

	参数：

		FD为频域值
		TD为时域值
		power为2的幂数

	返回值：

		无

	说明：

		本函数利用快速傅立叶变换实现傅立叶反变换
****************************************************/
void IFFT(COMPLEX * FD, COMPLEX * TD, int power)
{
	int i, count;
	COMPLEX *x;

	/*计算傅立叶反变换点数*/
	count=1<<power;

	/*分配运算所需存储器*/
	x=(COMPLEX *)malloc(sizeof(COMPLEX)*count);

	/*将频域点写入存储器*/
	memcpy(x,FD,sizeof(COMPLEX)*count);
	
	/*求频域点的共轭*/
	for(i=0;i<count;i++)
		x[i].im = -x[i].im;

	/*调用FFT*/
	FFT(x, TD, power);

	/*求时域点的共轭*/
	for(i=0;i<count;i++)
	{
		TD[i].re /= count;
		TD[i].im = -TD[i].im / count;
	}

	/*释放存储器*/
	free(x);
}

/*******************************************************
	DCT()

	参数：

		f为时域值
		F为频域值
		power为2的幂数

	返回值：

		无

	说明：

		本函数利用快速傅立叶变换实现快速离散余弦变换
********************************************************/
void DCT(double *f, double *F, int power)
{
	int i,count;
	COMPLEX *X;
	double s;

	/*计算离散余弦变换点数*/
	count=1<<power;
	
	/*分配运算所需存储器*/
	X=(COMPLEX *)malloc(sizeof(COMPLEX)*count*2);
	
	/*延拓*/
	memset(X,0,sizeof(COMPLEX)*count*2);
	
	/*将时域点写入存储器*/
	for(i=0;i<count;i++)
	{
		X[i].re=f[i];
	}
	
	/*调用快速傅立叶变换*/
	FFT(X,X,power+1);
	
	/*调整系数*/
	s=1/sqrt(count);
	F[0]=X[0].re*s;
	s*=sqrt(2);
	for(i=1;i<count;i++)
	{
		F[i]=(X[i].re*cos(i*PI/(count*2))+X[i].im*sin(i*PI/(count*2)))*s;
	}
	
	/*释放存储器*/
	free(X);
}

/************************************************************
	IDCT()

	参数：

		F为频域值
		f为时域值
		power为2的幂数

	返回值：

		无

	说明：

		本函数利用快速傅立叶反变换实现快速离散反余弦变换
*************************************************************/
void IDCT(double *F, double *f, int power)
{
	int i,count;
	COMPLEX *X;
	double s;

	/*计算离散反余弦变换点数*/
	count=1<<power;
	
	/*分配运算所需存储器*/
	X=(COMPLEX *)malloc(sizeof(COMPLEX)*count*2);
	
	/*延拓*/
	memset(X,0,sizeof(COMPLEX)*count*2);
	
	/*调整频域点，写入存储器*/
	for(i=0;i<count;i++)
	{
		X[i].re=F[i]*cos(i*PI/(count*2));
		X[i].im=F[i]*sin(i*PI/(count*2));
	}
	
	/*调用快速傅立叶反变换*/
	IFFT(X,X,power+1);
	
	/*调整系数*/
	s=1/sqrt(count);
	for(i=1;i<count;i++)
	{
		f[i]=(1-sqrt(2))*s*F[0]+sqrt(2)*s*X[i].re*count*2;
	}
	
	/*释放存储器*/
	free(X);
}

/**********************************************************
	WALh()

	参数：

		f为时域值
		W为频域值
		power为2的幂数

	返回值：

		无

	说明：

		本函数利用快速傅立叶变换实现快速沃尔什-哈达玛变换
*************************************************************/
void WALh(double *f, double *W, int power)
{
	int count;
	int i,j,k,bfsize,p;
	double *X1,*X2,*X;

	/*计算快速沃尔什变换点数*/

	count=1<<power;
	
	/*分配运算所需存储器*/
	X1=(double *)malloc(sizeof(double)*count);
	X2=(double *)malloc(sizeof(double)*count);
	
	/*将时域点写入存储器*/
	memcpy(X1,f,sizeof(double)*count);
	
	/*蝶形运算*/
	for(k=0;k<power;k++)
	{
		for(j=0;j<1<<k;j++)
		{
			bfsize=1<<(power-k);
			for(i=0;i<bfsize/2;i++)
			{
				p=j*bfsize;
				X2[i+p]=X1[i+p]+X1[i+p+bfsize/2];
				X2[i+p+bfsize/2]=X1[i+p]-X1[i+p+bfsize/2];
			}
		}
		X=X1;
		X1=X2;
		X2=X;
	}
	/*调整系数*/
//	for(i=0;i<count;i++)
//	{
//		W[i]=X1[i]/count;
//	}
	for(j=0;j<count;j++)
	{
		p=0;
		for(i=0;i<power;i++)
		{
			if (j&(1<<i)) p+=1<<(power-i-1);
		}
		W[j]=X1[p]/count;
	}
	
	/*释放存储器*/
	free(X1);
	free(X2);
}

/*********************************************************************
	IWALh()

	参数：

		W为频域值
		f为时域值
		power为2的幂数

	返回值：

		无

	说明：

		本函数利用快速沃尔什-哈达玛变换实现快速沃尔什-哈达玛反变换
**********************************************************************/
void IWALh(double *W, double *f, int power)
{
	int i, count;

	/*计算快速沃尔什反变换点数*/
	count=1<<power;

	/*调用快速沃尔什-哈达玛变换*/
	WALh(W, f, power);

	/*调整系数*/
	for(i=0;i<count;i++)
	{
		f[i] *= count;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// internal functions

#define Point(x,y) lpPoints[(x)+(y)*nWidth]

void GetPoints(int nWidth,int nHeight,BYTE *lpBits,BYTE *lpPoints)
{
	int x,y,p;
	int nByteWidth = WIDTHBYTES(nWidth*24); //nWidth*3;
	//if (nByteWidth%4) nByteWidth+=4-(nByteWidth%4);

	for(y=0;y<nHeight;y++)
	{
		for(x=0;x<nWidth;x++)
		{
			p=x*3+y*nByteWidth;
			lpPoints[x+y*nWidth]=(BYTE)(0.299*(float)lpBits[p+2]+0.587*(float)lpBits[p+1]+0.114*(float)lpBits[p]+0.1);
		}
	}
}

void PutPoints(int nWidth,int nHeight,BYTE *lpBits,BYTE *lpPoints)
{
	int x,y,p,p1;
	int nByteWidth = WIDTHBYTES(nWidth*24); //nWidth*3;
	//if (nByteWidth%4) nByteWidth+=4-(nByteWidth%4);

	for(y=0;y<nHeight;y++)
	{
		for(x=0;x<nWidth;x++)
		{
			p=x*3+y*nByteWidth;
			p1=x+y*nWidth;
			lpBits[p]=lpPoints[p1];
			lpBits[p+1]=lpPoints[p1];
			lpBits[p+2]=lpPoints[p1];
		}
	}
}
//////////////////////////////////////////////////////////////////////////////

/****************************************************
	FFTDIB()

	参数：

		hDIB为输入的DIB句柄

	返回值：

		成功为TRUE；失败为FALSE

	说明：

		本函数实现DIB位图的快速傅立叶变换
****************************************************/
BOOL FFTDIB(HDIB hDIB)
{
	if (hDIB == NULL)
		return FALSE;

	// start wait cursor
	WaitCursorBegin();

	HDIB hDib = NULL;
	HDIB hNewDib = NULL;
	// we only convolute 24bpp DIB, so first convert DIB to 24bpp
	WORD wBitCount = DIBBitCount(hDIB);
	if (wBitCount != 24)
	{
		hNewDib = ConvertDIBFormat(hDIB, 24, NULL);
		hDib = CopyHandle(hNewDib);
	}
	else
	{
		hNewDib = CopyHandle(hDIB);
		hDib = CopyHandle(hDIB);
	}

	if (hNewDib == NULL && hDib == NULL)
	{
		WaitCursorEnd();
		return FALSE;
	}

	// process!
	LPBYTE lpSrcDIB = (LPBYTE)GlobalLock(hDib);
	LPBYTE lpDIB = (LPBYTE)GlobalLock(hNewDib);
	LPBYTE lpInput = FindDIBBits(lpSrcDIB);
	LPBYTE lpOutput = FindDIBBits(lpDIB);
	int nWidth = DIBWidth(lpSrcDIB);
	int nHeight = DIBHeight(lpSrcDIB);
	
	int w=1,h=1,wp=0,hp=0;
	while(w*2<=nWidth)
	{
		w*=2;
		wp++;
	}
	while(h*2<=nHeight)
	{
		h*=2;
		hp++;
	}
	int x,y;
	BYTE *lpPoints=new BYTE[nWidth*nHeight];
	GetPoints(nWidth,nHeight,lpInput,lpPoints);

	COMPLEX *TD=new COMPLEX[w*h];
	COMPLEX *FD=new COMPLEX[w*h];

	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			TD[x+w*y].re=Point(x,y);
			TD[x+w*y].im=0;
		}
	}

	for(y=0;y<h;y++)
	{
		FFT(&TD[w*y],&FD[w*y],wp);
	}
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			TD[y+h*x]=FD[x+w*y];
//			TD[x+w*y]=FD[x*h+y];
		}
	}
	for(x=0;x<w;x++)
	{
		FFT(&TD[x*h],&FD[x*h],hp);
	}

	memset(lpPoints,0,nWidth*nHeight);
	double m;
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			m=sqrt(FD[x*h+y].re*FD[x*h+y].re+FD[x*h+y].im*FD[x*h+y].im)/100;
			if (m>255) m=255;
			Point((x<w/2?x+w/2:x-w/2),nHeight-1-(y<h/2?y+h/2:y-h/2))=(BYTE)(m);
		}
	}
	delete TD;
	delete FD;
	PutPoints(nWidth,nHeight,lpOutput,lpPoints);
	delete lpPoints;

	// recover
	DWORD dwSize = GlobalSize(hDib);
	memcpy(lpSrcDIB, lpDIB, dwSize);
	GlobalUnlock(hDib);
	GlobalUnlock(hNewDib);
	if (wBitCount != 24)
	{
		hNewDib = ConvertDIBFormat(hDib, wBitCount, NULL);

		lpSrcDIB = (LPBYTE)GlobalLock(hDIB);
		lpDIB = (LPBYTE)GlobalLock(hNewDib);
		dwSize = GlobalSize(hNewDib);
		memcpy(lpSrcDIB, lpDIB, dwSize);
		GlobalUnlock(hDIB);
		GlobalUnlock(hNewDib);
	}
	else
	{
		lpSrcDIB = (LPBYTE)GlobalLock(hDIB);
		lpDIB = (LPBYTE)GlobalLock(hDib);
		dwSize = GlobalSize(hDib);
		memcpy(lpSrcDIB, lpDIB, dwSize);
		GlobalUnlock(hDIB);
		GlobalUnlock(hDib);
	}

	// cleanup
	GlobalFree(hDib);
	GlobalFree(hNewDib);

	// return
	WaitCursorEnd();
	return TRUE;
}

/****************************************************
	DCTDIB()

	参数：

		hDIB为输入的DIB句柄

	返回值：

		成功为TRUE；失败为FALSE

	说明：

		本函数实现DIB位图的快速余弦变换
****************************************************/
BOOL DCTDIB(HDIB hDIB)
{
	if (hDIB == NULL)
		return FALSE;

	// start wait cursor
	WaitCursorBegin();

	HDIB hDib = NULL;
	HDIB hNewDib = NULL;
	// we only convolute 24bpp DIB, so first convert DIB to 24bpp
	WORD wBitCount = DIBBitCount(hDIB);
	if (wBitCount != 24)
	{
		hNewDib = ConvertDIBFormat(hDIB, 24, NULL);
		hDib = CopyHandle(hNewDib);
	}
	else
	{
		hNewDib = CopyHandle(hDIB);
		hDib = CopyHandle(hDIB);
	}

	if (hNewDib == NULL && hDib == NULL)
	{
		WaitCursorEnd();
		return FALSE;
	}

	// process!
	LPBYTE lpSrcDIB = (LPBYTE)GlobalLock(hDib);
	LPBYTE lpDIB = (LPBYTE)GlobalLock(hNewDib);
	LPBYTE lpInput = FindDIBBits(lpSrcDIB);
	LPBYTE lpOutput = FindDIBBits(lpDIB);
	int nWidth = DIBWidth(lpSrcDIB);
	int nHeight = DIBHeight(lpSrcDIB);

	int w=1,h=1,wp=0,hp=0;
	while(w*2<=nWidth)
	{
		w*=2;
		wp++;
	}
	while(h*2<=nHeight)
	{
		h*=2;
		hp++;
	}
	int x,y;
	BYTE *lpPoints=new BYTE[nWidth*nHeight];
	GetPoints(nWidth,nHeight,lpInput,lpPoints);

	double *f=new double[w*h];
	double *W=new double[w*h];

	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			f[x+y*w]=Point(x,y);
		}
	}

	for(y=0;y<h;y++)
	{
		DCT(&f[w*y],&W[w*y],wp);
	}
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			f[x*h+y]=W[x+w*y];
		}
	}
	for(x=0;x<w;x++)
	{
		DCT(&f[x*h],&W[x*h],hp);
	}
	double a;
	memset(lpPoints,0,nWidth*nHeight);

	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			a=fabs(W[x*h+y]);
			if (a>255) a=255;
			Point(x,nHeight-y-1)=(BYTE)(a);
		}
	}
	delete f;
	delete W;
	PutPoints(nWidth,nHeight,lpOutput,lpPoints);
	delete lpPoints;

	// recover
	DWORD dwSize = GlobalSize(hDib);
	memcpy(lpSrcDIB, lpDIB, dwSize);
	GlobalUnlock(hDib);
	GlobalUnlock(hNewDib);
	if (wBitCount != 24)
	{
		hNewDib = ConvertDIBFormat(hDib, wBitCount, NULL);

		lpSrcDIB = (LPBYTE)GlobalLock(hDIB);
		lpDIB = (LPBYTE)GlobalLock(hNewDib);
		dwSize = GlobalSize(hNewDib);
		memcpy(lpSrcDIB, lpDIB, dwSize);
		GlobalUnlock(hDIB);
		GlobalUnlock(hNewDib);
	}
	else
	{
		lpSrcDIB = (LPBYTE)GlobalLock(hDIB);
		lpDIB = (LPBYTE)GlobalLock(hDib);
		dwSize = GlobalSize(hDib);
		memcpy(lpSrcDIB, lpDIB, dwSize);
		GlobalUnlock(hDIB);
		GlobalUnlock(hDib);
	}

	// cleanup
	GlobalFree(hDib);
	GlobalFree(hNewDib);

	// return
	WaitCursorEnd();
	return TRUE;
}

/****************************************************
	WALhDIB()

	参数：

		hDIB为输入的DIB句柄

	返回值：

		成功为TRUE；失败为FALSE

	说明：

		本函数实现DIB位图的快速沃尔什-哈达玛变换
****************************************************/
BOOL WALhDIB(HDIB hDIB)
{
	if (hDIB == NULL)
		return FALSE;

	// start wait cursor
	WaitCursorBegin();

	HDIB hDib = NULL;
	HDIB hNewDib = NULL;
	// we only convolute 24bpp DIB, so first convert DIB to 24bpp
	WORD wBitCount = DIBBitCount(hDIB);
	if (wBitCount != 24)
	{
		hNewDib = ConvertDIBFormat(hDIB, 24, NULL);
		hDib = CopyHandle(hNewDib);
	}
	else
	{
		hNewDib = CopyHandle(hDIB);
		hDib = CopyHandle(hDIB);
	}

	if (hNewDib == NULL && hDib == NULL)
	{
		WaitCursorEnd();
		return FALSE;
	}

	// process!
	LPBYTE lpSrcDIB = (LPBYTE)GlobalLock(hDib);
	LPBYTE lpDIB = (LPBYTE)GlobalLock(hNewDib);
	LPBYTE lpInput = FindDIBBits(lpSrcDIB);
	LPBYTE lpOutput = FindDIBBits(lpDIB);
	int nWidth = DIBWidth(lpSrcDIB);
	int nHeight = DIBHeight(lpSrcDIB);

	int w=1,h=1,wp=0,hp=0;
	while(w*2<=nWidth)
	{
		w*=2;
		wp++;
	}
	while(h*2<=nHeight)
	{
		h*=2;
		hp++;
	}
	int x,y;
	BYTE *lpPoints=new BYTE[nWidth*nHeight];
	GetPoints(nWidth,nHeight,lpInput,lpPoints);

	double *f=new double[w*h];
	double *W=new double[w*h];

	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			f[x+y*w]=Point(x,y);
		}
	}

	for(y=0;y<h;y++)
	{
		WALh(f+w*y,W+w*y,wp);
	}
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			f[x*h+y]=W[x+w*y];
		}
	}
	for(x=0;x<w;x++)
	{
		WALh(f+x*h,W+x*h,hp);
	}
	double a;
	memset(lpPoints,0,nWidth*nHeight);

	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			a=fabs(W[x*h+y]*1000);
			if (a>255) a=255;
			Point(x,nHeight-y-1)=(BYTE)a;
		}
	}
	delete f;
	delete W;
	PutPoints(nWidth,nHeight,lpOutput,lpPoints);
	delete lpPoints;

	// recover
	DWORD dwSize = GlobalSize(hDib);
	memcpy(lpSrcDIB, lpDIB, dwSize);
	GlobalUnlock(hDib);
	GlobalUnlock(hNewDib);
	if (wBitCount != 24)
	{
		hNewDib = ConvertDIBFormat(hDib, wBitCount, NULL);

		lpSrcDIB = (LPBYTE)GlobalLock(hDIB);
		lpDIB = (LPBYTE)GlobalLock(hNewDib);
		dwSize = GlobalSize(hNewDib);
		memcpy(lpSrcDIB, lpDIB, dwSize);
		GlobalUnlock(hDIB);
		GlobalUnlock(hNewDib);
	}
	else
	{
		lpSrcDIB = (LPBYTE)GlobalLock(hDIB);
		lpDIB = (LPBYTE)GlobalLock(hDib);
		dwSize = GlobalSize(hDib);
		memcpy(lpSrcDIB, lpDIB, dwSize);
		GlobalUnlock(hDIB);
		GlobalUnlock(hDib);
	}

	// cleanup
	GlobalFree(hDib);
	GlobalFree(hNewDib);

	// return
	WaitCursorEnd();
	return TRUE;
}

