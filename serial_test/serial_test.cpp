
#include "stdafx.h"
#include "RsPort.h"
#include "SerialData.h"
#include <Windows.h>
#include <time.h>
#include <setupapi.h>
#include <string>

#define TS_RATIO		(0.0001)	// for ms
#define nMAX_USB		4
#define WORKING_SECOND	10*60


LARGE_INTEGER gts_freq, gts_r[100];
LARGE_INTEGER gts_s, gts_e, gts_g, gts_pre_g;

CString USB_COM_PORT[nMAX_USB] = { 0, };
CRsPort USB_DONGLE_COM_PORTS[8];

SerialData serialdata[nMAX_USB];

std::string USB_VID = "VID_1915";
std::string USB_PID_ARR[8] = { "PID_5200", "PID_5201", "PID_5202", "PID_5203","PID_5204","PID_5205","PID_5206","PID_5207" };

unsigned int gts_n = 0;
unsigned int work_cnt[8] = { 0 , };

void TMGK_TS_PRINT(LARGE_INTEGER* pts, unsigned int s, unsigned int e);
void PrintPerformaceResult(unsigned char Idx, CRsPort* portptr, SerialData* serialdataptr);
const std::string TCHARToString(const PTCHAR ptchar);
unsigned char FIND_USB_DONGLE();
void PRINT_PERFORM_TEST(LARGE_INTEGER* ptr_s, LARGE_INTEGER* ptr_e, LARGE_INTEGER* ptr_g);
void PrintPerformaceResult(unsigned char Idx, CRsPort* portptr, SerialData* serialdataptr);


/*****************************************************************************
* FUNCTION NAME :	main()
* DESCRIPTION   :
* PARAMETER     :
* RETURN VALUE  :
*****************************************************************************/
int main()
{
	unsigned int readLen;
	unsigned char result = 0;
	unsigned char deviceIdx = 0;

	QueryPerformanceFrequency(&gts_freq);
	printf("# (tick/sec: %lld)\r\n", gts_freq.QuadPart);
// find Serial Device in Device Manager
	result = FIND_USB_DONGLE();

	if(result <= 0)
	{
		printf("No Device\r\n");
		while (1);
		return 0;
	}
	else
	{
		printf("Find Device Num : %d\r\n",result);
	}
// open COM PORT
	for (deviceIdx = 0; deviceIdx < nMAX_USB; deviceIdx++)
	{
		USB_DONGLE_COM_PORTS[deviceIdx].CRsPort_USER_Init(USB_COM_PORT[deviceIdx]);
	}

	QueryPerformanceCounter(&gts_s);

	while(1)
	{
		QueryPerformanceCounter(&gts_e);
		gts_g.QuadPart = gts_e.QuadPart - gts_s.QuadPart;

		if(gts_g.QuadPart >= (gts_freq.QuadPart*WORKING_SECOND))
			break;

		for(deviceIdx = 0; deviceIdx < nMAX_USB; deviceIdx++)
		{
			if(USB_DONGLE_COM_PORTS[deviceIdx].m_Connect)
			{
				readLen = USB_DONGLE_COM_PORTS[deviceIdx].ReadCommPort(serialdata[deviceIdx].AddrRxDataBuf(), MAX_SERIAL_DATA_SIZE);
				
				if (readLen == MAX_SERIAL_DATA_SIZE)
				{
					if ( (gts_g.QuadPart - gts_pre_g.QuadPart) >= gts_freq.QuadPart)
					{
						serialdata[deviceIdx].MakeTxDataPattern(((unsigned char)work_cnt[deviceIdx]));
						USB_DONGLE_COM_PORTS[deviceIdx].WriteCommPort(serialdata[deviceIdx].AddrTxDataBuf(0), 240);

						gts_pre_g.QuadPart = gts_g.QuadPart;
					}

					serialdata[deviceIdx].Check_Data(serialdata[deviceIdx].GetRxPktNum());
					serialdata[deviceIdx].IncreaseRxPktNum();
				}
				else if ((readLen != 0) && (readLen != MAX_SERIAL_DATA_SIZE))
				{
					serialdata[deviceIdx].SaveWrongData(readLen);
				}

				work_cnt[deviceIdx]++;
			}
		}
	}

	PRINT_PERFORM_TEST(&gts_s, &gts_e, &gts_g);

	for(deviceIdx = 0; deviceIdx < nMAX_USB; deviceIdx++)
	{
		if (USB_DONGLE_COM_PORTS[deviceIdx].m_Connect)
			PrintPerformaceResult(deviceIdx,&USB_DONGLE_COM_PORTS[deviceIdx], &serialdata[deviceIdx]);
	}

	while (1);

    return 0;
}

/*****************************************************************************
* FUNCTION DECLARATION
*****************************************************************************/

/*****************************************************************************
* FUNCTION NAME :	TMGK_TS_PRINT(LARGE_INTEGER* pts, unsigned int s, unsigned int e)
* DESCRIPTION   :
* PARAMETER     :
* RETURN VALUE  :
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

/*****************************************************************************
* FUNCTION NAME :	PRINT_PERFORM_TEST(LARGE_INTEGER* ptr_s, LARGE_INTEGER* ptr_e, unsigned int pktnum)
* DESCRIPTION   :
* PARAMETER     :
* RETURN VALUE  :
*****************************************************************************/
void PRINT_PERFORM_TEST(LARGE_INTEGER* ptr_s, LARGE_INTEGER* ptr_e, LARGE_INTEGER* ptr_g)
{
	printf("\r\n");
	printf(" ################\r\n");
	printf(" ## time stamp ##\r\n");

	printf("%16llu(%10llu)(%10.3fms)\r\n", ptr_s->QuadPart, ptr_e->QuadPart - ptr_s->QuadPart, (double)((ptr_g->QuadPart)*(double)TS_RATIO));
}

