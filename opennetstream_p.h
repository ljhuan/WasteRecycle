#ifndef OPENNETSTREAM_P_H
#define OPENNETSTREAM_P_H

#include <QLibrary>
#include "opennetstream.h"

/**
 *  \brief 初始化库
 *  \param[in] szAuthAddr 认证域名
 *  \param[in] szPlatform 平台域名
 *  \param[in] szAppId    向平台申请的AppId
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_FiniLib()
 *
 *  \note
 *  该接口必须在调用其他接口之前调用
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_InitLib)(const char* szAuthAddr, const char* szPlatform, const char* szAppId);

/**
 *  \brief  反初始化库
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_InitLib()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_FiniLib)();

/** 
 *  \brief  设置登入传入参数
 *  \sa 
 */
typedef void (__stdcall *EZSDK_SetLoginParams)(LoginParamKey iKey, const char* szValue);

/** 
 *  \brief  获取AreaDomain, (海外不用区域才使用此接口)
 *  \return 返回平台地址
 *  \sa Login接口调用后, 再调用
 */
typedef const char* (__stdcall *EZSDK_GetLoginResponseParams)(LoginParamKey iKey);

/** 
 *  \brief  获取AreaDomain, (海外不用区域才使用此接口)
 *  \return 返回平台地址
 *  \sa Login接口调用后, 再调用
 */
typedef void (__stdcall *EZSDK_Logout)();

