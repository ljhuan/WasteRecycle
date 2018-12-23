/** \file      OpenNetStreamError.h
 *  \copyright HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *  \brief     开放SDK错误码定义
 *
 *  \author    panlong, peter
 *  \date      2014/12/15
 *
 *  \note      history: 
 *  \note      V2.4.6 2016/01/21 pikongxuan 新版本错误码定义
 */

#ifndef _H_OPENNETSTREAMERROR_H_ 
#define _H_OPENNETSTREAMERROR_H_

#define OPEN_SDK_NOERROR				0					///< 没有错误
#define OPEN_SDK_GENERAL_ERROR			-1					///< 常规（通用）错误

#define OPEN_SDK_BASE					10000				///< 开放SDK基础错误  
#define OPEN_SDK_JSON_ERROR				OPEN_SDK_BASE + 1	///< Json解析出错     
#define OPEN_SDK_ERROR					OPEN_SDK_BASE + 2	///< 获取平台数据出错 
#define OPEN_SDK_DEV_NO_SUPPORT			OPEN_SDK_BASE + 3	///< 不支持的设备     
#define OPEN_SDK_ALLOC_ERROR			OPEN_SDK_BASE + 4	///< 申请内存失败     
#define OPEN_SDK_PARAM_ERROR			OPEN_SDK_BASE + 5	///< 传入参数非法     
#define OPEN_SDK_SAFE_KEY_ERROR			OPEN_SDK_BASE + 6	///< 安全密钥出错     
#define OPEN_SDK_SEARCHING_ERROR		OPEN_SDK_BASE + 7	///< 录像搜索出错
#define OPEN_SDK_SYNC_ERROR				OPEN_SDK_BASE + 8	///< 同步参数出错  
#define OPEN_SDK_INTERFACE_NO_IMPL		OPEN_SDK_BASE + 9	///< 接口未实现，主要针对平台
#define OPEN_SDK_ORDER_ERROR			OPEN_SDK_BASE + 10  ///< 接口调用顺序出错
#define OPEN_SDK_SESSION_EXPIRED		OPEN_SDK_BASE + 11  ///< SESSION已失效，Session对象被释放

/******************************************************************************************************************/
/* 接口基本默认只返回0和-1, 如果接口返回-1, 则通过OpenSDK_GetLastErrorCode拿具体错误码
错误码格式{xbbbbb}, x表示错误码表示，x的值如下表：
/// 0: correct condition
/// 1: network setup exception,(no DNS, connection lost)
/// 2: user operation error, input invalid params, rely on lib error(load, init, unload)
/// 3: platform server error
/// 4: memory exception, system resource(alloc memory, create thread failed)
/// 5: opensdk dependent lib netstream errorcode.
/// 6: method not supported
/// 9: undefined error
*/
/******************************************************************************************************************/

//网络级别错误码，CURL反馈, 都是调开放平台接口报错（测试时首先保证登陆成功）
#define OPEN_SDK_NETWORK_SETUP_BASE			100000
#define	OPEN_SDK_COULDNT_RESOLVE_HOST		OPEN_SDK_NETWORK_SETUP_BASE + 6		///< 给定的远程主机没有得到解析，这里是指platform域名无法正常解析,可能是DNS没有配置或者机器没有连网。
#define OPEN_SDK_COULDNT_CONNECT			OPEN_SDK_NETWORK_SETUP_BASE + 7		///< 远程主机不可达，这里是指无法访问platform,可能是platform地址配置错误。
#define OPEN_SDK_OPERATION_TIMEOUT			OPEN_SDK_NETWORK_SETUP_BASE + 28	///< 请求操作超时, 超时时间为20s, 请求平台超时，请检查platform地址配置错误。

