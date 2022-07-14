/**
*	@file Rcb4BaseClass.cpp
*	@brief RCB4 base library
*	@author Kondo Kagaku Co.,Ltd.
*	@version 1.1.0
*
*	@date	2018/02/01
*	@date	2019/01/16　V1.1.0
*			・getConfigの不具合を修正
*			・moveRamToComCmdSynchronizeのmemcopyの不具合対応
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



//RCB4のconfigデータを取得する //////////////////////////////////////////////////////////
/**
* @brief RCB4のコンフィグデータを読み取る
* @return 成功かどうか判定のフラグ
* @retval true	通信成功、データ取得
* @retval false	通信失敗
* @attention class内のconfigDataに値が代入されます
**/
///////////////////////////////////////////////////////////////////////////////////
// (configデータを取得する)
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

//RCB4のconfigデータを取得する //////////////////////////////////////////////////////////
/**
* @brief RCB4のコンフィグデータを読み取る
* @param[out] confData	RCB4のコンフィグデータ
* @return 成功かどうか判定のフラグ
* @retval true	通信成功、データ取得
* @retval false	通信失敗
* @attention 返す値はclass内のconfigDataの値がコピーされます。(configDataの中にいったん入ります)
* @date 2019/01/15　confDataのアドレスにデータを入れていたので修正
**/
bool Rcb4BaseClass::getConfig(unsigned short *confData)
{
	if(getConfig() == false)	return	false;

	*confData = configData.word; 
	delay(100);
	return	true;
}


//Rcb4BaseClassで使用している送信バッファに何か入れようとしていないかチェックする///////////
/**
* @brief データ通信中で、送信バッファに値を入れようとしているかチェックする
* @param[in] *txData	現在入れようと思っている送信バッファ
* @return 成功かどうか判定のフラグ
* @retval true	現在入れようとしているバッファは何も使っていない
* @retval false	現在入れようとしているバッファは使っている最中である
* @attention Rcb4BaseClassで使っている送信バッファは共通で使っているので、送信中に変えないか確認を行っている
**/
bool Rcb4BaseClass::checkuning(byte *txData)		///データ通信中で、送信バッファに値を入れようとしているかチェック
{
	return (_isSynchronize == true && txBuf == txData) ? false: true;
}


///////////////////////////////////////////////////////////////////////////////////
//comand作成 (データを与えると送信コマンドを作成する
//
// 
//COMからRAMにデータを転送するコマンドを生成する///////////
/**
* @brief COMからRAMにデータを転送するコマンドを生成する
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] destAddr	RCB4に書き込む先頭アドレス
* @param[in] destData[]	書き込みデータ
* @param[in] destDataSize	書込みデータ数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @details COMからRCB4内部RAMのデータを書き換えるコマンドを生成します
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

	*ptr++ = (byte)destDataSize + 7;		//コマンドのデータ数
	*ptr++=  Move;							//moveコマンド命令
	*ptr++=  0x02;							//COM => RAM
	*ptr++=  wk.byte[0];					//書き込む先頭アドレス
	*ptr++=  wk.byte[1];					//
	*ptr++=  0x00;							//仕様
	for(int i = 0; i < destDataSize; i++)	//データを入れる
	{
		*ptr++=  destData[i];
	}
	setCheckSum(txData);					//チェックサム
	
	return destDataSize + 7;
	
}


/**
* @brief COMからRAMにデータを転送するコマンドを生成する
* @param[in] destAddr	RCB4に書き込む先頭アドレス
* @param[in] destData[]	書き込みデータ
* @param[in] destDataSize	書込みデータ数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details COMからRCB4内部RAMのデータを書き換えるコマンドを生成します
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/
int Rcb4BaseClass::moveComToRamCmd(int destAddr,byte destData[],int destDataSize)
{
	return moveComToRamCmd(txBuf, destAddr, destData, destDataSize);
}


//RAMの状態をCOMに転送するコマンド
/**
* @brief RAMのデータをCOMに送るコマンドを生成する
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] scrAddr	RCB4に書き込む先頭アドレス
* @param[in] scrDataSize	書込みデータ数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details RCB4の内のRAMのデータをCOMに送るコマンドを生成します
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
**/
int Rcb4BaseClass::moveRamToComCmd(byte *txData,int scrAddr,int scrDataSize)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;
	WordByte	wk;

	wk.word = scrAddr;

	ptr = txData;

	*ptr++ = 0x0a;		//コマンドのデータ数(ここのコマンドは固定)
	*ptr++=  Move;		//moveコマンド命令
	*ptr++=  0x20;		//RAM => COM
	*ptr++=  0x00;		//仕様(COM出力)
	*ptr++=  0x00;		//仕様
	*ptr++=  0x00;		//仕様
	*ptr++=  wk.byte[0];		//転送元のアドレス
	*ptr++=  wk.byte[1];		//
	*ptr++=  (byte)scrDataSize;	//送ってもらいたいbyte数

	setCheckSum(txData);		//チェックサム
	return 0x0a;

}

//RAMの状態をCOMに転送するコマンド
/**
* @brief RAMのデータをCOMに送るコマンドを生成する
* @param[in] scrAddr	RCB4に書き込む先頭アドレス
* @param[in] scrDataSize	書込みデータ数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details RCB4の内のRAMのデータをCOMに送るコマンドを生成します
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/
int Rcb4BaseClass::moveRamToComCmd(int scrAddr,int scrDataSize)
{
	return moveRamToComCmd(txBuf, scrAddr, scrDataSize);
}



//COMからのデータをICSに展開するコマンド
/**
* @brief COMからのデータをICSに展開するコマンドを生成する(short 2byteバージョン)
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] icsNum		ICS番号
* @param[in] offset		何バイト離れたところをアクセスするか
* @param[in] data		2byte分のデータ
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details COMからRCB4の内のICSエリアに割り当てられたデータに反映させます
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
**/
int Rcb4BaseClass::moveComToDeviceCmd (byte *txData,byte icsNum,byte offset,int data)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;
	WordByte	wk;

	ptr = txData;
	wk.word = data;
				
	*ptr++ = 0x09;		//コマンドのデータ数(ここのコマンドは固定)
	*ptr++=  Move;		//moveコマンド命令
	*ptr++=  0x12;		//com => Device
	*ptr++=  offset;	//デバイスの先頭からどれだけ離れているか
	*ptr++=  icsNum;	//ICS番号
	*ptr++=  0x00;		//仕様
	*ptr++=  wk.byte[0];	//データ
	*ptr++=  wk.byte[1];	//
	setCheckSum(txData);	//チェックサム
	return 0x09;

}



