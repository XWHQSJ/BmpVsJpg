// Jpeg.cpp: implementation of the CJpeg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>

#include "Jpeg.h"

#ifdef __cplusplus
	extern "C" {
#endif // __cplusplus

#include "jpeglib.h"

#ifdef __cplusplus
	}
#endif // __cplusplus

//
//
//

/*
 * <setjmp.h> is used for the optional error recovery mechanism shown in
 * the second part of the example.
 */

#include <setjmp.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// error handler, to avoid those pesky exit(0)'s

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

//
//
//

METHODDEF(void) my_error_exit (j_common_ptr cinfo);

//
//	to handle fatal errors.
//	the original JPEG code will just exit(0). can't really
//	do that in Windows....
//

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Always display the message. */
	MessageBox(GetActiveWindow(),buffer,"JPEG Fatal Error",MB_ICONSTOP);


	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

// store a scanline to our data buffer
void j_putRGBScanline(BYTE *jpegline, 
						 int widthPix,
						 BYTE *outBuf,
						 int row);

void j_putGrayScanlineToRGB(BYTE *jpegline, 
						 int widthPix,
						 BYTE *outBuf,
						 int row);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJpeg::CJpeg()
{
	m_strJPEGError = "No Error"; // yet
	m_pDib = NULL;
}

CJpeg::CJpeg(CDib *pDib)
{
	m_strJPEGError = "No Error"; // yet
	m_pDib = NULL;
	SetDib(pDib);
}

CJpeg::~CJpeg()
{
	if (m_pDib != NULL)
		delete m_pDib;
}

// free allocate memory
void CJpeg::FreeBuffer(BYTE *Buffer)
{
	delete[] Buffer;
}

// get error string
CString CJpeg::GetErrorString()
{
	return m_strJPEGError;
}

// load gif file
BOOL CJpeg::Load(LPCSTR lpstrFileName)
{
	UINT uWidth, uHeight, uWidthDW;

	// read the GIF to a packed buffer of RGB bytes
	BYTE *lpTmpBuffer = ReadJPEGFile(lpstrFileName, &uWidth, &uHeight);
	if (lpTmpBuffer == NULL)
		return FALSE;

	// do this before DWORD-alignment!!!
	// swap red and blue for display
	BGRFromRGB(lpTmpBuffer, uWidth, uHeight);

	// now DWORD-align for display
	BYTE *lpBuffer = MakeDwordAlign(lpTmpBuffer, uWidth, uHeight, &uWidthDW);
	FreeBuffer(lpTmpBuffer);

	// flip for display
	VertFlipBuf(lpBuffer, uWidthDW, uHeight);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = uWidth;
	bmiHeader.biHeight = uHeight;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = 0;
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

    // Allocate enough memory for the new CF_DIB, and copy bits 
	DWORD dwHeaderSize = sizeof(BITMAPINFOHEADER);
	DWORD dwBitsSize = WIDTHBYTES(uWidth*24) * uHeight;
    HDIB hDIB = GlobalAlloc(GHND, dwHeaderSize + dwBitsSize); 
	if (hDIB == NULL)
		return FALSE;

    LPBYTE lpDIB = (LPBYTE)GlobalLock(hDIB); 
    memcpy(lpDIB, (LPBYTE)&bmiHeader, dwHeaderSize); 
    memcpy(FindDIBBits((LPBYTE)lpDIB), lpBuffer, dwBitsSize); 
	FreeBuffer(lpBuffer);

	if (m_pDib != NULL)
		delete m_pDib;

	m_pDib = new CDib();
	m_pDib->Attach(hDIB);

	return TRUE;
}