//用户操作级别错误码
#define OPEN_SDK_USER_OPERATION_BASE		200000
#define OPEN_SDK_BAD_PARAMS					OPEN_SDK_USER_OPERATION_BASE + 1	///< 接口传入参数不符合要求
#define OPEN_SDK_SESSION_INVALID			OPEN_SDK_USER_OPERATION_BASE + 2	///< 当前Session不存在或者被释放,可能是SessionId传入值错误或者是Session已经被释放。
#define OPEN_SDK_VIDEO_RECORD_NOT_EXIST		OPEN_SDK_USER_OPERATION_BASE + 3	///< 指定时间段内录像记录不存在
#define OPEN_SDK_VIDEO_RECORD_SEARCHING		OPEN_SDK_USER_OPERATION_BASE + 4	///< 录像记录正在搜索
#define OPEN_SDK_STOP_ALARM_REC_FAILED		OPEN_SDK_USER_OPERATION_BASE + 5	///< 关闭告警失败, 可能是没有开启告警或者已经关闭告警
#define OPEN_SDK_PERMANENT_KEY_INVALID		OPEN_SDK_USER_OPERATION_BASE + 6	///< 验证码不正确
#define OPEN_SDK_PIC_DECRYPT_FAILED			OPEN_SDK_USER_OPERATION_BASE + 7	///< 图片解码失败
#define OPEN_SDK_PIC_CONTENT_INVALID		OPEN_SDK_USER_OPERATION_BASE + 8	///< 图片内容无效
#define OPEN_SDK_PIC_NO_NEED_DECRYPT		OPEN_SDK_USER_OPERATION_BASE + 9	///< 图片无需解码
#define OPEN_SDK_PIC_COULDNT_ALLOC_BUFFERS	OPEN_SDK_USER_OPERATION_BASE + 10	///< 无法分配图片资源内存，可能内存不足或者图片过大
#define OPEN_SDK_SDK_LOAD_FAILED            OPEN_SDK_USER_OPERATION_BASE + 11	///< 依赖库没有加载

//OpenApi接口级别错误码, 详细参考https://open.ys7.com/doc/book/index/api-code.html
#define OPEN_SDK_OPENAPI_BASE				300000
#define OPEN_SDK_RESPINFO_BAD				OPEN_SDK_OPENAPI_BASE + 1			///< 请求返回的信息,json无法正常解析,可能是PlatformAddr配置有问题
#define OPEN_SDK_RESPINFO_INVALID			OPEN_SDK_OPENAPI_BASE + 2			///< 请求返回信息格式有误
#define OPEN_SDK_DEVICE_RSP_TIMEOUT         OPEN_SDK_OPENAPI_BASE + 2009		///< 设备请求响应超时异常, 检测设备网络或者稍后重试
#define OPEN_SDK_DEVICE_SAFE_INVALID        OPEN_SDK_OPENAPI_BASE + 5002		///< 设备验证码错误, 请重新确认设备验证码
#define OPEN_SDK_ACCESSTOKEN_INVALID		OPEN_SDK_OPENAPI_BASE + 10002		///< accesstoken异常或者过期,accessToken异常或请求方法不存在
#define OPEN_SDK_SIGNATURE_ERROR			OPEN_SDK_OPENAPI_BASE + 10008		///< 表示输入参数有问题。平台显示签名错误
#define OPEN_SDK_APPKEY_NOMATCH_TOKEN_ERROR	OPEN_SDK_OPENAPI_BASE + 10018		///< AccessToken与Appkey不匹配, 请检查获取accessToken对应的appKey和SDK中设置的appKey是否一致
#define OPEN_SDK_USERID_PHONE_UNBIND		OPEN_SDK_OPENAPI_BASE + 10014		///< APPKEY下对应的第三方userId和phone未绑定
#define OPEN_SDK_CHANNEL_NOT_EXIST			OPEN_SDK_OPENAPI_BASE + 20001		///< 通道不存在,通道对应某一监控点
#define OPEN_SDK_DEVICE_OFFLINE	    		OPEN_SDK_OPENAPI_BASE + 20007		///< 设备不在线
#define OPEN_SDK_USER_NOTOWN_DEVICE			OPEN_SDK_OPENAPI_BASE + 20018		///< 该用户不拥有该设备
#define OPEN_SDK_SERVER_DATA_BAD			OPEN_SDK_OPENAPI_BASE + 49999		///< 数据异常, 反馈开发进一步确认问题
#define OPEN_SDK_SERVER_ERROR				OPEN_SDK_OPENAPI_BASE + 50000		///< 开平服务器异常, 反馈开发进一步确认问题

