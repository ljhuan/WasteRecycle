/** \file      OpenNetStreamInterFace.h
 *  \copyright HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *  \brief     开放SDK接口定义
 *
 *  \author    panlong, peter
 *  \date      2014/12/15
 *
 *  \note      history:
 *  \note      V0.0.1 2014/12/15 peter 修改文件注释 
 */

#ifndef _H_OPENNETSTREAMINTERFACE_H_
#define _H_OPENNETSTREAMINTERFACE_H_

#include "OpenNetStreamDefine.h"
#include "OpenNetStreamError.h"

#if defined (_WIN32) || defined(_WIN64)
#include <Windows.h>
#ifdef OPENNETSTREAM_EXPORTS
#  define OPENSDK_API __declspec(dllexport)
#else
#  define OPENSDK_API __declspec(dllimport)
#endif
#elif defined (OS_POSIX) || defined (__APPLE__) || defined(ANDROID) || defined (__linux__)
#include <cstdio>
#  define OPENSDK_API
#  define CALLBACK

#if defined(__linux__)
typedef unsigned int HWND;
#else
typedef void* HWND;
#endif

#else
#  error os not support!
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** 
 *  \ingroup alloc
 *  \brief 信息回调
 *  \param szSessionId  申请的会话ID
 *  \param iMsgType     消息号 #MessageType
 *	\param iErrorCode   错误码 OpenNetStreamError.h
 *  \param pMessageInfo 信息
 *  \param pUser		用户自定义数据 
 *
 *  \note  
 *	涉及的功能接口包括预览、回放、回放查询、布撤防、云台控制、语音对讲
 *  比方回放查询接口，当iMsgType == INS_RECORD_FILE时，pMessageInfo的格式如下
 *  \code {.json}
 *  {
 *		"FileSize":10, 
 *		"FileList": [
 *			{
 *				"StartTime":"",
 *				"EndTime":""
 *			}
 *		]
 *	}
 *  \endcode
 *  
 *  \sa OpenSDK_AllocSession()
 */
typedef void (CALLBACK *OpenSDK_MessageHandler)(const char* szSessionId, unsigned int iMsgType, unsigned int iErrorCode,const char *pMessageInfo, void *pUser);

/** \defgroup init 开放SDK初始化
 *  @{
 */

/** 
 *  \brief 初始化库, 支持配置平台地址
 *  \param[in] szAuthAddr 认证域名:开放平台认证中心地址，默认地址为：https://openauth.ys7.com .对于开发者而言，请填写此默认地址即可. 
 *  \param[in] szPlatform 平台域名:开放平台后台地址，默认地址为：https://open.ys7.com .对于开发者而言，请填写此默认地址即可.
 *  \param[in] szAppId    向平台申请的AppKey
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_FiniLib()
 *
 *  \note
 *  该接口必须在调用其他接口之前调用
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_InitLib(const char* szAuthAddr, const char* szPlatform, const char* szAppId);


/** 
 *  \brief 初始化库, 默认国内版本使用的接口
 *  \param[in] szAppId    向平台申请的AppKey
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_FiniLib()
 *
 *  \note
 *  该接口必须在调用其他接口之前调用
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Init(const char* szAppId);

/** 
 *  \brief  反初始化库
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_InitLib()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_FiniLib();

/** 
 *  \brief  全局配置信息, 根据ConfigKey进行配置
 *  \param[in] iKey    ConfigKey, 配置类型
 *  \param[in] iValue  配置数值, 如果key=CONFIG_DATA_UTF8, value=1时表示数据输出为Utf8格式
                                如果key=CONFIG_OPEN_STREAMTRANS, value=1时表示视频码流经过转封装库输出,用于录像
                                如果key=CONFIG_CLOSE_P2P, value=1时表示关闭P2P功能
 *  \return
 *  \sa 
 */
OPENSDK_API void CALLBACK OpenSDK_SetConfigInfo(ConfigKey iKey, const int iValue);

/** 
 *  \brief  设置平台地址,海外平台地址重定向后, 通过调用此接口重新设置平台地址.
 *  \sa 
 */
OPENSDK_API void CALLBACK OpenSDK_SetPlatformAddr(const char* szPlatform);

/** 
 *  \brief  设置
 *  \return 0表示成功，-1表示失败
 *  \sa 
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_SetAppID(const char* szAppId);

/** 
 *  \brief  设置AccessToken
 *  \return 0表示成功，-1表示失败
 *  \sa 
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_SetAccessToken(const char* szAccessToken);


/** @} */ // end of init

/** \defgroup alloc 分配会话
 *  @{
 */

/** 
 *  \brief 申请一个会话Id
 *  \param[in]  pHandle     设置回调函数， #OpenSDK_MessageHandler
 *  \param[in]  pUser       用户自定义数据，会在pHandle中原样抛出
 *  \param[out] pSession    用于接收分配的会话ID
 *  \param[out] iSessionLen pSession的长度
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_FreeSession()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_AllocSessionEx(OpenSDK_MessageHandler pHandle, void* pUser, char** pSession, int* iSessionLen);

/** 
 *  \brief 销毁SDK操作句柄
 *  \param[in] szSessionId 会话Id，通过OpenSDK_AllocSession()创建
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_AllocSession()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_FreeSession(const char* szSessionId);

/** 
 *  \brief  针对Session设置配置信息, 根据ConfigKey进行配置, 取流之前调用
 *  \param[in] iKey    ConfigKey, 配置类型
 *  \param[in] iValue  配置数值, 如果key=CONFIG_OPEN_STREAMTRANS, value=1时表示视频码流经过转封装库输出,用于录像
 *  \return
 *  \sa 
 */