/**
* @brief COMからのデータをICSに展開するコマンドを生成する(short 2byteバージョン)
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] idNum		サーボのID番号
* @param[in] sioNum		SIO(SioNum)の番号
* @param[in] offset		何バイト離れたところをアクセスするか
* @param[in] data		2byte分のデータ
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details COMからRCB4の内のICSエリアに割り当てられたデータに反映させます
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
**/
int Rcb4BaseClass::moveComToDeviceCmd (byte *txData,byte idNum,byte sioNum,byte offset,int data)
{
	return moveComToDeviceCmd (txData, icsNum2id(idNum, sioNum), offset, data);
}



/**
* @brief COMからのデータをICSに展開するコマンドを生成する(short 2byteバージョン)
* @param[in] icsNum		ICS番号
* @param[in] offset		何バイト離れたところをアクセスするか
* @param[in] data		2byte分のデータ
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details COMからRCB4の内のICSエリアに割り当てられたデータに反映させます
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/
int Rcb4BaseClass::moveComToDeviceCmd (byte icsNum,byte offset,int data)
{
	return moveComToDeviceCmd (txBuf, icsNum, offset, data);
}



/**
* @brief COMからのデータをICSに展開するコマンドを生成する(short 2byteバージョン)
* @param[in] idNum		サーボのID番号
* @param[in] sioNum		SIO(SioNum)の番号
* @param[in] offset		何バイト離れたところをアクセスするか
* @param[in] data		2byte分のデータ
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details COMからRCB4の内のICSエリアに割り当てられたデータに反映させます
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/
int Rcb4BaseClass::moveComToDeviceCmd (byte idNum,byte sioNum,byte offset,int data)
{
	return moveComToDeviceCmd (txBuf, icsNum2id(idNum, sioNum), offset, data);
}




//ICSのデータをCOMに転送するコマンド)
/**
* @brief ICSのデータをCOMに転送するコマンドを生成する
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] icsNum		ICS番号
* @param[in] offset		何バイト離れたところをアクセスするか
* @param[in] dataSize	取得するデータ数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details RCB4のICSエリアに割り当てられたデータをCOMに転送するコマンドを生成します
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
**/
int Rcb4BaseClass::moveDeviceToComCmd(byte *txData,byte icsNum,byte offset,byte dataSize)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;

	ptr = txData;

	*ptr++ = 0x0a;			//コマンドのデータ数(ここのコマンドは固定)
	*ptr++ = Move;			//moveコマンド命令
	*ptr++ = 0x21;			//Device => com 
	*ptr++ = offset;		//デバイスの先頭からどれだけ離れているか
	*ptr++ = 0x00;			//仕様(COM出力)
	*ptr++ = 0x00;			//仕様
	*ptr++ = 0x00;			//仕様
	*ptr++ = icsNum;		//ICS番号
	*ptr++ = dataSize;		//何バイトもらうか
	setCheckSum(txData);	//チェックサム	
	return 0x0a;

}



/**
* @brief ICSのデータをCOMに転送するコマンドを生成する
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] idNum		サーボのID番号
* @param[in] sioNum		SIO(SioNum)の番号
* @param[in] offset		何バイト離れたところをアクセスするか
* @param[in] dataSize	取得するデータ数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details RCB4のICSエリアに割り当てられたデータをCOMに転送するコマンドを生成します
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
**/
int Rcb4BaseClass::moveDeviceToComCmd(byte *txData,byte idNum,byte sioNum,byte offset,byte dataSize)
{
	return  moveDeviceToComCmd(txData, icsNum2id(idNum, sioNum), offset, dataSize);
}



/**
* @brief ICSのデータをCOMに転送するコマンドを生成する
* @param[in] icsNum		ICS番号
* @param[in] offset		何バイト離れたところをアクセスするか
* @param[in] dataSize	取得するデータ数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details RCB4のICSエリアに割り当てられたデータをCOMに転送するコマンドを生成します
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/
int Rcb4BaseClass::moveDeviceToComCmd(byte icsNum, byte offset, byte dataSize)
{
	return  moveDeviceToComCmd(txBuf, icsNum, offset, dataSize);
}

/**
* @brief ICSのデータをCOMに転送するコマンドを生成する
* @param[in] idNum		サーボのID番号
* @param[in] sioNum		SIO(SioNum)の番号
* @param[in] offset		何バイト離れたところをアクセスするか
* @param[in] dataSize	取得するデータ数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details RCB4のICSエリアに割り当てられたデータをCOMに転送するコマンドを生成します
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/
int Rcb4BaseClass::moveDeviceToComCmd(byte idNum,byte sioNum, byte offset, byte dataSize)
{
	return  moveDeviceToComCmd(txBuf, icsNum2id(idNum, sioNum), offset, dataSize);
}




//指定したアドレスにジャンプする
/**
* @brief 指定したアドレスにジャンプ(CALL)するコマンドを生成します
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] romAddr		ジャンプするアドレス
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @details ROMの指定アドレスにジャンプする命令を生成します。
* @details Call命令なので、終わったら元の場所に戻ります。
**/
int Rcb4BaseClass::callCmd(byte *txData,long romAddr)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;
	DwordByte	wk;
	
	wk.dword = (uint32_t)romAddr;
	ptr = txData;

	*ptr++ = 0x07;				//コマンドのデータ数(ここは固定)
	*ptr++ = Call;				//Callコマンド
	*ptr++ = wk.byte[0];		//アドレス
	*ptr++ = wk.byte[1];
	*ptr++ = wk.byte[2];
	*ptr++ = 0x00;				//判定項目　=>無条件で飛ぶ
	setCheckSum(txData);		//チェックサムを入れる
	return 0x07;

}



