#ifndef OPENNETSTREAM_H
#define OPENNETSTREAM_H

#include <QObject>
#include "OpenNetStreamDefine.h"
#include "OpenNetStreamInterFace.h"

enum VideoLevel
{
    VideoSmooth = 0,                ///< 流畅
    VideoBalanced = 1,              ///< 均衡
    VideoClear = 2,                 ///< 高清
    VideoHigh = 3                   ///< 超清
};

class OpenNetStreamPrivate;
class OpenNetStream: public QObject
{
    Q_OBJECT

public:
    explicit OpenNetStream(QObject *parent = 0);

public:
    static OpenNetStream* getInstance();
    ~OpenNetStream();

    int initLibrary(const QString& authAddr, const QString& platform, const QString appId);
    int releaseLibrary();

    void setAreaID(const QString& areaID);
    void setLoginVer(const QString& loginVer);
    void setDataUtf8();
    void setStreamTrans();
    void closeP2P();
    void setP2PMaxNum(int iP2PMaxNum);
    void setP2PCheckPeriod(int iP2PCheckPeriod);
    void setP2PExpire(int iP2PExpire);
    void setLogLevel(int iLogLevel);
    const char* getTokenExpireTime();
    const char* getAreaDomain();
    const char* getAccessToken();

    QString login();
    void    logout();

    int setAccessToken(const QString& accessId);
    int deviceAdd(const QString& accessId);
    int deviceOper(const QString& accessId, const QString& deviceId);

    QString allocSession(OpenSDK_MessageHandler handle, void* pUser, const bool bSync = false,const unsigned int iTimeOut = 0xffffffff);
    QString allocSessionEx(OpenSDK_MessageHandler handle, void* pUser);
    int freeSession(const QString& session);

    void setSessionStreamTrans(const QString& session, bool bOpen);
    int setVideoLevel(const QString& devSerial, const int iChannelNo, const int iVideoLevel);
    int startRealPlay(const QString& session, HWND hPlayWnd, const QString& devSerial, const int iChannelNo, const QString& safekey);
    int stopRealPlayEx(const QString& session);

    int startPlayBack(const QString& session, HWND hPlayWnd, const QString& devSerial, const int iChannelNo, const QString& safekey, const QString& startTime, const QString& stopTime);
    int playBackResume(const QString& session);
    int playBackPause(const QString& session);
    int stopPlayBackEx(const QString& session);
    int setDataCallBack(const QString& szSessionId, OpenSDK_DataCallBack pDataCallBack, void* pUser);

    int startDownload(const QString& session, const QString& devSerial, const int iChannelNo, const QString& safekey, const QString& startTime, const QString& stopTime);
    int stopDownload(const QString& session);

    int startSearch(const QString& session, const QString& devSerial, const int iChannelNo, const QString& startTime, const QString& stopTime);
    int getOSDTime(const QString& session, STREAM_TIME* pTime);

    int getDevListEx(int pageStart, int pageSize, void** pBuf, int* length);
    int getSharedDevList(int pageStart, int pageSize, void** pBuf, int* length);
    int getDevInfo(const QString& accessId, const QString& devSerial, void** pBuf, int* length);
    int getAlarmList(const QString& devSerial, const int iChannelNo, const QString& startTime, const QString& endTime, int alarmType, int status, int pageStart, int pageSize, void** pBuf, int* length);
    int setAlarmRead(const QString& accessId, const QString& alarmId);
    int deleteDevice(const QString& accessId, const QString& deviceId);
    int freeData(void* pBuf);

    int openSound(const QString& session);
    int closeSound(const QString& session);
    int getVolume(const QString& session);
    int setVolume(const QString& session, unsigned short uVolume);
    int startVoiceTalk(const QString& session, const QString& devSerial, const int iChannelNo);
    int stopVoiceTalk(const QString& session);
    int capturePicture(const QString& session, const QString& szFileName);
    int PTZCtrl(const QString& szSessionId, const QString& devSerial, const int iChannelNo, PTZCommand enCommand, PTZAction enAction, int iSpeed);
    int DevDefence(const QString& szSessionId, const QString& devSerial, const int iChannelNo, DefenceType enType, DefenceStatus enStatus, DefenceActor enActor);
    int GetAccessTokenSmsCode(const QString& szSignString);
    int VerifyAccessTokenSmsCode(const QString& szSmsCode, const QString& szUserId, const QString& szPhoneNumber, const QString& szAppKey);
    int GetHdSignSmsCode(const QString& szAccessToken, const QString& szSignString);
    int VerifyHdSignSmsCode(const QString& szAccessToken, const QString& szSmsCode, const QString& szUserId, const QString& szAppKey);
    int GetDevDetailInfo(const QString& devSerial, const int iChannelNo, const bool bUpdate, void** pDevDetailInfo, int* iLen );

	int HttpSendWithWait(const QString& szUri, const QString& szHeaderParam, const QString& szBody, char** pBuf, int* iLength);

	int Push_SetAlarmCallBack(OpenSDK_Push_MessageHandler pHandle, void* pUser);
	int Push_SetDeviceStatusCallBack(OpenSDK_Push_MessageHandler pHandle, void* pUser);
	int Push_SetTransparentChannelCallBack(OpenSDK_Push_MessageHandler pHandle, void* pUser);
    int Push_StartRecv(const QString& pushSecret);
	int Push_StopRecv();
	int DecryptPicture(const char* szAccessToken, const char* szPicURL, const char* szSerial, const char* szSafeKey, void** pPicBuf, int* iPicLen);

	int GetLastErrorCode();
	const std::string GetLastErrorDesc();

signals:

public slots:

private:
    OpenNetStreamPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(OpenNetStream)
    static OpenNetStream* m_instance;
};

#endif // OPENNETSTREAM_H
