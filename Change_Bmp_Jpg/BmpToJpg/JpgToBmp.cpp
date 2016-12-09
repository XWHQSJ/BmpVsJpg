#include "stdafx.h"
#include "JpgToBmp.h"



BYTE			And[9] = { 0,1,3,7,0xf,0x1f,0x3f,0x7f,0xff };
static  long	iclip[1024];
static  long	*iclp;


CJpgToBmp::CJpgToBmp(void)
{
	hBitmap = NULL;
	hImgData = NULL;
	ImgWidth = 0;
	ImgHeight = 0;
	interval = 0;
}

CJpgToBmp::~CJpgToBmp(void)
{

}

int CJpgToBmp::LoadJpegFile(char *JpegFileName, char *BmpFileName)
{
	HFILE			   hfjpg;
	DWORD 		       ImgSize;
	DWORD              JpegBufSize;
	HFILE              hfbmp;
	HGLOBAL			   hJpegBuf;
	int				   funcret;
	LPBITMAPINFOHEADER lpImgData;

	if ((hfjpg = _lopen(JpegFileName, OF_READ)) == HFILE_ERROR)
	{
		showerror(FUNC_FILE_ERROR);
		return 0;
	}
	//get jpg file length
	JpegBufSize = _llseek(hfjpg, 0L, SEEK_END);
	//rewind to the beginning of the file
	_llseek(hfjpg, 0L, SEEK_SET);

	if ((hJpegBuf = GlobalAlloc(GHND, JpegBufSize)) == NULL)
	{
		_lclose(hfjpg);
		showerror(FUNC_MEMORY_ERROR);
		return 0;
	}
	lpJpegBuf = (unsigned char  *)GlobalLock(hJpegBuf);
	_hread(hfjpg, (unsigned char  *)lpJpegBuf, JpegBufSize);
	_lclose(hfjpg);


	InitTable();

	if ((funcret = InitTag()) != FUNC_OK)
	{
		GlobalUnlock(hJpegBuf);
		GlobalFree(hJpegBuf);
		showerror(funcret);
		return 0;
	}
	//create new bitmapfileheader and bitmapinfoheader
	memset((char *)&bf, 0, sizeof(BITMAPFILEHEADER));
	memset((char *)&bi, 0, sizeof(BITMAPINFOHEADER));

	bi.biSize = (DWORD)sizeof(BITMAPINFOHEADER);
	bi.biWidth = (LONG)(ImgWidth);
	bi.biHeight = (LONG)(ImgHeight);
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	bi.biCompression = BI_RGB;
	NumColors = 0;
	LineBytes = (DWORD)WIDTHBYTES(bi.biWidth*bi.biBitCount);
	ImgSize = (DWORD)LineBytes*bi.biHeight;

	bf.bfType = 0x4d42;
	bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + NumColors * sizeof(RGBQUAD) + ImgSize;
	bf.bfOffBits = (DWORD)(NumColors * sizeof(RGBQUAD) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

	if ((hImgData = GlobalAlloc(GHND, ImgSize)) == NULL)
	{
		GlobalUnlock(hJpegBuf);
		GlobalFree(hJpegBuf);
		showerror(FUNC_MEMORY_ERROR);
		return 0;
	}
	lpImgData = (LPBITMAPINFOHEADER)GlobalLock(hImgData);
	lpPtr = (char *)lpImgData;

	if ((SampRate_Y_H == 0) || (SampRate_Y_V == 0))
	{
		GlobalUnlock(hJpegBuf);
		GlobalFree(hJpegBuf);
		GlobalUnlock(hImgData);
		GlobalFree(hImgData);
		hImgData = NULL;
		showerror(FUNC_FORMAT_ERROR);
		return 0;
	}

	funcret = Decode();

	if (funcret == FUNC_OK)
	{

		hfbmp = _lcreat(BmpFileName, 0);
		_lwrite(hfbmp, (LPSTR)&bf, sizeof(BITMAPFILEHEADER)); //写BMP文件头
		_lwrite(hfbmp, (LPSTR)&bi, sizeof(BITMAPINFOHEADER)); //写BMP文件信息
		_lwrite(hfbmp, (LPSTR)lpImgData, ImgSize);            //写BMP位图数据
		_lclose(hfbmp);
		GlobalUnlock(hJpegBuf);
		GlobalFree(hJpegBuf);
		GlobalUnlock(hImgData);
		return 1;
	}
	else
	{
		GlobalUnlock(hJpegBuf);
		GlobalFree(hJpegBuf);
		GlobalUnlock(hImgData);
		GlobalFree(hImgData);
		hImgData = NULL;
		showerror(funcret);
		return 0;
	}
}
/////////////////////////////////////////////////
void CJpgToBmp::showerror(int funcret)
{
	switch (funcret)
	{
	case FUNC_MEMORY_ERROR:
		printf("Error alloc memory!\n");
		exit(1);
		break;
	case FUNC_FILE_ERROR:
		printf("File not found!!\n");
		exit(1);
		break;
	case FUNC_FORMAT_ERROR:
		printf("File format error!\n");
		exit(1);
		break;
	}
}
////////////////////////////////////////////////////////////////////////////////
int CJpgToBmp::InitTag()
{
	BOOL finish = FALSE;
	BYTE id;
	short  llength;
	short  i, j, k;
	short  huftab1, huftab2;
	short  huftabindex;
	BYTE hf_table_index;
	BYTE qt_table_index;
	BYTE comnum;

	unsigned char  *lptemp;
	short  ccount;

	lp = lpJpegBuf + 2;

	while (!finish)
	{
		id = *(lp + 1);
		lp += 2;
		switch (id)
		{
		case M_APP0:
			llength = MAKEWORD(*(lp + 1), *lp);
			lp += llength;
			break;
		case M_DQT:
			llength = MAKEWORD(*(lp + 1), *lp);
			qt_table_index = (*(lp + 2)) & 0x0f;
			lptemp = lp + 3;
			if (llength < 80)
			{
				for (i = 0; i < 64; i++)
					qt_table[qt_table_index][i] = (short)*(lptemp++);
			}
			else
			{
				for (i = 0; i < 64; i++)
					qt_table[qt_table_index][i] = (short)*(lptemp++);
				qt_table_index = (*(lptemp++)) & 0x0f;
				for (i = 0; i < 64; i++)
					qt_table[qt_table_index][i] = (short)*(lptemp++);
			}
			lp += llength;
			break;
		case M_SOF0:
			llength = MAKEWORD(*(lp + 1), *lp);
			ImgHeight = MAKEWORD(*(lp + 4), *(lp + 3));
			ImgWidth = MAKEWORD(*(lp + 6), *(lp + 5));
			comp_num = *(lp + 7);
			if ((comp_num != 1) && (comp_num != 3))
				return FUNC_FORMAT_ERROR;
			if (comp_num == 3)
			{
				comp_index[0] = *(lp + 8);
				SampRate_Y_H = (*(lp + 9)) >> 4;
				SampRate_Y_V = (*(lp + 9)) & 0x0f;
				YQtTable = (short *)qt_table[*(lp + 10)];

				comp_index[1] = *(lp + 11);
				SampRate_U_H = (*(lp + 12)) >> 4;
				SampRate_U_V = (*(lp + 12)) & 0x0f;
				UQtTable = (short *)qt_table[*(lp + 13)];

				comp_index[2] = *(lp + 14);
				SampRate_V_H = (*(lp + 15)) >> 4;
				SampRate_V_V = (*(lp + 15)) & 0x0f;
				VQtTable = (short *)qt_table[*(lp + 16)];
			}
			else
			{
				comp_index[0] = *(lp + 8);
				SampRate_Y_H = (*(lp + 9)) >> 4;
				SampRate_Y_V = (*(lp + 9)) & 0x0f;
				YQtTable = (short *)qt_table[*(lp + 10)];

				comp_index[1] = *(lp + 8);
				SampRate_U_H = 1;
				SampRate_U_V = 1;
				UQtTable = (short *)qt_table[*(lp + 10)];

				comp_index[2] = *(lp + 8);
				SampRate_V_H = 1;
				SampRate_V_V = 1;
				VQtTable = (short *)qt_table[*(lp + 10)];
			}
			lp += llength;
			break;
		case M_DHT:
			llength = MAKEWORD(*(lp + 1), *lp);
			if (llength < 0xd0)
			{
				huftab1 = (short)(*(lp + 2)) >> 4;     //huftab1=0,1
				huftab2 = (short)(*(lp + 2)) & 0x0f;   //huftab2=0,1
				huftabindex = huftab1 * 2 + huftab2;
				lptemp = lp + 3;
				for (i = 0; i < 16; i++)
					code_len_table[huftabindex][i] = (short)(*(lptemp++));
				j = 0;
				for (i = 0; i < 16; i++)
					if (code_len_table[huftabindex][i] != 0)
					{
						k = 0;
						while (k < code_len_table[huftabindex][i])
						{
							code_value_table[huftabindex][k + j] = (short)(*(lptemp++));
							k++;
						}
						j += k;
					}
				i = 0;
				while (code_len_table[huftabindex][i] == 0)
					i++;
				for (j = 0; j < i; j++)
				{
					huf_min_value[huftabindex][j] = 0;
					huf_max_value[huftabindex][j] = 0;
				}
				huf_min_value[huftabindex][i] = 0;
				huf_max_value[huftabindex][i] = code_len_table[huftabindex][i] - 1;
				for (j = i + 1; j < 16; j++)
				{
					huf_min_value[huftabindex][j] = (huf_max_value[huftabindex][j - 1] + 1) << 1;
					huf_max_value[huftabindex][j] = huf_min_value[huftabindex][j] + code_len_table[huftabindex][j] - 1;
				}
				code_pos_table[huftabindex][0] = 0;
				for (j = 1; j < 16; j++)
					code_pos_table[huftabindex][j] = code_len_table[huftabindex][j - 1] + code_pos_table[huftabindex][j - 1];
				lp += llength;
			}  //if
			else
			{
				hf_table_index = *(lp + 2);
				lp += 2;
				while (hf_table_index != 0xff)
				{
					huftab1 = (short)hf_table_index >> 4;     //huftab1=0,1
					huftab2 = (short)hf_table_index & 0x0f;   //huftab2=0,1
					huftabindex = huftab1 * 2 + huftab2;
					lptemp = lp + 1;
					ccount = 0;
					for (i = 0; i < 16; i++)
					{
						code_len_table[huftabindex][i] = (short)(*(lptemp++));
						ccount += code_len_table[huftabindex][i];
					}
					ccount += 17;
					j = 0;
					for (i = 0; i < 16; i++)
						if (code_len_table[huftabindex][i] != 0)
						{
							k = 0;
							while (k < code_len_table[huftabindex][i])
							{
								code_value_table[huftabindex][k + j] = (short)(*(lptemp++));
								k++;
							}
							j += k;
						}
					i = 0;
					while (code_len_table[huftabindex][i] == 0)
						i++;
					for (j = 0; j < i; j++)
					{
						huf_min_value[huftabindex][j] = 0;
						huf_max_value[huftabindex][j] = 0;
					}
					huf_min_value[huftabindex][i] = 0;
					huf_max_value[huftabindex][i] = code_len_table[huftabindex][i] - 1;
					for (j = i + 1; j < 16; j++)
					{
						huf_min_value[huftabindex][j] = (huf_max_value[huftabindex][j - 1] + 1) << 1;
						huf_max_value[huftabindex][j] = huf_min_value[huftabindex][j] + code_len_table[huftabindex][j] - 1;
					}
					code_pos_table[huftabindex][0] = 0;
					for (j = 1; j < 16; j++)
						code_pos_table[huftabindex][j] = code_len_table[huftabindex][j - 1] + code_pos_table[huftabindex][j - 1];
					lp += ccount;
					hf_table_index = *lp;
				}  //while
			}  //else
			break;
		case M_DRI:
			llength = MAKEWORD(*(lp + 1), *lp);
			restart = MAKEWORD(*(lp + 3), *(lp + 2));
			lp += llength;
			break;
		case M_SOS:
			llength = MAKEWORD(*(lp + 1), *lp);
			comnum = *(lp + 2);
			if (comnum != comp_num)
				return FUNC_FORMAT_ERROR;
			lptemp = lp + 3;
			for (i = 0; i < comp_num; i++)
			{
				if (*lptemp == comp_index[0])
				{
					YDcIndex = (*(lptemp + 1)) >> 4;   //Y
					YAcIndex = ((*(lptemp + 1)) & 0x0f) + 2;
				}
				else {
					UVDcIndex = (*(lptemp + 1)) >> 4;   //U,V
					UVAcIndex = ((*(lptemp + 1)) & 0x0f) + 2;
				}
				lptemp += 2;
			}
			lp += llength;
			finish = TRUE;
			break;
		case M_EOI:
			return FUNC_FORMAT_ERROR;
			break;
		default:
			if ((id & 0xf0) != 0xd0)
			{
				llength = MAKEWORD(*(lp + 1), *lp);
				lp += llength;
			}
			else lp += 2;
			break;
		}  //switch
	} //while
	return FUNC_OK;
}
/////////////////////////////////////////////////////////////////
void CJpgToBmp::InitTable()
{
	short i, j;
	sizei = sizej = 0;
	ImgWidth = ImgHeight = 0;
	rrun = vvalue = 0;
	BitPos = 0;
	CurByte = 0;
	IntervalFlag = FALSE;
	restart = 0;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 64; j++)
			qt_table[i][j] = 0;
	comp_num = 0;
	HufTabIndex = 0;
	for (i = 0; i < 3; i++)
		comp_index[i] = 0;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 16; j++)
		{
			code_len_table[i][j] = 0;
			code_pos_table[i][j] = 0;
			huf_max_value[i][j] = 0;
			huf_min_value[i][j] = 0;
		}
	for (i = 0; i < 4; i++)
		for (j = 0; j < 256; j++)
			code_value_table[i][j] = 0;

	for (i = 0; i < 10 * 64; i++)
	{
		MCUBuffer[i] = 0;
		QtZzMCUBuffer[i] = 0;
	}
	for (i = 0; i < 4 * 64; i++)
	{
		Y[i] = 0;
		U[i] = 0;
		V[i] = 0;
	}
	for (i = 0; i < 64; i++)
		BlockBuffer[i] = 0;
	ycoef = ucoef = vcoef = 0;
}
/////////////////////////////////////////////////////////////////////////
int CJpgToBmp::Decode()
{
	int funcret;

	Y_in_MCU = SampRate_Y_H*SampRate_Y_V;
	U_in_MCU = SampRate_U_H*SampRate_U_V;
	V_in_MCU = SampRate_V_H*SampRate_V_V;
	H_YtoU = SampRate_Y_H / SampRate_U_H;
	V_YtoU = SampRate_Y_V / SampRate_U_V;
	H_YtoV = SampRate_Y_H / SampRate_V_H;
	V_YtoV = SampRate_Y_V / SampRate_V_V;
	Initialize_Fast_IDCT();
	while ((funcret = DecodeMCUBlock()) == FUNC_OK)
	{
		interval++;
		if ((restart) && (interval % restart == 0))
			IntervalFlag = TRUE;
		else
			IntervalFlag = FALSE;
		IQtIZzMCUComponent(0);
		IQtIZzMCUComponent(1);
		IQtIZzMCUComponent(2);
		GetYUV(0);
		GetYUV(1);
		GetYUV(2);
		StoreBuffer();
		sizej += SampRate_Y_H * 8;
		if (sizej >= ImgWidth)
		{
			sizej = 0;
			sizei += SampRate_Y_V * 8;
		}
		if ((sizej == 0) && (sizei >= ImgHeight))
			break;
	}
	return funcret;
}
/////////////////////////////////////////////////////////////////////////////////////////
void  CJpgToBmp::GetYUV(short flag)
{
	short	H, VV;
	short	i, j, k, h;
	int		*buf;
	int		*pQtZzMCU;

	switch (flag)
	{
	case 0:
		H = SampRate_Y_H;
		VV = SampRate_Y_V;
		buf = Y;
		pQtZzMCU = QtZzMCUBuffer;
		break;
	case 1:
		H = SampRate_U_H;
		VV = SampRate_U_V;
		buf = U;
		pQtZzMCU = QtZzMCUBuffer + Y_in_MCU * 64;
		break;
	case 2:
		H = SampRate_V_H;
		VV = SampRate_V_V;
		buf = V;
		pQtZzMCU = QtZzMCUBuffer + (Y_in_MCU + U_in_MCU) * 64;
		break;
	}
	for (i = 0; i < VV; i++)
		for (j = 0; j < H; j++)
			for (k = 0; k < 8; k++)
				for (h = 0; h < 8; h++)
					buf[(i * 8 + k)*SampRate_Y_H * 8 + j * 8 + h] = *pQtZzMCU++;
}
///////////////////////////////////////////////////////////////////////////////
void CJpgToBmp::StoreBuffer()
{
	short i, j;
	unsigned char  *lpbmp;
	unsigned char R, G, B;
	int y, u, v, rr, gg, bb;

	for (i = 0; i < SampRate_Y_V * 8; i++)
	{
		if ((sizei + i) < ImgHeight)
		{
			lpbmp = ((unsigned char *)lpPtr + (DWORD)(ImgHeight - sizei - i - 1)*LineBytes + sizej * 3);
			for (j = 0; j < SampRate_Y_H * 8; j++)
			{
				if ((sizej + j) < ImgWidth)
				{
					y = Y[i * 8 * SampRate_Y_H + j];
					u = U[(i / V_YtoU) * 8 * SampRate_Y_H + j / H_YtoU];
					v = V[(i / V_YtoV) * 8 * SampRate_Y_H + j / H_YtoV];
					rr = ((y << 8) + 18 * u + 367 * v) >> 8;
					gg = ((y << 8) - 159 * u - 220 * v) >> 8;
					bb = ((y << 8) + 411 * u - 29 * v) >> 8;
					R = (unsigned char)rr;
					G = (unsigned char)gg;
					B = (unsigned char)bb;
					if (rr & 0xffffff00) if (rr > 255) R = 255; else if (rr < 0) R = 0;
					if (gg & 0xffffff00) if (gg > 255) G = 255; else if (gg < 0) G = 0;
					if (bb & 0xffffff00) if (bb > 255) B = 255; else if (bb < 0) B = 0;
					*lpbmp++ = B;
					*lpbmp++ = G;
					*lpbmp++ = R;
				}
				else  break;
			}
		}
		else break;
	}
}
///////////////////////////////////////////////////////////////////////////////
int CJpgToBmp::DecodeMCUBlock()
{
	short *lpMCUBuffer;
	short i, j;
	int funcret;

	if (IntervalFlag)
	{
		lp += 2;
		ycoef = ucoef = vcoef = 0;
		BitPos = 0;
		CurByte = 0;
	}
	switch (comp_num)
	{
	case 3:
		lpMCUBuffer = MCUBuffer;
		for (i = 0; i < SampRate_Y_H*SampRate_Y_V; i++)  //Y
		{
			funcret = HufBlock(YDcIndex, YAcIndex);
			if (funcret != FUNC_OK)
				return funcret;
			BlockBuffer[0] = BlockBuffer[0] + ycoef;
			ycoef = BlockBuffer[0];
			for (j = 0; j < 64; j++)
				*lpMCUBuffer++ = BlockBuffer[j];
		}
		for (i = 0; i < SampRate_U_H*SampRate_U_V; i++)  //U
		{
			funcret = HufBlock(UVDcIndex, UVAcIndex);
			if (funcret != FUNC_OK)
				return funcret;
			BlockBuffer[0] = BlockBuffer[0] + ucoef;
			ucoef = BlockBuffer[0];
			for (j = 0; j < 64; j++)
				*lpMCUBuffer++ = BlockBuffer[j];
		}
		for (i = 0; i < SampRate_V_H*SampRate_V_V; i++)  //V
		{
			funcret = HufBlock(UVDcIndex, UVAcIndex);
			if (funcret != FUNC_OK)
				return funcret;
			BlockBuffer[0] = BlockBuffer[0] + vcoef;
			vcoef = BlockBuffer[0];
			for (j = 0; j < 64; j++)
				*lpMCUBuffer++ = BlockBuffer[j];
		}
		break;
	case 1:
		lpMCUBuffer = MCUBuffer;
		funcret = HufBlock(YDcIndex, YAcIndex);
		if (funcret != FUNC_OK)
			return funcret;
		BlockBuffer[0] = BlockBuffer[0] + ycoef;
		ycoef = BlockBuffer[0];
		for (j = 0; j < 64; j++)
			*lpMCUBuffer++ = BlockBuffer[j];
		for (i = 0; i < 128; i++)
			*lpMCUBuffer++ = 0;
		break;
	default:
		return FUNC_FORMAT_ERROR;
	}
	return FUNC_OK;
}
//int CJpgToBmp::HufBlock(unsigned char dchufindex, unsigned char achufindex)
//{
//	return 0;
//}
//////////////////////////////////////////////////////////////////
int CJpgToBmp::HufBlock(BYTE dchufindex, BYTE achufindex)
{
	short count = 0;
	short i;
	int funcret;

	//dc
	HufTabIndex = dchufindex;
	funcret = DecodeElement();
	if (funcret != FUNC_OK)
		return funcret;

	BlockBuffer[count++] = vvalue;
	//ac
	HufTabIndex = achufindex;
	while (count < 64)
	{
		funcret = DecodeElement();
		if (funcret != FUNC_OK)
			return funcret;
		if ((rrun == 0) && (vvalue == 0))
		{
			for (i = count; i < 64; i++)
				BlockBuffer[i] = 0;
			count = 64;
		}
		else
		{
			for (i = 0; i < rrun; i++)
				BlockBuffer[count++] = 0;
			BlockBuffer[count++] = vvalue;
		}
	}
	return FUNC_OK;
}
//////////////////////////////////////////////////////////////////////////////
int CJpgToBmp::DecodeElement()
{
	int thiscode, tempcode;
	unsigned short temp, valueex;
	short codelen;
	BYTE hufexbyte, runsize, tempsize, sign;
	BYTE newbyte, lastbyte;

	if (BitPos >= 1)
	{
		BitPos--;
		thiscode = (BYTE)CurByte >> BitPos;
		CurByte = CurByte&And[BitPos];
	}
	else
	{
		lastbyte = ReadByte();
		BitPos--;
		newbyte = CurByte&And[BitPos];
		thiscode = lastbyte >> 7;
		CurByte = newbyte;
	}
	codelen = 1;
	while ((thiscode < huf_min_value[HufTabIndex][codelen - 1]) ||
		(code_len_table[HufTabIndex][codelen - 1] == 0) ||
		(thiscode > huf_max_value[HufTabIndex][codelen - 1]))
	{
		if (BitPos >= 1)
		{
			BitPos--;
			tempcode = (BYTE)CurByte >> BitPos;
			CurByte = CurByte&And[BitPos];
		}
		else
		{
			lastbyte = ReadByte();
			BitPos--;
			newbyte = CurByte&And[BitPos];
			tempcode = (BYTE)lastbyte >> 7;
			CurByte = newbyte;
		}
		thiscode = (thiscode << 1) + tempcode;
		codelen++;
		if (codelen > 16)
			return FUNC_FORMAT_ERROR;
	}  //while
	temp = thiscode - huf_min_value[HufTabIndex][codelen - 1] + code_pos_table[HufTabIndex][codelen - 1];
	hufexbyte = (BYTE)code_value_table[HufTabIndex][temp];
	rrun = (short)(hufexbyte >> 4);
	runsize = hufexbyte & 0x0f;
	if (runsize == 0)
	{
		vvalue = 0;
		return FUNC_OK;
	}
	tempsize = runsize;
	if (BitPos >= runsize)
	{
		BitPos -= runsize;
		valueex = (BYTE)CurByte >> BitPos;
		CurByte = CurByte&And[BitPos];
	}
	else
	{
		valueex = CurByte;
		tempsize -= BitPos;
		while (tempsize > 8)
		{
			lastbyte = ReadByte();
			valueex = (valueex << 8) + (BYTE)lastbyte;
			tempsize -= 8;
		}  //while
		lastbyte = ReadByte();
		BitPos -= tempsize;
		valueex = (valueex << tempsize) + (lastbyte >> BitPos);
		CurByte = lastbyte&And[BitPos];
	}  //else
	sign = valueex >> (runsize - 1);
	if (sign)
		vvalue = valueex;
	else
	{
		valueex = valueex ^ 0xffff;
		temp = 0xffff << runsize;
		vvalue = -(short)(valueex^temp);
	}
	return FUNC_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
void CJpgToBmp::IQtIZzMCUComponent(short flag)
{
	short H, VV;
	short i, j;
	int *pQtZzMCUBuffer;
	short  *pMCUBuffer;

	switch (flag)
	{
	case 0:
		H = SampRate_Y_H;
		VV = SampRate_Y_V;
		pMCUBuffer = MCUBuffer;
		pQtZzMCUBuffer = QtZzMCUBuffer;
		break;
	case 1:
		H = SampRate_U_H;
		VV = SampRate_U_V;
		pMCUBuffer = MCUBuffer + Y_in_MCU * 64;
		pQtZzMCUBuffer = QtZzMCUBuffer + Y_in_MCU * 64;
		break;
	case 2:
		H = SampRate_V_H;
		VV = SampRate_V_V;
		pMCUBuffer = MCUBuffer + (Y_in_MCU + U_in_MCU) * 64;
		pQtZzMCUBuffer = QtZzMCUBuffer + (Y_in_MCU + U_in_MCU) * 64;
		break;
	}
	for (i = 0; i < VV; i++)
		for (j = 0; j < H; j++)
			IQtIZzBlock(pMCUBuffer + (i*H + j) * 64, pQtZzMCUBuffer + (i*H + j) * 64, flag);
}
//////////////////////////////////////////////////////////////////////////////////////////
void CJpgToBmp::IQtIZzBlock(short  *s, int * d, short flag)
{
	short i, j;
	short tag;
	short *pQt;
	int buffer2[8][8];
	int *buffer1;
	short offset;

	switch (flag)
	{
	case 0:
		pQt = YQtTable;
		offset = 128;
		break;
	case 1:
		pQt = UQtTable;
		offset = 0;
		break;
	case 2:
		pQt = VQtTable;
		offset = 0;
		break;
	}

	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
		{
			tag = Zig_Zag[i][j];
			buffer2[i][j] = (int)s[tag] * (int)pQt[tag];
		}
	buffer1 = (int *)buffer2;
	Fast_IDCT(buffer1);
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
			d[i * 8 + j] = buffer2[i][j] + offset;
}
///////////////////////////////////////////////////////////////////////////////
void CJpgToBmp::Fast_IDCT(int * block)
{
	short i;

	for (i = 0; i < 8; i++)
		idctrow(block + 8 * i);

	for (i = 0; i < 8; i++)
		idctcol(block + i);
}
///////////////////////////////////////////////////////////////////////////////
BYTE  CJpgToBmp::ReadByte()
{
	BYTE  i;

	i = *(lp++);
	if (i == 0xff)
		lp++;
	BitPos = 8;
	CurByte = i;
	return i;
}
///////////////////////////////////////////////////////////////////////
void CJpgToBmp::Initialize_Fast_IDCT()
{
	short i;

	iclp = iclip + 512;
	for (i = -512; i < 512; i++)
		iclp[i] = (i < -256) ? -256 : ((i > 255) ? 255 : i);
}
////////////////////////////////////////////////////////////////////////
void CJpgToBmp::idctrow(int * blk)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;
	//intcut
	if (!((x1 = blk[4] << 11) | (x2 = blk[6]) | (x3 = blk[2]) |
		(x4 = blk[1]) | (x5 = blk[7]) | (x6 = blk[5]) | (x7 = blk[3])))
	{
		blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] = blk[7] = blk[0] << 3;
		return;
	}
	x0 = (blk[0] << 11) + 128; // for proper rounding in the fourth stage 
							   //first stage
	x8 = W7*(x4 + x5);
	x4 = x8 + (W1 - W7)*x4;
	x5 = x8 - (W1 + W7)*x5;
	x8 = W3*(x6 + x7);
	x6 = x8 - (W3 - W5)*x6;
	x7 = x8 - (W3 + W5)*x7;
	//second stage
	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6*(x3 + x2);
	x2 = x1 - (W2 + W6)*x2;
	x3 = x1 + (W2 - W6)*x3;
	x1 = x4 + x6;
	x4 -= x6;
	x6 = x5 + x7;
	x5 -= x7;
	//third stage
	x7 = x8 + x3;
	x8 -= x3;
	x3 = x0 + x2;
	x0 -= x2;
	x2 = (181 * (x4 + x5) + 128) >> 8;
	x4 = (181 * (x4 - x5) + 128) >> 8;
	//fourth stage
	blk[0] = (x7 + x1) >> 8;
	blk[1] = (x3 + x2) >> 8;
	blk[2] = (x0 + x4) >> 8;
	blk[3] = (x8 + x6) >> 8;
	blk[4] = (x8 - x6) >> 8;
	blk[5] = (x0 - x4) >> 8;
	blk[6] = (x3 - x2) >> 8;
	blk[7] = (x7 - x1) >> 8;
}
//////////////////////////////////////////////////////////////////////////////
void CJpgToBmp::idctcol(int * blk)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;
	//intcut
	if (!((x1 = (blk[8 * 4] << 8)) | (x2 = blk[8 * 6]) | (x3 = blk[8 * 2]) |
		(x4 = blk[8 * 1]) | (x5 = blk[8 * 7]) | (x6 = blk[8 * 5]) | (x7 = blk[8 * 3])))
	{
		blk[8 * 0] = blk[8 * 1] = blk[8 * 2] = blk[8 * 3] = blk[8 * 4] = blk[8 * 5]
			= blk[8 * 6] = blk[8 * 7] = iclp[(blk[8 * 0] + 32) >> 6];
		return;
	}
	x0 = (blk[8 * 0] << 8) + 8192;
	//first stage
	x8 = W7*(x4 + x5) + 4;
	x4 = (x8 + (W1 - W7)*x4) >> 3;
	x5 = (x8 - (W1 + W7)*x5) >> 3;
	x8 = W3*(x6 + x7) + 4;
	x6 = (x8 - (W3 - W5)*x6) >> 3;
	x7 = (x8 - (W3 + W5)*x7) >> 3;
	//second stage
	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6*(x3 + x2) + 4;
	x2 = (x1 - (W2 + W6)*x2) >> 3;
	x3 = (x1 + (W2 - W6)*x3) >> 3;
	x1 = x4 + x6;
	x4 -= x6;
	x6 = x5 + x7;
	x5 -= x7;
	//third stage
	x7 = x8 + x3;
	x8 -= x3;
	x3 = x0 + x2;
	x0 -= x2;
	x2 = (181 * (x4 + x5) + 128) >> 8;
	x4 = (181 * (x4 - x5) + 128) >> 8;
	//fourth stage
	blk[8 * 0] = iclp[(x7 + x1) >> 14];
	blk[8 * 1] = iclp[(x3 + x2) >> 14];
	blk[8 * 2] = iclp[(x0 + x4) >> 14];
	blk[8 * 3] = iclp[(x8 + x6) >> 14];
	blk[8 * 4] = iclp[(x8 - x6) >> 14];
	blk[8 * 5] = iclp[(x0 - x4) >> 14];
	blk[8 * 6] = iclp[(x3 - x2) >> 14];
	blk[8 * 7] = iclp[(x7 - x1) >> 14];
}