/**
*	@file Rcb4BaseClass.cpp
*	@brief RCB4 base library
*	@author Kondo Kagaku Co.,Ltd.
*	@version 1.1.0
*
*	@date	2018/02/01
*	@date	2019/01/16�@V1.1.0
*			�EgetConfig�̕s����C��
*			�EmoveRamToComCmdSynchronize��memcopy�̕s��Ή�
*
*	@copyright &copy; Kondo Kagaku Co.,Ltd. 2019
**/
/* 
* Rcb4BaseClass.cpp
*
* Created: 2017/06/13 4:34:57
* Author: 5636m_000,T.Nobuhara
*/


#include "Rcb4BaseClass.h"



//RCB4��config�f�[�^���擾���� //////////////////////////////////////////////////////////
/**
* @brief RCB4�̃R���t�B�O�f�[�^��ǂݎ��
* @return �������ǂ�������̃t���O
* @retval true	�ʐM�����A�f�[�^�擾
* @retval false	�ʐM���s
* @attention class����configData�ɒl���������܂�
**/
///////////////////////////////////////////////////////////////////////////////////
// (config�f�[�^���擾����)
bool Rcb4BaseClass::getConfig()
{
	byte	wkbuf[10];
	if(moveRamToComCmdSynchronize(ConfigRamAddress ,0x02,wkbuf) != 0xffff)
	{
		configData.byte.low = wkbuf[0];
		configData.byte.high= wkbuf[1];
		return 	true;
	}
	else
	{
		return false;
	}
}

//RCB4��config�f�[�^���擾���� //////////////////////////////////////////////////////////
/**
* @brief RCB4�̃R���t�B�O�f�[�^��ǂݎ��
* @param[out] confData	RCB4�̃R���t�B�O�f�[�^
* @return �������ǂ�������̃t���O
* @retval true	�ʐM�����A�f�[�^�擾
* @retval false	�ʐM���s
* @attention �Ԃ��l��class����configData�̒l���R�s�[����܂��B(configData�̒��ɂ����������܂�)
* @date 2019/01/15�@confData�̃A�h���X�Ƀf�[�^�����Ă����̂ŏC��
**/
bool Rcb4BaseClass::getConfig(unsigned short *confData)
{
	if(getConfig() == false)	return	false;

	*confData = configData.word; 
	delay(100);
	return	true;
}


//Rcb4BaseClass�Ŏg�p���Ă��鑗�M�o�b�t�@�ɉ�������悤�Ƃ��Ă��Ȃ����`�F�b�N����///////////
/**
* @brief �f�[�^�ʐM���ŁA���M�o�b�t�@�ɒl�����悤�Ƃ��Ă��邩�`�F�b�N����
* @param[in] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @return �������ǂ�������̃t���O
* @retval true	���ݓ���悤�Ƃ��Ă���o�b�t�@�͉����g���Ă��Ȃ�
* @retval false	���ݓ���悤�Ƃ��Ă���o�b�t�@�͎g���Ă���Œ��ł���
* @attention Rcb4BaseClass�Ŏg���Ă��鑗�M�o�b�t�@�͋��ʂŎg���Ă���̂ŁA���M���ɕς��Ȃ����m�F���s���Ă���
**/
bool Rcb4BaseClass::checkuning(byte *txData)		///�f�[�^�ʐM���ŁA���M�o�b�t�@�ɒl�����悤�Ƃ��Ă��邩�`�F�b�N
{
	return (_isSynchronize == true && txBuf == txData) ? false: true;
}


///////////////////////////////////////////////////////////////////////////////////
//comand�쐬 (�f�[�^��^����Ƒ��M�R�}���h���쐬����
//
// 
//COM����RAM�Ƀf�[�^��]������R�}���h�𐶐�����///////////
/**
* @brief COM����RAM�Ƀf�[�^��]������R�}���h�𐶐�����
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] destAddr	RCB4�ɏ������ސ擪�A�h���X
* @param[in] destData[]	�������݃f�[�^
* @param[in] destDataSize	�����݃f�[�^��
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @details COM����RCB4����RAM�̃f�[�^������������R�}���h�𐶐����܂�
**/
int Rcb4BaseClass::moveComToRamCmd(byte *txData,int destAddr,byte destData[],int destDataSize)
{
	if(checkuning(txData) == false || destDataSize > 249)
	{
		return -1;
	}

	byte	*ptr;
	WordByte	wk;

	wk.word = destAddr;
	ptr = txData;

	*ptr++ = (byte)destDataSize + 7;		//�R�}���h�̃f�[�^��
	*ptr++=  Move;							//move�R�}���h����
	*ptr++=  0x02;							//COM => RAM
	*ptr++=  wk.byte[0];					//�������ސ擪�A�h���X
	*ptr++=  wk.byte[1];					//
	*ptr++=  0x00;							//�d�l
	for(int i = 0; i < destDataSize; i++)	//�f�[�^������
	{
		*ptr++=  destData[i];
	}
	setCheckSum(txData);					//�`�F�b�N�T��
	
	return destDataSize + 7;
	
}


/**
* @brief COM����RAM�Ƀf�[�^��]������R�}���h�𐶐�����
* @param[in] destAddr	RCB4�ɏ������ސ擪�A�h���X
* @param[in] destData[]	�������݃f�[�^
* @param[in] destDataSize	�����݃f�[�^��
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details COM����RCB4����RAM�̃f�[�^������������R�}���h�𐶐����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/
int Rcb4BaseClass::moveComToRamCmd(int destAddr,byte destData[],int destDataSize)
{
	return moveComToRamCmd(txBuf, destAddr, destData, destDataSize);
}


//RAM�̏�Ԃ�COM�ɓ]������R�}���h
/**
* @brief RAM�̃f�[�^��COM�ɑ���R�}���h�𐶐�����
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] scrAddr	RCB4�ɏ������ސ擪�A�h���X
* @param[in] scrDataSize	�����݃f�[�^��
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details RCB4�̓���RAM�̃f�[�^��COM�ɑ���R�}���h�𐶐����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
**/
int Rcb4BaseClass::moveRamToComCmd(byte *txData,int scrAddr,int scrDataSize)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;
	WordByte	wk;

	wk.word = scrAddr;

	ptr = txData;

	*ptr++ = 0x0a;		//�R�}���h�̃f�[�^��(�����̃R�}���h�͌Œ�)
	*ptr++=  Move;		//move�R�}���h����
	*ptr++=  0x20;		//RAM => COM
	*ptr++=  0x00;		//�d�l(COM�o��)
	*ptr++=  0x00;		//�d�l
	*ptr++=  0x00;		//�d�l
	*ptr++=  wk.byte[0];		//�]�����̃A�h���X
	*ptr++=  wk.byte[1];		//
	*ptr++=  (byte)scrDataSize;	//�����Ă��炢����byte��

	setCheckSum(txData);		//�`�F�b�N�T��
	return 0x0a;

}

//RAM�̏�Ԃ�COM�ɓ]������R�}���h
/**
* @brief RAM�̃f�[�^��COM�ɑ���R�}���h�𐶐�����
* @param[in] scrAddr	RCB4�ɏ������ސ擪�A�h���X
* @param[in] scrDataSize	�����݃f�[�^��
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details RCB4�̓���RAM�̃f�[�^��COM�ɑ���R�}���h�𐶐����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/
int Rcb4BaseClass::moveRamToComCmd(int scrAddr,int scrDataSize)
{
	return moveRamToComCmd(txBuf, scrAddr, scrDataSize);
}



//COM����̃f�[�^��ICS�ɓW�J����R�}���h
/**
* @brief COM����̃f�[�^��ICS�ɓW�J����R�}���h�𐶐�����(short 2byte�o�[�W����)
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] icsNum		ICS�ԍ�
* @param[in] offset		���o�C�g���ꂽ�Ƃ�����A�N�Z�X���邩
* @param[in] data		2byte���̃f�[�^
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details COM����RCB4�̓���ICS�G���A�Ɋ��蓖�Ă�ꂽ�f�[�^�ɔ��f�����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
**/
int Rcb4BaseClass::moveComToDeviceCmd (byte *txData,byte icsNum,byte offset,int data)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;
	WordByte	wk;

	ptr = txData;
	wk.word = data;
				
	*ptr++ = 0x09;		//�R�}���h�̃f�[�^��(�����̃R�}���h�͌Œ�)
	*ptr++=  Move;		//move�R�}���h����
	*ptr++=  0x12;		//com => Device
	*ptr++=  offset;	//�f�o�C�X�̐擪����ǂꂾ������Ă��邩
	*ptr++=  icsNum;	//ICS�ԍ�
	*ptr++=  0x00;		//�d�l
	*ptr++=  wk.byte[0];	//�f�[�^
	*ptr++=  wk.byte[1];	//
	setCheckSum(txData);	//�`�F�b�N�T��
	return 0x09;

}



