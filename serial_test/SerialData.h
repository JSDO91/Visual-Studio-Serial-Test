#include <iostream>
#define MAX_SERIAL_DATA_SIZE 240

#pragma once
class SerialData
{
	private:
		unsigned char tx_data[MAX_SERIAL_DATA_SIZE];
		unsigned char rx_data[MAX_SERIAL_DATA_SIZE];
		unsigned int gDataBadCnt;
		unsigned int gDataGoodCnt;
		unsigned int rx_data_num;
		unsigned int rx_data_wrong_num;
		unsigned int wrong_data[128];
		
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
};

