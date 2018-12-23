#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <string>
#include <QCoreApplication>
using namespace std;

#include "opennetstream.h"
#include "opennetstream_p.h"

//Begin PTR Define
#if defined(_WIN32) || defined(_WIN64)
#define EZOPENSDKNAME "OpenNetStream.dll"
#elif defined(__linux__)
#define EZOPENSDKNAME "libOpenNetStream.so"
#elif defined (ANDROID)
#define EZOPENSDKNAME "libOpenNetStream.so"
#elif defined(__APPLE__)
#define EZOPENSDKNAME "libOpenNetStream.dylib"
#endif  //END PTR Define

OpenNetStream* OpenNetStream::m_instance;

OpenNetStreamPrivate::OpenNetStreamPrivate(OpenNetStream *parent) :
    q_ptr(parent), m_library(NULL)
{
}

void OpenNetStreamPrivate::init()
{
    Q_Q(OpenNetStream);

    QString dllPath = QCoreApplication::applicationDirPath().append("/") + EZOPENSDKNAME;
    qDebug() << "dllpath:" << dllPath;
    m_library = new QLibrary(dllPath, q);
    if (m_library->load()) {
        m_apis.pOpenSDK_InitLib              = (EZSDK_InitLib             )m_library->resolve("OpenSDK_InitLib");
        m_apis.pOpenSDK_FiniLib              = (EZSDK_FiniLib             )m_library->resolve("OpenSDK_FiniLib");
        m_apis.pOpenSDK_SetLoginParams       = (EZSDK_SetLoginParams      )m_library->resolve("OpenSDK_SetLoginParams");
        m_apis.pOpenSDK_GetLoginResponseParams = (EZSDK_GetLoginResponseParams)m_library->resolve("OpenSDK_GetLoginResponseParams");
        m_apis.pOpenSDK_Logout               = (EZSDK_Logout              )m_library->resolve("OpenSDK_Logout");
        m_apis.pOpenSDK_SetAccessToken       = (EZSDK_SetAccessToken      )m_library->resolve("OpenSDK_SetAccessToken");
        m_apis.pOpenSDK_SetConfigInfo        = (EZSDK_SetConfigInfo		)m_library->resolve("OpenSDK_SetConfigInfo");
        m_apis.pOpenSDK_AllocSessionEx       = (EZSDK_AllocSessionEx      )m_library->resolve("OpenSDK_AllocSessionEx");
        m_apis.pOpenSDK_FreeSession          = (EZSDK_FreeSession         )m_library->resolve("OpenSDK_FreeSession");
        m_apis.pOpenSDK_SetSessionConfig     = (EZSDK_SetSessionConfig    )m_library->resolve("OpenSDK_SetSessionConfig");
        m_apis.pOpenSDK_startRealPlay        = (EZSDK_StartRealPlayEx     )m_library->resolve("OpenSDK_StartRealPlayEx");
        m_apis.pOpenSDK_setVideoLevel        = (EZSDK_SetVideoLevel       )m_library->resolve("OpenSDK_SetVideoLevel");
        m_apis.pOpenSDK_stopRealPlay         = (EZSDK_StopRealPlayEx      )m_library->resolve("OpenSDK_StopRealPlayEx");
        m_apis.pOpenSDK_startPlayBack        = (EZSDK_StartPlayBackEx     )m_library->resolve("OpenSDK_StartPlayBackEx");
        m_apis.pOpenSDK_PlayBackResume       = (EZSDK_PlayBackResume      )m_library->resolve("OpenSDK_PlayBackResume");
        m_apis.pOpenSDK_PlayBackPause        = (EZSDK_PlayBackPause       )m_library->resolve("OpenSDK_PlayBackPause");
        m_apis.pOpenSDK_stopPlayBack         = (EZSDK_StopPlayBackEx      )m_library->resolve("OpenSDK_StopPlayBackEx");
        m_apis.pOpenSDK_StartDownload        = (EZSDK_StartDownload       )m_library->resolve("OpenSDK_StartDownload");
        m_apis.pOpenSDK_StopDownload         = (EZSDK_StopDownload        )m_library->resolve("OpenSDK_StopDownload");
        m_apis.pOpenSDK_SetDataCallBack      = (EZSDK_SetDataCallBack     )m_library->resolve("OpenSDK_SetDataCallBack");
        m_apis.pOpenSDK_startSearch          = (EZSDK_StartSearchEx       )m_library->resolve("OpenSDK_StartSearchEx");
        m_apis.pOpenSDK_GetOSDTime           = (EZSDK_GetOSDTime          )m_library->resolve("OpenSDK_GetOSDTime");

        m_apis.pOpenSDK_OpenSound            = (EZSDK_OpenSound           )m_library->resolve("OpenSDK_OpenSound");
        m_apis.pOpenSDK_CloseSound           = (EZSDK_CloseSound          )m_library->resolve("OpenSDK_CloseSound");
        m_apis.pOpenSDK_GetVolume            = (EZSDK_GetVolume           )m_library->resolve("OpenSDK_GetVolume");
        m_apis.pOpenSDK_SetVolume            = (EZSDK_SetVolume           )m_library->resolve("OpenSDK_SetVolume");
        m_apis.pOpenSDK_StartVoiceTalk       = (EZSDK_StartVoiceTalkEx    )m_library->resolve("OpenSDK_StartVoiceTalkEx");
        m_apis.pOpenSDK_StopVoiceTalk        = (EZSDK_StopVoiceTalk       )m_library->resolve("OpenSDK_StopVoiceTalk");
        m_apis.pOpenSDK_CapturePicture       = (EZSDK_CapturePicture      )m_library->resolve("OpenSDK_CapturePicture");

        m_apis.pOpenSDK_Mid_Login            = (EZSDK_Mid_Login           )m_library->resolve("OpenSDK_Mid_Login");
        m_apis.pOpenSDK_Mid_Device_Add       = (EZSDK_Mid_Device_Add      )m_library->resolve("OpenSDK_Mid_Device_Add");
        m_apis.pOpenSDK_Mid_Device_Oper      = (EZSDK_Mid_Device_Oper     )m_library->resolve("OpenSDK_Mid_Device_Oper");

        m_apis.pOpenSDK_Data_GetDevListEx    = (EZSDK_Data_GetDevListEx   )m_library->resolve("OpenSDK_Data_GetDevListEx");
        m_apis.pOpenSDK_Data_GetSharedDevList = (EZSDK_Data_GetSharedDevList)m_library->resolve("OpenSDK_Data_GetSharedDevList");
        m_apis.pOpenSDK_Data_GetDeviceInfo   = (EZSDK_Data_GetDeviceInfo  )m_library->resolve("OpenSDK_Data_GetDeviceInfo");
        m_apis.pOpenSDK_Data_GetAlarmListEx  = (EZSDK_Data_GetAlarmListEx )m_library->resolve("OpenSDK_Data_GetAlarmListEx");
        m_apis.pOpenSDK_Data_SetAlarmRead    = (EZSDK_Data_SetAlarmRead   )m_library->resolve("OpenSDK_Data_SetAlarmRead");
        m_apis.pOpenSDK_Data_DeleteDevice    = (EZSDK_Data_DeleteDevice   )m_library->resolve("OpenSDK_Data_DeleteDevice");
        m_apis.pOpenSDK_Data_Free            = (EZSDK_Data_Free           )m_library->resolve("OpenSDK_Data_Free");
        m_apis.pOpenSDK_Data_GetDevDetailInfo   = (EZSDK_Data_GetDevDetailInfo)m_library->resolve("OpenSDK_Data_GetDevDetailInfo");

        m_apis.pOpenSDK_PTZCtrlEx                = (EZSDK_PTZCtrlEx               )m_library->resolve("OpenSDK_PTZCtrlEx");
        m_apis.pOpenSDK_DevDefenceEx             = (EZSDK_DevDefenceEx            )m_library->resolve("OpenSDK_DevDefenceEx");

        m_apis.pOpenSDK_GetAccessTokenSmsCode    = (EZSDK_GetAccessTokenSmsCode   )m_library->resolve("OpenSDK_GetAccessTokenSmsCode");
        m_apis.pOpenSDK_VerifyAccessTokenSmsCode = (EZSDK_VerifyAccessTokenSmsCode)m_library->resolve("OpenSDK_VerifyAccessTokenSmsCode");
        m_apis.pOpenSDK_GetHdSignSmsCode         = (EZSDK_GetHdSignSmsCode        )m_library->resolve("OpenSDK_GetHdSignSmsCode");
        m_apis.pOpenSDK_VerifyHdSignSmsCode      = (EZSDK_VerifyHdSignSmsCode     )m_library->resolve("OpenSDK_VerifyHdSignSmsCode");
		
		m_apis.pOpenSDK_HttpSendWithWait		 = (EZSDK_HttpSendWithWait		)m_library->resolve("OpenSDK_HttpSendWithWait");

		m_apis.pOpenSDK_Push_SetAlarmCallBack				= (EZSDK_Push_SetAlarmCallBack				)m_library->resolve("OpenSDK_Push_SetAlarmCallBack");
		m_apis.pOpenSDK_Push_SetDeviceStatusCallBack		= (EZSDK_Push_SetDeviceStatusCallBack			)m_library->resolve("OpenSDK_Push_SetDeviceStatusCallBack");
		m_apis.pOpenSDK_Push_SetTransparentChannelCallBack  = (EZSDK_Push_SetTransparentChannelCallBack	)m_library->resolve("OpenSDK_Push_SetTransparentChannelCallBack");
        m_apis.pOpenSDK_Push_StartRecvEx					= (EZSDK_Push_StartRecvEx						)m_library->resolve("OpenSDK_Push_StartRecvEx");
		m_apis.pOpenSDK_Push_StopRecv						= (EZSDK_Push_StopRecv						)m_library->resolve("OpenSDK_Push_StopRecv");
		m_apis.pOpenSDK_DecryptPicture						= (EZSDK_DecryptPicture						)m_library->resolve("OpenSDK_DecryptPicture");

		m_apis.pOpenSDK_GetLastErrorCode       = (EZSDK_GetLastErrorCode      )m_library->resolve("OpenSDK_GetLastErrorCode");
		m_apis.pOpenSDK_GetLastErrorDesc       = (EZSDK_GetLastErrorDesc      )m_library->resolve("OpenSDK_GetLastErrorDesc");
    }
    else {
        QMessageBox::critical(NULL, "Load OpenNetStream lib failed, path" + dllPath, m_library->errorString());
        exit(-1);
    }
}