//系统资源级别错误
#define OPEN_SDK_SYSTEM_RESOURCE_BASE		400000
#define OPEN_SDK_COULDNT_CREATE_THREAD		OPEN_SDK_SYSTEM_RESOURCE_BASE + 1	///< 创建线程失败
#define OPEN_SDK_COULDNT_ALLOC_BUFFERS		OPEN_SDK_SYSTEM_RESOURCE_BASE + 2	///< 申请内存资源失败

//不支持的错误码
#define OPEN_SDK_NOT_SUPPORT_BASE			600000
#define OPEN_SDK_DEV_NOT_SUPPORT			OPEN_SDK_NOT_SUPPORT_BASE + 1		///< 不支持非1.7设备
#define OPEN_SDK_API_NO_IMPLEMENT			OPEN_SDK_NOT_SUPPORT_BASE + 2		///< 接口未实现

//推送级别错误
#define OPEN_SDK_PUSH_BASE					700000
#define OPEN_SDK_PUSH_PARAM_ERROR			OPEN_SDK_PUSH_BASE + 10001			///< 传入参数非法
#define OPEN_SDK_PUSH_DATA_UNINIT_ERROR		OPEN_SDK_PUSH_BASE + 10002			///< 数据未初始化，请先调用Init接口初始化
#define OPEN_SDK_PUSH_NO_REGISTER_ERROR		OPEN_SDK_PUSH_BASE + 10003			///< 未向Push平台注册，即未调register接口
#define OPEN_SDK_PUSH_NO_MQTT_CREATE_ERROR	OPEN_SDK_PUSH_BASE + 10004			///< 未创建创建推送对象，即未调create接口
//MQTT Client错误码
#define OPEN_SDK_PUSH_MQTT_DISCONNECTED_ERROR		OPEN_SDK_PUSH_BASE + 20003			///< sdk同push服务器断开连接
#define OPEN_SDK_PUSH_MQTT_MAX_MESSAGES_ERROR		OPEN_SDK_PUSH_BASE + 20004			///< 达到消息接收上限
#define OPEN_SDK_PUSH_MQTT_BAD_UTF8_STRING_ERROR	OPEN_SDK_PUSH_BASE + 20005			///< 不合法的UTF-8字符串
#define OPEN_SDK_PUSH_MQTT_NULL_PARAMETER_ERROR		OPEN_SDK_PUSH_BASE + 20006			///< 传入参数为空指针
//MQTT Protocol错误码
#define OPEN_SDK_PUSH_MQTT_VERSION_INVALID_ERROR		OPEN_SDK_PUSH_BASE + 30001		///< 连接失败，协议版本不支持
#define OPEN_SDK_PUSH_MQTT_IDENTIFIER_ERROR				OPEN_SDK_PUSH_BASE + 30002		///< 连接失败，唯一标识不正确
#define OPEN_SDK_PUSH_MQTT_SERVER_UNAVAILABLE_ERROR		OPEN_SDK_PUSH_BASE + 30003		///< 连接失败，服务不存在
#define OPEN_SDK_PUSH_MQTT_BAD_USERNAME_PASSWORD_ERROR	OPEN_SDK_PUSH_BASE + 30004		///< 连接失败，mqtt用户名和密码不正确
#define OPEN_SDK_PUSH_MQTT_NOT_AUTHORIZED_ERROR			OPEN_SDK_PUSH_BASE + 30005		///< 连接失败，未授权
//Push Platform错误码
#define OPEN_SDK_PUSH_PLATFORM_RESPINFO_BAD				OPEN_SDK_PUSH_BASE + 40001		///< 请求返回的信息,json无法正常解析,可能是PlatformAddr配置有问题
#define OPEN_SDK_PUSH_PLATFORM_RESPINFO_INVALID			OPEN_SDK_PUSH_BASE + 40002		///< 请求返回信息格式有误
#define OPEN_SDK_PUSH_PLATFORM_SESSION_INVALID_ERROR	OPEN_SDK_PUSH_BASE + 40003		///< Session Invalid
#define OPEN_SDK_PUSH_PLATFORM_UNAUTHORIZED_ERROR		OPEN_SDK_PUSH_BASE + 40401		///< Bad credentials, 可能是PushId有误或者不支持
//Push 系统资源错误码
#define OPEN_SDK_PUSH_COULDNT_CREATE_THREAD				OPEN_SDK_PUSH_BASE + 50001		///< 创建线程失败
#define OPEN_SDK_PUSH_COULDNT_ALLOC_BUFFERS				OPEN_SDK_PUSH_BASE + 50002		///< 申请内存资源失败
//Push 网络级别错误码
#define	OPEN_SDK_PUSH_COULDNT_RESOLVE_HOST				OPEN_SDK_PUSH_BASE + 60006		///< 给定的远程主机没有得到解析，这里是指platform域名无法正常解析,可能是DNS没有配置或者机器没有连网。
#define OPEN_SDK_PUSH_COULDNT_CONNECT					OPEN_SDK_PUSH_BASE + 60007		///< 远程主机不可达，这里是指无法访问platform,可能是platform地址配置错误。
#define OPEN_SDK_PUSH_OPERATION_TIMEOUT					OPEN_SDK_PUSH_BASE + 60028		///< 请求操作超时, 超时时间为20s, 请求平台超时，请检查platform地址配置错误。