/**
* @brief 指定したアドレスにジャンプ(CALL)するコマンドを生成します
* @param[in] romAddr		ジャンプするアドレス
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details ROMの指定アドレスにジャンプする命令を生成します。
* @details Call命令なので、終わったら元の場所に戻ります。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/
int Rcb4BaseClass::callCmd(long romAddr)
{
	return callCmd(txBuf, romAddr);
}





//サーボを1つだけ動かす)
/**
* @brief 指定したサーボモータを1個だけ動かすコマンドを作成します
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] servoData	動かそうとしているサーボを1まとまりにした型
* @param[in] frame	フレーム数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 1つだけサーボを動かします。
* @details ServoData型には、サーボモータのID以外にもSIOと動かすポジションのデータが入ります
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
**/	
int Rcb4BaseClass::runSingleServoCmd (byte *txData,ServoData servoData,byte frame)
{
	return runSingleServoCmd (txData, servoData.Id, servoData.Sio, servoData.Data, frame);
}


/**
* @brief 指定したサーボモータを1個だけ動かすコマンドを作成します
* @param[in] servoData	動かそうとしているサーボを1まとまりにした型
* @param[in] frame	フレーム数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 1つだけサーボを動かします。
* @details ServoData型には、サーボモータのID以外にもSIOと動かすポジションのデータが入ります
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/	
int Rcb4BaseClass::runSingleServoCmd (ServoData servoData,byte frame)
{
	return runSingleServoCmd (txBuf, servoData.Id, servoData.Sio, servoData.Data, frame);
}



/**
* @brief 指定したサーボモータを1個だけ動かすコマンドを作成します
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] idNum	サーボモータのID番号
* @param[in] sioNum	接続しているSIO(SioNum)の番号
* @param[in] pos	サーボモータのポジションデータ(目標値)
* @param[in] frame	フレーム数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 1つだけサーボを動かします。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
**/	
int Rcb4BaseClass::runSingleServoCmd (byte *txData,byte idNum,byte sioNum,int pos,byte frame)
{
	if(checkuning(txData) == false)	return -1;

	byte	*ptr;
	WordByte wk;

	ptr = txData;
	wk.word = pos;
	
	*ptr++ = 0x07;						//コマンドのデータ数(ここは固定)
	*ptr++ = SingleServo;				//1個サーボを動かすためのコマンド
	*ptr++ = icsNum2id(idNum, sioNum);	//ICS番号
	*ptr++ = frame;						//フレーム数
	*ptr++=  wk.byte[0];				//ポジションデータ
	*ptr++=  wk.byte[1];
	setCheckSum(txData);				//チェックサムを入れる
	return 0x07;
}

/**
* @brief 指定したサーボモータを1個だけ動かすコマンドを作成します
* @param[in] idNum	サーボモータのID番号
* @param[in] sioNum	接続しているSIO(SioNum)の番号
* @param[in] pos	サーボモータのポジションデータ(目標値)
* @param[in] frame	フレーム数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 1つだけサーボを動かします。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/	
int Rcb4BaseClass::runSingleServoCmd (byte idNum,byte sioNum,int pos,byte frame)
{
	return runSingleServoCmd (txBuf, idNum, sioNum, pos, frame);
}


/**
* @brief 複数サーボモータにアクセスするときにコマンドで「動作させるサーボの指定」部分を作成
* @param[in] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @return 「動作させるサーボの指定」のデータ
* @details 「動作させるサーボの指定」には1bitずつ順番に動かす部分(フラグ)を立てていきます
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
///	サーボのデータをソートするのとしていサーボデータの返却
///
/**
* @brief 複数サーボモータにアクセスするときにサーボの順番の指定があるのでソートする
* @param[in,out] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @return 「動作させるサーボの指定」のデータ
* @retval true ソート成功
* @retval false	データ数が多すぎたり不正なICS番号が割り当てられようとしたとき
* @details サーボモータを複数動かす時、順番に並んでいる必要があるのでそのデータをソートします
* @attention servoDatasそのものをソートするので、中身が書き換わります。
* @attention データを保持しておきたい場合は、コピーさせておいてから使用してください
**/	
bool Rcb4BaseClass::sortServoDatas(ServoData servoDatas[],byte servoCount)
{
	const int servoMaxCount = 36;
	
	if(servoCount > servoMaxCount)	//サーボの個数以上あるようなら抜ける
	{
		return false;
	}
	
	ServoData bufServoDatas[servoMaxCount];//一時的に保存するバッファ
	
	int i;		//ループ用
	
	for(i = 0;i<servoMaxCount;i++)		//とりあえず0xFFで初期化
	{
		bufServoDatas[i].Id = 0xFF;
		bufServoDatas[i].Sio = 0xFF;	
	}
	
	for(i = 0;i<servoCount;i++)//サーボの個数分くりかえし、該当する配列にデータを入れていく
	{
		byte sioNum =  icsNum2id(servoDatas[i].Id,servoDatas[i].Sio);	//ICS番号を計算する
		
		if(sioNum > servoMaxCount)		//ICS番号が変な数になった場合は抜ける
		{
			return false;
		}
		
		//当てはまるところにデータを入れる
		bufServoDatas[sioNum].Id = servoDatas[i].Id;
		bufServoDatas[sioNum].Sio = servoDatas[i].Sio;
		bufServoDatas[sioNum].Data = servoDatas[i].Data;
		
	}
	
	byte j = 0;
	
	for(i = 0;i<servoMaxCount;i++)		//当てはまる順番から代入
	{
		if(bufServoDatas[i].Id != 0xFF)		//順番に回していってデータがあった場合
		{
			//値を代入
			servoDatas[j].Id = bufServoDatas[i].Id;
			servoDatas[j].Sio = bufServoDatas[i].Sio;
			servoDatas[j].Data = bufServoDatas[i].Data;
			
			j++;	//次に進める
			
			if(j >= servoCount)	//サーボがデータ数終わったら抜ける
			{
				break;
			}
		}	
	}
	
	return true;
	
}