OpenNetStream::OpenNetStream(QObject *parent) :
    QObject(parent),
    d_ptr(new OpenNetStreamPrivate(this))
{
    Q_D(OpenNetStream);
    d->init();
}

OpenNetStream::~OpenNetStream()
{
    delete d_ptr;
}

OpenNetStream* OpenNetStream::getInstance()
{
    if (m_instance == NULL) {
        m_instance = new OpenNetStream;
    }
    return m_instance;
}

int OpenNetStream::initLibrary(const QString& authAddr, const QString& platform, const QString appId)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_InitLib(authAddr.toStdString().c_str(), platform.toStdString().c_str(), appId.toStdString().c_str());
}

int OpenNetStream::releaseLibrary()
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_FiniLib();
}

void OpenNetStream::setAreaID(const QString& areaID)
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_SetLoginParams(LOGIN_AREAID, areaID.toStdString().c_str());
}

void OpenNetStream::setLoginVer(const QString& loginVer)
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_SetLoginParams(LOGIN_VER, loginVer.toStdString().c_str());
}

void OpenNetStream::setDataUtf8()
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_SetConfigInfo(CONFIG_DATA_UTF8, 1);
}

void OpenNetStream::setStreamTrans()
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_SetConfigInfo(CONFIG_OPEN_STREAMTRANS, 1);
}