OPENSDK_API void CALLBACK OpenSDK_SetSessionConfig(const char* szSessionId, ConfigKey iKey, const int iValue);

/** @} */ // end of alloc

/** \defgroup play 预览接口
 *  @{
 */

/** 
 *  \brief 数据回调格式
 *  \param enType 数据类型，参见 #DataType
 *  \param pData  数据内容
 *  \param iLen   数据长度
 *  \param pUser  用户自定义数据
 */
typedef void (CALLBACK *OpenSDK_DataCallBack)(DataType enType, char* const pData, int iLen, void* pUser);

/** 
 *  \brief 设置数据回调
 *  \param[in] szSessionId   会话ID
 *  \param[in] pDataCallBack 回调函数
 *  \param[in] pUser         用户自定义数据，会通过pDataCallBack原样抛出
 *  \return 0表示成功，-1表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_SetDataCallBack(const char* szSessionId, OpenSDK_DataCallBack pDataCallBack, void* pUser);

/** 
 *  \brief 设置或者切换清晰度, 根据OpenSDK_Data_GetDevDetailInfo接口返回信息videoQualityInfos来判断是否支持对应的清晰度类型
 *  \param[in] szDevSerial   设备序列号
 *  \param[in] iChannelNo    设备通道号
 *  \param[in] iVideoLevel   视频质量，输入范围在0-3之间, 0-流畅; 1-均衡; 2-高清; 3-超清
 *  \return 0表示成功，非0表示失败
 *  \sa OpenSDK_StopRealPlay()
 *	\sa OpenSDK_SetDataCallBack
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_SetVideoLevel(const char* szDevSerial, const int iChannelNo, const int iVideoLevel);

/** 
 *  \brief 开始播放
           异步接口，返回值只是表示操作成功，不代表播放成功
           如果接口返回-1, 结果根据消息回调函数msgid判断, 若msgid=INS_PLAY_START, 表示成功, 若msgid=INS_PLAY_EXCEPTION, 表示失败
 *  \param[in] szSessionId   会话ID
 *  \param[in] hPlayWnd      播放窗口句柄, 如果窗口句柄为NULL表示纯粹取流，不做播放
 *  \param[in] szDevSerial   设备序列号
 *  \param[in] iChannelNo    设备通道号
 *  \param[in] szSafeKey	 视频加密密钥,如果视频未加密, 可以设置为NULL 
 *  \return 0表示成功，非0表示失败
 *  \sa OpenSDK_stopRealPlay
 *	\sa OpenSDK_SetDataCallBack
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_StartRealPlayEx(const char* szSessionId, HWND hPlayWnd, const char* szDevSerial, const int iChannelNo, const char* szSafeKey);

/** 
 *  \brief 停止播放播放
           异步接口，返回值只是表示操作成功，不代表停止播放成功
           如果接口返回-1, 结果根据消息回调函数msgid判断, 若msgid=INS_PLAY_STOP, 表示成功
 *  \param[in] szSessionId 会话ID
 *  \return 0表示成功，非0表示失败
 *  \sa OpenSDK_StartRealPlay()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_StopRealPlayEx(const char* szSessionId);

/** @} */ // end of play

/** \defgroup playback 回放接口
 *  @{
 */

/** 
 *  \brief 录像搜索，包含云存储和设备SD卡录像, 如果关联NVR优先搜索NVR录像
           异步接口，返回值只是表示操作成功，不代表搜索成功
           如果接口返回-1, 结果根据消息回调函数msgid判断, 若msgid=INS_RECORD_SEARCH_END, 表示成功, 若msgid= INS_RECORD_SEARCH_FAILED, 表示失败
 *  \param[in] szSessionId   会话Id
 *  \param[in] szDevSerial   设备序列号
 *  \param[in] iChannelNo    设备通道号
 *  \param[in] szStartTime   开始时间
 *  \param[in] szStopTime    停止时间 
 *  \return 0表示成功，-1表示失败 
 *  \note
 *  时间格式为：yyyy-MM-dd hh:mm:ss
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_StartSearchEx(const char* szSessionId, const char* szDevSerial, const int iChannelNo, const char* szStartTime, const char* szStopTime);

/** 
*  \brief 开始回放
          异步接口，返回值只是表示操作成功，不代表播放成功
          如果接口返回-1, 结果根据消息回调函数msgid判断, 若msgid=INS_PLAY_START, 表示成功, 若msgid=INS_PLAY_EXCEPTION, 表示失败
 *  \param[in] szSessionId   会话Id
 *  \param[in] hPlayWnd      播放窗口句柄, 如果窗口句柄为NULL表示纯粹取流，不做播放
 *  \param[in] szDevSerial   设备序列号
 *  \param[in] iChannelNo    设备通道号
 *  \param[in] szSafeKey     视频加密密钥
 *  \param[in] szStartTime   开始时间
 *  \param[in] szStopTime    停止时间
 *  \return 0表示成功，非0表示失败
 *  \sa OpenSDK_StopPlayBack()
 *  \note 
 *  时间格式为：yyyy-MM-dd hh:mm:ss
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_StartPlayBackEx(const char* szSessionId, HWND hPlayWnd, const char* szDevSerial, const int iChannelNo, const char* szSafeKey, const char* szStartTime, const char* szStopTime);

/** 
 *  \brief 恢复回放
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，非0表示失败
 *  \sa OpenSDK_PlayBackPause()
 *  \note
 *  该接口只能在OpenSDK_StartPlayBack()调用之后才能调用
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_PlayBackResume(const char* szSessionId);

/** 
 *  \brief 暂停回放
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，非0表示失败
 *	\sa OpenSDK_PlayBackResume()
 *  \note 
 *  该接口只能在OpenSDK_StartPlayBack() 调用之后才能调用
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_PlayBackPause(const char* szSessionId);

/** 
 *  \brief 停止回放
		   异步接口，返回值只是表示操作成功，不代表停止播放成功
           如果接口返回-1, 结果根据消息回调函数msgid判断, 若msgid=INS_PLAY_STOP, 表示成功
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，非0表示失败
 *	\sa OpenSDK_StartPlayBack()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_StopPlayBackEx(const char* szSessionId);

/** @} */ // end of playback

