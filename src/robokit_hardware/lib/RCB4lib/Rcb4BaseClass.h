
/** 
*  @file Rcb4BaseClass.h
* @brief Rcb4 arduino library header file
* @date 2018/01/29
* @version 1.1.0
* @copyright &copy; Kondo Kagaku Co.,Ltd. 2019
*
* @date 2018/01/29 First edition
* @date	2019/01/16�@V1.1.0
*			�EgetConfig�̕s����C��
*			�EmoveRamToComCmdSynchronize��memcopy�̕s��Ή�
*
* @mainpage Rcb4BaseClass�̊T�v
* ���̃��C�u�����͋ߓ��Ȋw�����{�b�g�p�R���g���[���{�[�h��Arduino����A�N�Z�X�ł���v���O�����ɂȂ�܂�<br>
* HeartToHeart4 Ver2.2�ȍ~�ɑΉ����Ă��܂��B<br>
* ���s�ł́Aarduino���i�Ŏg����悤�ɐ݌v���Ă��܂��B<br>
* �g��������яڍׂ́A���L����HP���������������B<br>
* <A HREF="http://kondo-robot.com/">http://kondo-robot.com/</A><br>
* �s�������܂�����A����HP���Q�Ƃɂ��A�����������B<br>
* ����BaseClass�́ARCB4�̊�{�ƂȂ镔�����L�ڂ��Ă��܂��B
* �ʐM�̈Ⴂ���z�����邽��HardwareSerial��SoftwareSerial�ɔh�����Ă��܂��B
* 
**/

/* 
* Rcb4BaseClass.h
*
* Created: 2017/06/13 4:34:57
* Author: 5636m_000,T.Nobuhara
*/


#ifndef __RCB4_BASE_CLASS_H__
#define __RCB4_BASE_CLASS_H__


typedef uint8_t byte;

//#include "Arduino.h"

/*! @class Rcb4BaseClass
    @brief  RCB4�𓮂������ߏ����ݒ��R�}���h���܂Ƃ߂��N���X�ł��B
    		���̃N���X���x�[�X�ɔh�������邱�Ƃŗl�X�ȒʐM�`���ɑΉ��ł���悤�ɂȂ�܂��B
*/
class Rcb4BaseClass
{
//variables
public:
	static constexpr int Version = 220;	//!< �o�[�W�����ԍ�

/**
 * @enum AckType
 * @brief ACK�̒l
  */	
	enum AckType: byte
	{
		Ack=0x06,		//!< ACK�̒l	(0x06)
		Nack =0x15		//!< NACK�̒l	(0x15)
	};

	//�R�}���h�̗񋓌^
/**
 * @enum CommandTypes
 * @brief RCB4�̃R�}���h�̒�`
 */
	enum CommandTypes: byte
	{
		Move            = 0x00,		//!<	Move���߂̃R�}���h�l
		Jump            = 0x0B,		//!<	Jump���߂̃R�}���h�l
		Call            = 0x0C,		//!<	Call���߂̃R�}���h�l
		SingleServo     = 0x0F,		//!<	�T�[�{���[�^�P�ɑ΂��Ė��߂̃R�}���h�l
		ConstFrameServo = 0x10,		//!<	�T�[�{���[�^�����ɑ΂��ẴR�}���h�l
		ServoParam      = 0x12,		//!<	�X�g���b�`��X�s�[�h��ύX���邽�߂̃R�}���h�l
		AckCheck        = 0xFE,		//!<	ACK�`�F�b�N�p�̃R�}���h�l
		None            = 0xFF		//!<	�����Ȃ�
	};
	
	//move�R�}���h�Ŏg���T�u�R�}���h)
/**
 * @enum SubMoveCmd
 * @brief move�R�}���h�̃T�u�R�}���h�̒�`
  */
	enum SubMoveCmd: byte
	{
		RamToCom     = 0x20,	//!< RAM�̒l��COM��
		ComToRam     = 0x02,	//!< COM����̐��l��RAM��
		DeviceToCom  = 0x21,	//!< Device(ICS)�̒l��COM��
		ComToDevice  = 0x12,	//!< COM����̐��l��Device(ICS)��
	};
	