void OpenNetStream::closeP2P()
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_SetConfigInfo(CONFIG_CLOSE_P2P, 1);
}

void OpenNetStream::setP2PMaxNum(int iP2PMaxNum)
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_SetConfigInfo(CONFIG_P2P_MAXNUM, iP2PMaxNum);
}

void OpenNetStream::setP2PCheckPeriod(int iP2PCheckPeriod)
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_SetConfigInfo(CONFIG_P2P_CHECK_PERIOD, iP2PCheckPeriod);
}

void OpenNetStream::setP2PExpire(int iP2PExpire)
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_SetConfigInfo(CONFIG_P2P_EXPIRE, iP2PExpire);
}

void OpenNetStream::setLogLevel(int iLogLevel)
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_SetConfigInfo(CONFIG_LOG_LEVEL, iLogLevel);
}

const char* OpenNetStream::getTokenExpireTime()
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_GetLoginResponseParams(LOGIN_TOKEN_EXPIRETIME);
}

const char* OpenNetStream::getAreaDomain()
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_GetLoginResponseParams(LOGIN_AREADOMAIN);
}

const char* OpenNetStream::getAccessToken()
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_GetLoginResponseParams(LOGIN_ACCESS_TOKEN);
}

QString OpenNetStream::login()
{
    Q_D(OpenNetStream);
    char* pToken = NULL;
    int length = 0;
    d->m_apis.pOpenSDK_Mid_Login(&pToken, &length);
    qDebug()<<"OpenSDK_Mid_Login token:"<<pToken;
    QString token = pToken;
    return token;
}