/** \defgroup info 视频信息获取
 *  @{
 */

/** 
 *  \brief 获取OSD时间
 *  \param[in]  szSessionId 会话Id
 *  \param[out] pTime       OSD时间，格式见 #STREAM_TIME 
 *  \return 0表示成功，-1表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_GetOSDTime(const char* szSessionId, STREAM_TIME *pTime);

/** @} */ // end of info

/** \defgroup sound 音频相关接口
 *  @{
 */

/** 
 *  \brief 打开声音
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_CloseSound()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_OpenSound(const char* szSessionId);

/** 
 *  \brief 关闭声音
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_OpenSound()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_CloseSound(const char* szSessionId);

/**
 *  \brief 获取音量
 *  \param[in] szSessionId 会话Id
 *  \return 小于0表示失败，其他值表示音量大小(0-100之间)
 *  \sa OpenSDK_SetVolume()
 */
OPENSDK_API unsigned short CALLBACK OpenSDK_GetVolume(const char* szSessionId);

/** 
 *  \brief 设置音量
 *  \param[in] szSessionId 会话Id
 *  \param[in] uVolume     音量大小，0-100之间
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_GetVolume()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_SetVolume(const char* szSessionId, unsigned short uVolume);

/** @} */ // end of sound

/** \defgroup voice_talk 语音对讲
 *  @{
 */

/** 
 *  \brief 开启语音对讲, 不支持多个设备同时对接
           异步接口，返回值只是表示操作成功，不代表对讲开启成功
           如果接口返回-1, 结果根据消息回调函数msgid判断, 若msgid=INS_VOICETALK_START, 表示成功, 若msgid=INS_VOICETALK_STOP, 表示失败
 *  \param[in] szSessionId   会话Id
 *  \param[in] szDevSerial   设备序列号
 *  \param[in] iChannelNo    设备通道号
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_StopVoiceTalk()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_StartVoiceTalkEx(const char* szSessionId, const char* szDevSerial, const int iChannelNo);

/** 
 *  \brief 结束语音对讲
 *  \param[in] szSessionId 会话Id
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_StartVoiceTalk()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_StopVoiceTalk(const char* szSessionId);

/** @} */ // end of voice_talk

/** 
 *  \brief 截屏，StartRealPlay成功（回调函数中收到MsgId == INS_PLAY_START）情况下使用
 *  \param[in] szSessionId 会话Id
 *  \param[in] szFileName  图片保存路径，格式为JPG,需要为UTF-8格式
 *  \return 0表示成功，-1表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_CapturePicture(const char* szSessionId, const char* szFileName);

/** \defgroup midpage 中间页
 *  @{
 */

/** 
 *  \brief  设置登入传入参数
 *  \param[in] iKey        传入参数类型, LOGIN_AREAID 或者 LOGIN_VER
 *  \param[in] szValue     Key对应参数参数内容
 *  \sa 
 */
OPENSDK_API void CALLBACK OpenSDK_SetLoginParams(LoginParamKey iKey, const char* szValue);

/** 
 *  \brief  获取登入后返回参数
 *  \param[in] iKey        传入参数类型, LOGIN_AREADOMAIN 或者 LOGIN_TOKEN_EXPIRETIME 或者 LOGIN_ACCESS_TOKEN
 *  \param[in] szValue     Key对应参数参数内容
 *  \sa OpenSDK_Mid_Login之后调用
 */
OPENSDK_API const char* CALLBACK OpenSDK_GetLoginResponseParams(LoginParamKey iKey);


/** 
 *  \brief  注销接口, 同OpenSDK_Mid_Login接口配对使用
 *  \sa OpenSDK_Mid_Login之后调用
 */
OPENSDK_API void CALLBACK OpenSDK_Logout();

/** 
 *  \brief 登陆接口
 *  \param[out] szAccessToken   认证Token
 *  \param[out] iAccessTokenLen 认证Token长度
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_Data_Free()，用于释放szAccessToken申请的内存
 */
OPENSDK_API int CALLBACK OpenSDK_Mid_Login( char** szAccessToken, int* iAccessTokenLen );

/** 
 *  \brief 设备添加
 *  \param[in] szAccessToken 认证Token
 *  \return 0表示成功，非0表示失败
 */
OPENSDK_API int CALLBACK OpenSDK_Mid_Device_Add(const char* szAccessToken);

/** 
 *  \brief 设备操作
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szDeviceId    设备Id
 *  \return 0表示成功，-1表示失败
 */
OPENSDK_API int CALLBACK OpenSDK_Mid_Device_Oper(const char* szAccessToken, const char* szDeviceId);

/** @} */ // end of midpage

/** \defgroup data 数据接口
 *  @{
 */

