#ifndef _JPG_TO_BMP_H_
#define _JPG_TO_BMP_H_

#include <math.h>
#include "JPEG.h"


#define WIDTHBYTES(i)    ((i+31)/32*4)
#define PI 3.1415926535


//define return value of function
#define FUNC_OK 0
#define FUNC_MEMORY_ERROR 1
#define FUNC_FILE_ERROR 2
#define FUNC_FORMAT_ERROR 3


class CJpgToBmp
{
public:
	CJpgToBmp(void);
	virtual ~CJpgToBmp(void);

public:
	
	BOOL LoadJpegFile(char *JpegFileName, char *BmpFileName);
	void showerror(int funcret);
	int  InitTag();
	void InitTable();
	int  Decode();
	int  DecodeMCUBlock();
	int  HufBlock(unsigned char dchufindex,unsigned char achufindex);
	int  DecodeElement();
	void IQtIZzMCUComponent(short flag);
	void IQtIZzBlock(short  *s ,int * d,short flag);
	void GetYUV(short flag);
	void StoreBuffer();
	BYTE ReadByte();
	void Initialize_Fast_IDCT();
	void Fast_IDCT(int * block);
	void idctrow(int * blk);
	void idctcol(int * blk);

private:
	BITMAPFILEHEADER   bf;
	BITMAPINFOHEADER   bi;
	HBITMAP            hBitmap;
	HGLOBAL            hImgData;
	DWORD              NumColors;
	DWORD              LineBytes;
	DWORD              ImgWidth;
	DWORD			   ImgHeight;
	unsigned int       PcxBytesPerLine;
	LPSTR              lpPtr;
	//////////////////////////////////////////////////
	//variables used in jpeg function
	short			SampRate_Y_H,SampRate_Y_V;
	short			SampRate_U_H,SampRate_U_V;
	short			SampRate_V_H,SampRate_V_V;
	short			H_YtoU,V_YtoU,H_YtoV,V_YtoV;
	short			Y_in_MCU,U_in_MCU,V_in_MCU;
	unsigned char   *lpJpegBuf;
	unsigned char   *lp;
	short			qt_table[3][64];
	short			comp_num;
	BYTE			comp_index[3];
	BYTE		    YDcIndex,YAcIndex,UVDcIndex,UVAcIndex;
	BYTE			HufTabIndex;
	short		    *YQtTable,*UQtTable,*VQtTable;
	short		    code_pos_table[4][16],code_len_table[4][16];
	unsigned short	code_value_table[4][256];
	unsigned short	huf_max_value[4][16],huf_min_value[4][16];
	short			BitPos,CurByte;
	short			rrun,vvalue;
	short			MCUBuffer[10*64];
	int				QtZzMCUBuffer[10*64];
	short			BlockBuffer[64];
	short			ycoef,ucoef,vcoef;
	BOOL			IntervalFlag;
	short			interval;
	int				Y[4*64],U[4*64],V[4*64];
	DWORD		    sizei,sizej;
	short 			restart;
};





#endif