void OpenNetStream::logout()
{
    Q_D(OpenNetStream);
    d->m_apis.pOpenSDK_Logout();
}

int OpenNetStream::setAccessToken(const QString& accessId)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_SetAccessToken(accessId.toStdString().c_str());
}

int OpenNetStream::deviceAdd(const QString& accessId)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_Mid_Device_Add(accessId.toStdString().c_str());
}

int OpenNetStream::deviceOper(const QString& accessId, const QString& deviceId)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_Mid_Device_Oper(accessId.toStdString().c_str(), deviceId.toStdString().c_str());
}

QString OpenNetStream::allocSessionEx(OpenSDK_MessageHandler handle, void* pUser)
{
    Q_D(OpenNetStream);
    char* pSession = NULL;
    int length = 0;
    d->m_apis.pOpenSDK_AllocSessionEx(handle, pUser, &pSession, &length);
    QString session = pSession;
    freeData(pSession);
    return session;
}

int OpenNetStream::freeSession(const QString& session)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_FreeSession(session.toStdString().c_str());
}

void OpenNetStream::setSessionStreamTrans(const QString& session, bool bOpen)
{
    Q_D(OpenNetStream);
    int iValue = 0;
    if (bOpen)
    {
        iValue = 1;
    }
    d->m_apis.pOpenSDK_SetSessionConfig(session.toStdString().c_str(), CONFIG_OPEN_STREAMTRANS, iValue);
}

int OpenNetStream::setVideoLevel(const QString& devSerial, const int iChannelNo, const int iVideoLevel)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_setVideoLevel(devSerial.toStdString().c_str(), iChannelNo, iVideoLevel);
}

int OpenNetStream::startRealPlay(const QString& session, HWND hPlayWnd, const QString& devSerial, const int iChannelNo, const QString& safekey)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_startRealPlay(session.toStdString().c_str(), hPlayWnd,
        devSerial.toStdString().c_str(), iChannelNo, safekey.toStdString().c_str());
}

int OpenNetStream::stopRealPlayEx(const QString& session)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_stopRealPlay(session.toStdString().c_str());
}

int OpenNetStream::startPlayBack(const QString& session, HWND hPlayWnd, const QString& devSerial, const int iChannelNo, const QString& safekey, const QString& startTime, const QString& stopTime)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_startPlayBack(session.toStdString().c_str(), hPlayWnd,
        devSerial.toStdString().c_str(), iChannelNo, safekey.toStdString().c_str(), 
        startTime.toStdString().c_str(), stopTime.toStdString().c_str());
}

int OpenNetStream::playBackResume(const QString& session)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_PlayBackResume(session.toStdString().c_str());
}

int OpenNetStream::playBackPause(const QString& session)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_PlayBackPause(session.toStdString().c_str());
}

int OpenNetStream::stopPlayBackEx(const QString& session)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_stopPlayBack(session.toStdString().c_str());
}

int OpenNetStream::setDataCallBack(const QString& szSessionId, OpenSDK_DataCallBack pDataCallBack, void* pUser)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_SetDataCallBack(szSessionId.toStdString().c_str(),pDataCallBack,pUser);
}

int OpenNetStream::startDownload(const QString& session, const QString& devSerial, const int iChannelNo, const QString& safekey, const QString& startTime, const QString& stopTime)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_StartDownload(session.toStdString().c_str(), devSerial.toStdString().c_str(), 
        iChannelNo, safekey.toStdString().c_str(), startTime.toStdString().c_str(), stopTime.toStdString().c_str());
}

int OpenNetStream::stopDownload(const QString& session)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_StopDownload(session.toStdString().c_str());
}

int OpenNetStream::startSearch(const QString& session, const QString& devSerial, const int iChannelNo, const QString& startTime, const QString& stopTime)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_startSearch(session.toStdString().c_str(), 
        devSerial.toStdString().c_str(), iChannelNo,
        startTime.toStdString().c_str(), stopTime.toStdString().c_str());
}

int OpenNetStream::getOSDTime(const QString& session, STREAM_TIME* pTime)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_GetOSDTime(session.toStdString().c_str(), pTime);
}

int OpenNetStream::getDevListEx(int pageStart, int pageSize, void** pBuf, int* length)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_Data_GetDevListEx(pageStart, pageSize, pBuf, length);
}

