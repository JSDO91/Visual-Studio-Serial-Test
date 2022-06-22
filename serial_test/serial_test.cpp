// serial_test.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "RsPort.h"
#include "SerialData.h"


int main()
{
	CString portname = _T("COM6");
	CRsPort serialport(portname);
	SerialData serialdata;

	unsigned int readLen;
	
	serialport.WriteCommPort(serialdata.AddrTxDataBuf(), 1);

	while (serialport.IsCommPortOpen())
	{
		readLen = serialport.ReadCommPort(serialdata.AddrRxDataBuf(), MAX_SERIAL_DATA_SIZE);

		if (readLen == MAX_SERIAL_DATA_SIZE)
		{
			serialdata.Check_Data(serialdata.GetRxPktNum());
			serialdata.IncreaseRxPktNum();
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

	serialdata.Print();

    return 0;
}