	//RAM�ɕۑ�����Ă���擪�A�h���X�ꗗ
/**
 * @enum RamAddr
 * @brief RAM�ɕۑ�����Ă���A�h���X�̈ꗗ
  */
	enum RamAddr: unsigned short
	{
		ConfigRamAddress         = 0x0000,		//!< �V�X�e���ݒ�RAM�A�h���X(RAM)�i0x00�Œ�j
		ProgramCounterRamAddress = 0x0002,		//!< �v���O�����J�E���^�[�ݒ�A�h���X(RAM)�i0x02�Œ�j
		AdcRamAddress            = 0x0022,		//!< AD�ϊ���0�̒l�̃A�h���X(0x22�Œ�)
		PioModeAddres			 = 0x0038,		//!< PIO�̓��o�͐ݒ� 
		PioAddress               = 0x003A,		//!< PIO�|�[�g�l
		KrrButtonDataAddress     = 0x0350,		//!< KRR�̃{�^���f�[�^���L�^����Ă���A�h���X
		KrrPa1Address            = 0x0352,		//!< PA1�f�[�^(���̌�2byte���f�[�^������)
		//StartupCmdRomAddress     = 0x0444,	//!< �X�^�[�g�A�b�v���[�V�������Đ�����R�}���h�̃A�h���X
		//MainLoopCmd              = 0x044B		//!< ���C�����[�v�����ŏ��̃A�h���X

	};
	
	
	
	//ROM�ɕۑ�����Ă���擪�A�h���X�ꗗ)
/**
 * @enum RomAddr
 * @brief ROM�ɕۑ�����Ă���A�h���X�̈ꗗ
  */
	enum RomAddr: long
	{
		StartupCmdRomAddress = 0x0444,		//!< �X�^�[�g�A�b�v���[�V�������Đ�����R�}���h�̃A�h���X
		MainLoopCmd = 0x044B,				//!< ���C�����[�v�����ŏ��̃A�h���X
		MotionRomAddress = 0x0b80
	};
	
	static constexpr int AdcCount = 11;								//!< AD�ϊ���̃`�����l����)
	static constexpr int AdcSingleDataCount = 2;						//!< AD�ϊ���P�̂̃f�[�^�o�C�g��
	static constexpr int AdcDataCount = AdcCount * AdcSingleDataCount;	//!< AD�ϊ���S�̂̃f�[�^�o�C�g��

	//�f�o�C�X�A�h���X�̃I�t�Z�b�g�̒�`(��A�h���X����ǂꂭ�炢����Ă��邩)
/**
 * @enum DeviceAddrOffset
 * @brief (ICS)�f�o�C�X�̐ݒ�l���i�[����Ă���e�擪�̃A�h���X����̏���
 */
	enum DeviceAddrOffset:byte
	{
		CategoryAddressOffset      = 0x00,		//!< �V�X�e���ݒ�RAM�A�h���X(RAM)�i0x00�Œ�j
		IDAddressOffset            = 0x01,		//!< RAM�AROM���ID�f�[�^�̈ʒu
		TrimAddressOffset          = 0x02,		//!< RAM�AROM��̃g�����f�[�^�̈ʒu
		MotorPositionAddressOffset = 0x04,		//!< RAM�AROM��̎����l�f�[�^�̈ʒu
		PositionAddressOffset      = 0x06,		//!< RAM�AROM��̃|�W�V�����f�[�^�̈ʒu
		frameAddressOffset         = 0x08,		//!< RAM�AROM��̃t���[�����f�[�^�i��ԑ��x�E�X�s�[�h�j�̈ʒu
		Mixing1AddressOffset       = 0x0E,		//!< (14)RAM�AROM��̃~�L�V���O�P�f�[�^�̈ʒu
		Mixing1RatioAddressOffset  = 0x10,		//!< (16)RAM�AROM��̃~�L�V���O�P�{���f�[�^�̈ʒu
		Mixing2AddressOffset       = 0x11,		//!< (17)RAM�AROM��̃~�L�V���O2�f�[�^�̈ʒu
		Mixing2RatioAddressOffset  = 0x13		//!< (19)RAM�AROM��̃~�L�V���O2�f�[�^�̈ʒu
	};