/**
* @brief COM����̃f�[�^��ICS�ɓW�J����R�}���h�𐶐�����(short 2byte�o�[�W����)
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] idNum		�T�[�{��ID�ԍ�
* @param[in] sioNum		SIO(SioNum)�̔ԍ�
* @param[in] offset		���o�C�g���ꂽ�Ƃ�����A�N�Z�X���邩
* @param[in] data		2byte���̃f�[�^
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details COM����RCB4�̓���ICS�G���A�Ɋ��蓖�Ă�ꂽ�f�[�^�ɔ��f�����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
**/
int Rcb4BaseClass::moveComToDeviceCmd (byte *txData,byte idNum,byte sioNum,byte offset,int data)
{
	return moveComToDeviceCmd (txData, icsNum2id(idNum, sioNum), offset, data);
}



/**
* @brief COM����̃f�[�^��ICS�ɓW�J����R�}���h�𐶐�����(short 2byte�o�[�W����)
* @param[in] icsNum		ICS�ԍ�
* @param[in] offset		���o�C�g���ꂽ�Ƃ�����A�N�Z�X���邩
* @param[in] data		2byte���̃f�[�^
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details COM����RCB4�̓���ICS�G���A�Ɋ��蓖�Ă�ꂽ�f�[�^�ɔ��f�����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/
int Rcb4BaseClass::moveComToDeviceCmd (byte icsNum,byte offset,int data)
{
	return moveComToDeviceCmd (txBuf, icsNum, offset, data);
}



/**
* @brief COM����̃f�[�^��ICS�ɓW�J����R�}���h�𐶐�����(short 2byte�o�[�W����)
* @param[in] idNum		�T�[�{��ID�ԍ�
* @param[in] sioNum		SIO(SioNum)�̔ԍ�
* @param[in] offset		���o�C�g���ꂽ�Ƃ�����A�N�Z�X���邩
* @param[in] data		2byte���̃f�[�^
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details COM����RCB4�̓���ICS�G���A�Ɋ��蓖�Ă�ꂽ�f�[�^�ɔ��f�����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/
int Rcb4BaseClass::moveComToDeviceCmd (byte idNum,byte sioNum,byte offset,int data)
{
	return moveComToDeviceCmd (txBuf, icsNum2id(idNum, sioNum), offset, data);
}




//ICS�̃f�[�^��COM�ɓ]������R�}���h)
/**
* @brief ICS�̃f�[�^��COM�ɓ]������R�}���h�𐶐�����
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] icsNum		ICS�ԍ�
* @param[in] offset		���o�C�g���ꂽ�Ƃ�����A�N�Z�X���邩
* @param[in] dataSize	�擾����f�[�^��
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details RCB4��ICS�G���A�Ɋ��蓖�Ă�ꂽ�f�[�^��COM�ɓ]������R�}���h�𐶐����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
**/
int Rcb4BaseClass::moveDeviceToComCmd(byte *txData,byte icsNum,byte offset,byte dataSize)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;

	ptr = txData;

	*ptr++ = 0x0a;			//�R�}���h�̃f�[�^��(�����̃R�}���h�͌Œ�)
	*ptr++ = Move;			//move�R�}���h����
	*ptr++ = 0x21;			//Device => com 
	*ptr++ = offset;		//�f�o�C�X�̐擪����ǂꂾ������Ă��邩
	*ptr++ = 0x00;			//�d�l(COM�o��)
	*ptr++ = 0x00;			//�d�l
	*ptr++ = 0x00;			//�d�l
	*ptr++ = icsNum;		//ICS�ԍ�
	*ptr++ = dataSize;		//���o�C�g���炤��
	setCheckSum(txData);	//�`�F�b�N�T��	
	return 0x0a;

}



/**
* @brief ICS�̃f�[�^��COM�ɓ]������R�}���h�𐶐�����
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] idNum		�T�[�{��ID�ԍ�
* @param[in] sioNum		SIO(SioNum)�̔ԍ�
* @param[in] offset		���o�C�g���ꂽ�Ƃ�����A�N�Z�X���邩
* @param[in] dataSize	�擾����f�[�^��
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details RCB4��ICS�G���A�Ɋ��蓖�Ă�ꂽ�f�[�^��COM�ɓ]������R�}���h�𐶐����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
**/
int Rcb4BaseClass::moveDeviceToComCmd(byte *txData,byte idNum,byte sioNum,byte offset,byte dataSize)
{
	return  moveDeviceToComCmd(txData, icsNum2id(idNum, sioNum), offset, dataSize);
}



/**
* @brief ICS�̃f�[�^��COM�ɓ]������R�}���h�𐶐�����
* @param[in] icsNum		ICS�ԍ�
* @param[in] offset		���o�C�g���ꂽ�Ƃ�����A�N�Z�X���邩
* @param[in] dataSize	�擾����f�[�^��
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details RCB4��ICS�G���A�Ɋ��蓖�Ă�ꂽ�f�[�^��COM�ɓ]������R�}���h�𐶐����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/
int Rcb4BaseClass::moveDeviceToComCmd(byte icsNum, byte offset, byte dataSize)
{
	return  moveDeviceToComCmd(txBuf, icsNum, offset, dataSize);
}

/**
* @brief ICS�̃f�[�^��COM�ɓ]������R�}���h�𐶐�����
* @param[in] idNum		�T�[�{��ID�ԍ�
* @param[in] sioNum		SIO(SioNum)�̔ԍ�
* @param[in] offset		���o�C�g���ꂽ�Ƃ�����A�N�Z�X���邩
* @param[in] dataSize	�擾����f�[�^��
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details RCB4��ICS�G���A�Ɋ��蓖�Ă�ꂽ�f�[�^��COM�ɓ]������R�}���h�𐶐����܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/
int Rcb4BaseClass::moveDeviceToComCmd(byte idNum,byte sioNum, byte offset, byte dataSize)
{
	return  moveDeviceToComCmd(txBuf, icsNum2id(idNum, sioNum), offset, dataSize);
}




//�w�肵���A�h���X�ɃW�����v����
/**
* @brief �w�肵���A�h���X�ɃW�����v(CALL)����R�}���h�𐶐����܂�
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] romAddr		�W�����v����A�h���X
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @details ROM�̎w��A�h���X�ɃW�����v���閽�߂𐶐����܂��B
* @details Call���߂Ȃ̂ŁA�I������猳�̏ꏊ�ɖ߂�܂��B
**/
int Rcb4BaseClass::callCmd(byte *txData,long romAddr)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;
	DwordByte	wk;
	
	wk.dword = (uint32_t)romAddr;
	ptr = txData;

	*ptr++ = 0x07;				//�R�}���h�̃f�[�^��(�����͌Œ�)
	*ptr++ = Call;				//Call�R�}���h
	*ptr++ = wk.byte[0];		//�A�h���X
	*ptr++ = wk.byte[1];
	*ptr++ = wk.byte[2];
	*ptr++ = 0x00;				//���荀�ځ@=>�������Ŕ��
	setCheckSum(txData);		//�`�F�b�N�T��������
	return 0x07;

}



/**
* @brief �w�肵���A�h���X�ɃW�����v(CALL)����R�}���h�𐶐����܂�
* @param[in] romAddr		�W�����v����A�h���X
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details ROM�̎w��A�h���X�ɃW�����v���閽�߂𐶐����܂��B
* @details Call���߂Ȃ̂ŁA�I������猳�̏ꏊ�ɖ߂�܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/
int Rcb4BaseClass::callCmd(long romAddr)
{
	return callCmd(txBuf, romAddr);
}





//�T�[�{��1����������)
/**
* @brief �w�肵���T�[�{���[�^��1�����������R�}���h���쐬���܂�
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] servoData	���������Ƃ��Ă���T�[�{��1�܂Ƃ܂�ɂ����^
* @param[in] frame	�t���[����
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details 1�����T�[�{�𓮂����܂��B
* @details ServoData�^�ɂ́A�T�[�{���[�^��ID�ȊO�ɂ�SIO�Ɠ������|�W�V�����̃f�[�^������܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
**/	
int Rcb4BaseClass::runSingleServoCmd (byte *txData,ServoData servoData,byte frame)
{
	return runSingleServoCmd (txData, servoData.Id, servoData.Sio, servoData.Data, frame);
}


