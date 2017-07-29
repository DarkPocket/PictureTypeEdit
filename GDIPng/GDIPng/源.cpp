//#define UNICODE
#include <windows.h>


#ifndef ULONG_PTR
#define ULONG_PTR unsigned long*
#endif
#include <gdiplus.h>
#include <iostream>
using namespace std;

using namespace Gdiplus;

//ת��ͼƬ
int translatePic(WCHAR OriFilePath[MAX_PATH], WCHAR destFilePath[MAX_PATH], UINT  newHeight, UINT  newWidth);

int translatePic(WCHAR OriFilePath[MAX_PATH], WCHAR destFilePath[MAX_PATH]);
//���ñ�����
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
//��ȡ�ļ���׺��
int   getExtension(WCHAR  filePath[MAX_PATH], WCHAR  fileExten[MAX_PATH]);


int   getExtension(WCHAR  filePath[MAX_PATH], WCHAR  fileExten[MAX_PATH])
{
	int len;
	int pos = wcslen(filePath);
	len = pos;

	while (pos > 0)
	{
		if (filePath[pos] == '.')
		{
			break;
		}
		--pos;
	}
	if (pos == 0)
	{
		wcscpy_s(fileExten, 1, L"");
		return -1;
	}

	wcscpy_s(fileExten, len - pos, &filePath[pos + 1]);

	return 0;
}





int translatePic(WCHAR OriFilePath[MAX_PATH], WCHAR destFilePath[MAX_PATH], UINT uNewHeight, UINT uNewWidth)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Status stat = GenericError;
	CLSID encoderClsid;
	stat = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (0 != stat)
	{
		return -1;
	}
	// ����״̬  
	stat = GenericError;

	WCHAR  fileExten[MAX_PATH] = { 0 };
	if (getExtension(OriFilePath, fileExten) != 0)
	{
		//����ļ����޺�׺��
		cout << "file error " << endl;
		return -1;
	}

	Image *pImage = new Image(OriFilePath);

	if (NULL == pImage) {
		return -2;

	}
	// ��ȡ����  
	UINT unOriHeight = pImage->GetHeight();
	UINT unOriWidth = pImage->GetWidth();

	if (unOriWidth < 1 || unOriHeight < 1) {
		return -3;
	}

	WCHAR  destPicType[16] = { 0 };
	WCHAR   PicType[16] = { 0 };
	//��ȡĿ���ļ���ʽ-��׺��
	if (getExtension(destFilePath, PicType) != 0)
	{
		//����ļ� ��׺��
		cout << "file error " << endl;
		return -1;
	}

	if (wcscmp(PicType, L"jpg") == 0);
	{
		wcscpy_s(PicType, L"jpeg");
	}
	//�õ�Ŀ���ļ���׺��  ���� ת�����ʽ

	wsprintf(destPicType, L"image/%s", PicType);
	/*
	if (wcscmp(PicType, L"png") == 0);
	{
		wcout << endl;
	}*/

	//���ñ��뷽ʽ
	if (!GetEncoderClsid(destPicType, &encoderClsid)) {
		//		return -4;
	}
	//	REAL =float
	REAL srcWidth = unOriWidth;
	REAL srcHeight = unOriHeight;
	RectF rectDest(0.0f, 0.0f, (REAL)uNewWidth, (REAL)uNewHeight);
	Bitmap *pTempBitmap = new Bitmap(uNewWidth, uNewHeight);
	Graphics *pGraphics = NULL;
	if (!pTempBitmap)
	{
		return -5;
	}
	pGraphics = Graphics::FromImage(pTempBitmap);
	if (!pGraphics)
	{
		return -6;
	}
	stat = pGraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	if (0 != stat)
	{
		return -7;
	}
	stat = pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	if (0 != stat)
	{
		return -8;
	}
	stat = pGraphics->DrawImage(pImage, rectDest, 0.0f, 0.0f, srcWidth, srcHeight, UnitPixel, NULL, NULL, NULL);

	if (0 != stat)
	{
		return -9;
	}

	//�ļ�д��
	stat = pTempBitmap->Save(destFilePath, &encoderClsid, NULL);
	if (0 != stat)
	{
		return -10;
	}

	//�ͷ���Դ
	if (pGraphics)
	{
		delete pGraphics;
		pGraphics = NULL;
	}
	if (pTempBitmap)
	{
		delete pTempBitmap;
		pTempBitmap = NULL;
	}
	if (pImage)
	{
		delete pImage;
		pImage = NULL;
	}

	GdiplusShutdown(gdiplusToken);
	return 0;
}



int translatePic(WCHAR OriFilePath[MAX_PATH], WCHAR destFilePath[MAX_PATH])
{//ֻ�޸ĸ�ʽ ���ı��С�ߴ�
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	int  stat = 0;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	Image *pImage = new Image(OriFilePath);

	if (NULL == pImage) {
		return -2;
	}
	// ��ȡԭʼͼƬ����  
	UINT uNewHeight = pImage->GetHeight();
	UINT uNewWidth = pImage->GetWidth();

	if (pImage)
	{
		delete pImage;
		pImage = NULL;
	}
	GdiplusShutdown(gdiplusToken);

	stat = translatePic(OriFilePath, destFilePath, uNewHeight, uNewWidth);

	return stat;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // �豸ӵ�е�ͼƬ����������  
	UINT  size = 0;         // ��ȡ���б����������Ļ�������С  
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  //ʧ��  
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // ʧ��  
	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // �ɹ�  
		}
	}
	free(pImageCodecInfo);
	return -1;  //ʧ��  
}


int test()
{

	WCHAR file[MAX_PATH] = L"1234567.jpeg";
	WCHAR fileDest[MAX_PATH] = L"1234ab.bmp";
	WCHAR buf[256] = { 0 };


	getExtension(file, buf);

	translatePic(file, fileDest);

	wcout << file << "   " << buf << endl;

	return 0;
}

int main()
{
	//����wcout �������
	wcout.imbue(std::locale("chs"));

	test();



	system("pause");
	return 0;
}

