/** \file      OpenNetStreamDefine.h
 *  \copyright HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *  \brief     开放SDK数据结构定义
 *
 *  \author    panlong, peter
 *  \date      2014/12/15
 *
 *  \note      history:
 *  \note      V0.0.1 2014/12/15 peter 增加头文件注释 
 */

#ifndef _H_OPENNETSTREAMDEFINE_H_
#define _H_OPENNETSTREAMDEFINE_H_

/** \typedef OPENSDK_RESULT
 *  \brief   opensdk返回值，0表示成功，非0表示失败
 */
typedef int OPENSDK_RESULT; 

/** \enum  MessageType 
 *  \brief 消息类型定义
 */
enum MessageType
{
	INS_PLAY_EXCEPTION            = 0,   ///< 播放异常，通常是设备断线或网络异常造成
	INS_PLAY_RECONNECT            = 1,   ///< 重连，实时流播放时内部会自动重连
	INS_PLAY_RECONNECT_EXCEPTION  = 2,   ///< 重连异常
	INS_PLAY_START                = 3,   ///< 播放开始
	INS_PLAY_STOP                 = 4,   ///< 播放终止
	INS_PLAY_ARCHIVE_END          = 5,   ///< 播放结束，回放结束时会有此消息

	INS_VOICETALK_START           = 16,  ///< 语音对讲开始
	INS_VOICETALK_STOP            = 17,  ///< 语音对讲停止
	INS_VOICETALK_EXCEPTION       = 18,  ///< 语音对讲异常 
	INS_PTZ_EXCEPTION             = 19,  ///< 云台控制异常
	INS_RECORD_FILE               = 20,  ///< 查询的录像文件(录像搜索结果)
	INS_RECORD_SEARCH_END         = 21,  ///< 录像查询结束（暂不使用）
	INS_RECORD_SEARCH_FAILED      = 22,  ///< 录像查询失败
	INS_DEFENSE_SUCCESS			  = 23,  ///< 布防成功
	INS_DEFENSE_FAILED			  = 24,  ///< 布防失败
	INS_PLAY_ARCHIVE_EXCEPTION    = 28,	 ///< 回放异常结束，可能是接收数据超时
	INS_PTZCTRL_SUCCESS			  = 46,  ///< 云台控制命令发送成功
	INS_PTZCTRL_FAILED			  = 47 	 ///< 云台控制失败
};

/** \struct STREAM_TIME
 *  \brief  开放SDK获取OSD时间格式定义
 */
typedef struct _STREAM_TIME
{
	unsigned int iYear;					 ///< 年
	unsigned int iMonth;				 ///< 月
	unsigned int iDay;					 ///< 日
	unsigned int iHour;					 ///< 时
	unsigned int iMinute;				 ///< 分
	unsigned int iSecond;				 ///< 秒
}STREAM_TIME, *LP_STREAM_TIME;

/** \struct NetStreamCallbackMessage
 *  \brief 经过NetStream处理所反馈的信息                                                                    
 */
typedef struct _NetStreamCallBackMessage
{
public:
	_NetStreamCallBackMessage(void):
	  iErrorCode(0), pMessageInfo(0){}
	int iErrorCode;			///< 消息回调反馈的错误码
	char *pMessageInfo;					///< 消息回调反馈的信息
} NSCBMsg, *LP_NSCBMsg;

/** \enum  AlarmType 
 *  \brief 开放SDK告警类型定义
 */
enum AlarmType
{
	ALARM_TYPE_ALL 					= -1,           ///< 全部
	BODY_SENSOR_EVENT				= 10000,		///< 人体感应事件
	EMERGENCY_BUTTON_EVENT 			= 10001,		///< 紧急遥控按钮事件
	MOTION_DETECT_ALARM 			= 10002,		///< 移动侦测告警
	BABY_CRY_ALARM 					= 10003,		///< 婴儿啼哭告警
	MAGNETIC_ALARM 					= 10004,		///< 门磁告警
	SMOKE_DETECTOR_ALARM 			= 10005,		///< 烟感告警
	COMBUSTIBLE_GAS_ALARM 			= 10006,		///< 可燃气体告警
	FLOOD_IN_ALARM 					= 10008,		///< 水浸告警
	EMERGENCY_BUTTON_ALARM 			= 10009,		///< 紧急按钮告警
	BODY_SENSOR_ALARM 				= 10010,		///< 人体感应告警
	SHELTER_ALARM					= 10011, 		///< 遮挡告警
	VIDEO_LOSS_ALARM				= 10012,		///< 视频丢失
	LINE_DETECTION_ALARM			= 10013,		///< 越界侦测
	FIELD_DETECTION_ALARM			= 10014,		///< 区域入侵
	FACE_DETECTION_ALARM			= 10015,		///< 人脸检测事件
	DOOR_BELL_ALARM					= 10016,		///< 智能门铃告警
	DEVOFFLINE_ALARM				= 10017,		///< 摄像机失去关联告警
	CURTAIN_ALARM					= 10018,		///< 幕帘告警
	MOVE_MAGNETOMETER_ALARM			= 10019,		///< 单体门磁告警
	SCENE_CHANGE_DETECTION_ALARM	= 10020,		///< 场景变更侦测
	DEFOCUS_ALARM					= 10021,		///< 虚焦侦测
	AUDIO_EXCEPTION_ALARM			= 10022,		///< 音频异常侦测
	LEFT_DETECTION_ALARM			= 10023,		///< 物品遗留侦测
	TAKE_DETECTION_ALARM			= 10024,		///< 物品拿取侦测
	PARKING_DETECTION_ALARM			= 10025,		///< 非法停车侦测
	HIGH_DENSITY_DETECTION_ALARM	= 10026,		///< 人员聚集侦测
	LOITER_DETECTION_ALARM			= 10027,		///< 徘徊检测侦测
	RUN_DETECTION_ALARM				= 10028,		///< 快速移动侦测
	ENTER_AREA_DETECTION_ALARM		= 10029,		///< 进入区域侦测
	EXIT_AREA_DETECTION_ALARM		= 10030,		///< 离开区域侦测
	MAG_GIM_ALARM					= 10031,		///< 磁干扰告警
	UNDER_VOLTAGE_ALARM				= 10032,		///< 电池欠压告警
	INTRUSION_ALARM					= 10033,		///< 闯入告警
	IO_00_ALARM						= 10100,		///< IO告警
	IO_01_ALARM						= 10101,		///< IO-1告警
	IO_02_ALARM						= 10102,		///< IO-2告警
	IO_03_ALARM						= 10103,		///< IO-3告警
	IO_04_ALARM						= 10104,		///< IO-4告警
	IO_05_ALARM						= 10105,		///< IO-5告警
	IO_06_ALARM						= 10106,		///< IO-6告警
	IO_07_ALARM						= 10107,		///< IO-7告警
	IO_08_ALARM						= 10108,		///< IO-8告警
	IO_09_ALARM						= 10109,		///< IO-9告警
	IO_10_ALARM						= 10110,		///< IO-10告警
	IO_11_ALARM						= 10111,		///< IO-11告警
	IO_12_ALARM						= 10112,		///< IO-12告警
	IO_13_ALARM						= 10113,		///< IO-13告警
	IO_14_ALARM						= 10114,		///< IO-14告警
	IO_15_ALARM						= 10115,		///< IO-15告警
	IO_16_ALARM						= 10116         ///< IO-16告警
};