// save Jpeg file
BOOL CJpeg::Save(LPCSTR lpstrFileName, CDib* pDib, BOOL bColor, int nQuality)
{
	if (pDib == NULL)
		pDib = m_pDib;
	if (pDib == NULL)
		return FALSE;

	HDIB hDib = CopyHandle(pDib->GetHandle());
	if (hDib == NULL)
		return FALSE;

	CDib* pDibTmp = new CDib;
	pDibTmp->Attach(hDib);

	if (pDibTmp->GetBitCount() != 24)
		pDibTmp->ConvertFormat(24);

	UINT uWidth  = pDibTmp->GetWidth();
	UINT uHeight = pDibTmp->GetHeight();

	// convert from DIB format (DWORD aligned, vertically flipped, red and blue swapped)
	BYTE* tmp = ClearDwordAlign(pDibTmp->GetBitsPtr(),
									uWidth,
									WIDTHBYTES(uWidth * 24),
									uHeight);
	if (tmp == NULL)
		return FALSE;

	// convert from DIB
	VertFlipBuf(tmp, uWidth*3, uHeight);

	BGRFromRGB(tmp, uWidth, uHeight);

	BOOL bSuccess = WriteJPEGFile(lpstrFileName,
							tmp,
							uWidth, 
							uHeight,
							bColor,
							nQuality);

	delete pDibTmp;
	FreeBuffer(tmp);

	return bSuccess;
}

//
// copies BYTE buffer into DWORD-aligned BYTE buffer
// return addr of new buffer
//
BYTE * CJpeg::MakeDwordAlign(BYTE *dataBuf,
								 UINT widthPix,				// pixels!!
								 UINT height,
								 UINT *uiOutWidthBytes)		// bytes!!!
{
	////////////////////////////////////////////////////////////
	// what's going on here? this certainly means trouble 
	if (dataBuf==NULL)
		return NULL;

	////////////////////////////////////////////////////////////
	// how big is the smallest DWORD-aligned buffer that we can use?
	UINT uiWidthBytes;
	uiWidthBytes = WIDTHBYTES(widthPix * 24);

	DWORD dwNewsize=(DWORD)((DWORD)uiWidthBytes * 
							(DWORD)height);
	BYTE *pNew;

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	pNew=(BYTE *)new BYTE[dwNewsize];
	if (pNew==NULL) {
		return NULL;
	}
	
	////////////////////////////////////////////////////////////
	// copy row-by-row
	UINT uiInWidthBytes = widthPix * 3;
	UINT uiCount;
	for (uiCount=0;uiCount < height;uiCount++) 
	{
		BYTE * bpInAdd;
		BYTE * bpOutAdd;
		ULONG lInOff;
		ULONG lOutOff;

		lInOff=uiInWidthBytes * uiCount;
		lOutOff=uiWidthBytes * uiCount;

		bpInAdd= dataBuf + lInOff;
		bpOutAdd= pNew + lOutOff;

		memcpy(bpOutAdd,bpInAdd,uiInWidthBytes);
	}

	*uiOutWidthBytes=uiWidthBytes;
	return pNew;
}

//
//	vertically flip a buffer 
//	note, this operates on a buffer of widthBytes bytes, not pixels!!!
//
BOOL CJpeg::VertFlipBuf(BYTE* inbuf, UINT widthBytes, UINT height)
{   
	BYTE  *tb1;
	BYTE  *tb2;

	if (inbuf==NULL)
		return FALSE;

	UINT bufsize;

	bufsize=widthBytes;

	tb1= (BYTE *)new BYTE[bufsize];
	if (tb1==NULL) 
	{
		return FALSE;
	}

	tb2= (BYTE *)new BYTE [bufsize];
	if (tb1==NULL) 
	{
		return FALSE;
	}
	
	UINT row_cnt;     
	ULONG off1=0;
	ULONG off2=0;

	for (row_cnt=0;row_cnt<(height+1)/2;row_cnt++) 
	{
		off1=row_cnt*bufsize;
		off2=((height-1)-row_cnt)*bufsize;   
		
		memcpy(tb1,inbuf+off1,bufsize);
		memcpy(tb2,inbuf+off2,bufsize);	
		memcpy(inbuf+off1,tb2,bufsize);
		memcpy(inbuf+off2,tb1,bufsize);
	}	

	delete [] tb1;
	delete [] tb2;

	return TRUE;
}        

//
//	swap Rs and Bs
//
//	Note! this does its stuff on buffers with a whole number of pixels
//	per data row!!
//
BOOL CJpeg::BGRFromRGB(BYTE *buf, UINT widthPix, UINT height)
{
	if (buf==NULL)
		return FALSE;

	UINT col, row;
	for (row=0;row<height;row++) 
	{
		for (col=0;col<widthPix;col++) 
		{
			LPBYTE pRed, pGrn, pBlu;
			pRed = buf + row * widthPix * 3 + col * 3;
			pGrn = buf + row * widthPix * 3 + col * 3 + 1;
			pBlu = buf + row * widthPix * 3 + col * 3 + 2;

			// swap red and blue
			BYTE tmp;
			tmp = *pRed;
			*pRed = *pBlu;
			*pBlu = tmp;
		}
	}
	return TRUE;
}