/**
 *  \brief 申请一个会话Id
 *  \param[in]  pHandle     设置回调函数， #OpenSDK_MessageHandler
 *  \param[in]  pUser       用户自定义数据，会在pHandle中原样抛出
 *  \param[out] pSession    用于接收分配的会话ID
 *  \param[out] iSessionLen pSession的长度
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_FreeSession()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_AllocSessionEx)(OpenSDK_MessageHandler pHandle, void* pUser, char** pSession, int* iSessionLen);

/**
 *  \brief 销毁SDK操作句柄
 *  \param[in] szSessionId 会话Id，通过OpenSDK_AllocSession()创建
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_AllocSession()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_FreeSession)(const char* szSessionId);

//针对session设置配置信息
typedef void (__stdcall *EZSDK_SetSessionConfig)(const char* szSessionId, ConfigKey iKey, const int iValue);

//切换清晰度
typedef OPENSDK_RESULT (__stdcall *EZSDK_SetVideoLevel)(const char* szDevSerial, const int iChannelNo, const int iVideoLevel);


/**
 *  \brief 预览
 *  \param[in] szSessionId   会话ID
 *  \param[in] hPlayWnd      播放窗口句柄
 *  \param[in] szCameraId    监控点ID
 *  \param[in] szAccessToken 认证token
 *  \param[in] iVideoLevel   视频质量，输入范围在0-2之间
 *  \param[in] szSafeKey	 视频加密密钥
 *  \param[in] szAppKey      从平台获取的AppKey
 *  \return 0表示成功，非0表示失败
 *  \sa OpenSDK_StopRealPlay()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_StartRealPlayEx)(const char* szSessionId, HWND hPlayWnd, const char* szDevSerial, const int iChannelNo, const char* szSafeKey);

/**
 *  \brief 停止预览
 *  \param[in] szSessionId 会话ID
 *  \return 0表示成功，非0表示失败
 *  \sa OpenSDK_StartRealPlay()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_StopRealPlayEx)(const char* szSessionId);

/**
 *  \brief 回放
 *  \param[in] szSessionId   会话Id
 *  \param[in] hPlayWnd      播放窗口句柄
 *  \param[in] szCameraId    监控点ID
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szSafeKey     视频加密密钥
 *  \param[in] szStartTime   开始时间
 *  \param[in] szStopTime    停止时间
 *  \param[in] szAppKey      从平台获取的AppKey
 *  \return 0表示成功，非0表示失败
 *  \sa OpenSDK_StopPlayBack()
 *  \note
 *  时间格式为：yyyy-MM-dd hh:mm:ss
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_StartPlayBackEx)(const char* szSessionId, HWND hPlayWnd, const char* szDevSerial, const int iChannelNo, const char* szSafeKey, const char* szStartTime, const char* szStopTime);

/**
 *  \brief 恢复回放
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，非0表示失败
 *  \sa OpenSDK_PlayBackPause()
 *  \note
 *  该接口只能在OpenSDK_StartPlayBack()调用之后才能调用
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_PlayBackResume)(const char* szSessionId);

/**
 *  \brief 暂停回放
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，非0表示失败
 *	\sa OpenSDK_PlayBackResume()
 *  \note
 *  该接口只能在OpenSDK_StartPlayBack() 调用之后才能调用
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_PlayBackPause)(const char* szSessionId);

/**
 *  \brief 停止回放
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，非0表示失败
 *	\sa OpenSDK_StartPlayBack()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_StopPlayBackEx)(const char* szSessionId);


typedef OPENSDK_RESULT (__stdcall *EZSDK_StartDownload)(const char* szSessionId, const char* szDevSerial, const int iChannelNo, const char* szSafeKey, const char* szStartTime, const char* szStopTime);

typedef OPENSDK_RESULT (__stdcall *EZSDK_StopDownload)(const char* szSessionId);





/**
 *  \brief 设置数据回调
 *  \param[in] szSessionId   会话ID
 *  \param[in] pDataCallBack 回调函数
 *  \param[in] pUser         用户自定义数据，会通过pDataCallBack原样抛出
 *  \return 0表示成功，-1表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_SetDataCallBack)(const char* szSessionId, OpenSDK_DataCallBack pDataCallBack, void* pUser);

/**
 *  \brief 录像搜索，包含云存储和设备SD卡录像
 *  \param[in] szSessionId   会话Id
 *  \param[in] szCameraId    监控点Id
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szStartTime   开始时间
 *  \param[in] szStopTime    停止时间
 *  \return 0表示成功，非0表示失败
 *  \note
 *  时间格式为：yyyy-MM-dd hh:mm:ss
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_StartSearchEx)(const char* szSessionId, const char* szDevSerial, const int iChannelNo, const char* szStartTime, const char* szStopTime);
/**
 *  \brief 获取OSD时间
 *  \param[in]  szSessionId 会话Id
 *  \param[out] pTime       OSD时间，格式见 #STREAM_TIME
 *  \return 0表示成功，-1表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_GetOSDTime)(const char* szSessionId, STREAM_TIME *pTime);

/**
 *  \brief 打开声音
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_CloseSound()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_OpenSound)(const char* szSessionId);

/**
 *  \brief 关闭声音
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_OpenSound()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_CloseSound)(const char* szSessionId);

/**
 *  \brief 获取音量
 *  \param[in] szSessionId 会话Id
 *  \return 小于0表示失败，其他值表示音量大小(0-100之间)
 *  \sa OpenSDK_SetVolume()
 */
typedef unsigned short (__stdcall *EZSDK_GetVolume)(const char* szSessionId);

/**
 *  \brief 设置音量
 *  \param[in] szSessionId 会话Id
 *  \param[in] uVolume     音量大小，0-100之间
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_GetVolume()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_SetVolume)(const char* szSessionId, unsigned short uVolume);

/**
 *  \brief 开启语音对讲
 *  \param[in] szSessionId   会话Id
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szCameraId    摄像头Id
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_StopVoiceTalk()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_StartVoiceTalkEx)(const char* szSessionId, const char* szDevSerial, const int iChannelNo);

/**
 *  \brief 结束语音对讲
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_StartVoiceTalk()
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_StopVoiceTalk)(const char* szSessionId);

/**
 *  \ingroup info
 *  \brief 截屏
 *  \param[in] szSessionId 会话Id
 *  \param[in] szFileName  图片保存路径，格式为JPG
 *  \return 0表示成功，-1表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_CapturePicture)(const char* szSessionId, const char* szFileName);

/**
 *  \brief 登陆接口
 *  \param[out] szAccessToken   认证Token
 *  \param[out] iAccessTokenLen 认证Token长度
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_Data_Free()，用于释放szAccessToken申请的内存
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_Mid_Login)(char** pAccessToken, int* iAccessTokenLen);

/**
 *  \brief 设备添加
 *  \param[in] szAccessToken 认证Token
 *  \return 0表示成功，-1表示失败
 */