int OpenNetStream::getSharedDevList(int pageStart, int pageSize, void** pBuf, int* length)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_Data_GetSharedDevList(pageStart, pageSize, pBuf, length);
}

int OpenNetStream::getDevInfo(const QString& accessId, const QString& devSerial, void** pBuf, int* length)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_Data_GetDeviceInfo) {
        return d->m_apis.pOpenSDK_Data_GetDeviceInfo(accessId.toStdString().c_str(), devSerial.toStdString().c_str(), pBuf, length);
    }
    return -1;
}

int OpenNetStream::getAlarmList(const QString& devSerial, const int iChannelNo, const QString& startTime, const QString& endTime, int alarmType, int status, int pageStart, int pageSize, void** pBuf, int* length)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_Data_GetAlarmListEx) {
        return d->m_apis.pOpenSDK_Data_GetAlarmListEx(devSerial.toStdString().c_str(), iChannelNo,
                    startTime.toStdString().c_str(), endTime.toStdString().c_str(),
                    alarmType, status, pageStart, pageSize, pBuf, length);
    }
    return -1;
}

int OpenNetStream::setAlarmRead(const QString& accessId, const QString& alarmId)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_Data_SetAlarmRead) {
        return d->m_apis.pOpenSDK_Data_SetAlarmRead(accessId.toStdString().c_str(), alarmId.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::deleteDevice(const QString& accessId, const QString& cameraId)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_Data_DeleteDevice(accessId.toStdString().c_str(), cameraId.toStdString().c_str());
}

int OpenNetStream::freeData(void* pBuf)
{
    Q_D(OpenNetStream);
    return d->m_apis.pOpenSDK_Data_Free(pBuf);
}

int OpenNetStream::openSound(const QString& session)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_OpenSound) {
        return d->m_apis.pOpenSDK_OpenSound(session.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::closeSound(const QString& session)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_CloseSound) {
        return d->m_apis.pOpenSDK_CloseSound(session.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::getVolume(const QString& session)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_GetVolume) {
        return d->m_apis.pOpenSDK_GetVolume(session.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::setVolume(const QString& session, unsigned short uVolume)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_SetVolume) {
        return d->m_apis.pOpenSDK_SetVolume(session.toStdString().c_str(), uVolume);
    }
    return -1;
}

int OpenNetStream::startVoiceTalk(const QString& session, const QString& devSerial, const int iChannelNo)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_StartVoiceTalk) {
        return d->m_apis.pOpenSDK_StartVoiceTalk(session.toStdString().c_str(), devSerial.toStdString().c_str(), iChannelNo);
    }
    return -1;
}

int OpenNetStream::stopVoiceTalk(const QString& session)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_StopVoiceTalk) {
        return d->m_apis.pOpenSDK_StopVoiceTalk(session.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::capturePicture(const QString& session, const QString& szFileName)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_CapturePicture) {
        return d->m_apis.pOpenSDK_CapturePicture(session.toStdString().c_str(), szFileName.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::PTZCtrl(const QString& szSessionId, const QString& devSerial, const int iChannelNo, PTZCommand enCommand, PTZAction enAction, int iSpeed)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_PTZCtrlEx) {
        return d->m_apis.pOpenSDK_PTZCtrlEx(szSessionId.toStdString().c_str(), \
            devSerial.toStdString().c_str(), iChannelNo, enCommand, enAction, iSpeed);
    }
    return -1;
}

int OpenNetStream::DevDefence(const QString& szSessionId, const QString& devSerial, const int iChannelNo, DefenceType enType, DefenceStatus enStatus, DefenceActor enActor)
{
    qDebug()<<"Defence By Camera"<<endl;
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_DevDefenceEx) {
        return d->m_apis.pOpenSDK_DevDefenceEx(szSessionId.toStdString().c_str(), \
                                devSerial.toStdString().c_str(), iChannelNo, enType, enStatus, enActor);
    }
    return -1;
}

