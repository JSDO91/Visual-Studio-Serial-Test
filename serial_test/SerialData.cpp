#include "stdafx.h"
#include "SerialData.h"

using namespace std;

SerialData::SerialData() : gDataGoodCnt(0), gDataBadCnt(0), rx_data_num(0), rx_data_wrong_num(0), tx_data{ 0, }, rx_data{ 0, }, wrong_data{0, }
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
			gDataBadCnt++;
		}
		else
			gDataGoodCnt++;
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
	rx_data_num++;
}

unsigned int SerialData::GetRxPktNum()
{
	return this->rx_data_num;
}

void SerialData::SaveWrongData(unsigned int WrongDataLength)
{
	wrong_data[rx_data_wrong_num] = WrongDataLength;
	rx_data_wrong_num++;
	if (rx_data_wrong_num > 127)
		rx_data_wrong_num = 0;
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
	cout << "Total Rx Packet Cnt "			<< rx_data_num		<< endl;
	cout << "Total Data Good Byte Cnt "		<< gDataGoodCnt		<< endl;
	cout << "Total Data Bad  Byte Cnt  "	<< gDataBadCnt		<< endl;
	cout << "Total Wrong Size Packet Cnt "	<< rx_data_wrong_num << endl;
}