typedef int (__stdcall *EZSDK_Mid_Device_Add)(const char* szStrAccessToken);

/**
 *  \brief 设备操作
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szDeviceId    设备Id
 *  \return 0表示成功，-1表示失败
 */
typedef int (__stdcall *EZSDK_Mid_Device_Oper)(const char* szStrAccessToken, const char* szDeviceId);

/**
 *  \brief 获取摄像头列表
 *  \param[in]  iPageStart    分页起始页，从0开始
 *  \param[in]  iPageSize     分页大小
 *  \param[out] pBuf          摄像头列表的JSON字符串
 *  \param[out] iLength       获取到的数据大小
 *  \return 0表示成功，-1表示失败
 *
 */


typedef OPENSDK_RESULT (__stdcall *EZSDK_Data_GetDevListEx)(int iPageStart, int iPageSize, void** pBuf, int* iLength);

typedef OPENSDK_RESULT (__stdcall *EZSDK_Data_GetSharedDevList)(int iPageStart, int iPageSize, void** pBuf, int* iLength);

/**
 *  \brief 获取单个设备信息
 *  \param[in] szDeviceSerial 设备序列号
 *  \param[out] pBuf          设备信息的JSON字符串
 *  \param[out] iLength       获取到的数据大小
 *  \return 0表示成功，非0表示失败
 *
 *  \note
 *  设备信息的JSON格式如下
 *  \code {.json}
 *	{
 *		"result": {
 *			"data": [
 *				{
 *					"deviceId": "", "deviceSerial": "", "cameraId": "", "cameraNo": 1, "cameraName": "", "status": 1, "isShared": "0", "picUrl": "", "isEncrypt": 0
 *				}
 *			],
 *			"code": "200",
 *			"msg": "操作成功!"
 *		}
 *	}
 *  \endcode
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_Data_GetDeviceInfo)(const char* szAccessToken, const char* szDeviceSerial, void** pBuf, int* iLength);


/**
 *  \brief 获取告警列表
 *  \param[in]  szAccessToken 认证Token
 *  \param[in]  szCameraId    监控点Id
 *  \param[in]  szStartTime   开始时间
 *  \param[in]  szEndTime     结束时间
 *  \param[in]  iAlarmType    告警类型，参见 #AlarmType
 *  \param[in]  iStatus       告警状态，0表示未读，1表示已读，2表示所有
 *  \param[in]  iPageStart    分页起始页，从0开始
 *  \param[in]  iPageSize     分页大小
 *  \param[out] pBuf		  告警信息列表
 *  \param[out] iLength       告警信息列表长度
 *  \return 0表示成功，-1表示失败
 *
 *  \note
 *  时间格式为：yyyy-MM-dd hh:mm:ss
 *
 *  \note
 *  告警信息列表JSON格式如下：
 *  \code {.json}
 *  {
 *		"resultCode":"200",
 *		"count":2,
 *		"alarmList":[
 *			{
 *				"alarmId":"", "objectName":"", "alarmType":"", "alarmStart":""
 *			}
 *		]
 *	}
 *  \endcode
 */

typedef OPENSDK_RESULT (__stdcall *EZSDK_Data_GetAlarmListEx)(const char* szDevSerial, const int iChannelNo, const char* szStartTime, const char* szEndTime, int iAlarmType, int iStatus, int iPageStart, int iPageSize, void** pBuf, int* iLength);