/**
* @brief �w�肵���T�[�{���[�^��1�����������R�}���h���쐬���܂�
* @param[in] servoData	���������Ƃ��Ă���T�[�{��1�܂Ƃ܂�ɂ����^
* @param[in] frame	�t���[����
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details 1�����T�[�{�𓮂����܂��B
* @details ServoData�^�ɂ́A�T�[�{���[�^��ID�ȊO�ɂ�SIO�Ɠ������|�W�V�����̃f�[�^������܂�
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/	
int Rcb4BaseClass::runSingleServoCmd (ServoData servoData,byte frame)
{
	return runSingleServoCmd (txBuf, servoData.Id, servoData.Sio, servoData.Data, frame);
}



/**
* @brief �w�肵���T�[�{���[�^��1�����������R�}���h���쐬���܂�
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] idNum	�T�[�{���[�^��ID�ԍ�
* @param[in] sioNum	�ڑ����Ă���SIO(SioNum)�̔ԍ�
* @param[in] pos	�T�[�{���[�^�̃|�W�V�����f�[�^(�ڕW�l)
* @param[in] frame	�t���[����
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details 1�����T�[�{�𓮂����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
**/	
int Rcb4BaseClass::runSingleServoCmd (byte *txData,byte idNum,byte sioNum,int pos,byte frame)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;
	WordByte wk;

	ptr = txData;
	wk.word = pos;
	
	*ptr++ = 0x07;						//�R�}���h�̃f�[�^��(�����͌Œ�)
	*ptr++ = SingleServo;				//1�T�[�{�𓮂������߂̃R�}���h
	*ptr++ = icsNum2id(idNum, sioNum);	//ICS�ԍ�
	*ptr++ = frame;						//�t���[����
	*ptr++=  wk.byte[0];				//�|�W�V�����f�[�^
	*ptr++=  wk.byte[1];
	setCheckSum(txData);				//�`�F�b�N�T��������
	return 0x07;
}

/**
* @brief �w�肵���T�[�{���[�^��1�����������R�}���h���쐬���܂�
* @param[in] idNum	�T�[�{���[�^��ID�ԍ�
* @param[in] sioNum	�ڑ����Ă���SIO(SioNum)�̔ԍ�
* @param[in] pos	�T�[�{���[�^�̃|�W�V�����f�[�^(�ڕW�l)
* @param[in] frame	�t���[����
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details 1�����T�[�{�𓮂����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/	
int Rcb4BaseClass::runSingleServoCmd (byte idNum,byte sioNum,int pos,byte frame)
{
	return runSingleServoCmd (txBuf, idNum, sioNum, pos, frame);
}


/**
* @brief �����T�[�{���[�^�ɃA�N�Z�X����Ƃ��ɃR�}���h�Łu���삳����T�[�{�̎w��v�������쐬
* @param[in] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @return �u���삳����T�[�{�̎w��v�̃f�[�^
* @details �u���삳����T�[�{�̎w��v�ɂ�1bit�����Ԃɓ���������(�t���O)�𗧂ĂĂ����܂�
**/	
int64_t	Rcb4BaseClass::setServoNo (ServoData servoDatas[],byte servoCount)
{
	byte	no;
	int64_t	ret = 0;
	for(int i = 0; i < servoCount ; i++)
	{
		no = icsNum2id(servoDatas[i].Id, servoDatas[i].Sio);
		int64_t sf = 0x1;
		ret |=  (sf << no);
	}

	return ret;
}


///
///	�T�[�{�̃f�[�^���\�[�g����̂Ƃ��Ă��T�[�{�f�[�^�̕ԋp
///
/**
* @brief �����T�[�{���[�^�ɃA�N�Z�X����Ƃ��ɃT�[�{�̏��Ԃ̎w�肪����̂Ń\�[�g����
* @param[in,out] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @return �u���삳����T�[�{�̎w��v�̃f�[�^
* @retval true �\�[�g����
* @retval false	�f�[�^��������������s����ICS�ԍ������蓖�Ă��悤�Ƃ����Ƃ�
* @details �T�[�{���[�^�𕡐����������A���Ԃɕ���ł���K�v������̂ł��̃f�[�^���\�[�g���܂�
* @attention servoDatas���̂��̂��\�[�g����̂ŁA���g�����������܂��B
* @attention �f�[�^��ێ����Ă��������ꍇ�́A�R�s�[�����Ă����Ă���g�p���Ă�������
**/	
bool Rcb4BaseClass::sortServoDatas(ServoData servoDatas[],byte servoCount)
{
	const int servoMaxCount = 36;
	
	if(servoCount > servoMaxCount)	//�T�[�{�̌��ȏ゠��悤�Ȃ甲����
	{
		return false;
	}
	
	ServoData bufServoDatas[servoMaxCount];//�ꎞ�I�ɕۑ�����o�b�t�@
	
	int i;		//���[�v�p
	
	for(i = 0;i<servoMaxCount;i++)		//�Ƃ肠����0xFF�ŏ�����
	{
		bufServoDatas[i].Id = 0xFF;
		bufServoDatas[i].Sio = 0xFF;	
	}
	
	for(i = 0;i<servoCount;i++)//�T�[�{�̌������肩�����A�Y������z��Ƀf�[�^�����Ă���
	{
		byte sioNum =  icsNum2id(servoDatas[i].Id,servoDatas[i].Sio);	//ICS�ԍ����v�Z����
		
		if(sioNum > servoMaxCount)		//ICS�ԍ����ςȐ��ɂȂ����ꍇ�͔�����
		{
			return false;
		}
		
		//���Ă͂܂�Ƃ���Ƀf�[�^������
		bufServoDatas[sioNum].Id = servoDatas[i].Id;
		bufServoDatas[sioNum].Sio = servoDatas[i].Sio;
		bufServoDatas[sioNum].Data = servoDatas[i].Data;
		
	}
	
	byte j = 0;
	
	for(i = 0;i<servoMaxCount;i++)		//���Ă͂܂鏇�Ԃ�����
	{
		if(bufServoDatas[i].Id != 0xFF)		//���Ԃɉ񂵂Ă����ăf�[�^���������ꍇ
		{
			//�l����
			servoDatas[j].Id = bufServoDatas[i].Id;
			servoDatas[j].Sio = bufServoDatas[i].Sio;
			servoDatas[j].Data = bufServoDatas[i].Data;
			
			j++;	//���ɐi�߂�
			
			if(j >= servoCount)	//�T�[�{���f�[�^���I������甲����
			{
				break;
			}
		}	
	}
	
	return true;
	
}


//�����̃T�[�{�𓮂���
/**
* @brief �w�肵���T�[�{���[�^�𕡐��������R�}���h�̐���
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @param[in] frame	�t���[����
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �����T�[�{���[�^�𓮂����R�}���h�𐶐����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention ServoData��data�ɂ̓|�W�V�����f�[�^�����܂�
**/	
int Rcb4BaseClass::runConstFrameServoCmd (byte *txData,ServoData servoDatas[],byte servoCount,byte frame)
{

	if(checkuning(txData) == false)	return -1;	

	//�����̓f�[�^��ύX���Ă��܂��̂ŁA�R�s�[���Ă���
	ServoData sDataBuf[servoCount];
	memcpy(sDataBuf,servoDatas , sizeof(ServoData)*servoCount);
	
	byte	*ptr;
	QwordByte wk;

	ptr = txData;

	wk.qword = setServoNo (sDataBuf, servoCount);		//�u���삳����T�[�{�̎w��v���v�Z�����Ă���
	
	*ptr++ = servoCount * 2 + 9;						//���C�Ɉ�ԍŏ������M�f�[�^��
	*ptr++ = ConstFrameServo;							//�����T�[�{�𓮂����R�}���h
	*ptr++ = wk.byte[0];								//�u���삳����T�[�{�̎w��v����
	*ptr++ = wk.byte[1];
	*ptr++ = wk.byte[2];
	*ptr++ = wk.byte[3];
	*ptr++ = wk.byte[4];
	*ptr++ = frame;										//�t���[����
	
	if(sortServoDatas(sDataBuf, servoCount) == false)		//�T�[�{�̃f�[�^�����Ԃɕ��בւ���
	{
		return -1;
	}


	
	for(int i = 0; i < servoCount ; i++)	//�T�[�{�̃f�[�^�����Ԃɓ���Ă���
	{
		wk.word[0] = sDataBuf[i].Data;
		*ptr++=  wk.byte[0];
		*ptr++=  wk.byte[1];
	}
	
	setCheckSum(txData);		//�Ō�̓`�F�b�N�T��������
	
	return servoCount * 2 + 9;
	
}


