#ifndef TESTFACEAPI_TESTFACEAPI_MANAGER_H
#define TESTFACEAPI_TESTFACEAPI_MANAGER_H
#include "baidu_face_api.h"
#include <string>
// 该类可作为人脸库管理类，包括人脸注册，删除，组管理等
class Manager
{
public:
    Manager();
    ~Manager();
public:
    // 测试注册
    void user_add(BaiduFaceApi *api);
    // 批量导入注册
    void user_add_list(BaiduFaceApi *api, const std::string& file_name, 
        const std::string& file_path);
    // 批量读取目录下面图片，导入人脸库 ,folder_path为图片目录，导入扩展名为jpg的人脸图片
    void batch_user_add(BaiduFaceApi* api, const std::string& folder_path);
    // 测试人脸更新
    void user_update(BaiduFaceApi *api);
    // 人脸删除测试
    void user_face_delete(BaiduFaceApi *api);
    // 用户删除测试
    void user_delete(BaiduFaceApi *api);
    // 创建用户组测试
    void group_add(BaiduFaceApi *api);
    // 用户组删除测试
    void group_delete(BaiduFaceApi *api);
    // 用户信息查询接口测试
    void get_user_info(BaiduFaceApi *api);
    // 用户组列表查询接口
    void get_user_list(BaiduFaceApi *api);
    //组列表查询接口
    void get_group_list(BaiduFaceApi *api);
};

#endif