//NetStream的错误码
#define OPEN_SDK_NETSTREAM_BASE				500000
#define OPEN_SDK_START_TALK_FAILED			OPEN_SDK_NETSTREAM_BASE + 1			///< 对讲开启传入参数有误
#define OPEN_SDK_TALK_OPENED				OPEN_SDK_NETSTREAM_BASE + 2			///< 对讲已经开启

//未定义的错误码
#define OPEN_SDK_UNDEFINE_BASE				900000
#define OPEN_SDK_ALLOCSESSION_FAILED		OPEN_SDK_UNDEFINE_BASE + 1			///< AllocSession 失败
#define OPEN_SDK_SEARCH_RECORD_FAILED		OPEN_SDK_UNDEFINE_BASE + 2			///< 查询回放记录失败
#define OPEN_SDK_START_ALARM_REC_FAILED		OPEN_SDK_UNDEFINE_BASE + 3			///< 开启告警失败


//预览时回调接口反馈的错误码, 下列为常见错误码
/*
 *  \brief 消息回调 取流 错误码定义(MessageInfo), 公共错误码起始：UE000；业务错误码起始：UE100
 */
#define OPEN_SDK_STREAM_ACCESSTOKEN_ERROR_OR_EXPIRE 				"UE001" 	///< accesstoken失效，请重新获取accesstoken
#define OPEN_SDK_STREAM_PU_NO_RESOURCE      						"UE101"	    ///< 设备连接数过大, 升级设备固件版本,海康设备可咨询客服获取升级流程
#define OPEN_SDK_STREAM_TRANSF_DEVICE_OFFLINE   	 				"UE102" 	///< 设备不在线，确认设备上线之后重试
#define OPEN_SDK_STREAM_INNER_TIMEOUT   	 				        "UE103" 	///< 播放失败，请求连接设备超时，检测设备网路连接是否正常
#define OPEN_SDK_STREAM_INNER_VERIFYCODE_ERROR   	 				"UE104" 	///< 视频验证码错误，建议查看设备上标记的验证码
#define OPEN_SDK_STREAM_PLAY_FAIL       			 				"UE105" 	///< 视频播放失败, 请查看消息回调中具体错误码iErrorCode
#define OPEN_SDK_STREAM_TRANSF_TERMINAL_BINDING   	 				"UE106" 	///< 当前账号开启了终端绑定，只允许指定设备登录操作
#define OPEN_SDK_STREAM_VIDEO_RECORD_NOTEXIST   	 				"UE108"     ///< 未查找到录像文件
#define OPEN_SDK_STREAM_VTDU_CONCURRENT_LIMIT          	 			"UE109"     ///< 取流并发路数限制,请升级为企业版
#define OPEN_SDK_STREAM_UNSUPPORTED            	 				    "UE110" 	///< 设备不支持的清晰度类型, 请根据设备预览能力级选择.
#define OPEN_SDK_STREAM_DEVICE_RETURN_ON_VIDEO_SOURCE               "UE111" 	///< 设备返回无视频源, 请检测设备是否接触良好.


#endif	//_H_OPENNETSTREAMERROR_H_