/**
 *  \brief 设置告警已读
 *  \param[in] szAlarmId 告警ID
 *  \return 0表示成功，非0表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_Data_SetAlarmRead)(const char* szAccessToken, const char* szAlarmId);

/**
 *  \brief 删除设备
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szDeviceId    设备Id
 *  \return 0表示成功，-1表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_Data_DeleteDevice)( const char* szAccessId, const char* szDeviceId);

/**
 *  \brief 销毁SDK分配的内存
 *  \param[in] pBuf SDK分配的内存
 *  \return 0表示成功，-1表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_Data_Free)(void* pBuf);

/**
 *  \brief 云台控制
 *  \param[in] szSessionId   会话ID
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szCameraId	 监控点Id
 *  \param[in] enCommand	 云台控制命令，参见 #PTZCommand
 *  \param[in] enAction		 云台操作命令，参见 #PTZAction
 *  \param[in] iSpeed		 速度0-7之间
 *  \return 0表示成功，-1表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_PTZCtrlEx)(const char* szSessionId, const char* szDevSerial, const int iChannelNo, PTZCommand enCommand, PTZAction enAction, int iSpeed);

/**
 *  \brief 按监控点ID进行布撤防
 *  \param[in] szSessionId   会话Id
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szCameraId    监控点Id
 *  \param[in] enType        布撤防类型
 *  \param[in] enStatus      布撤防状态
 *  \param[in] enActor		 布撤防设备类型
 *  \return 非负表示成功，-1表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_DevDefenceEx)(const char* szSessionId, const char* szDevSerial, const int iChannelNo, DefenceType enType, DefenceStatus enStatus, DefenceActor enActor);

/**
 *  \brief 获取AccessToken短信验证码
 *  \param[in]  szSignString 签名字符串
 *  \return 0表示成功，其他值表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_GetAccessTokenSmsCode)(const char* szSignString);

/**
 *  \brief 验证AccessToken短信验证码
 *  \param[in] szSmsCode     短信验证码
 *  \param[in] szUserId      用户Id
 *  \param[in] szPhoneNumber 用户手机号码
 *  \param[in] szAppKey      从平台获取的AppKey
 *  \return 0表示成功，其他值表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_VerifyAccessTokenSmsCode)(const char* szSmsCode, const char* szUserId, const char* szPhoneNumber, const char* szAppKey);

/**
 *  \brief 获取硬件特征码短信验证码
 *  \param[in]  szAccessToken 认证Token
 *  \param[in]  szSignString  签名字符串
 *  \return 0表示成功，其他值表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_GetHdSignSmsCode)(const char* szAccessToken, const char* szSignString);

/**
 *  \brief 验证硬件特征码短信验证码
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szSmsCode     短信验证码
 *  \param[in] szUserId      用户Id
 *  \param[in] szAppKey      从平台获取的AppKey
 *  \return 0表示成功，其他值表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_VerifyHdSignSmsCode)(const char* szAccessToken, const char* szSmsCode, const char* szUserId, const char* szAppKey);

/** 
 *  \brief 更新摄像头信息到缓存
 *  \param[in] szCameraId		监控点ID
 *  \param[in] szAccessToken	认证Token
 *  \return 0表示成功，其他值表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_UpdateCameraInfoToLocal)(const char* szCameraId, const char* szAccessToken, bool& isEncrypt);

/** 
 *  \brief 获取设备详情监控点详细信息
 *  \param[in] szCameraId		监控点ID
 *  \param[in] szAccessToken	认证Token
 *	\param[out] pCameraInfo		监控点信息，需要调用OpenSDK_FreeData接口释放
 *	\param[out] iLength			监控点信息的长度
 *
 *  \note 
 *  告警信息列表JSON格式如下：
 *  \code {.json}
 *  {
 *		"cameraID":"",			// 监控点ID
 *		"devSerial":"",			// 设备序列号
 *		"channelNo":"",			// 通道号
 *		"videoLevel":""			// 视频质量 2-高清 1-标清 0-流畅
 *		"type":""				// 1 网络录像机 2 模拟录像机
 *		"capability":""			// 2-2-1 支持低中高，低中为子码流，高为主码流
 *	}
 *  \endcode
 *  \return 0表示成功，其他值表示失败
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_GetCameraInfo)(const char* szCameraId, const char* szAccessToken, void** pCameraInfo, int* iLength);

typedef OPENSDK_RESULT (__stdcall *EZSDK_Data_GetDevDetailInfo)(const char* szDevSerial, const int iChannelNo, const bool bUpdate, void** pDevDetailInfo, int* iLength);

typedef OPENSDK_RESULT (__stdcall* EZSDK_HttpSendWithWait)(const char* szUri, const char* szHeaderParam, const char* szBody, char** pBuf, int* iLength);


typedef OPENSDK_RESULT (__stdcall *EZSDK_Push_SetAlarmCallBack)(OpenSDK_Push_MessageHandler pHandle, void* pUser);
typedef OPENSDK_RESULT (__stdcall *EZSDK_Push_SetDeviceStatusCallBack)(OpenSDK_Push_MessageHandler pHandle, void* pUser);
typedef OPENSDK_RESULT (__stdcall *EZSDK_Push_SetTransparentChannelCallBack)(OpenSDK_Push_MessageHandler pHandle, void* pUser);

typedef OPENSDK_RESULT (__stdcall *EZSDK_Push_StartRecvEx)(const char* szPushSecret);
typedef OPENSDK_RESULT (__stdcall *EZSDK_Push_StopRecv)();
typedef OPENSDK_RESULT (__stdcall *EZSDK_DecryptPicture)(const char* szAccessToken, const char* szPicURL, const char* szSerail, const char* szSafeKey, void** pPicBuf, int* iPicLen);

typedef OPENSDK_RESULT (__stdcall *EZSDK_SetAccessToken)(const char* szAccessToken);

typedef void (__stdcall *EZSDK_SetConfigInfo)(ConfigKey iKey, const int iValue);

/** 
 *  \brief 获取错误码接口
 *  \return 错误码
 *  \sa OpenSDK_GetLastErrorDesc
 */