/** 
 *  \brief 获取摄像头列表
 *  \param[in]  iPageStart    分页起始页，从0开始
 *  \param[in]  iPageSize     分页大小, 限制最大1000,默认100, 建议不超过100
 *  \param[out] pBuf          摄像头列表的JSON字符串
 *  \param[out] iLength       获取到的数据大小
 *  \return 0表示成功，-1表示失败 
 *
 *  \note 
 *  摄像头列表的JSON格式如下
 *  \code {.json}
 *{
 *  "page" : {
 *     "page" : 0,                                             //页码
 *     "size" : 20,                                            //页大小
 *     "total" : 1                                             //账号下所有设备数量
 *  },
 *  "data" : [
 *     {
 *        "addTime" : 1512652274000.0,                          //设备被用户添加时间，精确到毫秒
 *        "alarmSoundMode" : 0,                                 //告警声音模式
 *  	  "cameraNum" : 2,                                      //视频数
 *        "category" : "UNKNOWN",                               //设备大类
 *        "defence" : 0,                                        //布A1设备布撤防状态，0:睡眠 8:在家 16:外出, 非A1设备，0-撤防 1-布防
 *        "detectorNum" : 0,                                    //探测器数
 *        "detectorInfo" : null,                                //探测器信息
 *        "deviceCover" : "https://i.ys7.com/image/DVR/1.jpeg", //设备封面
 *        "deviceName" : "DS-7104N-SN/C(129103497)",            //设备名称
 *        "deviceSerial" : "129103497",                         //设备序列号
 *        "deviceType" : "DS-7104N-SN/C",                       //设备类型
 *        "deviceVersion" : "V3.0.21 build 170417",             //设备版本号
 *        "isEncrypt" : 0,                                      //是否加密，0：不加密，1：加密
 *        "status" : 2,                                         //在线状态，1-在线，2-不在线
 *        "supportExtShort" : "1|1|1|1|0|0|0|1|1|0|0|-1|0...",  //设备能力集
 *        "cameraInfo" : [                                      //视频信息
 *           {
 *              "cameraCover" : "https://i.ys7.com/...",        //通道封面
 *              "cameraName" : "视频1@DS-7104N-SN/C(129103497)",//视频名camera
 *              "cameraNo" : 1,                                 //视频号
 *              "deviceSerial" : "129103497",                   //设备序列号
 *              "isShared" : "0",                               //分享状态，1：分享所有者，0：未分享，2：分享接受者（表示此摄像头是别人分享给我的）
 *              "videoLevel" : 0,                               //视频质量，0-流畅，1-均衡，2-高清，3-超清
 *              "videoQualityInfos" : [
 *                 {
 *                    "streamType" : 2,
 *                    "videoLevel" : 0,
 *                    "videoQualityName" : "流畅"
 *                 },
 *                 {
 *                    "streamType" : 1,
 *                    "videoLevel" : 2,
 *                    "videoQualityName" : "高清"
 *                 }
 *              ]
 *           },
 *           {
 *              "cameraCover" : "https://i.ys7.com/assets/imgs/public/homeDevice.jpeg",
 *              "cameraName" : "视频2@DS-7104N-SN/C(129103497)",
 *              "cameraNo" : 2,
 *              "deviceSerial" : "129103497",
 *              "isShared" : "0",
 *              "videoLevel" : 0,
 *              "videoQualityInfos" : [
 *                 {
 *                    "streamType" : 2,
 *                    "videoLevel" : 0,
 *                    "videoQualityName" : "流畅"
 *                 },
 *                 {
 *                    "streamType" : 1,
 *                    "videoLevel" : 2,
 *                    "videoQualityName" : "高清"
 *                 }
 *              ]
 *           }
 *        ]
 *     }
 *  ]
 *}
*  \endcode
*/
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Data_GetDevListEx(int iPageStart, int iPageSize, void** pBuf, int* iLength);