int OpenNetStream::GetAccessTokenSmsCode(const QString& szSignString)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_GetAccessTokenSmsCode) {
        return d->m_apis.pOpenSDK_GetAccessTokenSmsCode(szSignString.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::VerifyAccessTokenSmsCode(const QString& szSmsCode, const QString& szUserId, const QString& szPhoneNumber, const QString& szAppKey)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_VerifyAccessTokenSmsCode) {
        return d->m_apis.pOpenSDK_VerifyAccessTokenSmsCode(szSmsCode.toStdString().c_str(), szUserId.toStdString().c_str(), szPhoneNumber.toStdString().c_str(), szAppKey.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::GetHdSignSmsCode(const QString& szAccessToken, const QString& szSignString)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_GetHdSignSmsCode) {
        return d->m_apis.pOpenSDK_GetHdSignSmsCode(szAccessToken.toStdString().c_str(), szSignString.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::VerifyHdSignSmsCode(const QString& szAccessToken, const QString& szSmsCode, const QString& szUserId, const QString& szAppKey)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_VerifyHdSignSmsCode) {
        return d->m_apis.pOpenSDK_VerifyHdSignSmsCode(szAccessToken.toStdString().c_str(), szSmsCode.toStdString().c_str(), szUserId.toStdString().c_str(), szAppKey.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::GetDevDetailInfo(const QString& devSerial, const int iChannelNo, const bool bUpdate, void** pDevDetailInfo, int* iLen )
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_Data_GetDevDetailInfo){
        return d->m_apis.pOpenSDK_Data_GetDevDetailInfo(devSerial.toStdString().c_str(), iChannelNo, bUpdate, pDevDetailInfo, iLen);
    }
    return -1;
}

int OpenNetStream::HttpSendWithWait(const QString& szUri, const QString& szHeaderParam, const QString& szBody, char** pBuf, int* iLength)
{
	Q_D(OpenNetStream);
	if (d->m_apis.pOpenSDK_HttpSendWithWait)
	{
		return d->m_apis.pOpenSDK_HttpSendWithWait(szUri.toStdString().c_str(), szHeaderParam.toStdString().c_str(), szBody.toStdString().c_str(), pBuf, iLength);
	}
	return -1;
}

int OpenNetStream::Push_SetAlarmCallBack(OpenSDK_Push_MessageHandler pHandle, void* pUser)
{
	Q_D(OpenNetStream);
	if (d->m_apis.pOpenSDK_Push_SetAlarmCallBack) {
		return d->m_apis.pOpenSDK_Push_SetAlarmCallBack(pHandle, pUser);
	}
	return -1;
}

int OpenNetStream::Push_SetDeviceStatusCallBack(OpenSDK_Push_MessageHandler pHandle, void* pUser)
{
	Q_D(OpenNetStream);
	if (d->m_apis.pOpenSDK_Push_SetDeviceStatusCallBack) {
		return d->m_apis.pOpenSDK_Push_SetDeviceStatusCallBack(pHandle, pUser);
	}
	return -1;
}

int OpenNetStream::Push_SetTransparentChannelCallBack(OpenSDK_Push_MessageHandler pHandle, void* pUser)
{
	Q_D(OpenNetStream);
	if (d->m_apis.pOpenSDK_Push_SetTransparentChannelCallBack) {
		return d->m_apis.pOpenSDK_Push_SetTransparentChannelCallBack(pHandle, pUser);
	}
	return -1;
}

int OpenNetStream::Push_StartRecv(const QString& pushSecret)
{
    Q_D(OpenNetStream);
    if (d->m_apis.pOpenSDK_Push_StartRecvEx) {
        return d->m_apis.pOpenSDK_Push_StartRecvEx(pushSecret.toStdString().c_str());
    }
    return -1;
}

int OpenNetStream::Push_StopRecv()
{
	Q_D(OpenNetStream);
	if (d->m_apis.pOpenSDK_Push_StopRecv) {
		return d->m_apis.pOpenSDK_Push_StopRecv();
	}
	return -1;
}

int OpenNetStream::DecryptPicture(const char* szAccessToken, const char* szPicURL, const char* szSerial, const char* szSafeKey, void** pPicBuf, int* iPicLen)
{
	Q_D(OpenNetStream);
	if (d->m_apis.pOpenSDK_DecryptPicture) {
		return d->m_apis.pOpenSDK_DecryptPicture(szAccessToken, szPicURL, szSerial, szSafeKey, pPicBuf, iPicLen);
	}
	return -1;
}

int OpenNetStream::GetLastErrorCode()
{
	Q_D(OpenNetStream);
	if (d->m_apis.pOpenSDK_GetLastErrorCode) {
		return d->m_apis.pOpenSDK_GetLastErrorCode();
	}
	return -1;
}

const std::string OpenNetStream::GetLastErrorDesc()
{
	Q_D(OpenNetStream);
	if (d->m_apis.pOpenSDK_GetLastErrorDesc) {
		return d->m_apis.pOpenSDK_GetLastErrorDesc();
	}
	return "";
}