/*****************************************************************************
* FUNCTION NAME :	TCHARToString()
* DESCRIPTION   :
* PARAMETER     :
* RETURN VALUE  :
*****************************************************************************/
const std::string TCHARToString(const PTCHAR ptchar)
{
	int len = wcslen((PTCHAR)ptchar);

	char* pschar = new char[2 * len + 1];
	wcstombs(pschar, (PTCHAR)ptchar, 2 * len + 1);
	std::string s = pschar;

	delete[] pschar;

	return s;
}

/*****************************************************************************
* FUNCTION NAME :	FIND_USB_DONGLE()
* DESCRIPTION   :
* PARAMETER     :
* RETURN VALUE  :
*****************************************************************************/
unsigned char FIND_USB_DONGLE() 
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA stDevInfoData = SP_DEVINFO_DATA();
	std::string instanceID, comName, className;
	size_t nPos;
	unsigned char findDeviceNum = 0;

	hDevInfo = SetupDiGetClassDevs(
		0L,
		0L,
		0L,
		DIGCF_PRESENT | DIGCF_ALLCLASSES | DIGCF_PROFILE
	);

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		printf("hDevInfo == 0x%x", INVALID_HANDLE_VALUE);
		return 0;
	}
		
	stDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &stDevInfoData); i++) 
	{
		TCHAR szInstanceId[MAX_PATH] = { 0 };
		TCHAR szClassName[MAX_PATH] = { 0 };
		TCHAR szFriendlyName[MAX_PATH] = { 0 };
		TCHAR szClassDescription[MAX_PATH] = { 0 };
		TCHAR szDeviceDescription[MAX_PATH] = { 0 };

		TCHAR CLASS_NAME[MAX_PATH] = _T("Ports");
		TCHAR device[16] = { 0, };//"\\\\.\\COM";

		// Get Device Instance ID
		BOOL bResult = SetupDiGetDeviceInstanceId(
			hDevInfo,
			&stDevInfoData,
			szInstanceId,
			_countof(szInstanceId),
			0
		);

		if (!bResult) {
			_tprintf(_T("Failed to get device instance ID\n"));
			continue;
		}

		(VOID)SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&stDevInfoData,
			SPDRP_CLASS,
			0,
			(PBYTE)szClassName,
			_countof(szClassName),
			0
		);

		(VOID)SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&stDevInfoData,
			SPDRP_DEVICEDESC,
			0,
			(PBYTE)szDeviceDescription,
			_countof(szDeviceDescription),
			0
		);

		(VOID)SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&stDevInfoData,
			SPDRP_FRIENDLYNAME,
			0,
			(PBYTE)szFriendlyName,
			_countof(szFriendlyName),
			0
		);

		(VOID)SetupDiGetClassDescription(
			&stDevInfoData.ClassGuid,
			szClassDescription,
			_countof(szClassDescription),
			0
		);
		
		className	= TCHARToString(szClassName);

		if(className == "Ports")
		{
			instanceID	= TCHARToString(szInstanceId);
			nPos		= instanceID.find(USB_VID);

			if (nPos != std::string::npos)
			{
				for (short findidx = 0; findidx < MAX_PATH; findidx++)
				{
					if ((szFriendlyName[findidx] == L'C') && (szFriendlyName[findidx + 1] == L'O') && (szFriendlyName[findidx + 2] == L'M'))
						_stprintf_s(device, _countof(device), _T("\\\\.\\COM%d"), _ttoi(&szFriendlyName[findidx + 3]));
				}

				// find PID
				for (int PIDidx = 0; PIDidx < nMAX_USB; PIDidx++)
				{
					if (instanceID.find(USB_PID_ARR[PIDidx]) != std::string::npos)
						USB_COM_PORT[PIDidx] = device;
				}

				_tprintf(_T("[%d]\n"), i);
				_tprintf(_T("-- Class: %s\n"), szClassName);
				_tprintf(_T("-- Friendly Name: %s\n"), szFriendlyName);
				_tprintf(_T("-- Instance ID: %s\n"), szInstanceId);
				_tprintf(_T("-- Class Description: %s\n"), szClassDescription);
				_tprintf(_T("-- Device Description: %s\n"), szDeviceDescription);
				_tprintf(_T("\n"));

				findDeviceNum++;

			}
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return findDeviceNum;
}

/*****************************************************************************
* FUNCTION NAME :	PrintPerformaceResult(CRsPort* portptr, serialData* serialdataptr)
* DESCRIPTION   :
* PARAMETER     :
* RETURN VALUE  :
*****************************************************************************/
void PrintPerformaceResult(unsigned char Idx, CRsPort* portptr, SerialData* serialdataptr)
{
	printf("###################Device[%d]###############################\r\n",Idx);
	printf("Rx Packet Num 			: %d\r\nRx Bytes Num 			: %d\r\n",serialdataptr->GetRxPktNum(),(serialdataptr->GetRxPktNum())*MAX_SERIAL_DATA_SIZE);
	printf("Compare Data Good Cnt 	: %d\r\nCompare Data Bad Cnt 	: %d\r\nDrop Data Cnt 		: %d\r\n",serialdataptr->GetCmpDataGoodCnt(),serialdataptr->GetCmpDataBadCnt(), serialdataptr->GetDropDataCnt());
	printf("Wrong Data Length Cnt	: %d\r\n",serialdataptr->GetWrongDataCnt());
	printf("############################################################\r\n");
}