/** 
 *  \brief 获取他人分享的摄像头列表
 *  \param[in]  iPageStart    分页起始页，从0开始
 *  \param[in]  iPageSize     分页大小, 限制最大1000,默认100, 建议不超过100
 *  \param[out] pBuf          摄像头列表的JSON字符串
 *  \param[out] iLength       获取到的数据大小
 *  \return 0表示成功，-1表示失败 
 *
 *  \note 
 *  摄像头列表的JSON格式如下
 *  \code {.json}
 *{
 *  "page" : {
 *     "page" : 0,                                             //页码
 *     "size" : 20,                                            //页大小
 *     "total" : 1                                             //账号下所有设备数量
 *  },
 *  "data" : [
 *     {
 *        "addTime" : 1512652274000.0,                          //设备被用户添加时间，精确到毫秒
 *        "alarmSoundMode" : 0,                                 //告警声音模式
 *  	  "cameraNum" : 2,                                      //视频数
 *        "category" : "UNKNOWN",                               //设备大类
 *        "defence" : 0,                                        //布A1设备布撤防状态，0:睡眠 8:在家 16:外出, 非A1设备，0-撤防 1-布防
 *        "detectorNum" : 0,                                    //探测器数
 *        "detectorInfo" : null,                                //探测器信息
 *        "deviceCover" : "https://i.ys7.com/image/DVR/1.jpeg", //设备封面
 *        "deviceName" : "DS-7104N-SN/C(129103497)",            //设备名称
 *        "deviceSerial" : "129103497",                         //设备序列号
 *        "deviceType" : "DS-7104N-SN/C",                       //设备类型
 *        "deviceVersion" : "V3.0.21 build 170417",             //设备版本号
 *        "isEncrypt" : 0,                                      //是否加密，0：不加密，1：加密
 *        "status" : 2,                                         //在线状态，1-在线，2-不在线
 *        "supportExtShort" : "1|1|1|1|0|0|0|1|1|0|0|-1|0...",  //设备能力集
 *        "cameraInfo" : [                                      //视频信息
 *           {
 *              "cameraCover" : "https://i.ys7.com/...",        //通道封面
 *              "cameraName" : "视频1@DS-7104N-SN/C(129103497)",//视频名camera
 *              "cameraNo" : 1,                                 //视频号
 *              "deviceSerial" : "129103497",                   //设备序列号
 *              "isShared" : "0",                               //分享状态，1：分享所有者，0：未分享，2：分享接受者（表示此摄像头是别人分享给我的）
 *              "videoLevel" : 0,                               //视频质量，0-流畅，1-均衡，2-高清，3-超清
 *              "videoQualityInfos" : [
 *                 {
 *                    "streamType" : 2,
 *                    "videoLevel" : 0,
 *                    "videoQualityName" : "流畅"
 *                 },
 *                 {
 *                    "streamType" : 1,
 *                    "videoLevel" : 2,
 *                    "videoQualityName" : "高清"
 *                 }
 *              ]
 *           },
 *           {
 *              "cameraCover" : "https://i.ys7.com/assets/imgs/public/homeDevice.jpeg",
 *              "cameraName" : "视频2@DS-7104N-SN/C(129103497)",
 *              "cameraNo" : 2,
 *              "deviceSerial" : "129103497",
 *              "isShared" : "0",
 *              "videoLevel" : 0,
 *              "videoQualityInfos" : [
 *                 {
 *                    "streamType" : 2,
 *                    "videoLevel" : 0,
 *                    "videoQualityName" : "流畅"
 *                 },
 *                 {
 *                    "streamType" : 1,
 *                    "videoLevel" : 2,
 *                    "videoQualityName" : "高清"
 *                 }
 *              ]
 *           }
 *        ]
 *     }
 *  ]
 *}
*  \endcode
*/
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Data_GetSharedDevList(int iPageStart, int iPageSize, void** pBuf, int* iLength);

/** 
 *  \brief 获取设备详细信息, 包括预览能力级, PTZ, 对讲能力级等.
 *  \param[in]  szDevSerial     设备序列号
 *  \param[in]  iChannelNo      设备通道号
 *  \param[in]  bUpdate         是否重新获取设备详细信息, true:重新从萤石平台拿数据, 否则拿缓存信息. true的时机为用户重新登录或者需要刷新设备信息.
 *	\param[out] pDevDetailInfo	监控点信息，需要调用OpenSDK_FreeData接口释放
 *	\param[out] iLength			监控点信息的长度
 *
 *  \note 
 *  告警信息列表JSON格式如下：
 *  \code {.json}
 *  {
 *		"devSerial":"426931084",	        // 设备序列号
 *		"channelNo":1,				        // 通道号
 *		"videoLevel":2,				        // 视频质量 2-高清 1-标清 0-流畅
 *		"type":1,					        // 1 网络录像机 2 模拟录像机
 *		"capability":"2-2-1",		        // 预览能力级, 2-2-1 三挡分别对应流畅,均衡,高清, 1:主码流, 2:子码流, 0:不支持
 *		"isEncrypt":0,				        // 是否开启安全模式:1-开启,0-关闭, 即视频图像是否加密
 *      "support_talk":1,                   // 是否支持对讲:1-支持, 0-不支持
 *      "support_ptz":0,                    // 是否支持云台控制:1-支持, 0-不支持
 *      "ptz_top_bottom":1,                 // 是否支持云台上下:1-支持, 0-不支持
 *      "ptz_left_right":1                  // 是否支持云台左右:1-支持, 0-不支持
 *      "ptz_preset":1,                     // 是否支持云台预置点:1-支持, 0-不支持
 *      "videoQualityInfos": [              // 设备支持的预览能力级详情
 *          {
 *              "videoQualityName":"流畅",  // 清晰度名称
 *              "videoLevel":0,             // 清晰度数值
 *              "streamType":2              // 主子码流类型
 *          },
 *          {
 *              "videoQualityName":"均衡",
 *              "videoLevel":1,
 *              "streamType":2
 *          },
 *          {
 *              "videoQualityName":"高清",
 *              "videoLevel":2,
 *              "streamType":1
 *          }
 *      ]
 *	}
 *  \endcode
 *  \return 0表示成功，其他值表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Data_GetDevDetailInfo(const char* szDevSerial, const int iChannelNo, const bool bUpdate, void** pDevDetailInfo, int* iLength);

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
 *					"deviceId": "",						// 设备在平台的唯一标识
 *					"deviceSerial": "421877673",		// 设备序列号
 *					"cameraId": "",						// 监控点ID
 *					"cameraNo": 1,						// 设备通道号
 *					"cameraName": "",					// 监控点名称
 *					"status": 1,						// 监控点在线状态, 0-离线; 1-在线
 *					"isShared": "0",					// 设备分享状态, 0-未共享;1-共享所有者;2-共享接受者
 *					"picUrl": "",						// 监控点图片url
 *					"isEncrypt": 0						// 加密状态, 0-未加密;1-加密
 *					"defence" : 1,						// 布撤防状态, 0-未布防; 1-布防
 *					"videoLevel" : 0					// 视频清晰度, 0-流畅; 1-均衡; 2-高清
 *				}
 *			],
 *			"code": "200",
 *			"msg": "操作成功!"
 *		}
 *	}
 *  \endcode
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Data_GetDeviceInfo(const char* szAccessToken, const char* szDeviceSerial, void** pBuf, int* iLength);

/** 
 *  \brief 获取告警列表
 *  \param[in]  szAccessToken 认证Token
 *  \param[in]  szDevSerial   设备序列号
 *  \param[in]  iChannelNo    通道号
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
 {
     "result": {
         "page": {                                          // 分页信息
             "total": 694,                                  // 告警总数量
             "page": 0,                                     // 分页号
             "size": 5                                      // 分页大小
         },
         "data": [                                          // 告警信息
             {
             "alarmId": "96610038558312441",                // 消息ID
             "alarmName": "test设备",                       // 告警源名称
             "alarmType": 10000,                            // 告警类型
             "alarmStart": "2016-08-30 15:33:18",           // 告警开始时间
             "cameraNo": 1,                                 // 通道号 
             "isEncrypt": 0,                                // 是否加密，0-不加密 1-加密
             "isChecked": 0,                                // 是否已读，0-未读 1-已读
             "recState":1,                                  // 存储类型,0-无存储，1-萤石，2-百度,4-sd卡存储，5-萤石和sd，6-百度和sd
             "preTime": 10,                                 // 预录时间，单位秒
             "delayTime": 30,                               // 延迟录像时间，单位秒
             "deviceSerial": "427734168",                   // 设备序列号
             "alarmPicUrl": "https://test.ys7.com/HIK_14?isEncrypted=0&isCloudStored=0", //告警图片地址, 如果加密,调用OpenSDK_DecryptPicture解析
             "relationAlarms": null,                        // 关联的告警消息
             "customerType": null,                          // 透传设备参数类型
             "customerInfo": null                           // 透传设备参数内容
             },
             ...
         ],
         "code": "200",
         "msg": "操作成功!"
     }
 }
 *  \endcode
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Data_GetAlarmListEx(const char* szDevSerial, const int iChannelNo, const char* szStartTime, const char* szEndTime, AlarmType iAlarmType, int iStatus, int iPageStart, int iPageSize, void** pBuf, int* iLength);

/** 
 *  \brief 解密告警图片(建议加密的图片才调用，非加密图片直接下载，可以通过url里面isEncrypted=1来区分)
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szPicURL  图片URL,https://wuhancloudpictest.ys7.com:8083/...?isEncrypted=1&isCloudStored=0
 *  \param[in] szSerail  告警图片对应的设备序列号
 *  \param[in] szSafeKey 解密密钥，默认是设备验证码
 *  \param[out] pPicBuf	 解密后图片内容（需要调用OpenSDK_Data_Free释放内存）
 *	\param[out] iPicLen	 pPicBuf的长度
 *  \return 0表示成功，-1表示失败, 失败时调用GetLastErrorCode(), 考虑错误: OPEN_SDK_PERMANENT_KEY_INVALID,OPEN_SDK_PIC_CONTENT_INVALID
 *  \sa OpenSDK_Push_StartRecv()
 */