/**
* @brief �w�肵���T�[�{���[�^�𕡐��������R�}���h�̐���
* @param[in] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @param[in] frame	�t���[����
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �����T�[�{���[�^�𓮂����R�}���h�𐶐����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention ServoData��data�ɂ̓|�W�V�����f�[�^�����܂�
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/	
int Rcb4BaseClass::runConstFrameServoCmd (ServoData servoDatas[],byte servoCount,byte frame)
{
	return runConstFrameServoCmd (txBuf, servoDatas, servoCount, frame);
}




//��x�ɕ����T�[�{�̃p�����[�^��ύX����)
/**
* @brief �w�肵���T�[�{���[�^�𕡐��p�����[�^��ς���R�}���h�̐���
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] servoParameter (Stretch:1 Speed:2)
* @param[in] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �����T�[�{���[�^�𓮂����R�}���h�𐶐����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention ServoData��data�ɂ̓X�g���b�`��X�s�[�h�f�[�^�����܂�(Max 127)
**/	
int Rcb4BaseClass::setParametersBaseCmd (byte *txData, byte servoParameter,ServoData servoDatas[],byte servoCount)
{
	if(checkuning(txData) == false)	return -1;

	
	//�����̓f�[�^��ύX���Ă��܂��̂ŁA�R�s�[���Ă���
	ServoData sDataBuf[servoCount];
	memcpy(sDataBuf,servoDatas , sizeof(ServoData)*servoCount);


	byte	*ptr;
	QwordByte wk;

	ptr = txData;

	if(sortServoDatas(sDataBuf, servoCount) == false)		//�R�s�[�����f�[�^�̓\�[�g���Ă���
	{
		return -1;
	}

	wk.qword = setServoNo (sDataBuf, servoCount);		//�u���삳����T�[�{�̎w��v���v�Z�����Ă���

	*ptr++ = servoCount + 9;		//�R�}���h�̃f�[�^��
	*ptr++ = ServoParam;			//�p�����[�^�ύX�R�}���h
	*ptr++ = wk.byte[0];			//�u���삳����T�[�{�̎w��v
	*ptr++ = wk.byte[1];			
	*ptr++ = wk.byte[2];
	*ptr++ = wk.byte[3];
	*ptr++ = wk.byte[4];
	*ptr++ = servoParameter;		//(Stretch:1 Speed:2)
	for(int i = 0; i < servoCount ; i++)				//�\�[�g�����T�[�{�̃f�[�^����
	{
		*ptr++=  (byte)( sDataBuf[i].Data & 0x00ff);
	}
	setCheckSum(txData);
	return servoCount + 9;
}



//��x�ɕ����T�[�{�̃p�����[�^��ύX����)
/**
* @brief �w�肵���T�[�{���[�^�𕡐��p�����[�^��ς���R�}���h�̐���
* @param[in] servoParameter (Stretch:1 Speed:2)
* @param[in] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �����T�[�{���[�^�𓮂����R�}���h�𐶐����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention ServoData��data�ɂ̓X�g���b�`��X�s�[�h�f�[�^�����܂�(Max 127)
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/	
int Rcb4BaseClass::setParametersBaseCmd (byte servoParameter,ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txBuf, servoParameter, servoDatas, servoCount);
}



//��x�ɃT�[�{���[�^�̃X�s�[�h���w�肷��
/**
* @brief �w�肵���T�[�{���[�^�𕡐��X�s�[�h��ς���R�}���h�̐���
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �����T�[�{���[�^�𓮂����R�}���h�𐶐����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention ServoData��data�ɂ̓X�s�[�h�f�[�^�����܂�(Max 127)
**/	
int Rcb4BaseClass::setSpeedCmd (byte *txData,ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txData, 0x02, servoDatas, servoCount);
}



/**
* @brief �w�肵���T�[�{���[�^�𕡐��X�s�[�h��ς���R�}���h�̐���

* @param[in] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �����T�[�{���[�^�𓮂����R�}���h�𐶐����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention ServoData��data�ɂ̓X�s�[�h�f�[�^�����܂�(Max 127)
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/	
int Rcb4BaseClass::setSpeedCmd (ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txBuf, 0x02, servoDatas, servoCount);
}



//(��x�ɃT�[�{���[�^�̃X�g���b�`���w�肷��)
/**
* @brief �w�肵���T�[�{���[�^�𕡐��X�g���b�`��ς���R�}���h�̐���
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @param[in] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �����T�[�{���[�^�𓮂����R�}���h�𐶐����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention ServoData��data�ɂ̓X�g���b�`�f�[�^�����܂�(Max 127)
**/	
int Rcb4BaseClass::setStretchCmd (byte *txData,ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txData, 0x01, servoDatas, servoCount);
}



/**
* @brief �w�肵���T�[�{���[�^�𕡐��X�g���b�`��ς���R�}���h�̐���
* @param[in] servoDatas[]	�A�N�Z�X����T�[�{�̃f�[�^�̔z��
* @param[in] servoCount	�A�N�Z�X����T�[�{�̃f�[�^�̔z��(�T�[�{�̐�)
* @return ���M�f�[�^��
* @retval -1	�o�b�t�@�ɉ����g���Ă��邩�T�C�Y��249byte�ȏ�
* @details �����T�[�{���[�^�𓮂����R�}���h�𐶐����܂��B
* @details �Ԓl��txData�̃f�[�^byte����Ԃ��܂��̂ŁA���M����̂Ɏg�p���܂�
* @attention ServoData��data�ɂ̓X�g���b�`�f�[�^�����܂�(Max 127)
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/	
int Rcb4BaseClass::setStretchCmd (ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txBuf, 0x01, servoDatas, servoCount);
}

//ack��Ԃ��R�}���h�̍쐬
/**
* @brief ack�R�}���h�̐���
* @param[out] *txData	���ݓ���悤�Ǝv���Ă��鑗�M�o�b�t�@
* @return ���M�f�[�^��
* @details ACK�̃R�}���h�𐶐�����
**/	
int Rcb4BaseClass::acknowledgeCmd (byte *txData)
{
	byte	*ptr;
	ptr = txData;

	*ptr++ = 0x04;				//�R�}���h�̃f�[�^��(���̃R�}���h�͌Œ�)
	*ptr++ = AckCheck;			//ACK�R�}���h
	*ptr++ = Ack;				//ACK
	setCheckSum(txData);		//�`�F�b�N�T��
	return	0x04;
}

/**
* @brief ack�R�}���h�̐���
* @return ���M�f�[�^��
* @details ACK�̃R�}���h�𐶐�����
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/	
int Rcb4BaseClass::acknowledgeCmd ()
{
	return acknowledgeCmd(txBuf);
}

//////////////////////////////////////////////////////////////////////////////////////////
//�ʐM�֌W�i�����̓C���^�[�t�F�C�X�N���X�j

//���ۂɑ���M���s��
//bool Rcb4BaseClass::synchronize(byte txBuf[],byte txLen,byte rxBuf[],byte rxLen)
//{
//
//	isSynchronize = true;
//	if (rcbHardSerial != nullptr)
//	{
//
//		isSynchronize = true;
//		rcbHardSerial->flush();
//
//		for(byte i = 0; i < txLen; i++)
//		{
//			rcbHardSerial->write(txBuf[i]);
//	}
//
//		rcbHardSerial->setTimeout(g_timeout);
//		if(rcbHardSerial->readBytes(rxBuf,rxLen) == rxLen)
//		{
//			isSynchronize = false;
//			return checkReverceCmd(rxBuf,txBuf[1]);
//	}
//		else
//		{//��ѱ��
//			isSynchronize = false;
//			return	false;
//	}
//	}
//	else
//	{
//		return false;
//
//	}
//
//	return true;
//}
//



//ack�𑗂��ĂȂ����Ă��邩�m�F����
/**
* @brief ack�R�}���h�𑗐M
* @return ACK�R�}���h�㍇�۔���
* @retval False �R�}���h���Ԃ��Ă��Ȃ������������NCK���Ԃ��Ă����Ƃ�
* @details ACK�̃R�}���h�𑗐M����
* @attention �R�}���h�f�[�^��Rcb4BaseClass��txBuf���ɓ���܂�
**/	
bool Rcb4BaseClass::checkAcknowledge()
{
	bool checkFlag = true; 
		
	acknowledgeCmd (txBuf);		//ACK�R�}���h�̐���
	
	checkFlag = synchronize(txBuf, txBuf[0], rxBuf, 4);		//�R�}���h�̑��M
	
	if( false == checkFlag || rxBuf[2] == Nack)				//���۔���
	{
		return false;
	}
	else					//���Ȃ��ꍇ��true��Ԃ�
	{
		return true;
	}
}