/** \enum  AlarmNotifyType
 *  \brief 开放SDK告警通知类型定义
 */
typedef enum _AlarmNotifyType
{
	ALARM_NOTIFY_CONNECT_EXCEPTION = 100,	///< SDK同萤石平台连接异常
	ALARM_NOTIFY_RECONNECT_SUCCESS,			///< 重连成功
	ALARM_NOTIFY_RECONNECT_FAILED			///< 重连失败
} AlarmNotifyType;

/** \enum PTZCommand
 *  \brief 云台控制命令
 */
enum PTZCommand
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	UPLEFT,
	DOWNLEFT,
	UPRIGHT,
	DOWNRIGHT,
	ZOOMIN,
	ZOOMOUT,
	FOCUSNEAR,
	FOCUSFAR,
	IRISSTARTUP,
	IRISSTOPDOWN,
	LIGHT,
	WIPER,
	AUTO
};

/** \enum PTZAction
 *  \brief 云台操作命令
 *  一般情况下，鼠标按下代表开始，鼠标松开代表停止
 */
enum PTZAction
{
	START,							 
	STOP
};

/** \enum  DefenceType 
 *  \brief 布撤防告警类型
 */
enum DefenceType 
{
	PIR,							///< 红外
	ATHOME,                         ///< 在家，A1设备
	OUTDOOR,					    ///< 外出
	BABYCRY,						///< 婴儿啼哭
	MOTIONDETECT,					///< 移动侦测
	GLOBAL							///< 全部
};

/** \enum  DefenceStatus 
 *  \brief 布撤防状态
 */
enum DefenceStatus
{
	UN_DEFENCE,						///< 撤防
	DEFENCE,						///< 布防
	UN_SUPPORT,						///< 不支持
	FORCE_DEFENCE 					///< 强制布防，A1设备
};

enum DefenceActor
{
	D,								///< 设备
	V,								///< 视频通道
	I 								///< IO通道
};

/** \enum  DataType 
 *  \brief 音视频流数据类型
 */
enum DataType {
	NET_DVR_SYSHEAD       = 1,      ///< 流头
	NET_DVR_STREAMDATA    = 2,		///< 流数据
	NET_DVR_RECV_END      = 3       ///< 结束标记
};

/** \enum  LoginParamKey 
 *  \brief 登录相关参数类型
 */
enum LoginParamKey {
	LOGIN_AREAID              = 1,      ///< 区域号, 目前只有海外使用
	LOGIN_VER                 = 2,		///< 登录页面版本信息
    LOGIN_AREADOMAIN          = 3,      ///< 登录后,重定向的域名
    LOGIN_TOKEN_EXPIRETIME    = 4,      ///< 登录后,Token失效时间
    LOGIN_ACCESS_TOKEN        = 5       ///< 登录后,获取AccessToken
};

/** \enum  ConfigKey 
 *  \brief 配置类型
 */
enum ConfigKey {
    CONFIG_DATA_UTF8           = 1,      ///< 数据输出使用UTF8
    CONFIG_OPEN_STREAMTRANS    = 2,      ///< 码流数据输出经过转封装处理, 用于录像存储
    CONFIG_CLOSE_P2P           = 3,       ///< P2P开关, 用于关闭P2P
    CONFIG_LOG_LEVEL           = 4,       ///< 配置日志等级,参见 #OpenSDK_LogLevel
    CONFIG_P2P_MAXNUM          = 6,
    CONFIG_P2P_CHECK_PERIOD    = 7,
    CONFIG_P2P_EXPIRE          = 8
};

#endif
