#include "stdafx.h"
#include "SerialData.h"

using namespace std;

/*
	unsigned char tx_data[MAX_SERIAL_DATA_SIZE];
	unsigned char rx_data[MAX_SERIAL_DATA_SIZE];
	unsigned int DataBadCnt;
	unsigned int DataGoodCnt;
	unsigned int RxDataNum;
	unsigned int RxDataWrongCnt;
	unsigned int wrong_data[MAX_SEIAL_WRONG_DATA];
*/

SerialData::SerialData() : DataGoodCnt(0), DataBadCnt(0), RxDataNum(0), RxDataWrongCnt(0), tx_data{ 0, }, rx_data{ 0, }, wrong_data{0, }
{

}

SerialData::~SerialData()
{

}

void SerialData::Check_Data(unsigned int InitData)
{
	unsigned char init = (unsigned char)(InitData & 0xFF);
	unsigned char idx = 0;

	for (idx = 0; idx < MAX_SERIAL_DATA_SIZE; idx++)
	{
		if (rx_data[idx] != (unsigned char)((idx + init) & 0xFF))
		{
			DataBadCnt++;
		}
		else
			DataGoodCnt++;
	}
}


unsigned char* SerialData::AddrTxDataBuf(unsigned int idx)
{
	return &tx_data[idx];
}

unsigned char* SerialData::AddrRxDataBuf()
{
	return &rx_data[0];
}

void SerialData::IncreaseRxPktNum()
{
	RxDataNum++;
}

unsigned int SerialData::GetRxPktNum()
{
	return this->RxDataNum;
}

void SerialData::SaveWrongData(unsigned int WrongDataLength)
{
	static unsigned short RxDataWrongIdx = 0;
	
	wrong_data[RxDataWrongIdx] = WrongDataLength;
	
	RxDataWrongIdx++;
	RxDataWrongCnt++;

	if (RxDataWrongIdx > 127)
		RxDataWrongIdx = 0;
}

void SerialData::MakeTxDataPattern(unsigned char init_val)
{
	unsigned int idx = 0;
	for (idx = 0; idx < MAX_SERIAL_DATA_SIZE; idx++)
	{
		tx_data[idx] = idx + init_val;
	}
}

void SerialData::Print()
{
	cout << "Total Rx Packet Cnt "			<< RxDataNum		<< endl;
	cout << "Total Data Good Byte Cnt "		<< DataGoodCnt		<< endl;
	cout << "Total Data Bad  Byte Cnt  "	<< DataBadCnt		<< endl;
	cout << "Total Wrong Size Packet Cnt "	<< RxDataWrongCnt 	<< endl;
}

unsigned int SerialData::GetCmpDataGoodCnt()
{
	return  this->DataGoodCnt;	
}

unsigned int SerialData::GetCmpDataBadCnt()
{
	return  this->DataBadCnt;	
}

unsigned int SerialData::GetWrongDataCnt()
{
	return  this->RxDataWrongCnt;	
}