//複数のサーボを動かす
/**
* @brief 指定したサーボモータを複数個動かすコマンドの生成
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @param[in] frame	フレーム数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 複数個サーボモータを動かすコマンドを生成します。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention ServoDataのdataにはポジションデータを入れます
**/	
int Rcb4BaseClass::runConstFrameServoCmd (byte *txData,ServoData servoDatas[],byte servoCount,byte frame)
{

	if(checkuning(txData) == false)	return -1;	

	//引数はデータを変更してしまうので、コピーしておく
	ServoData sDataBuf[servoCount];
	memcpy(sDataBuf,servoDatas , sizeof(ServoData)*servoCount);
	
	byte	*ptr;
	QwordByte wk;

	ptr = txData;

	wk.qword = setServoNo (sDataBuf, servoCount);		//「動作させるサーボの指定」を計算させておく
	
	*ptr++ = servoCount * 2 + 9;						//何気に一番最初が送信データ数
	*ptr++ = ConstFrameServo;							//複数サーボを動かすコマンド
	*ptr++ = wk.byte[0];								//「動作させるサーボの指定」を代入
	*ptr++ = wk.byte[1];
	*ptr++ = wk.byte[2];
	*ptr++ = wk.byte[3];
	*ptr++ = wk.byte[4];
	*ptr++ = frame;										//フレーム数
	
	if(sortServoDatas(sDataBuf, servoCount) == false)		//サーボのデータを順番に並べ替える
	{
		return -1;
	}


	
	for(int i = 0; i < servoCount ; i++)	//サーボのデータを順番に入れていく
	{
		wk.word[0] = sDataBuf[i].Data;
		*ptr++=  wk.byte[0];
		*ptr++=  wk.byte[1];
	}
	
	setCheckSum(txData);		//最後はチェックサムを入れる
	
	return servoCount * 2 + 9;
	
}


/**
* @brief 指定したサーボモータを複数個動かすコマンドの生成
* @param[in] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @param[in] frame	フレーム数
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 複数個サーボモータを動かすコマンドを生成します。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention ServoDataのdataにはポジションデータを入れます
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/	
int Rcb4BaseClass::runConstFrameServoCmd (ServoData servoDatas[],byte servoCount,byte frame)
{
	return runConstFrameServoCmd (txBuf, servoDatas, servoCount, frame);
}




//一度に複数サーボのパラメータを変更する)
/**
* @brief 指定したサーボモータを複数個パラメータを変えるコマンドの生成
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] servoParameter (Stretch:1 Speed:2)
* @param[in] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 複数個サーボモータを動かすコマンドを生成します。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention ServoDataのdataにはストレッチやスピードデータを入れます(Max 127)
**/	
int Rcb4BaseClass::setParametersBaseCmd (byte *txData, byte servoParameter,ServoData servoDatas[],byte servoCount)
{
	if(checkuning(txData) == false)	return -1;

	
	//引数はデータを変更してしまうので、コピーしておく
	ServoData sDataBuf[servoCount];
	memcpy(sDataBuf,servoDatas , sizeof(ServoData)*servoCount);


	byte	*ptr;
	QwordByte wk;

	ptr = txData;

	if(sortServoDatas(sDataBuf, servoCount) == false)		//コピーしたデータはソートしておく
	{
		return -1;
	}

	wk.qword = setServoNo (sDataBuf, servoCount);		//「動作させるサーボの指定」を計算させておく

	*ptr++ = servoCount + 9;		//コマンドのデータ数
	*ptr++ = ServoParam;			//パラメータ変更コマンド
	*ptr++ = wk.byte[0];			//「動作させるサーボの指定」
	*ptr++ = wk.byte[1];			
	*ptr++ = wk.byte[2];
	*ptr++ = wk.byte[3];
	*ptr++ = wk.byte[4];
	*ptr++ = servoParameter;		//(Stretch:1 Speed:2)
	for(int i = 0; i < servoCount ; i++)				//ソートしたサーボのデータを代入
	{
		*ptr++=  (byte)( sDataBuf[i].Data & 0x00ff);
	}
	setCheckSum(txData);
	return servoCount + 9;
}



//一度に複数サーボのパラメータを変更する)
/**
* @brief 指定したサーボモータを複数個パラメータを変えるコマンドの生成
* @param[in] servoParameter (Stretch:1 Speed:2)
* @param[in] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 複数個サーボモータを動かすコマンドを生成します。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention ServoDataのdataにはストレッチやスピードデータを入れます(Max 127)
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/	
int Rcb4BaseClass::setParametersBaseCmd (byte servoParameter,ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txBuf, servoParameter, servoDatas, servoCount);
}



//一度にサーボモータのスピードを指定する
/**
* @brief 指定したサーボモータを複数個スピードを変えるコマンドの生成
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 複数個サーボモータを動かすコマンドを生成します。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention ServoDataのdataにはスピードデータを入れます(Max 127)
**/	
int Rcb4BaseClass::setSpeedCmd (byte *txData,ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txData, 0x02, servoDatas, servoCount);
}



/**
* @brief 指定したサーボモータを複数個スピードを変えるコマンドの生成

* @param[in] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 複数個サーボモータを動かすコマンドを生成します。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention ServoDataのdataにはスピードデータを入れます(Max 127)
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/	
int Rcb4BaseClass::setSpeedCmd (ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txBuf, 0x02, servoDatas, servoCount);
}



//(一度にサーボモータのストレッチを指定する)
/**
* @brief 指定したサーボモータを複数個ストレッチを変えるコマンドの生成
* @param[out] *txData	現在入れようと思っている送信バッファ
* @param[in] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 複数個サーボモータを動かすコマンドを生成します。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention ServoDataのdataにはストレッチデータを入れます(Max 127)
**/	
int Rcb4BaseClass::setStretchCmd (byte *txData,ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txData, 0x01, servoDatas, servoCount);
}