typedef OPENSDK_RESULT (__stdcall *EZSDK_GetLastErrorCode)();

/** 
 *  \brief 获取错误描述
 *  \return 错误码描述
 *  \sa OpenSDK_GetLastErrorCode
 */
typedef const char* (__stdcall *EZSDK_GetLastErrorDesc)();

struct tagNETSTREAMAPI
{
    EZSDK_InitLib                 pOpenSDK_InitLib;
    EZSDK_FiniLib                 pOpenSDK_FiniLib;
    EZSDK_SetLoginParams          pOpenSDK_SetLoginParams;
    EZSDK_GetLoginResponseParams  pOpenSDK_GetLoginResponseParams;
    EZSDK_Logout                  pOpenSDK_Logout;
    EZSDK_AllocSessionEx          pOpenSDK_AllocSessionEx;
    EZSDK_FreeSession             pOpenSDK_FreeSession;
    EZSDK_SetSessionConfig        pOpenSDK_SetSessionConfig;
    EZSDK_SetVideoLevel           pOpenSDK_setVideoLevel;
    EZSDK_StartRealPlayEx         pOpenSDK_startRealPlay;
    EZSDK_StopRealPlayEx          pOpenSDK_stopRealPlay;
    EZSDK_StartPlayBackEx         pOpenSDK_startPlayBack;
    EZSDK_PlayBackResume          pOpenSDK_PlayBackResume;
    EZSDK_PlayBackPause           pOpenSDK_PlayBackPause;
    EZSDK_StopPlayBackEx          pOpenSDK_stopPlayBack;
    EZSDK_StartDownload           pOpenSDK_StartDownload;
    EZSDK_StopDownload            pOpenSDK_StopDownload;
    EZSDK_SetDataCallBack         pOpenSDK_SetDataCallBack;
    EZSDK_StartSearchEx           pOpenSDK_startSearch;
    EZSDK_GetOSDTime              pOpenSDK_GetOSDTime;