//////////////////////////////////////////////////////////////////////////
//�`�F�b�N�T�����v�Z�̌v�Z
/**
* @brief �R�}���h�̃`�F�b�N�T�����v�Z���܂�
* @param[in] dataBytes[]	�R�}���h���������f�[�^
* @return �R�}���h�̃`�F�b�N�T��
* @details �R�}���h�̃`�F�b�N�T�����v�Z���܂�
* @attention �R�}���h�̐擪�ɂ̓R�}���h�̃f�[�^���������Ă��܂�
**/	
byte	Rcb4BaseClass::CheckSum(byte dataBytes[])
{
	byte sum = 0;
	for(int i = 0; i < dataBytes[0] - 1; i++ )
	{
		sum += dataBytes[i];
	}
	return sum;
}



//�R�}���h�̃`�F�b�N�T�����v�Z���������)
/**
* @brief �R�}���h�̃`�F�b�N�T�����v�Z���A�Ō���ɂ���܂�
* @param[in,out] dataBytes[]	�R�}���h���������f�[�^
* @return ����ꂽ���ǂ����̔���
* @retval False �R�}���h�̐擪��0��������
* @details �R�}���h�̃`�F�b�N�T�����v�Z���A�Ō���ɑ�����܂�
* @attention �R�}���h�̐擪�ɂ̓R�}���h�̃f�[�^���������Ă��܂�
**/	
bool Rcb4BaseClass::setCheckSum(byte dataBytes[])
{
	if(dataBytes[0] == 0)
	{
		return false;
	}
	else
	{
		dataBytes[dataBytes[0] -1 ] = CheckSum(dataBytes); 
		return true;
	}
}