/**
* @brief 指定したサーボモータを複数個ストレッチを変えるコマンドの生成
* @param[in] servoDatas[]	アクセスするサーボのデータの配列
* @param[in] servoCount	アクセスするサーボのデータの配列数(サーボの数)
* @return 送信データ数
* @retval -1	バッファに何か使われているかサイズが249byte以上
* @details 複数個サーボモータを動かすコマンドを生成します。
* @details 返値はtxDataのデータbyte数を返しますので、送信するのに使用します
* @attention ServoDataのdataにはストレッチデータを入れます(Max 127)
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/	
int Rcb4BaseClass::setStretchCmd (ServoData servoDatas[],byte servoCount)
{
	return	setParametersBaseCmd (txBuf, 0x01, servoDatas, servoCount);
}

//ackを返すコマンドの作成
/**
* @brief ackコマンドの生成
* @param[out] *txData	現在入れようと思っている送信バッファ
* @return 送信データ数
* @details ACKのコマンドを生成する
**/	
int Rcb4BaseClass::acknowledgeCmd (byte *txData)
{
	byte	*ptr;
	ptr = txData;

	*ptr++ = 0x04;				//コマンドのデータ数(このコマンドは固定)
	*ptr++ = AckCheck;			//ACKコマンド
	*ptr++ = Ack;				//ACK
	setCheckSum(txData);		//チェックサム
	return	0x04;
}

/**
* @brief ackコマンドの生成
* @return 送信データ数
* @details ACKのコマンドを生成する
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/	
int Rcb4BaseClass::acknowledgeCmd ()
{
	return acknowledgeCmd(txBuf);
}

//////////////////////////////////////////////////////////////////////////////////////////
//通信関係（ここはインターフェイスクラス）

//実際に送受信を行う
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
//		{//ﾀｲﾑｱｳﾄ
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



//ackを送ってつながっているか確認する
/**
* @brief ackコマンドを送信
* @return ACKコマンド後合否判定
* @retval False コマンドが返ってこなかった時およびNCKが返ってきたとき
* @details ACKのコマンドを送信する
* @attention コマンドデータはRcb4BaseClassのtxBuf内に入ります
**/	
bool Rcb4BaseClass::checkAcknowledge()
{
	bool checkFlag = true; 
		
	acknowledgeCmd (txBuf);		//ACKコマンドの生成
	
	checkFlag = synchronize(txBuf, txBuf[0], rxBuf, 4);		//コマンドの送信
	
	if( false == checkFlag || rxBuf[2] == Nack)				//合否判定
	{
		return false;
	}
	else					//問題ない場合はtrueを返す
	{
		return true;
	}
}



