#pragma comment(lib, "BaiduFaceApi.lib")
#pragma comment(lib, "opencv_world320.lib")
#pragma comment(lib, "json_vc71_libmt.lib")
#include "test_face.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <regex>
#include <sstream>
#include <fstream>
#include <iterator>
#include <thread>
#include <Windows.h>
#include <io.h>
#include "opencv2\opencv.hpp"
#include "baidu_face_api.h"
#include "image_base64.h"
#include "cv_help.h"
#include "json\json.h"
#include "liveness.h"
#include "manager.h"
#include "compare.h"
#include "set\setting.h"
#include "image_buf.h"

using namespace cv;
using namespace std;

// 获取时间戳
std::time_t get_timestamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    return timestamp;
}

//测试获取设备指纹device_id
void test_get_device_id(BaiduFaceApi *api)
{
    std::string res = api->get_device_id();
    std::cout << "---res is:" << res << std::endl;
}

// 是否授权
void test_is_auth(BaiduFaceApi *api)
{
    bool authed = api->is_auth();
    if (authed)
    {
        std::cout << "authed is true" << std::endl;
    }
    else
    {
        std::cout << "authed is false" << std::endl;
    }
}
// 测试获取人脸属性
void test_face_attr(BaiduFaceApi* api)
{
    // 获取人脸属性，通过传入图片base64(1:base64图片，2为图片路径）
   // std::string base64 = ImageBase64::file2base64("d:\\1.jpg");
   // std::string res = api->face_attr(base64.c_str(), 1);
    // 获取人脸属性，通过传入图片路径(1:base64图片，2为图片路径）
   // std::string res = api->face_attr("d:\\1.jpg", 2);
   // 获取人脸属性，通过传入图片二进制buffer
    std::string out_buf;
    // 读取文件到二进制buffer
    int buf_len = ImageBuf::get_buf("d:\\1.jpg", out_buf);
    std::string res = api->face_attr_by_buf((unsigned char*)out_buf.c_str(), buf_len);
    std::cout << "res is:" << res << std::endl;
}

// 测试获取人脸质量
void test_face_quality(BaiduFaceApi* api)
{
    // 获取人脸属性，通过传入图片base64(1:base64图片，2为图片路径）
    // std::string base64 = ImageBase64::file2base64("d:\\1.jpg");
    // std::string res = api->face_quality(base64.c_str(), 1);
    // 获取人脸属性，通过传入图片路径(1:base64图片，2为图片路径）
     std::string res = api->face_quality("d:\\12.jpg", 2);
    // 获取人脸属性，通过传入图片二进制buffer
  /*  std::string out_buf;
    // 读取文件到二进制buffer
    std::cout << "in test_face_quality" << std::endl;
    int buf_len = ImageBuf::get_buf("d:\\12.jpg", out_buf);
    std::cout << "in test_face_quality 1111" << std::endl;
    std::string res = api->face_quality_by_buf((unsigned char*)out_buf.c_str(), buf_len); */
    std::cout << "res is:" << res << std::endl;
}

// 通过修改setting类里的对应参数，来达到调整人脸检测策略的目的
void test_face_setting(BaiduFaceApi* api)
{
    Setting *setptr = new Setting();
    // setptr->set_blur_thr(api);
    // setptr->set_detect_in_video_interval(api);
    // setptr->set_eulur_angle_thr(api);
    // setptr->set_illum_thr(api);
    // setptr->set_is_check_quality(api);
    setptr->set_min_face_size(api);
    //  setptr->set_notFace_thr(api);
    //  setptr->set_occlu_thr(api);
    //  setptr->set_track_by_detection_interval(api);
    delete setptr;
}
// 人脸信息管理，如人脸注册，更新，组管理等
void test_face_manager(BaiduFaceApi* api)
{
    Manager *mgrptr = new Manager();
    // 人脸注册
    // mgrptr->user_add(api);
    // 批量注册
  //  mgrptr->batch_user_add(api, "d:\\image");
    //人脸更新
   //  mgrptr->user_update(api);
    // 人脸删除
    // mgrptr->user_face_delete(api);
    // 用户删除
   // mgrptr->user_delete(api);
    // 用户组添加
    //  mgrptr->group_add(api);
    // 用户组删除 
    //  mgrptr->group_delete(api);
    // 用户信息查询接口
    mgrptr->get_user_info(api);
    // 用户组列表查询接口
    //  mgrptr->get_user_list(api);
    //组列表查询接口
    //  mgrptr->get_group_list(api);
    delete mgrptr;
}
// 测试比对，1:1人脸比对，1：N人脸比对,特征值比对等
void test_compare(BaiduFaceApi* api)
{
    Compare *comptr = new Compare();
    //1:1对比
   //  comptr->match(api);
    //1:N对比
     comptr->identify(api);
   //  comptr->identify_by_feature(api);
   // comptr->identify(api);
    //图片和视频对比
   // comptr->match_by_img_and_frame(api, "d:\\1.jpg");
    //以下为特征值测试demo
    // comptr->get_face_feature_by_img(api);
   // comptr->get_face_feature_by_frame(api);
    // comptr->get_face_feature_by_buf(api);
     //特征值比较
  //  comptr->compare_feature(api);
    // 测试获取特征值及特征值比较(比较综合，可根据参数不同比较)
   // comptr->get_face_feature_and_match(api);
    delete comptr;
}

// 活体采集及检测
void test_liveness_track(BaiduFaceApi* api)
{
    Liveness* liveptr = new Liveness();
    // liveptr->ir_liveness_track_by_img(api);
     // usb摄像头进行人脸采集
     liveptr->usb_track_face_info(api);
    // rgb图像进行人脸采集
    //rgb单目活体检测
    // liveptr->rgb_liveness_track_by_img(api);
     // ir图像进行单目活体检测
    // liveptr->ir_liveness_track_by_img(api);
    // liveptr->usb_rgb_track(api);
    // 对单张图片进行检测人脸信息 
    // liveptr->image_track_face_info(api);
    // 传入二进制图片buffer
   // liveptr->image_track_face_info_by_buf(api);
    //ir和rgb双目活体检测
    // liveptr->ir_rgb_track(api);
    // depth深度活体检测
   //  liveptr->rgb_depth_track_by_buf(api);
     // 奥比中光双目摄像头深度活体
    // liveptr->rgb_depth_track_by_mat_with_orbe(api);
     // 华杰艾米双目摄像头深度活体
    // liveptr->rgb_depth_track_by_mat_with_hjimi(api);
     delete liveptr;
}


// 入口函数
/*int main()
{
    std::cout << "in main" << std::endl;
    //api实例指针
    BaiduFaceApi *api = new BaiduFaceApi();
    //初始化sdk
    // 若采用证件照模式，请把id_card设为true，否则为false，证件照模式和非证件照模式提取的人脸特征值不同，
    // 不能混用
    bool id_card = false;
    api->sdk_init(id_card);
    // 提前加载人脸库到内存
    api->load_db_face();
    std::time_t time_begin = get_timestamp();
    std::cout << "in main" << std::endl;
    // test_get_device_id(api);
    // test_face_setting(api);
    // test_face_attr(api);
    // test_face_quality(api);
    // test_face_manager(api);
    test_compare(api);
    // test_multi_thread();

   // test_liveness_track(api);
    
    // test_is_auth(api);
    std::time_t time_end = get_timestamp();
    std::cout << "time  cost is :" << time_end - time_begin << "ms" << std::endl;
    delete api;
    getchar();
    return 0;
}*/


TestFace::TestFace()
{
}

TestFace::~TestFace()
{
}