//�`�F�b�N�T���������Ă��邩�m�F����
/**
* @brief �����Ă����R�}���h�̃`�F�b�N�T���������Ă��邩�m�F���܂�
* @param[in] dataBytes[]	�R�}���h���������f�[�^
* @return �`�F�b�N�T���̔���
* @retval False �`�F�b�N�T�����Ԉ���Ă����Ƃ��A�f�[�^����0��������
* @details �����Ă����R�}���h�̃`�F�b�N�T���������Ă��邩�m�F���܂�
* @attention �R�}���h�̐擪�ɂ̓f�[�^���������Ă��܂�
**/	
bool Rcb4BaseClass::checkCheckSum(byte dataBytes[])
{
	if(dataBytes[0] == 0)
	{
		return false;
	}
	else
	{
		if(dataBytes[dataBytes[0] -1 ] == CheckSum(dataBytes))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}


//�R�}���h���Œ�������Ă��邩�m�F����)
/**
* @brief �ԐM�f�[�^���ȈՓI�`�F�b�N�����܂�
* @param[in] dataBytes[]	�ԐM�f�[�^���������z��
* @param[in] cmd			���M�����R�}���h
* @return �ԐM�f�[�^�̔���
* @retval False �`�F�b�N�T�����Ԉ���Ă����Ƃ��A2byte�ڂ����M�R�}���h�ƈ���Ă����Ƃ�
* @details �ԐM�f�[�^�̊ȈՃ`�F�b�N�����܂�
		 �ԐM�f�[�^�̃`�F�b�N�T������уR�}���h�����̃`�F�b�N
* @attention �R�}���h�̐擪�ɂ̓f�[�^���������Ă��܂�
**/	
bool Rcb4BaseClass::checkReverceCmd(byte dataBytes[],byte cmd)
{

//	const	byte cnmtbl[8] = {Move,	Jump, Call,	SingleServo, ConstFrameServo,ServoParam,AckCheck,None};	//�L�������
	if(checkCheckSum(dataBytes) == true && dataBytes[1] == cmd)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//
//	moveRamToComCmd����f�[�^��M�܂ł̊֐�
//
/**
* @brief Move�R�}���h��RAM��̃f�[�^��COM�ɓ]�����܂�
* @param[in] scrAddr	�擾����RAM�f�[�^�̐擪�A�h���X
* @param[in] scrDataSize	�擾����RAM�̃f�[�^��
* @param[out] rxDara[]		��M�f�[�^(������RAM�̃f�[�^�̂�)
* @return �ԐM�f�[�^�̃f�[�^��
* @retval -1 (0xFFFF) ���M���s��
* @details RAM��w�肵���擪�A�h���X����w�肵���f�[�^�����̃f�[�^��COM�ɓ]�����܂�
* @note �Ԃ�l�̓R�}���h���܂܂Ȃ��f�[�^�ɂȂ�܂��B
* @date 2019/01/16 rxBuf�̐擪�A�h���X�����Ƃ�memcopy���s���Ɨ\�����ʃf�[�^�Ō듮�삷��̂�for���Ńf�[�^��}������悤�ɕύX
**/	
unsigned short Rcb4BaseClass::moveRamToComCmdSynchronize(int scrAddr,byte scrDataSize,byte	rxDara[])
{	
	
	if(	moveRamToComCmd(scrAddr ,scrDataSize) == -1)	return 0xffff;



	if(synchronize(txBuf, txBuf[0], rxBuf, scrDataSize + 3) == true)
	{
		//2019/01/16 memcopy�������rxBuf[0]�ɗ\�����ʃf�[�^������ꍇ������̂ŁAfor�ɕύX
		for(int i = 0;i<scrDataSize;i++)
		{
			rxDara[i] = rxBuf[i+2];
		}
		
		return scrDataSize;
		
		return 0xffff;
	}
	else
	{
	
		return 0xffff;
	}
}


//
//	moveComToRamCmd����ACK��M�܂ł̊֐�
//
/**
* @brief Move�R�}���h��RAM��̃f�[�^��COM�ɓ]�����܂�
* @param[in] scrAddr	�擾����RAM�f�[�^�̐擪�A�h���X
* @param[in] destData[]	���M�f�[�^�z��(�P���Ƀf�[�^�̂�)
* @param[out] destDataSize	���M�f�[�^�T�C�Y(�P���Ƀf�[�^�̂�)
* @return �ʐM���蓙
* @retval False ���M���s��
* @details COM����RAM��̎w�肵���A�h���X�Ƀf�[�^���������݂܂�
**/	
bool	Rcb4BaseClass::moveComToRamCmdSynchronize(int scrAddr,byte destData[],int destDataSize)
{

	if(	moveComToRamCmd(scrAddr ,destData, destDataSize) == -1)	return false;	
	
	if(synchronize(txBuf, txBuf[0], rxBuf, 4) == true)
	{			
		return rxBuf[2] == Ack ?  true: false;
	}
	else
	{
		return false;
	}
}

//
//����M��`�b�j����
/**
* @brief ��������R�}���h�𑗂��āAACK���Ԃ��Ă��邩�m�F
* @param[in] *txData	���M�f�[�^
* @return �ʐM���蓙
* @retval False ���M���s��
* @details ��������R�}���h�𑗂��āAACK���Ԃ��Ă��邩�m�F
* @note txData�̐擪�f�[�^�ɂ͑��M�f�[�^�̃f�[�^���������Ă��܂�
**/	
bool	Rcb4BaseClass::SynchronizeAck(byte	*txData)
{
	if(synchronize(txData, *txData, rxBuf, 4) == true)
	{
		return rxBuf[2] == Ack ?  true: false;
	}
	else
	{
		return false;
	}
}




//////////////////////////////////////////////////////////////////////////////////////////
//PIO�֌W
//PIO�̌��݂̏�Ԃ��擾����
/**
* @brief ���݂�PIO�̏�Ԃ��擾���܂�
* @return ���݂�PIO�̏��(2byte���܂߂��ׂ�)
* @retval -1(0xFFFF) �ʐM���s
* @details Move���߂��g���Č��݂�PIO��H��L�̏�Ԃ��擾���܂�
* @attention 1bit������ł��Ȃ��̂Œ���
**/	
unsigned short Rcb4BaseClass::getPio()
{
	byte	wkbuf[10];
	if(moveRamToComCmdSynchronize(PioAddress ,0x02,wkbuf) != 0xffff)
	{
		WordByte wk;
		wk.byte[0] = wkbuf[0];
		wk.byte[1] = wkbuf[1];

		wk.word &= 0x03FF;		//����Ȃ�bit�̓}�X�N��������

		return 	wk.word;	
	}
	else
	{
		return 0xffff;
	}
}

//PIO�̏�Ԃ�ύX����)
/**
* @brief ���݂�PIO�̏�Ԃ�ύX���܂�
* @param[in] pioData	PIO�S�̂̃f�[�^
* @return �ʐM�������������ǂ���
* @retval False �ʐM���s
* @details Move���߂��g���Č��݂�PIO��H��L�̏�Ԃɂ��܂�
* @attention 1bit���ύX�ł��Ȃ��̂Œ���
**/	
bool Rcb4BaseClass::setPio (unsigned short pioData)
{
	return moveComToRamCmdSynchronize(PioAddress, (byte*)&pioData,2);
}

//PIO�̓��o�͂��擾����
/**
* @brief ���݂�PIO�̓��o�͏�Ԃ��擾���܂�
* @return ���݂�PIO�̏��(2byte���܂߂��ׂ�)
* @retval -1(0xFFFF) �ʐM���s
* @details Move���߂��g���Č��݂�PIO�̓��͂��o�͂����擾���܂�
* @note �Ή�����bit�́@1:�o�́@0:����
* @attention 1bit������ł��Ȃ��̂Œ���
**/	
unsigned short Rcb4BaseClass::getPioMode()
{
	byte	wkbuf[10];
	if(moveRamToComCmdSynchronize(PioModeAddres ,0x02,wkbuf) != 0xffff)
	{
		WordByte wk;
		wk.byte[0] = wkbuf[0];
		wk.byte[1] = wkbuf[1];

		wk.word &= 0x03FF;		//����Ȃ�bit�̓}�X�N��������

		return 	wk.word;	
	}
	else
	{
		return 0xffff;
	}
}


//PIO�̓��o�͂̎w���ύX����)
/**
* @brief ���݂�PIO�̓��o�͏�Ԃ�ύX���܂�
* @param[in] pioModeData	PIO�S�̂̓��o�̓f�[�^
* @return �ʐM�������������ǂ���
* @retval False �ʐM���s
* @details Move���߂��g���Č��݂�PIO���͂Əo�͂�ύX���܂�
* @note �Ή�����bit�́@1:�o�́@0:����
* @attention 1bit���ύX�ł��Ȃ��̂Œ���
**/
bool Rcb4BaseClass::setPioMode (unsigned short pioModeData)
{
	return moveComToRamCmdSynchronize(PioModeAddres, (byte*)&pioModeData,2);
}



//////////////////////////////////////////////////////////////////////////////////////////
//AD�֌W
//AD�̏�Ԃ��擾����
/**
* @brief ���ݎw�肵��AD�̒l���擾���܂�
* @param[in] adPort	AD�|�[�g�̎w��(0�`10)
* @return �w�肵��AD�̒l(����\10bit)
* @retval -1(0xFFFF) �ʐM���s
* @details Move���߂��g���Ďw�肵�����݂�AD�l���擾���܂�
* @note	 AD0���擾����ƌ��݂̓d���d���l�𕪈������l���ǂ߂܂�
**/	
unsigned short Rcb4BaseClass::getAdData(int adPort)
{
	byte	wkbuf[10];
	if(moveRamToComCmdSynchronize(AdcRamAddress + adPort * 2 ,0x02,wkbuf) != 0xffff)
	{
		WordByte wk;
		wk.byte[0] = wkbuf[0];
		wk.byte[1] = wkbuf[1];
		
		return 	wk.word;
	}
	else
	{
		return 0xffff;
	}

}

//���ׂĂ�AD�̏�Ԃ��擾����
/**
* @brief ���݂�AD�̒l���ׂĂ��擾���܂�
* @param[out] adDatas[] ���ׂĂ�AD�l���i�[����z��
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details Move���߂��g���Ďw�肵�����݂�AD�l���擾���܂�
* @note	 AD0���擾����ƌ��݂̓d���d���l�𕪈������l���ǂ߂܂�
* @attention adDatas[]��11���̔z����ɗp�ӂ��Ă�������
**/
bool Rcb4BaseClass::getAllAdData(int	adDatas[])
{
	byte	wkbuf[30];
	
	if(moveRamToComCmdSynchronize(AdcRamAddress  ,AdcDataCount ,wkbuf) != 0xffff)
	{
		int	*iptr = (int*)&wkbuf;
		
		for(int i = 0; i < AdcCount; i++)
		{
			adDatas[i] = *iptr++;
		}
		return 	true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//�{�^���f�[�^�𑗕t
//KRR�̂��ׂẴf�[�^��ݒ肷��
/**
* @brief KRR�̗̈�Ƀf�[�^�����ׂđ��M����(�^���I�Ƀ{�^���f�[�^�𑗂�)
* @param[in] buttonData �{�^���f�[�^
* @param[in] adData 4byte���̃A�i���O�f�[�^���w��
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details Move���߂��g����KRR�̗̈�Ƀf�[�^�����ׂđ��M���܂�
* @note	 �{�^���f�[�^��RCB4�ɂ�����܂��B
* @note	�T���v�����[�V�������{�^�����g���K�ɂ��Ă��郂�[�V�����𓮂������Ɏg�p���܂�
* @note	�{�^���������ĂȂ���Ԃ�������܂ŉ������ςȂ��ɂȂ�܂�
* @attention RCB4��KRR��ڑ����Ȃ��ł�������
**/
bool Rcb4BaseClass::setKrrData (unsigned short buttonData, byte adData[])
{
	byte	wk[6];
	WordByte wwk;

	wwk.word = buttonData;

	wk[0] = wwk.byte[0];
	wk[1] = wwk.byte[1];
	memcpy(&wk[2],adData,4);
	return moveComToRamCmdSynchronize(KrrButtonDataAddress, wk,6);
}



//KRR�̃{�^���f�[�^��ݒ肷��
/**
* @brief KRR�̗̈�Ƀ{�^���f�[�^�𑗐M����(�^���I�Ƀ{�^���f�[�^�𑗂�)
* @param[in] buttonData �{�^���f�[�^
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details Move���߂��g����KRR�̗̈�Ƀ{�^���f�[�^�𑗐M���܂�
* @note	�{�^���f�[�^��RCB4�ɂ�����܂��B
* @note	�T���v�����[�V�������{�^�����g���K�ɂ��Ă��郂�[�V�����𓮂������Ɏg�p���܂�
* @note	�{�^���������ĂȂ���Ԃ�������܂ŉ������ςȂ��ɂȂ�܂�
* @attention RCB4��KRR��ڑ����Ȃ��ł�������
**/
bool Rcb4BaseClass::setKrrButtonData (unsigned short buttonData)
{
	byte wk[2];
	
	WordByte wwk;

	wwk.word = buttonData;
	
	wk[0] = wwk.byte[1];
	wk[1] = wwk.byte[0];			//�{�^���f�[�^�̓v���O�������猩��Ƌt�ɔz�u����Ă���
	
	return moveComToRamCmdSynchronize(KrrButtonDataAddress, wk,2);
}

//KRR��AD�̃f�[�^��ݒ肷��)
/**
* @brief KRR�̗̈�Ƀ{�^���f�[�^�𑗐M����(�^���I�Ƀ{�^���f�[�^�𑗂�)
* @param[in] paPort �w�肷��AD�|�[�g(1�`4)
* @param[in] adData AD�f�[�^(7bit)
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details Move���߂��g����KRR��AD�f�[�^�𑗐M���܂�
* @note	 KRR�Ɏ�M����Ă���AD�̗̈�Ƀf�[�^�ɂ�����܂��B
* @note	 KRR��AD�l���g���ă��[�V������ύX���Ă���ꍇ�ɊO������ύX�ł��܂�
* @attention RCB4��KRR��ڑ����Ȃ��ł�������
**/
bool Rcb4BaseClass::setKrrAdData (byte paPort, byte adData)
{
	byte	wk[2];

	wk[0] =  adData;
	return moveComToRamCmdSynchronize(KrrPa1Address + (paPort - 1), wk,1);

}



//////////////////////////////////////////////////////////////////////////////////////////
//�T�[�{���[�^
//�T�[�{���[�^�P�̂𓮂���
/**
* @brief �w�肵���T�[�{���[�^�P�̂𓮍삳���܂�
* @param[in] id		�T�[�{���[�^��ID�ԍ�
* @param[in] sio	SIO�̒l(SioNum)
* @param[in] pos	�T�[�{���[�^�̖ڕW�|�W�V�����l
* @param[in] frame	�t���[����(�����قǂ�����蓮��)
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details �w�肵���T�[�{���[�^��ID,SIO,POS,Frame�����g�p���ē������܂�
* @note	 ���[�V�����Đ����Ɏw�肵���ꍇ�̓��[�V�����̊p�x�f�[�^���D�悳��܂�
**/
bool Rcb4BaseClass::setSingleServo (byte id, byte sio,int pos,byte frame)
{
	if(runSingleServoCmd(id, sio, pos, frame) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



/**
* @brief �w�肵���T�[�{���[�^�P�̂𓮍삳���܂�
* @param[in] servoData		�T�[�{���[�^�̃f�[�^���ЂƂ����܂�ɂ�������
* @param[in] frame	�t���[����(�����قǂ�����蓮��)
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details servoData�ɓ����Ă���T�[�{���[�^�̏������Ƃ�,Frame�����g�p���ē������܂�
* @note	 ���[�V�����Đ����Ɏw�肵���ꍇ�̓��[�V�����̊p�x�f�[�^���D�悳��܂�
**/
bool Rcb4BaseClass::setSingleServo (ServoData servoData,byte frame)
{
	if(runConstFrameServoCmd(&servoData, 1, frame) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



/**
* @brief �w�肵���T�[�{���[�^�P�̂��t���[�ɂ��܂�
* @param[in] id		�T�[�{���[�^��ID
* @param[in] sio	SIO�̒l(SioNum)
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details ID,SIO�Ŏw�肵���T�[�{���[�^���t���[�ɂ��܂�
* @attention	 Free���w�肷���Hold�̃R�}���h�����s�����܂�Free��Ԃ�ۂ������܂�
**/
bool Rcb4BaseClass::setFreeSingleServo (byte id, byte sio)
{
	if(runSingleServoCmd(id, sio, 0x8000, 1) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



/**
* @brief �w�肵���T�[�{���[�^�P�̂Ƀg���N�������܂�
* @param[in] id		�T�[�{���[�^��ID
* @param[in] sio	SIO�̒l(SioNum)
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details ID,SIO�Ŏw�肵���T�[�{���[�^�Ƀg���N�������܂�
			Free��Ԃ���g���N���������Ԃɕ��A���܂�
* @note	 Free���w�肷���Hold�̃R�}���h�����s�����܂�Free��Ԃ�ۂ������܂�
**/
bool Rcb4BaseClass::setHoldSingleServo (byte id, byte sio)
{
	if(runSingleServoCmd(id, sio, 0x7FFF, 1) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



//�w�肵�������̃T�[�{���[�^�𓮂���)
/**
* @brief �w�肵���T�[�{���[�^�����𓮍삳���܂�
* @param[in] servoDatas[]		�T�[�{���[�^�̃f�[�^���ЂƂ����܂�ɂ������̂𕡐���
* @param[in] servoCount servoDatas�̃T�[�{�̐�
* @param[in] frame	�t���[����(�����قǂ�����蓮��)
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details servoData�ɓ����Ă��镡���̃T�[�{���[�^�̏������Ƃ�Frame�����g�p���ē������܂�
* @note	 ���[�V�����Đ����Ɏw�肵���ꍇ�̓��[�V�����̊p�x�f�[�^���D�悳��܂�
**/
bool Rcb4BaseClass::setServoPos (ServoData servoDatas[],byte servoCount,byte frame)
{
	if(runConstFrameServoCmd(servoDatas, servoCount, frame) == -1)	return false;
	return	SynchronizeAck(txBuf);
}

//�w�肵�������̃T�[�{���[�^���t���[�ɂ���
/**
* @brief �w�肵���T�[�{���[�^�P�̂��t���[�ɂ��܂�
* @param[in] servoDatas[]		�T�[�{���[�^�̃f�[�^���ЂƂ����܂�ɂ������̂𕡐���
* @param[in] servoCount servoDatas�̃T�[�{�̐�
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details servoDatas[]�Ŏw�肵���T�[�{���[�^���t���[�ɂ��܂�
* @attention	 Free���w�肷���Hold�̃R�}���h�����s�����܂�Free��Ԃ�ۂ������܂�
* @attention	 servoDatas��Data��0x8000�ɏ��������܂�
**/
bool Rcb4BaseClass::setFreePos (ServoData servoDatas[],byte servoCount)
{
	for(int i = 0; i < servoCount; i++)
	{
		servoDatas[i].Data = 0x8000;
	}
	return	setServoPos (servoDatas, servoCount, 1);
}



/**
* @brief �w�肵�������̃T�[�{���[�^�Ƀg���N�������܂�
* @param[in] servoDatas[]		�T�[�{���[�^�̃f�[�^���ЂƂ����܂�ɂ������̂𕡐���
* @param[in] servoCount servoDatas�̃T�[�{�̐�
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details servoDatas[]�Ŏw�肵���T�[�{���[�^���t���[�ɂ��܂�
			Free��Ԃ���g���N���������Ԃɕ��A���܂�
* @note	 Free���w�肷���Hold�̃R�}���h�����s�����܂�Free��Ԃ�ۂ������܂�
* @attention	 servoDatas��Data��0x7FFF�ɏ��������܂�
**/
bool Rcb4BaseClass::setHoldPos (ServoData servoDatas[],byte servoCount)
{
	for(int i = 0; i < servoCount; i++)
	{
		servoDatas[i].Data = 0x7FFF;
	}
	return	setServoPos (servoDatas, servoCount, 1);
}



//�w�肵���T�[�{���[�^�̃|�W�V�����f�[�^���擾����
/**
* @brief �w�肵���T�[�{���[�^�̃|�W�V�����f�[�^���擾���܂�
* @param[in] id		�T�[�{���[�^��ID
* @param[in] sio	SIO�̒l(SioNum)
* @return �w�肵���T�[�{���[�^�̃|�W�V�����f�[�^
* @retval -1 �ʐM���s
* @details ID,SIO�Ŏw�肵���T�[�{���[�^�̃|�W�V�����f�[�^���擾���܂�
**/
int Rcb4BaseClass::getSinglePos(byte id,byte sio)
{
	if(moveDeviceToComCmd(id,sio, MotorPositionAddressOffset, 2) == -1)	return -1;
	if(synchronize(txBuf, txBuf[0], rxBuf, 5) == true)
	{
		WordByte	wk;
		wk.byte[0] = rxBuf[2];
		wk.byte[1] = rxBuf[2];
		return wk.word;
	}
	else
	{
		return -1;
	}
}

//�w�肵���T�[�{���[�^�̃X�s�[�h��ύX����
/**
* @brief �w�肵���T�[�{���[�^�̃X�s�[�h��ύX���܂�
* @param[in] servoDatas[]		�T�[�{���[�^�̃f�[�^���ЂƂ����܂�ɂ������̂𕡐���
* @param[in] servoCount servoDatas�̃T�[�{�̐�
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details servoDatas[]�Ŏw�肵���T�[�{���[�^�̃X�s�[�h(�o�͂̏���l)��ύX���܂�
* @note	 servoDatas��Data�ɂ̓X�s�[�h�l�����܂�
**/
bool Rcb4BaseClass::setServoSpeed (ServoData servoDatas[],byte servoCount)
{
	
	if(setSpeedCmd(servoDatas, servoCount) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



//�w�肵���T�[�{���[�^�̃X�g���b�`��ύX����
/**
* @brief �w�肵���T�[�{���[�^�̃X�g���b�`��ύX���܂�
* @param[in] servoDatas[]		�T�[�{���[�^�̃f�[�^���ЂƂ����܂�ɂ������̂𕡐���
* @param[in] servoCount servoDatas�̃T�[�{�̐�
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details servoDatas[]�Ŏw�肵���T�[�{���[�^�̃X�g���b�`(�_�炩��)��ύX���܂�
* @note	 servoDatas��Data�ɂ̓X�g���b�`�l�����܂�
**/
bool Rcb4BaseClass::setServoStretch (ServoData servoDatas[],byte servoCount)
{
	
	if(setStretchCmd(servoDatas, servoCount) == -1) return false;
	return	SynchronizeAck(txBuf);

}

//ID��SIO����ICS�ԍ��ɕϊ�����
/**
* @brief ID��SIO�̔ԍ�����ICS�ԍ��ɕύX���܂�
* @param[in] id		�T�[�{���[�^��ID
* @param[in] sio	SIO�̒l(SioNum)
* @return ICS�ԍ�
* @details ID��SIO�̔ԍ�����RCB4�����ň���ICS�ԍ��ɕύX���܂�
**/
byte Rcb4BaseClass::icsNum2id(byte id,byte sio)
{
	return id * 2 + (sio - 1);
}

//////////////////////////////////////////////////////////////////////////////////////////
//�T�[�{���[�^

//�w�肵�����[�V�������Đ�����
/**
* @brief �w�肵�����[�V�������Đ������܂�
* @param[in] motionNum		�Đ������郂�[�V�����ԍ�
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details �w�肵�����[�V�������Đ������܂�
* @note		suspend() => resetProgramCounter() => setMotionNum() => resume() 
* @note		�ꎞ���f  => �v���O�����J�E���^�����Z�b�g => ���������[�V�����A�h���X�ɃW�����v => ���[�V�����̍ĊJ
			�̎菇�Ń��[�V�������Đ������܂�
* @attention	���݁A���̃��[�V�������Đ����ł������I�Ɏ~�߂Ă���Đ������܂�
**/
bool Rcb4BaseClass::motionPlay(int motionNum)
{
	
	if(suspend() == false)	return false;
	
	if(resetProgramCounter() == false)	return false;
	
	if(setMotionNum(motionNum) == false)	return false;
	
	return resume ();
}



//���[�V�������ꎞ��~����
/**
* @brief ���[�V�������ꎞ��~�����܂�
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details ���[�V�������ꎞ��~�����܂�
* @note		���m�ɂ́A�V�X�e�����W�X�^��EnableRunEeprom�l��ύX���܂�
				�x�N�^�W�����v�𖳌������Ė����@�Ȃǂ̃{�^���f�[�^�ŃW�����v����̂���߂܂�
* @attention	���݁A���̃��[�V�������Đ����ł������I�Ɏ~�߂܂�
* @attention	configData�l�𒼐ڕς��܂��̂ŁA���ӂ��K�v�ł�
**/
bool Rcb4BaseClass::suspend()
{
	return suspend(&(configData.word));
}



/**
* @brief ���[�V�������ꎞ��~�����܂�
* @param[in,out] config		�O���Ŋo���Ă���config�f�[�^
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details ���[�V�������ꎞ��~�����܂�
* @note		���m�ɂ́A�V�X�e�����W�X�^��EnableRunEeprom�l��ύX���܂�
				�x�N�^�W�����v�𖳌������Ė����@�Ȃǂ̃{�^���f�[�^�ŃW�����v����̂���߂܂�
* @attention	���݁A���̃��[�V�������Đ����ł������I�Ɏ~�߂܂�
* @attention	config���𒼐ڕς��܂��̂ŁA���ӂ��K�v�ł�
**/
bool Rcb4BaseClass::suspend(unsigned short *config)
{
	ConfigData *cData = (ConfigData *)config;
	
	cData->bit.EnableRunEeprom =0;			//EEPROM�Ŏ��s���̕������~�߂�
	//configData.bit.EnableServoResponse = 0;		//�T�[�{�̃��X�|���X��Ԃ��Ȃ�����
	cData->bit.EnableReferenceTable = 0;	//�x�N�^�W�����v�𖳌��ɂ���
	//configData.bit.EnableSio = 1;				//SIO�͓������܂܂ɂ���
	
	return moveComToRamCmdSynchronize(ConfigRamAddress, (byte*)config,2);
	

}



//ROM�̃��[�V�����A�h���X�ɔ��
/**
* @brief �w�肵�����[�V�����ɃW�����v���܂�
* @param[in] motionNum		�W�����v���郂�[�V�����ԍ�
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details �v���O�����J�E���^�����Z�b�g���܂��B
* @note		CALL���߂Ŏw���������[�V�����̐擪�A�h���X�ɃW�����v���܂�
* @attention	���݁A���̃��[�V�������Đ����ł������I�ɃW�����v����̂Œ��ӂ��K�v�ł�
**/
bool Rcb4BaseClass::setMotionNum(byte motionNum)
{
	if(callCmd(txBuf	,motionAddr2motionNum(motionNum)) == -1) return false;
	return SynchronizeAck(txBuf);
}



//�v���t�����J�E���^�����Z�b�g���܂�
/**
* @brief �v���t�����J�E���^�����Z�b�g���܂�
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details �v���O�����J�E���^�����Z�b�g���܂��B
* @note		�v���O�����J�E���^��RCB4�����ݒ��̃A�h���X�Ɉړ������܂��B
			�܂��A������EEPROM�X�V�t���O��0�ɂ��Ă����܂�
* @attention	���݁A���̃��[�V�������Đ����ł������I�ɕς��܂��̂ŁA���ӂ��K�v�ł�
**/
bool Rcb4BaseClass::resetProgramCounter()
{
	WordByte wwk;
	byte	wk[10];

	wwk.word = MainLoopCmd;
	memset(wk,0x00,10);
	wk[0] = wwk.byte[0];  
	wk[1] = wwk.byte[1]; 
	return moveComToRamCmdSynchronize(ProgramCounterRamAddress, wk,10);
}



//���[�V���������X�^�[�g(���A)�����܂�
/**
* @brief ���[�V�������ĊJ�����܂�
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details ���[�V�������ꎞ��~�����܂�
* @note		���m�ɂ́A�V�X�e�����W�X�^��EnableRunEeprom�l��ύX���܂�
				�x�N�^�W�����v��L�������Ė����@�Ȃǂ̃{�^���f�[�^�ŃW�����v�ł���悤�ɂ��܂�
* @attention	���݁A���̃��[�V�������Đ����ł������I�Ɏ~�߂܂�
* @attention	configData�l�𒼐ڕς��܂��̂ŁA���ӂ��K�v�ł�
**/
bool Rcb4BaseClass::resume ()
{
	return resume(&(configData.word));
}



/**
* @brief ���[�V�������ĊJ�����܂�
* @param[in,out] config		�O���Ŋo���Ă���config�f�[�^
* @return �ʐM���������ǂ���
* @retval False �ʐM���s
* @details ���[�V�������ꎞ��~�����܂�
* @note		���m�ɂ́A�V�X�e�����W�X�^��EnableRunEeprom�l��ύX���܂�
				�x�N�^�W�����v��L�������Ė����@�Ȃǂ̃{�^���f�[�^�ŃW�����v�ł���悤�ɂ��܂�
* @attention	config���𒼐ڕς��܂��̂ŁA���ӂ��K�v�ł�
**/
bool Rcb4BaseClass::resume (unsigned short *config)
{
	ConfigData *cData = (ConfigData *)config;
	
	cData->bit.EnableRunEeprom =1;			//EEPROM�̓ǂݍ��ݓ�����X�^�[�g������
	//configData.bit.EnableServoResponse = 0;		//�T�[�{�̃��X�|���X��Ԃ��Ȃ�����
	cData->bit.EnableReferenceTable = 1;	//�x�N�^�W�����v��L���ɂ���
	//configData.bit.EnableSio = 1;				//SIO�͓������܂܂ɂ���
	
	return moveComToRamCmdSynchronize(ConfigRamAddress, (byte*)config,2);
	
}



//���[�V�����ԍ������[�V�����̐擪�A�h���X�ɕϊ�
/**
* @brief ���[�V�����ԍ����烂�[�V�����̐擪�A�h���X�����߂܂�
* @return ���[�V�����ԍ��̐擪�̃A�h���X
* @details ���[�V�����ԍ����烂�[�V�����̐擪�A�h���X�����߂܂�
* @note		�A�h���X�Ɋւ��Ă�RCB-4�v���O���~���O�}�j���A�����������������B
* @attention	���[�V�����A�h���X�́AHTH4��V2.2�ȍ~�̃A�h���X�ɂȂ�܂��B
* @attention	V2.0�ȑO�̃A�h���X�ł͌듮������܂��̂Œ��ӂ��K�v�ł�
**/
long Rcb4BaseClass::motionAddr2motionNum(byte motionNum)
{
	//return (motionNum -1)* MotionSingleDataCount + MotionRomAddress;
	return (long)(motionNum -1)* (long)MotionSingleDataCount + MotionRomAddress;
}



