BYTE* CJpeg::ClearDwordAlign(BYTE *inBuf,
							UINT widthPix, 
							UINT widthBytes,
							UINT height)
{
	if (inBuf==NULL)
		return FALSE;


	BYTE *tmp;
	tmp=(BYTE *)new BYTE[height * widthPix * 3];
	if (tmp==NULL)
		return NULL;

	UINT row;

	for (row=0;row<height;row++) 
	{
		memcpy((tmp+row * widthPix * 3), 
				(inBuf + row * widthBytes), 
				widthPix * 3);
	}

	return tmp;
}



//读取JPEG文件
//参数：
//lpstrFileName——包含JPEG文件的全路径名
//uWidth——图像的宽度
//uHeight——图像的高度
//返回值为解压后的数据缓冲区指针

BYTE* CJpeg::ReadJPEGFile(LPCSTR lpstrFileName, UINT *uWidth, UINT *uHeight)
{
	*uWidth=0;
	*uHeight=0;

	
	//定义JPEG文件的解压信息
	struct jpeg_decompress_struct cinfo;
	
	//定义JPEG文件的错误信息
	struct my_error_mgr jerr;
	
	//定义缓冲区
	FILE * infile;		
	JSAMPARRAY buffer;	
	int row_stride;		
	char buf[250];

	
    //打开JPEG文件
	if ((infile = fopen(lpstrFileName, "rb")) == NULL) 
	{
		sprintf(buf, "JPEG :\nCan't open %s\n", lpstrFileName);
		m_strJPEGError = buf;
		return NULL;
	}

	
    //为JPEG文件解压对象分配内存并对其初始化

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;



	if (setjmp(jerr.setjmp_buffer)) 
	{
		

		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return NULL;
	}


	jpeg_create_decompress(&cinfo);


    //设定数据源 
	jpeg_stdio_src(&cinfo, infile);
    //读取JPEG文件参数

	(void) jpeg_read_header(&cinfo, TRUE);

      //开始解压
	(void) jpeg_start_decompress(&cinfo);
	
	BYTE *dataBuf;

	
	dataBuf=(BYTE *)new BYTE[cinfo.output_width * 3 * cinfo.output_height];
	if (dataBuf==NULL) 
	{

		m_strJPEGError = "JpegFile :\nOut of memory";

		jpeg_destroy_decompress(&cinfo);
		
		fclose(infile);

		return NULL;
	}


	*uWidth = cinfo.output_width;
	*uHeight = cinfo.output_height;
	

	row_stride = cinfo.output_width * cinfo.output_components;

	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	

	//读取扫描线
	while (cinfo.output_scanline < cinfo.output_height) 
	{
	
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
	
		if (cinfo.out_color_components==3) 
		{
			
			j_putRGBScanline(buffer[0], 
							*uWidth,
							dataBuf,
							cinfo.output_scanline-1);

		} 
		else if (cinfo.out_color_components==1) 
		{

		
			j_putGrayScanlineToRGB(buffer[0], 
								*uWidth,
								dataBuf,
								cinfo.output_scanline-1);

		}

	}


     //完成解压
	(void) jpeg_finish_decompress(&cinfo);

    //释放JPEG解压对象

	jpeg_destroy_decompress(&cinfo);

	
	fclose(infile);

	
	return dataBuf;
}