	//SIO�ԍ��̒�`
/**
 * @enum SioNum
 * @brief Sio�R�l�N�^�̔ԍ��̒�`
 */
	enum SioNum:byte
	{
		SIO1_4 = 0x01,		//!< SIO1-4�������l
		SIO5_8 = 0x02		//!< SIO5-8�������l
	};

	//KRR�p�̃{�^���̒�`
/**
 * @enum KRR_BUTTON
 * @brief KRR����M����{�^���f�[�^�̒�`
 * @brief ���������̏ꍇ�͊e�f�[�^�̘_���a���Ƃ�܂�
 */
	enum KRR_BUTTON : unsigned short
	{
		KRR_BUTTON_NONE     = 0x0000,		//!<	����������Ă��Ȃ�
		KRR_BUTTON_UP       = 0x0001,		//!<	��
		KRR_BUTTON_DOWN     = 0x0002,		//!<	��
		KRR_BUTTON_RIGHT    = 0x0004,		//!<	��
		KRR_BUTTON_LEFT     = 0x0008,		//!<	��
		KRR_BUTTON_TRIANGLE = 0x0010,		//!<	��
		KRR_BUTTON_CROSS    = 0x0020,		//!<	�~
		KRR_BUTTON_CIRCLE   = 0x0040,		//!<	��
		KRR_BUTTON_SQUARE   = 0x0100,		//!<	��
		KRR_BUTTON_S1       = 0x0200,		//!<	�V�t�g1 ����O
		KRR_BUTTON_S2       = 0x0400,		//!<	�V�t�g2 ����
		KRR_BUTTON_S3       = 0x0800,		//!<	�V�t�g3 �E��O
		KRR_BUTTON_S4       = 0x1000,		//!<	�V�t�g4 �E��
		KRR_BUTTON_FALSE    = 0xFFFF		//!<	�G���[�l(��M���s��)
	};
	static constexpr int	MotionSingleDataCount = 2048;	//!< 1���[�V����������̃f�[�^�o�C�g��
	static constexpr int	MaxMotionCount = 120;			//!< ���[�V�����̍ő吔)
	static constexpr long	MotionDataCount = 2048 * 120;	//!< ���[�V�����f�[�^�S�̂̍ő�o�C�g��

/**
 * @struct ServoData
 * @brief �T�[�{���[�^��ID�����SIO�ƃ|�W�V�����̉�������f�[�^���i�[���܂��B
 * @brief �����T�[�{���[�^�𓮂�������p�����[�^��ς����肵�܂�
 */
	struct ServoData
	{
		byte	Id;		//!< ICS�@���ID�ԍ��̊i�[�ꏊ
		byte	Sio;	//!< sio�̔ԍ�(1 or 2)
		int		Data;	//!< �|�W�V�����f�[�^�₻�̑��f�[�^������
	};
	
	

public:

	
/**
 * @union ConfigData
 * @brief 2byte�̃R���t�B�O�f�[�^��e�ՂɃA�N�Z�X�ł���悤���p�̂ɂ��܂�
 */
typedef union {
			/// �r�b�g�t�B�[���\����
			/* @struct bit
				@brief �e���ڂ��w��ł��܂�
			*/
			struct {
				byte EnableSio :1;				//!< b0:ICS�X�C�b�`
				byte EnableRunEeprom :1;		//!< b1: EEPROM�v���O�������s�X�C�b�`
				byte EnableServoResponse :1;	//!< b2: ��ԓ���I�����b�Z�[�W�X�C�b�`
				byte EnableReferenceTable :1;	//!< b3:�x�N�^�W�����v�X�C�b�`
				byte Frame :2;					//!< b4, b5 �o�͎������W�X�^[4:5]
				byte Baudrate :2;				//!< b6, b7 COM�{�[���[�g���W�X�^[6:7]�������Ƃ��Ԃ�Ȃ����m�F
				byte ZeroFlag :1;				//!< b8 �[���t���O
				byte CarrayFlag :1;				//!< b9 �L�����[�t���O
				byte ProgramError :1;			//!< b10 �v���O�����G���[�t���O
				byte RFU :1;					//!< b11,b12 ���g�p
				byte IcsBaudrate :2;			//!< b13,b14 ICS�X�C�b�`�{�[���[�g���W�X�^[13:14] �������Ƃ��Ԃ�Ȃ����m�F
				byte GreenLED :1;				//!< b15 LED���W�X�^
			} bit;
			/// WORD(2�o�C�g)�f�[�^��1�o�C�g���Ƃɕ������Ďg�p����
			/* @struct byte
				@brief byte�P�ʂŃA�N�Z�X�ł��܂�
			*/
			struct {
				byte low;						//!< low 8 bit
				byte high;						//!< high 8 bit
			} byte;
			unsigned short word;				//!< 16�r�b�gWORD�f�[�^(ConfigData����x�ɃA�N�Z�X�ł���悤��)
		} ConfigData;
		
ConfigData configData;	//!< �R���t�B�O�f�[�^�̊i�[�ꏊ 

protected:


/**
 * @union WordByte
 * @brief 2byte(short�^)�̃f�[�^���A�N�Z�X�ł���悤���p�̂ɂ��܂�
 */
	typedef	union {
		uint8_t		byte[2];	//!< byte x2
		uint16_t	word;		//!< WordByte(short)
	} WordByte;

/**
 * @union DwordByte
 * @brief 4byte(long�^)�̃f�[�^���A�N�Z�X�ł���悤���p�̂ɂ��܂�
 */
	typedef	union {
		uint8_t		byte[4];	//!< byte x4
		uint16_t	word[2];	//!< WordByte(short) x2
		uint32_t	dword;		//!< DoubleWord(long)
	} DwordByte;
/**
 * @union QwordByte
 * @brief 8byte(long long�^)�̃f�[�^���A�N�Z�X�ł���悤���p�̂ɂ��܂�
 */
	typedef	union {
		uint8_t		byte[8];	//!< byte x8
		uint16_t	word[4];	//!< WordByte(short) x4
		uint32_t	dword[2];	//!< DoubleWord(long) x2
		uint64_t	qword;		//!< QuadWord(long long)
	} QwordByte;


protected:

	byte txBuf[128];	//!< ���M�p�o�b�t�@	
	byte rxBuf[128];	//!< ��M�p�o�b�t�@


	
	//�ʐM�p�t���O ���ݑ���M��(synchronize)�����f���܂� 
	//true:���s��   false:�ҋ@��
	bool _isSynchronize = false;	//!< ���ݑ���M��(synchronize)�����f���܂� true:���s��   false:�ҋ@��
	
	

private:

//functions
public:

/**
*	@brief �ʐM�����ǂ����Ԃ��֐�
*	@retval true ���M��
*	@retval false �ҋ@��
*/
	bool isSynchronize(){return _isSynchronize;}			// (get�̂�, set�̓N���X���ł̂݉\)

	bool getConfig(unsigned short *confData);
	bool getConfig();

	//command�����֐�

	int moveComToRamCmd(byte *txData,int destAddr,byte destData[],int destDataSize);
	int moveComToRamCmd(int destAddr,byte destData[],int destDataSize);
	
	int moveRamToComCmd(byte *txData,int scrAddr,int scrDataSize);
	int moveRamToComCmd(int scrAddr,int scrDataSize);

	int moveComToDeviceCmd (byte *txData,byte idNum,byte sioNum,byte offset,int data);
	int moveComToDeviceCmd (byte idNum,byte sioNum,byte offset,int data);
	int moveComToDeviceCmd (byte *txData,byte icsNum,byte offset,int data);
	int moveComToDeviceCmd (byte icsNum,byte offset,int data);

	int moveDeviceToComCmd(byte *txData,byte idNum,byte sioNum,byte offset,byte dataSize);
	int moveDeviceToComCmd(byte idNum,byte sioNum, byte offset, byte dataSize);
	int moveDeviceToComCmd(byte *txData,byte icsNum,byte offset,byte dataSize);
	int moveDeviceToComCmd(byte icsNum, byte offset, byte dataSize);
	
	int callCmd(byte *txData,long romAddr);
	int callCmd(long romAddr);

	int runSingleServoCmd (byte *txData,ServoData servoData,byte frame);
	int runSingleServoCmd (ServoData servoData,byte frame);

	int runSingleServoCmd (byte *txData,byte idNum,byte sioNum,int pos,byte frame);
	int runSingleServoCmd (byte idNum,byte sioNum,int pos,byte frame);
	
	
	
