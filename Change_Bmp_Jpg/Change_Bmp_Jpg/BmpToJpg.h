#ifndef _BMP_TO_JPG_H_
#define _BMP_TO_JPG_H_
#pragma once

class CBmpToJpg
{
public:
	CBmpToJpg(void);
	virtual ~CBmpToJpg(void);
	bool AddFont(const char *pSrcName, const char *pDstName, const char *pText, int iXPos, int iYPos);
	bool BMPToJPG(const char *pBmpFile, const char *pJpgFile, long iQuality);
	bool JPGToBMP(const char *pJpgFile, const char *pBmpFile);
private:
	WCHAR *BStr2WStr(const char *pSource, int &iDstLen);
};

#endif