BOOL CJpeg::WriteJPEGFile(LPCSTR lpstrFileName, 
						BYTE *dataBuf,
						UINT widthPix,
						UINT height,
						BOOL color, 
						int quality)
{
	if (dataBuf==NULL)
		return FALSE;
	if (widthPix==0)
		return FALSE;
	if (height==0)
		return FALSE;

	LPBYTE tmp;
	if (!color) 
	{
		tmp = (BYTE*)new BYTE[widthPix*height];
		if (tmp==NULL) 
		{
			m_strJPEGError = "Memory error";
			return FALSE;
		}

		UINT row,col;
		for (row=0;row<height;row++) 
		{
			for (col=0;col<widthPix;col++) 
			{
				LPBYTE pRed, pGrn, pBlu;
				pRed = dataBuf + row * widthPix * 3 + col * 3;
				pGrn = dataBuf + row * widthPix * 3 + col * 3 + 1;
				pBlu = dataBuf + row * widthPix * 3 + col * 3 + 2;

				// 计算图像亮度值
				int lum = (int)(.299 * (double)(*pRed) + .587 * (double)(*pGrn) + .114 * (double)(*pBlu));
				LPBYTE pGray;
				pGray = tmp + row * widthPix + col;
				*pGray = (BYTE)lum;
			}
		}
	}
    //定义压缩信息
	struct jpeg_compress_struct cinfo;
     //定义缓冲区
	FILE * outfile;			
	int row_stride;		
    //定义错误信息
	struct my_error_mgr jerr;

    //为JPEG文件压缩对象分配内存并对其初始化
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;


	if (setjmp(jerr.setjmp_buffer)) 
	{
		

		jpeg_destroy_compress(&cinfo);
		fclose(outfile);

		if (!color) 
		{
			delete [] tmp;
		}
		return FALSE;
	}


	jpeg_create_compress(&cinfo);

	
      //打开文件并设定数据目标 
	if ((outfile = fopen(lpstrFileName, "wb")) == NULL) 
	{
		char buf[250];
		sprintf(buf, "JpegFile :\nCan't open %s\n", lpstrFileName);
		m_strJPEGError = buf;
		return FALSE;
	}

	jpeg_stdio_dest(&cinfo, outfile);

	
	//设置压缩参数
	cinfo.image_width = widthPix; 	
	cinfo.image_height = height;
	if (color) 
	{
		cinfo.input_components = 3;	
		cinfo.in_color_space = JCS_RGB; 	
	} 
	else 
	{
		cinfo.input_components = 1;	
		cinfo.in_color_space = JCS_GRAYSCALE; 
	}

   jpeg_set_defaults(&cinfo);
  
   jpeg_set_quality(&cinfo, quality, TRUE );

  
  //开始压缩
  jpeg_start_compress(&cinfo, TRUE);

  
  //写入扫描线
  row_stride = widthPix * 3;

  while (cinfo.next_scanline < cinfo.image_height) 
  {
   
	LPBYTE outRow;
	if (color) 
	{
		outRow = dataBuf + (cinfo.next_scanline * widthPix * 3);
	} 
	else 
	{
		outRow = tmp + (cinfo.next_scanline * widthPix);
	}

    (void) jpeg_write_scanlines(&cinfo, &outRow, 1);
  }

  
  //完成压缩

  jpeg_finish_compress(&cinfo);

 
  fclose(outfile);

  
  //释放压缩对象
  jpeg_destroy_compress(&cinfo);

  if (!color)
	  delete [] tmp;
  

  return TRUE;
}

//
//	stash a scanline
//

void j_putRGBScanline(BYTE *jpegline, 
					 int widthPix,
					 BYTE *outBuf,
					 int row)
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) {
		BYTE iRed, iBlu, iGrn;
		LPBYTE oRed, oBlu, oGrn;

		iRed = *(jpegline + count * 3 + 0);
		iGrn = *(jpegline + count * 3 + 1);
		iBlu = *(jpegline + count * 3 + 2);

		oRed = outBuf + offset + count * 3 + 0;
		oGrn = outBuf + offset + count * 3 + 1;
		oBlu = outBuf + offset + count * 3 + 2;

		*oRed = iRed;
		*oGrn = iGrn;
		*oBlu = iBlu;
	}
}

//
//	stash a gray scanline
//

void j_putGrayScanlineToRGB(BYTE *jpegline, 
							 int widthPix,
							 BYTE *outBuf,
							 int row)
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) 
	{
		BYTE iGray;
		LPBYTE oRed, oBlu, oGrn;

		// get our grayscale value
		iGray = *(jpegline + count);

		oRed = outBuf + offset + count * 3;
		oGrn = outBuf + offset + count * 3 + 1;
		oBlu = outBuf + offset + count * 3 + 2;

		*oRed = iGray;
		*oGrn = iGray;
		*oBlu = iGray;
	}
}