OPENSDK_API int CALLBACK OpenSDK_DecryptPicture(const char* szAccessToken, const char* szPicURL, const char* szSerail, const char* szSafeKey, void** pPicBuf, int* iPicLen);

/** 
 *  \brief 设置告警已读
 *  \param[in] szAlarmId 告警ID
 *  \return 0表示成功，非0表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Data_SetAlarmRead(const char* szAccessToken, const char* szAlarmId);

/** 
 *  \brief 删除设备
 *  \param[in] szAccessToken 认证Token
 *  \param[in] szDeviceId    设备Id
 *  \return 0表示成功，非0表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Data_DeleteDevice( const char* szAccessToken, const char* szDeviceId);

/** 
 *  \brief 销毁SDK分配的内存
 *  \param[in] pBuf SDK分配的内存
 *  \return 0表示成功，-1表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Data_Free(void* pBuf);

/** @} */ // end of data

/** \defgroup device 设备操作相关接口
 *  @{
 */

/**
 *  \brief 云台控制
           异步接口，返回值只是表示操作成功，不代表云台控制成功
           如果接口返回-1, 结果根据消息回调函数msgid判断, 若msgid=INS_PTZCTRL_SUCCESS, 表示成功, 若msgid=INS_PTZCTRL_FAILED, 表示失败
 *  \param[in] szSessionId   会话ID
 *  \param[in] szDevSerial	 设备序列号
 *  \param[in] iChannelNo  	 设备通道号
 *  \param[in] enCommand	 云台控制命令，参见 #PTZCommand
 *  \param[in] enAction		 云台操作命令，参见 #PTZAction
 *  \param[in] iSpeed		 速度0-7之间
 *  \return 0表示成功，-1表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_PTZCtrlEx(const char* szSessionId, const char* szDevSerial, const int iChannelNo, PTZCommand enCommand, PTZAction enAction, int iSpeed);

/** 
 *  \brief 按监控点ID进行布撤防, 针对NVR的通道进行布撤防，需要NVR和IPC支持之间协议支持。
           异步接口，返回值只是表示操作成功，不代表布撤防成功
           如果接口返回-1, 结果根据消息回调函数msgid判断, 若msgid=INS_DEFENSE_SUCCESS, 表示成功, 若msgid=INS_DEFENSE_FAILED, 表示失败
 *  \param[in] szSessionId   会话Id
 *  \param[in] szDevSerial	 设备序列号
 *  \param[in] iChannelNo  	 设备通道号
 *  \param[in] enType        布撤防类型
 *  \param[in] enStatus      布撤防状态
 *  \param[in] enActor		 布撤防设备类型
 *  \return 非负表示成功，-1表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_DevDefenceEx(const char* szSessionId, const char* szDevSerial, const int iChannelNo, DefenceType enType, DefenceStatus enStatus, DefenceActor enActor);

/** @} */ // end of device

