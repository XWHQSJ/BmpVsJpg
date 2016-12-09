// Jpeg.h: interface for the CJpeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JPEG_H__63087660_0165_11D3_9481_000021003EA5__INCLUDED_)
#define AFX_JPEG_H__63087660_0165_11D3_9481_000021003EA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dib.h"

class CJpeg  
{
public:
	CJpeg();
	CJpeg(CDib *pDib);
	virtual ~CJpeg();


public:
     //获取CDib
	CDib * GetDib()
	{
		return m_pDib;
	}
     //设置CDib
	BOOL SetDib(CDib *pDib)
	{
		if (pDib == NULL)
			return FALSE;

		if (m_pDib != NULL)
			delete m_pDib;

		m_pDib = pDib->Clone();

		return (m_pDib != NULL);
	}
	// 载入JPEG文件
	BOOL Load(LPCSTR lpstrFileName);
	// 存储JPEG文件
	BOOL Save(LPCSTR lpstrFileName, 
			  CDib* pDib = NULL,
			  BOOL bColor = TRUE,
			  int nQuality = 75);
	// 获取错误信息
	CString GetErrorString();

private:
	
	//读取JPEG文件到缓冲区
	BYTE* ReadJPEGFile(LPCSTR lpstrFileName, UINT *uWidth, UINT *uHeight);
	
	//将位图文件存储为JPEG文件
	BOOL WriteJPEGFile(LPCTSTR lpstrFileName,	// path
					   BYTE *dataBuf,			// RGB buffer
					   UINT width,				// pixels
					   UINT height,				// rows
					   BOOL color,				// TRUE = RGB
												// FALSE = Grayscale
    				   int quality);			// 0 - 100

	////////////////////////////////////////////////////////////////
	// allocates a DWORD-aligned buffer, copies data buffer
	// caller is responsible for delete []'ing the buffer

	BYTE* MakeDwordAlign(BYTE *dataBuf,			// input buf
						 UINT widthPix,				// input pixels
						 UINT height,				// lines
						 UINT *uiOutWidthBytes);	// new width bytes


	void FreeBuffer(BYTE *Buffer);
	////////////////////////////////////////////////////////////////
	// if you have a DWORD aligned buffer, this will copy the
	// RGBs out of it into a new buffer. new width is widthPix * 3 bytes
	// caller is responsible for delete []'ing the buffer
	
	BYTE *ClearDwordAlign(BYTE *inBuf,				// input buf
						UINT widthPix,				// input size
						UINT widthBytes,			// input size
						UINT height);
	BOOL VertFlipBuf(BYTE * inbuf,						// input buf
				   UINT widthBytes,							// input width bytes
				   UINT height);							// height
	BOOL BGRFromRGB(BYTE *buf,							// input buf
					UINT widthPix,								// width in pixels
					UINT height);								// lines

	// data
	CString m_strJPEGError;

//data
private:
	//CDib对象
	CDib*	m_pDib;
};

#endif // !defined(AFX_JPEG_H__63087660_0165_11D3_9481_000021003EA5__INCLUDED_)