	int runConstFrameServoCmd (byte *txData,ServoData servoDatas[],byte servoCount,byte frame);
	int runConstFrameServoCmd (ServoData servoData[],byte servoCount,byte frame);

	int setParametersBaseCmd (byte *txData, byte servoParameter,ServoData servoDatas[],byte servoCount);
	int setParametersBaseCmd (byte servoParameter,ServoData servoData[],byte servoCount);

	int setSpeedCmd (byte *txData,ServoData servoDatas[],byte servoCount);
	int setSpeedCmd (ServoData servoData[],byte servoCount);

	int setStretchCmd (byte *txData,ServoData servoDatas[],byte servoCount);
	int setStretchCmd (ServoData servoData[],byte servoCount);

	int acknowledgeCmd (byte *txData);
	int acknowledgeCmd ();


//�f�[�^����M (���������\�[�X�ɖ����̂ŋL��)///////////////////////////////////////////////////////////////////
/**
* @brief ICS�ʐM�̑���M
* @param[in,out] *txBuf
* @param[in] txLen
* @param[out] *rxBuf ��M�i�[�o�b�t�@
* @param[in] rxLen  ��M�f�[�^��
* @retval true �ʐM����
* @retval false �ʐM���s
* @attention �C���^�[�t�F�C�X�A�q�N���X�Œ�`���邱��
* @attention ���M�f�[�^���A��M�f�[�^���̓R�}���h�ɂ���ĈႤ�̂Œ��ӂ���
* 
**/
	virtual bool synchronize(byte txBuf[],byte txLen,byte rxBuf[],byte rxLen);	//�C���^�[�t�F�C�X�A�q�N���X�Œ�`���邱��
	
	bool checkAcknowledge();
	bool setCheckSum(byte dataBytes[]);
	bool checkCheckSum(byte dataBytes[]);
	bool checkReverceCmd(byte dataBytes[],byte cmd);


	unsigned short getPio();
	bool setPio (unsigned short pioData);
	unsigned short getPioMode ();
	bool setPioMode (unsigned short pioModeData);

	unsigned short getAdData(int adPort);
	bool getAllAdData(int	adDatas[11]);

	bool setKrrData (unsigned short buttonData, byte adData[4]);
	bool setKrrButtonData (unsigned short buttonData);
	bool setKrrAdData (byte paPort, byte adData);



	bool setSingleServo (byte id, byte sio,int pos,byte frame);
	bool setSingleServo (ServoData servoData,byte frame);
	bool setFreeSingleServo (byte id, byte sio);
	bool setHoldSingleServo (byte id, byte sio);
	
	bool setServoPos (ServoData servoDatas[],byte servoCount,byte frame);
	bool setFreePos (ServoData servoDatas[],byte servoCount);
	bool setHoldPos (ServoData servoDatas[],byte servoCount);
	
	int getSinglePos(byte id,byte sio);
	
	bool setServoSpeed (ServoData servoDatas[],byte servoCount);
	bool setServoStretch (ServoData servoDatas[],byte servoCount);
	
	byte icsNum2id(byte id,byte sio);
	
	
	
	
	bool motionPlay(int motionNum);
	bool suspend();
	bool suspend(unsigned short *config);
	bool setMotionNum(byte motionNum);
	bool resetProgramCounter();

	bool resume (unsigned short *configData);
	bool resume ();
	
	long motionAddr2motionNum(byte motionNum);

protected:

	//Rcb4BaseClass( const Rcb4BaseClass &c );
	//Rcb4BaseClass& operator=( const Rcb4BaseClass &c );
	byte	CheckSum(byte dataBytes[]);
	bool	checkuning(byte *txData);
	int64_t	setServoNo (ServoData servoDatas[],byte servoCount);
	bool sortServoDatas(ServoData servoDatas[],byte ServoCount);
	
	unsigned short moveRamToComCmdSynchronize(int scrAddr,byte scrDataSize,byte	rxDara[]);
	bool	moveComToRamCmdSynchronize(int scrAddr,byte destData[],int destDataSize);
	bool	SynchronizeAck(byte	*txData);
	

	
private:

}; //Rcb4BaseClass

#endif //__RCB4_BASE_CLASS_H__