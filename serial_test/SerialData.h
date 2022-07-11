#include <iostream>
#define MAX_SERIAL_DATA_SIZE 	240
#define MAX_SEIAL_WRONG_DATA	128

#define CmpDataGoodCnt			0
#define CmpDataBadCnt			1
#define CmpDataWrongNumCnt		2

#pragma once
class SerialData
{
	private:
		unsigned char tx_data[MAX_SERIAL_DATA_SIZE];
		unsigned char rx_data[MAX_SERIAL_DATA_SIZE];
		unsigned int DataBadCnt;
		unsigned int DataGoodCnt;
		unsigned int RxDataNum;
		unsigned int RxDataWrongCnt;
		unsigned int wrong_data[MAX_SEIAL_WRONG_DATA];
		
	public:
		SerialData();
		~SerialData();
		void Check_Data(unsigned int InitData);
		unsigned char* AddrTxDataBuf(unsigned int idx);
		unsigned char* AddrRxDataBuf();
		void IncreaseRxPktNum();
		unsigned int GetRxPktNum();
		void SaveWrongData(unsigned int WrongDataLength);
		void MakeTxDataPattern(unsigned char init_val);
		void Print();
		unsigned int GetCmpDataGoodCnt();
		unsigned int GetCmpDataBadCnt();
		unsigned int GetWrongDataCnt();
};