/** \defgroup util_tool 通用工具接口
*  @{
*/

/** 
 *  \brief Http请求接口
 *  \param[in] szUri			请求地址
 *  \param[in] szHeaderParam	头部参数
 *  \param[in] szBody			Body数据
 *  \param[in] pBuf				返回报文的内容
 *  \param[in] iLength			返回报文的长度
 *  \return 0表示成功，其他值表示失败
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_HttpSendWithWait(const char* szUri, const char* szHeaderParam, const char* szBody, char** pBuf, int* iLength);
/** @} */ // end of util_tool

/** \defgroup push 消息推送
 *  @{
 */
/** 
 *  \brief 告警消息推送回调函数
 *  \param szDesc	   推送描述信息
 *  \param szContent   推送内容
 *	\param szDetail	   完整的推送信息
 *  \param pUser       用户自定义数据
 *  \sa OpenSDK_Push_SetAlarmCallBack()
 *	\sa OpenSDK_Push_SetDeviceStatusCallBack()
 *  \sa OpenSDK_Push_SetTransparentChannelCallBack()
 *  \note 
 *  告警推送内容(szContent) JSON格式如下：
 *  \code {.json}
 *	{
 *		"msgType": "1",							// 消息类型
 *		"alarmTime": "2012-12-25 15:25:42",		// 告警时间
 *		"deviceSeril": "421877673",			    // 设备序列号
 *		"channelID": "1",						// 通道ID
 *		"alarmType": "11",						// 告警类型
 *		"picUrl": "http://www.hikvision.com/",	// 图片短地址URL
 *		"videoUrl": "http://www.hikvision.com/",// 视频URL
 *		"custominfo":"sn=507669173^cn=30",		// 自定义内容，涉及告警防区
 *		"custominfoType":"CS-A1-32W_01",	    // 自定义内容的类型
 *      "alarmPicUrl":""                        // 图片URL,有效期1天, 通过调用OpenSDK_DecryptPicture接口来解密
 *      "isEncrypt":1                           // 图片是否加密, 1:加密, 0:非加密
 *	}
 *  \endcode
 *  \note 
 *  消息推送内容JSON格式如下：
 *  \code {.json}
 *	{
 *		"msgType": "3",							// 消息类型, 设备状态触发为3
 *		"devMsgType": "1",						// 推送类型号,下线状态为1
 *		"msgTime": "2016-01-27 16:09:39",		// 消息的时间
 *		"deviceSeril": "421877673",				// 设备序列号
 *		"channelID": "1"						// 通道ID
 *	}
 *  \endcode
 *  \note 
 *  通明通道消息推送内容JSON格式如下：
 *  \code {.json}
 *	{
 *		"msgType": "com,hikvision.open.transparent",	// 消息类型，透传数据类型
 *		"msgSeq": "123456789",							// 消息ID
 *		"createDate": 1456070400000.0,					// 消息时间, 时间戳(日期)
 *		"data": "hello",								// 消息内容，即透传的数据
 *		"sn": "421877673",								// 设备序列号
 *	}
 *  \endcode
 */
typedef void (CALLBACK *OpenSDK_Push_MessageHandler)(const char* szDesc, const char* szContent,  const char* szDetail, void* pUser);

/** 
 *  \brief 设置告警推送回调
 *  \param[in] pHandle			告警推送函数
 *  \param[in] pUser			用户自定义数据，回调函数会原样抛出
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_Push_AlarmHandler
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Push_SetAlarmCallBack(OpenSDK_Push_MessageHandler pHandle, void* pUser);

/** 
 *  \brief 设置设备下线状态推送
 *  \param[in] pHandle			设备状态推送函数
 *  \param[in] pUser			用户自定义数据，回调函数会原样抛出
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_Push_MessageHandler
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Push_SetDeviceStatusCallBack(OpenSDK_Push_MessageHandler pHandle, void* pUser);

/** 
 *  \brief 设置设备透明通道推送
 *  \param[in] handle			告警推送函数
 *	\param[in] publishHandle	推送消息函数
 *  \param[in] pUser			用户自定义数据，回调函数会原样抛出
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_Push_MessageHandler
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Push_SetTransparentChannelCallBack(OpenSDK_Push_MessageHandler pHandle, void* pUser);

/** 
 *  \brief 开启接收推送, 不推荐应用在服务器端上
 *  \param[in] szPushSecret  接入Push所使用密钥，创建应用的时候有平台生成
 *  \return 0表示成功，-1表示失败, 失败时调用GetLastErrorCode()
 *  \sa OpenSDK_Push_StopRecv()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Push_StartRecvEx(const char* szPushSecret);

/** 
 *  \brief 关闭接收推送
 *  \return 0表示成功，-1表示失败
 *  \sa OpenSDK_Push_StartRecv()
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_Push_StopRecv();

/** @} */ // end of push


/** \defgroup ddns 设备DDNS相关接口
 *  @{
 */
/** 
 *  \brief Get area list of all the nations. use in register.
 *  \return 
 *  \sa
 */

OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_GetAreaList(void** pBuf, int* iBufLen);