//////////////////////////////////////////////////////////////////////////
//チェックサムを計算の計算
/**
* @brief コマンドのチェックサムを計算します
* @param[in] dataBytes[]	コマンドが入ったデータ
* @return コマンドのチェックサム
* @details コマンドのチェックサムを計算します
* @attention コマンドの先頭にはコマンドのデータ数が入っています
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



//コマンドのチェックサムを計算し代入する)
/**
* @brief コマンドのチェックサムを計算し、最後尾にいれます
* @param[in,out] dataBytes[]	コマンドが入ったデータ
* @return 入れれたかどうかの判定
* @retval False コマンドの先頭が0だった時
* @details コマンドのチェックサムを計算し、最後尾に代入します
* @attention コマンドの先頭にはコマンドのデータ数が入っています
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



//チェックサムがあっているか確認する
/**
* @brief 送られてきたコマンドのチェックサムがあっているか確認します
* @param[in] dataBytes[]	コマンドが入ったデータ
* @return チェックサムの判定
* @retval False チェックサムが間違っていたとき、データ数が0だった時
* @details 送られてきたコマンドのチェックサムがあっているか確認します
* @attention コマンドの先頭にはデータ数が入っています
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


//コマンドが最低限あっているか確認する)
/**
* @brief 返信データが簡易的チェックをします
* @param[in] dataBytes[]	返信データが入った配列
* @param[in] cmd			送信したコマンド
* @return 返信データの判定
* @retval False チェックサムが間違っていたとき、2byte目が送信コマンドと違っていたとき
* @details 返信データの簡易チェックをします
		 返信データのチェックサムおよびコマンド部分のチェック
* @attention コマンドの先頭にはデータ数が入っています
**/	
bool Rcb4BaseClass::checkReverceCmd(byte dataBytes[],byte cmd)
{

//	const	byte cnmtbl[8] = {Move,	Jump, Call,	SingleServo, ConstFrameServo,ServoParam,AckCheck,None};	//有効ｺﾏﾝﾄﾞ
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
//	moveRamToComCmdからデータ受信までの関数
//
/**
* @brief MoveコマンドでRAM上のデータをCOMに転送します
* @param[in] scrAddr	取得するRAMデータの先頭アドレス
* @param[in] scrDataSize	取得するRAMのデータ数
* @param[out] rxDara[]		受信データ(純粋にRAMのデータのみ)
* @return 返信データのデータ数
* @retval -1 (0xFFFF) 送信失敗等
* @details RAM上指定した先頭アドレスから指定したデータ数分のデータをCOMに転送します
* @note 返り値はコマンドを含まないデータになります。
* @date 2019/01/16 rxBufの先頭アドレスをもとにmemcopyを行うと予期せぬデータで誤動作するのでfor文でデータを挿入するように変更
**/	
unsigned short Rcb4BaseClass::moveRamToComCmdSynchronize(int scrAddr,byte scrDataSize,byte	rxDara[])
{	
	
	if(	moveRamToComCmd(scrAddr ,scrDataSize) == -1)	return 0xffff;



	if(synchronize(txBuf, txBuf[0], rxBuf, scrDataSize + 3) == true)
	{
		//2019/01/16 memcopyをするとrxBuf[0]に予期せぬデータが入る場合があるので、forに変更
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
//	moveComToRamCmdからACK受信までの関数
//
/**
* @brief MoveコマンドでRAM上のデータをCOMに転送します
* @param[in] scrAddr	取得するRAMデータの先頭アドレス
* @param[in] destData[]	送信データ配列(単純にデータのみ)
* @param[out] destDataSize	送信データサイズ(単純にデータのみ)
* @return 通信判定等
* @retval False 送信失敗等
* @details COMからRAM上の指定したアドレスにデータを書きこみます
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
//送受信後ＡＣＫ判定
/**
* @brief 何かしらコマンドを送って、ACKが返ってくるか確認
* @param[in] *txData	送信データ
* @return 通信判定等
* @retval False 送信失敗等
* @details 何かしらコマンドを送って、ACKが返ってくるか確認
* @note txDataの先頭データには送信データのデータ数が入っています
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
//PIO関係
//PIOの現在の状態を取得する
/**
* @brief 現在のPIOの状態を取得します
* @return 現在のPIOの状態(2byteを含めすべて)
* @retval -1(0xFFFF) 通信失敗
* @details Move命令を使って現在のPIOのHかLの状態を取得します
* @attention 1bitずつ判定できないので注意
**/	
unsigned short Rcb4BaseClass::getPio()
{
	byte	wkbuf[10];
	if(moveRamToComCmdSynchronize(PioAddress ,0x02,wkbuf) != 0xffff)
	{
		WordByte wk;
		wk.byte[0] = wkbuf[0];
		wk.byte[1] = wkbuf[1];

		wk.word &= 0x03FF;		//いらないbitはマスクをかける

		return 	wk.word;	
	}
	else
	{
		return 0xffff;
	}
}

//PIOの状態を変更する)
/**
* @brief 現在のPIOの状態を変更します
* @param[in] pioData	PIO全体のデータ
* @return 通信が成功したかどうか
* @retval False 通信失敗
* @details Move命令を使って現在のPIOのHかLの状態にします
* @attention 1bitずつ変更できないので注意
**/	
bool Rcb4BaseClass::setPio (unsigned short pioData)
{
	return moveComToRamCmdSynchronize(PioAddress, (byte*)&pioData,2);
}

//PIOの入出力を取得する
/**
* @brief 現在のPIOの入出力状態を取得します
* @return 現在のPIOの状態(2byteを含めすべて)
* @retval -1(0xFFFF) 通信失敗
* @details Move命令を使って現在のPIOの入力か出力かを取得します
* @note 対応したbitの　1:出力　0:入力
* @attention 1bitずつ判定できないので注意
**/	
unsigned short Rcb4BaseClass::getPioMode()
{
	byte	wkbuf[10];
	if(moveRamToComCmdSynchronize(PioModeAddres ,0x02,wkbuf) != 0xffff)
	{
		WordByte wk;
		wk.byte[0] = wkbuf[0];
		wk.byte[1] = wkbuf[1];

		wk.word &= 0x03FF;		//いらないbitはマスクをかける

		return 	wk.word;	
	}
	else
	{
		return 0xffff;
	}
}


//PIOの入出力の指定を変更する)
/**
* @brief 現在のPIOの入出力状態を変更します
* @param[in] pioModeData	PIO全体の入出力データ
* @return 通信が成功したかどうか
* @retval False 通信失敗
* @details Move命令を使って現在のPIO入力と出力を変更します
* @note 対応したbitの　1:出力　0:入力
* @attention 1bitずつ変更できないので注意
**/
bool Rcb4BaseClass::setPioMode (unsigned short pioModeData)
{
	return moveComToRamCmdSynchronize(PioModeAddres, (byte*)&pioModeData,2);
}



//////////////////////////////////////////////////////////////////////////////////////////
//AD関係
//ADの状態を取得する
/**
* @brief 現在指定したADの値を取得します
* @param[in] adPort	ADポートの指定(0～10)
* @return 指定したADの値(分解能10bit)
* @retval -1(0xFFFF) 通信失敗
* @details Move命令を使って指定した現在のAD値を取得します
* @note	 AD0を取得すると現在の電源電圧値を分圧した値が読めます
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

//すべてのADの状態を取得する
/**
* @brief 現在のADの値すべてを取得します
* @param[out] adDatas[] すべてのAD値を格納する配列
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details Move命令を使って指定した現在のAD値を取得します
* @note	 AD0を取得すると現在の電源電圧値を分圧した値が読めます
* @attention adDatas[]は11個分の配列を先に用意してください
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
//ボタンデータを送付
//KRRのすべてのデータを設定する
/**
* @brief KRRの領域にデータをすべて送信する(疑似的にボタンデータを送る)
* @param[in] buttonData ボタンデータ
* @param[in] adData 4byte分のアナログデータを指定
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details Move命令を使ってKRRの領域にデータをすべて送信します
* @note	 ボタンデータをRCB4におくります。
* @note	サンプルモーション等ボタンをトリガにしているモーションを動かす時に使用します
* @note	ボタンを押してない状態をおくるまで押しっぱなしになります
* @attention RCB4にKRRを接続しないでください
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



//KRRのボタンデータを設定する
/**
* @brief KRRの領域にボタンデータを送信する(疑似的にボタンデータを送る)
* @param[in] buttonData ボタンデータ
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details Move命令を使ってKRRの領域にボタンデータを送信します
* @note	ボタンデータをRCB4におくります。
* @note	サンプルモーション等ボタンをトリガにしているモーションを動かす時に使用します
* @note	ボタンを押してない状態をおくるまで押しっぱなしになります
* @attention RCB4にKRRを接続しないでください
**/
bool Rcb4BaseClass::setKrrButtonData (unsigned short buttonData)
{
	byte wk[2];
	
	WordByte wwk;

	wwk.word = buttonData;
	
	wk[0] = wwk.byte[1];
	wk[1] = wwk.byte[0];			//ボタンデータはプログラムから見ると逆に配置されている
	
	return moveComToRamCmdSynchronize(KrrButtonDataAddress, wk,2);
}

//KRRのADのデータを設定する)
/**
* @brief KRRの領域にボタンデータを送信する(疑似的にボタンデータを送る)
* @param[in] paPort 指定するADポート(1～4)
* @param[in] adData ADデータ(7bit)
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details Move命令を使ってKRRのADデータを送信します
* @note	 KRRに受信されているADの領域にデータにおくります。
* @note	 KRRのAD値を使ってモーションを変更している場合に外部から変更できます
* @attention RCB4にKRRを接続しないでください
**/
bool Rcb4BaseClass::setKrrAdData (byte paPort, byte adData)
{
	byte	wk[2];

	wk[0] =  adData;
	return moveComToRamCmdSynchronize(KrrPa1Address + (paPort - 1), wk,1);

}



//////////////////////////////////////////////////////////////////////////////////////////
//サーボモータ
//サーボモータ単体を動かす
/**
* @brief 指定したサーボモータ単体を動作させます
* @param[in] id		サーボモータのID番号
* @param[in] sio	SIOの値(SioNum)
* @param[in] pos	サーボモータの目標ポジション値
* @param[in] frame	フレーム数(多いほどゆっくり動く)
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details 指定したサーボモータをID,SIO,POS,Frame数を使用して動かします
* @note	 モーション再生中に指定した場合はモーションの角度データが優先されます
**/
bool Rcb4BaseClass::setSingleServo (byte id, byte sio,int pos,byte frame)
{
	if(runSingleServoCmd(id, sio, pos, frame) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



/**
* @brief 指定したサーボモータ単体を動作させます
* @param[in] servoData		サーボモータのデータをひとかたまりにしたもの
* @param[in] frame	フレーム数(多いほどゆっくり動く)
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details servoDataに入っているサーボモータの情報をもとに,Frame数を使用して動かします
* @note	 モーション再生中に指定した場合はモーションの角度データが優先されます
**/
bool Rcb4BaseClass::setSingleServo (ServoData servoData,byte frame)
{
	if(runConstFrameServoCmd(&servoData, 1, frame) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



/**
* @brief 指定したサーボモータ単体をフリーにします
* @param[in] id		サーボモータのID
* @param[in] sio	SIOの値(SioNum)
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details ID,SIOで指定したサーボモータをフリーにします
* @attention	 Freeを指定するとHoldのコマンドが発行されるまでFree状態を保ち続けます
**/
bool Rcb4BaseClass::setFreeSingleServo (byte id, byte sio)
{
	if(runSingleServoCmd(id, sio, 0x8000, 1) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



/**
* @brief 指定したサーボモータ単体にトルクをかけます
* @param[in] id		サーボモータのID
* @param[in] sio	SIOの値(SioNum)
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details ID,SIOで指定したサーボモータにトルクをかけます
			Free状態からトルクをかける状態に復帰します
* @note	 Freeを指定するとHoldのコマンドが発行されるまでFree状態を保ち続けます
**/
bool Rcb4BaseClass::setHoldSingleServo (byte id, byte sio)
{
	if(runSingleServoCmd(id, sio, 0x7FFF, 1) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



//指定した複数のサーボモータを動かす)
/**
* @brief 指定したサーボモータ複数を動作させます
* @param[in] servoDatas[]		サーボモータのデータをひとかたまりにしたものを複数個
* @param[in] servoCount servoDatasのサーボの数
* @param[in] frame	フレーム数(多いほどゆっくり動く)
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details servoDataに入っている複数のサーボモータの情報をもとにFrame数を使用して動かします
* @note	 モーション再生中に指定した場合はモーションの角度データが優先されます
**/
bool Rcb4BaseClass::setServoPos (ServoData servoDatas[],byte servoCount,byte frame)
{
	if(runConstFrameServoCmd(servoDatas, servoCount, frame) == -1)	return false;
	return	SynchronizeAck(txBuf);
}

//指定した複数のサーボモータをフリーにする
/**
* @brief 指定したサーボモータ単体をフリーにします
* @param[in] servoDatas[]		サーボモータのデータをひとかたまりにしたものを複数個
* @param[in] servoCount servoDatasのサーボの数
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details servoDatas[]で指定したサーボモータをフリーにします
* @attention	 Freeを指定するとHoldのコマンドが発行されるまでFree状態を保ち続けます
* @attention	 servoDatasのDataが0x8000に書き換わります
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
* @brief 指定した複数のサーボモータにトルクをかけます
* @param[in] servoDatas[]		サーボモータのデータをひとかたまりにしたものを複数個
* @param[in] servoCount servoDatasのサーボの数
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details servoDatas[]で指定したサーボモータをフリーにします
			Free状態からトルクをかける状態に復帰します
* @note	 Freeを指定するとHoldのコマンドが発行されるまでFree状態を保ち続けます
* @attention	 servoDatasのDataが0x7FFFに書き換わります
**/
bool Rcb4BaseClass::setHoldPos (ServoData servoDatas[],byte servoCount)
{
	for(int i = 0; i < servoCount; i++)
	{
		servoDatas[i].Data = 0x7FFF;
	}
	return	setServoPos (servoDatas, servoCount, 1);
}



//指定したサーボモータのポジションデータを取得する
/**
* @brief 指定したサーボモータのポジションデータを取得します
* @param[in] id		サーボモータのID
* @param[in] sio	SIOの値(SioNum)
* @return 指定したサーボモータのポジションデータ
* @retval -1 通信失敗
* @details ID,SIOで指定したサーボモータのポジションデータを取得します
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

//指定したサーボモータのスピードを変更する
/**
* @brief 指定したサーボモータのスピードを変更します
* @param[in] servoDatas[]		サーボモータのデータをひとかたまりにしたものを複数個
* @param[in] servoCount servoDatasのサーボの数
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details servoDatas[]で指定したサーボモータのスピード(出力の上限値)を変更します
* @note	 servoDatasのDataにはスピード値を入れます
**/
bool Rcb4BaseClass::setServoSpeed (ServoData servoDatas[],byte servoCount)
{
	
	if(setSpeedCmd(servoDatas, servoCount) == -1)	return false;
	return	SynchronizeAck(txBuf);
}



//指定したサーボモータのストレッチを変更する
/**
* @brief 指定したサーボモータのストレッチを変更します
* @param[in] servoDatas[]		サーボモータのデータをひとかたまりにしたものを複数個
* @param[in] servoCount servoDatasのサーボの数
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details servoDatas[]で指定したサーボモータのストレッチ(柔らかさ)を変更します
* @note	 servoDatasのDataにはストレッチ値を入れます
**/
bool Rcb4BaseClass::setServoStretch (ServoData servoDatas[],byte servoCount)
{
	
	if(setStretchCmd(servoDatas, servoCount) == -1) return false;
	return	SynchronizeAck(txBuf);

}

//IDとSIOからICS番号に変換する
/**
* @brief IDとSIOの番号からICS番号に変更します
* @param[in] id		サーボモータのID
* @param[in] sio	SIOの値(SioNum)
* @return ICS番号
* @details IDとSIOの番号からRCB4内部で扱うICS番号に変更します
**/
byte Rcb4BaseClass::icsNum2id(byte id,byte sio)
{
	return id * 2 + (sio - 1);
}

//////////////////////////////////////////////////////////////////////////////////////////
//サーボモータ

//指定したモーションを再生する
/**
* @brief 指定したモーションを再生させます
* @param[in] motionNum		再生させるモーション番号
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details 指定したモーションを再生させます
* @note		suspend() => resetProgramCounter() => setMotionNum() => resume() 
* @note		一時中断  => プログラムカウンタをリセット => 動かすモーションアドレスにジャンプ => モーションの再開
			の手順でモーションを再生させます
* @attention	現在、他のモーションを再生中でも強制的に止めてから再生させます
**/
bool Rcb4BaseClass::motionPlay(int motionNum)
{
	
	if(suspend() == false)	return false;
	
	if(resetProgramCounter() == false)	return false;
	
	if(setMotionNum(motionNum) == false)	return false;
	
	return resume ();
}



//モーションを一時停止する
/**
* @brief モーションを一時停止させます
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details モーションを一時停止させます
* @note		正確には、システムレジスタのEnableRunEeprom値を変更します
				ベクタジャンプを無効化して無線機などのボタンデータでジャンプするのをやめます
* @attention	現在、他のモーションを再生中でも強制的に止めます
* @attention	configData値を直接変えますので、注意が必要です
**/
bool Rcb4BaseClass::suspend()
{
	return suspend(&(configData.word));
}



/**
* @brief モーションを一時停止させます
* @param[in,out] config		外部で覚えているconfigデータ
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details モーションを一時停止させます
* @note		正確には、システムレジスタのEnableRunEeprom値を変更します
				ベクタジャンプを無効化して無線機などのボタンデータでジャンプするのをやめます
* @attention	現在、他のモーションを再生中でも強制的に止めます
* @attention	config内を直接変えますので、注意が必要です
**/
bool Rcb4BaseClass::suspend(unsigned short *config)
{
	ConfigData *cData = (ConfigData *)config;
	
	cData->bit.EnableRunEeprom =0;			//EEPROMで実行中の部分を止める
	//configData.bit.EnableServoResponse = 0;		//サーボのレスポンスを返さなくする
	cData->bit.EnableReferenceTable = 0;	//ベクタジャンプを無効にする
	//configData.bit.EnableSio = 1;				//SIOは動いたままにする
	
	return moveComToRamCmdSynchronize(ConfigRamAddress, (byte*)config,2);
	

}



//ROMのモーションアドレスに飛ぶ
/**
* @brief 指定したモーションにジャンプします
* @param[in] motionNum		ジャンプするモーション番号
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details プログラムカウンタをリセットします。
* @note		CALL命令で指示したモーションの先頭アドレスにジャンプします
* @attention	現在、他のモーションを再生中でも強制的にジャンプするので注意が必要です
**/
bool Rcb4BaseClass::setMotionNum(byte motionNum)
{
	if(callCmd(txBuf	,motionAddr2motionNum(motionNum)) == -1) return false;
	return SynchronizeAck(txBuf);
}



//プロフラムカウンタをリセットします
/**
* @brief プロフラムカウンタをリセットします
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details プログラムカウンタをリセットします。
* @note		プログラムカウンタをRCB4初期設定後のアドレスに移動させます。
			また、同時にEEPROM更新フラグも0にしておきます
* @attention	現在、他のモーションを再生中でも強制的に変わりますので、注意が必要です
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



//モーションをリスタート(復帰)させます
/**
* @brief モーションを再開させます
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details モーションを一時停止させます
* @note		正確には、システムレジスタのEnableRunEeprom値を変更します
				ベクタジャンプを有効化して無線機などのボタンデータでジャンプできるようにします
* @attention	現在、他のモーションを再生中でも強制的に止めます
* @attention	configData値を直接変えますので、注意が必要です
**/
bool Rcb4BaseClass::resume ()
{
	return resume(&(configData.word));
}



/**
* @brief モーションを再開させます
* @param[in,out] config		外部で覚えているconfigデータ
* @return 通信が成功かどうか
* @retval False 通信失敗
* @details モーションを一時停止させます
* @note		正確には、システムレジスタのEnableRunEeprom値を変更します
				ベクタジャンプを有効化して無線機などのボタンデータでジャンプできるようにします
* @attention	config内を直接変えますので、注意が必要です
**/
bool Rcb4BaseClass::resume (unsigned short *config)
{
	ConfigData *cData = (ConfigData *)config;
	
	cData->bit.EnableRunEeprom =1;			//EEPROMの読み込み動作をスタートさせる
	//configData.bit.EnableServoResponse = 0;		//サーボのレスポンスを返さなくする
	cData->bit.EnableReferenceTable = 1;	//ベクタジャンプを有効にする
	//configData.bit.EnableSio = 1;				//SIOは動いたままにする
	
	return moveComToRamCmdSynchronize(ConfigRamAddress, (byte*)config,2);
	
}



//モーション番号をモーションの先頭アドレスに変換
/**
* @brief モーション番号からモーションの先頭アドレスを求めます
* @return モーション番号の先頭のアドレス
* @details モーション番号からモーションの先頭アドレスを求めます
* @note		アドレスに関してはRCB-4プログラミングマニュアルをご覧ください。
* @attention	モーションアドレスは、HTH4のV2.2以降のアドレスになります。
* @attention	V2.0以前のアドレスでは誤動作をしますので注意が必要です
**/
long Rcb4BaseClass::motionAddr2motionNum(byte motionNum)
{
	//return (motionNum -1)* MotionSingleDataCount + MotionRomAddress;
	return (long)(motionNum -1)* (long)MotionSingleDataCount + MotionRomAddress;
}



















