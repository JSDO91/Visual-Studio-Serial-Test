// serial_test.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "RsPort.h"
#include "SerialData.h"
#include <Windows.h>
#include <time.h>


#define TS_RATIO		(0.0001)	// for ms

LARGE_INTEGER gts_freq, gts_r[100];

unsigned int gts_n = 0;

/*****************************************************************************
* FUNCTION DECLARATION
*****************************************************************************/
void TMGK_TS_PRINT(LARGE_INTEGER* pts, unsigned int s, unsigned int e)
{
	printf("\r\n");
	printf(" ################\r\n");
	printf(" ## time stamp ##\r\n");

	for (; s<e; s++)
	{
		printf("ts_r[%2d]: %16llu(%10llu)(%10.3fms)\r\n",
			s,
			pts[s].QuadPart,
			pts[s].QuadPart - pts[s - 1].QuadPart,
			(double)((pts[s].QuadPart - pts[s - 1].QuadPart)*(double)TS_RATIO));
	}
	s--;

	printf("total:    %16llu(%10llu)(%10.3fms)\r\n",
		pts[s].QuadPart,
		pts[s].QuadPart - pts[0].QuadPart,
		(double)((pts[s].QuadPart - pts[0].QuadPart)*(double)TS_RATIO));
	printf(" ################\r\n");
}



int main()
{
	QueryPerformanceFrequency(&gts_freq);
	printf("# (tick/sec: %lld)\r\n", gts_freq.QuadPart);

	CString portname = _T("COM6");
	CRsPort serialport(portname);
	SerialData serialdata;

	unsigned int readLen;
	
	serialport.WriteCommPort(serialdata.AddrTxDataBuf(), 1);

	while (1)
	{


		readLen = serialport.ReadCommPort(serialdata.AddrRxDataBuf(), MAX_SERIAL_DATA_SIZE);

		if (readLen == MAX_SERIAL_DATA_SIZE)
		{
			if(serialdata.GetRxPktNum() == 0)
				QueryPerformanceCounter(&gts_r[gts_n++]);

			serialdata.Check_Data(serialdata.GetRxPktNum());
			serialdata.IncreaseRxPktNum();

			if (serialdata.GetRxPktNum() == 1720)
			{
				QueryPerformanceCounter(&gts_r[gts_n++]);
				break;
			}
		}
		else if (readLen == 40)
		{
			break;
		}
		else if (readLen != 0)
		{
			serialdata.SaveWrongData(readLen);
		}
	}

	TMGK_TS_PRINT(gts_r, 1, gts_n);
	serialdata.Print();

	while (1);

    return 0;
}

#if 0
void main()
{
	QueryPerformanceFrequency(&gts_freq);
	printf("# (tick/sec: %lld)\r\n", gts_freq.QuadPart);

	////////////////
	/* time stamp */
	QueryPerformanceCounter(&gts_r[gts_n++]);
	////////////////

	// ...

	Sleep(1000);

	////////////////
	/* time stamp */
	QueryPerformanceCounter(&gts_r[gts_n++]);
	////////////////

	TMGK_TS_PRINT(gts_r, 1, gts_n);

	while (1);
}
#endif