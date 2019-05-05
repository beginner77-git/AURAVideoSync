// AuraSyncGDI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define GDIPVER     0x0110 
#include <iostream>
#include <string>
#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <stdlib.h> 
#include <vector>
#include <algorithm>
#include "AURALightingSDK.h"
#pragma comment(lib, "gdiplus.lib")

using namespace std;
using namespace Gdiplus;

//vector holding the color and number of pixels
std::vector<std::pair<unsigned char, unsigned int>> redChannel;
std::vector<std::pair<unsigned char, unsigned int>> greenChannel;
std::vector<std::pair<unsigned char, unsigned int>> blueChannel;


int SaveScreenshot()
{
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	HWND hMyWnd = GetDesktopWindow();
	RECT r;
	int w, h;
	HDC dc, hdcCapture;
	int nBPP, nCapture;
	LPBYTE lpCapture;
	Bitmap *pScreenShot;

	// get the area of my application's window     
	GetWindowRect(hMyWnd, &r);
	dc = GetWindowDC(hMyWnd); 
	w = r.right - r.left;
	h = r.bottom - r.top;
	nBPP = GetDeviceCaps(dc, BITSPIXEL);
	hdcCapture = CreateCompatibleDC(dc);

	// create the buffer for the screenshot
	BITMAPINFO bmiCapture = { sizeof(BITMAPINFOHEADER), w, -h, 1, nBPP, BI_RGB, 0, 0, 0, 0, 0, };

	// create a container and take the screenshot
	HBITMAP hbmCapture = CreateDIBSection(dc, &bmiCapture, DIB_PAL_COLORS, (LPVOID *)&lpCapture, NULL, 0);

	// failed to take it
	if (!hbmCapture) {
		DeleteDC(hdcCapture);
		DeleteDC(dc);
		GdiplusShutdown(gdiplusToken);
		printf("failed to take the screenshot. err: %d\n", GetLastError());
		return 0;
	}

	// copy the screenshot buffer
	nCapture = SaveDC(hdcCapture);
	SelectObject(hdcCapture, hbmCapture);
	BitBlt(hdcCapture, 0, 0, w, h, dc, 0, 0, SRCCOPY);
	RestoreDC(hdcCapture, nCapture);
	DeleteDC(hdcCapture);
	DeleteDC(dc);

	// save the buffer to a file   
	pScreenShot = new Bitmap(hbmCapture, (HPALETTE)NULL);
	UINT numEntries;
	pScreenShot->GetHistogramSize(HistogramFormatRGB, &numEntries);

	UINT* ch0 = new UINT[numEntries];
	UINT* ch1 = new UINT[numEntries];
	UINT* ch2 = new UINT[numEntries];

	pScreenShot->GetHistogram(HistogramFormatRGB, numEntries, ch0, ch1, ch2, NULL);

	for (UINT j = 0; j < numEntries; ++j)
	{
		redChannel.push_back(std::make_pair(j, ch0[j]));
		greenChannel.push_back(std::make_pair(j, ch1[j]));
		blueChannel.push_back(std::make_pair(j, ch2[j]));
	}

	delete ch0;
	delete ch1;
	delete ch2;
	delete pScreenShot;
	DeleteObject(hbmCapture);
	GdiplusShutdown(gdiplusToken);
	return 0;
}


EnumerateMbControllerFunc EnumerateMbController;
SetMbColorFunc SetMbColor;
SetMbModeFunc SetMbMode;
GetMbLedCountFunc GetMbLedCount;
GetMbColorFunc GetMbColor;

int main() {
	HMODULE hLib = nullptr;
	hLib = LoadLibraryA("AURA_SDK.dll");
	(FARPROC&)EnumerateMbController = GetProcAddress(hLib, "EnumerateMbController");
	(FARPROC&)SetMbMode = GetProcAddress(hLib, "SetMbMode");
	(FARPROC&)SetMbColor = GetProcAddress(hLib, "SetMbColor");
	(FARPROC&)GetMbLedCount = GetProcAddress(hLib, "GetMbLedCount");
	(FARPROC&)GetMbColor = GetProcAddress(hLib, "GetMbColor");
	DWORD count = EnumerateMbController(NULL, 0);

	MbLightControl* _mbLightCtrl;
	int _mbLedControllers;
	std::cout << "Reached";
	_mbLightCtrl = new MbLightControl[count];
	EnumerateMbController(_mbLightCtrl, count);
	_mbLedControllers = count;
	std::cout << "ReachedN";
	int deviceid = 0;
	int ledCount = GetMbLedCount(_mbLightCtrl[deviceid]); std::cout << "Reached" << ledCount;
	SetMbMode(_mbLightCtrl[deviceid], 1);

	while(true) {
		Sleep(100);
		SaveScreenshot();

		//sort by the number of pixels
		std::sort(redChannel.begin(), redChannel.end(), [](std::pair<unsigned char, unsigned int>& a, std::pair<unsigned char, unsigned int>& b) { return a.second > b.second; });
		std::sort(blueChannel.begin(), blueChannel.end(), [](std::pair<unsigned char, unsigned int>& a, std::pair<unsigned char, unsigned int>& b) { return a.second > b.second; });
		std::sort(greenChannel.begin(), greenChannel.end(), [](std::pair<unsigned char, unsigned int>& a, std::pair<unsigned char, unsigned int>& b) { return a.second > b.second; });
		
		std::cout << "Channel maximum RED: " << (int)redChannel[0].first << " GREEN: " << (int)greenChannel[0].first << " BLUE: " << (int)blueChannel[0].first << endl;
		
		int maxColor = 140;
		int blackTreshold = 10;
		//never turn off the leds
		if (redChannel[0].first < blackTreshold && redChannel[0].first < blackTreshold && blueChannel[0].first < blackTreshold)
			redChannel[0].first = greenChannel[0].first = blueChannel[0].first = 10;
		//get dominant color
		/*	for (int i = 0; i != redChannel.size(); i++)
			{
				if (redChannel[i].first > maxColor || greenChannel[i].first > maxColor || blueChannel[i].first > maxColor)
				{
					std::cout << "Channel maximum at " << i << " RED: " << (int)redChannel[i].first << " GREEN: " << (int)greenChannel[i].first << " BLUE: " << (int)blueChannel[i].first << endl;
					//redChannel[0].first = greenChannel[0].first = blueChannel[0].first = 10;
					redChannel[0].first = redChannel[i].first;
					greenChannel[0].first = greenChannel[i].first;
					blueChannel[0].first = blueChannel[i].first;
					break;
				}
			}*/
	
		byte *colors = new byte[ledCount * 3];
		//Red
		colors[0] = redChannel[0].first;
		colors[3] = redChannel[0].first;
		colors[6] = redChannel[0].first;
		colors[9] = redChannel[0].first;
		colors[12] = redChannel[0].first; //header Red

		//Green
		colors[1] = greenChannel[0].first;
		colors[4] = greenChannel[0].first;
		colors[7] = greenChannel[0].first;
		colors[10] = greenChannel[0].first;
		colors[13] = greenChannel[0].first; //header Green

		//Blue
		colors[2] = blueChannel[0].first;
		colors[5] = blueChannel[0].first;
		colors[8] = blueChannel[0].first;
		colors[11] = blueChannel[0].first;
		colors[14] = blueChannel[0].first; //header Blue
		SetMbColor(_mbLightCtrl[deviceid], colors,  ledCount * 3);

		redChannel.clear();
		greenChannel.clear();
		blueChannel.clear();
	}
	return 0;
}