/** 
 *  \brief This API is used to turn page to get the DDNS information of all the devices in the account.
 *  \param[in]  iPageStart    Start page, start from 0. The default is 0.
 *  \param[in]  iPageSize     Page size, the default is 10, the maximum is 50.
 *  \param[out]  pBuf          Ezviz platform return Data.
 *  \param[out] iBufLen       The size of pBuf.
 *  \return 0 success，otherwise return Ezviz platform error code. reference to https://open.ys7.com/doc/en/HTTP/api-code.html
 *  \sa
 *  \note 
 *  摄像头列表的JSON格式如下
 *  \code {.json}
 *	{

 *		"page":{
 *          "total":1,
 *          "page":0,
 *          "size":1
 *      },     
 *		"data":[
 *          {
 *              "upnpMappingMode": 0,                               // upnp的映射模式
 *              "domain": "test123456",                             // 设备域名
 *              "hiddnsHttpPort": 0,                                // 设备远程Http端口
 *              "hiddnsHttpsPort": 0,                               // 设备远程Https端口
 *              "hiddnsCmdPort": 0,                                 // 设备SDKCMD控制端口
 *              "hiddnsRtspPort": 0,                                // 设备rtsp端口
 *              "mappingHiddnsHttpPort": 81,                        // 设备远程Http映射端口
 *              "mappingHiddnsCmdPort": 80,                         // 设备远程cmd映射端口
 *              "deviceIp": "212.140.29.17",                        // 设备ip地址
 *              "subSerial": "704122511",                           // 设备序列号
 *              "serial": "CS-C13-31WFR0120170112AACC704122511",    // 设备完整编号
 *              "deviceName": "C2C(704122511)"                      // 设备名称
 *           }
 *		]
 *	}
 *  \endcode
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_GetDeviceDDNSList(int iPageStart, int iPageSize, void** pBuf, int* iBufLen);

/** 
 *  \brief This API is used to set the device DDNS domain, including the shared device's DDNS domain.
 *  \param[in]  szDevSerial    Device serial No.
 *  \param[in]  szDomain       Device domain, 6 to 32 characters are allowed, including lower-case letters, digits and dashes. 
                               The first character should be a letter, and the last character cannot be a dash. 
                               It can also be device serial No. When editing device domain rule, this field is required.
 *  \return 0 success，otherwise return Ezviz platform error code. reference to https://open.ys7.com/doc/en/HTTP/api-code.html
 *  \sa
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_SetDeviceDDNSDomain(const char* szDevSerial, const char* szDomain);

/** 
 *  \brief This API is used to get the device DDNS information, including the shared device DDNS information.
 *  \param[in]  szDevSerial    Device serial No., at least one of the deviceSerial and domain is required. 
                               If the deviceSerial and domain are both provided, searching DDNS information of deviceSerial device is preferred.
 *  \param[in]  szDomain       Device domain name.
 *  \return 0 success，otherwise return Ezviz platform error code. reference to https://open.ys7.com/doc/en/HTTP/api-code.html
 *  \sa
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_GetDeviceDDNSInfo(const char* szDevSerial, const char* szDomain, void** pBuf, int* iBufLen);

/** 
 *  \brief This API is used to set device DDNS auto mapping mode by single device.
 *  \param[in]  szDevSerial    Device serial No.
 *  \return 0 success，otherwise return Ezviz platform error code. reference to https://open.ys7.com/doc/en/HTTP/api-code.html
 *  \sa
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_SetDeviceDDNSAutoMapping(const char* szDevSerial);

/** 
 *  \brief This API is used to set device DDNS manual mapping mode by single device.
 *  \param[in]  szDevSerial    Device serial No.
 *  \param[in]  iCmdPort       CMD (server) port.
 *  \param[in]  iHttpPort      HTTP port.
 *  \return 0 success，otherwise return Ezviz platform error code. reference to https://open.ys7.com/doc/en/HTTP/api-code.html
 *  \sa
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_SetDeviceDDNSManualMapping(const char* szDevSerial, const int iCmdPort, const int iHttpPort);

/** 
 *  \brief This API is used to Share DDNS information to other account by single device.
 *  \param[in]  szDevSerial    Device serial No.
 *  \param[in]  szAccount      Account to share. It can be email address, mobile phone number (contains country code) or not full digital user name.
 *  \return 0 success，otherwise return Ezviz platform error code. reference to https://open.ys7.com/doc/en/HTTP/api-code.html
 *  \sa
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_ShareDeviceDDNSInfo(const char* szDevSerial, const char* szAccount);

/** 
 *  \brief This API is used to turn page to get the DDNS information of all the shared devices in the current account.
 *  \param[in]  iPageStart    Start page, start from 0. The default is 0.
 *  \param[in]  iPageSize     Page size, the default is 10, the maximum is 50.
 *  \param[out] pBuf          Ezviz platform return Data.
 *  \param[out] iBufLen       The size of pBuf.
 *  \return 0 success，otherwise return Ezviz platform error code. reference to https://open.ys7.com/doc/en/HTTP/api-code.html
 *  \sa
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_GetShareDeviceDDNSList(int iPageStart, int iPageSize, void** pBuf, int* iBufLen);

/** @} */ // end of DDNS



/** \defgroup errorinfo 错误码信息接口
 *  @{
 */
/** 
 *  \brief 获取错误码接口
 *  \return 错误码
 *  \sa OpenSDK_GetLastErrorDesc
 */
OPENSDK_API OPENSDK_RESULT CALLBACK OpenSDK_GetLastErrorCode();

/** 
 *  \brief 错误码描述
 *  \return 错误码描述字段
 *  \sa OpenSDK_GetLastErrorCode
 */
OPENSDK_API const char* CALLBACK OpenSDK_GetLastErrorDesc();

/** @} */ // end of errorinfo


#ifdef __cplusplus
}
#endif

#endif