    EZSDK_OpenSound               pOpenSDK_OpenSound;
    EZSDK_CloseSound              pOpenSDK_CloseSound;
    EZSDK_GetVolume               pOpenSDK_GetVolume;
    EZSDK_SetVolume               pOpenSDK_SetVolume;
    EZSDK_StartVoiceTalkEx        pOpenSDK_StartVoiceTalk;
    EZSDK_StopVoiceTalk           pOpenSDK_StopVoiceTalk;
    EZSDK_CapturePicture          pOpenSDK_CapturePicture;

    EZSDK_Mid_Login               pOpenSDK_Mid_Login;
    EZSDK_Mid_Device_Add          pOpenSDK_Mid_Device_Add;
    EZSDK_Mid_Device_Oper         pOpenSDK_Mid_Device_Oper;

    EZSDK_Data_GetDevListEx       pOpenSDK_Data_GetDevListEx;
    EZSDK_Data_GetSharedDevList   pOpenSDK_Data_GetSharedDevList;
    EZSDK_Data_GetDeviceInfo      pOpenSDK_Data_GetDeviceInfo;
    EZSDK_Data_GetAlarmListEx     pOpenSDK_Data_GetAlarmListEx;
    EZSDK_Data_SetAlarmRead       pOpenSDK_Data_SetAlarmRead;
    EZSDK_Data_DeleteDevice       pOpenSDK_Data_DeleteDevice;
    EZSDK_Data_Free               pOpenSDK_Data_Free;

    EZSDK_PTZCtrlEx                   pOpenSDK_PTZCtrlEx;
    
    EZSDK_DevDefenceEx                pOpenSDK_DevDefenceEx;

    EZSDK_GetAccessTokenSmsCode       pOpenSDK_GetAccessTokenSmsCode;
    EZSDK_VerifyAccessTokenSmsCode    pOpenSDK_VerifyAccessTokenSmsCode;
    EZSDK_GetHdSignSmsCode            pOpenSDK_GetHdSignSmsCode;
    EZSDK_VerifyHdSignSmsCode         pOpenSDK_VerifyHdSignSmsCode;
    EZSDK_Data_GetDevDetailInfo       pOpenSDK_Data_GetDevDetailInfo;

	EZSDK_Push_SetAlarmCallBack		        pOpenSDK_Push_SetAlarmCallBack;
	EZSDK_Push_SetDeviceStatusCallBack        pOpenSDK_Push_SetDeviceStatusCallBack;
	EZSDK_Push_SetTransparentChannelCallBack  pOpenSDK_Push_SetTransparentChannelCallBack;
    EZSDK_Push_StartRecvEx                    pOpenSDK_Push_StartRecvEx;

	EZSDK_Push_StopRecv					    pOpenSDK_Push_StopRecv;
	EZSDK_DecryptPicture					    pOpenSDK_DecryptPicture;
	EZSDK_HttpSendWithWait				    pOpenSDK_HttpSendWithWait;

    EZSDK_SetAccessToken                      pOpenSDK_SetAccessToken;
    EZSDK_SetConfigInfo                       pOpenSDK_SetConfigInfo;

	EZSDK_GetLastErrorCode			        pOpenSDK_GetLastErrorCode;
	EZSDK_GetLastErrorDesc			        pOpenSDK_GetLastErrorDesc;
};

class OpenNetStreamPrivate
{
    Q_DECLARE_PUBLIC(OpenNetStream)

public:
    OpenNetStreamPrivate(OpenNetStream* parent);
    OpenNetStream* const q_ptr;

    void init();

    tagNETSTREAMAPI m_apis;
    QLibrary* m_library;
};

#endif // OPENNETSTREAM_P_H
