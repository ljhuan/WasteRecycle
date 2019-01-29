
#include "manager.h"
#include <iostream>
#include <stdio.h>
#include <regex>
#include <sstream>
#include <fstream>
#include <iterator>
#include <thread>
#include <Windows.h>
#include <io.h>
#include "usb_device\device_enumerator.h"
#include "image_buf.h"
#include "image_base64.h"
#include "cv_help.h"
#include "json\json.h"
/**
* 该类可作为人脸库管理类，包括人脸注册，删除，组管理等
*/
Manager::Manager()
{
}

Manager::~Manager()
{
}

// 测试注册
void Manager::user_add(BaiduFaceApi *api)
{
    std::cout << "int test_user_add" << std::endl;
    // 传入图片文件地址注册
   // std::string res = api->user_add("test_user1", "test_group", 
      //  "d:\\zhoujielun.jpg", 2, "test_user_info");

  //  std::string res1 = api->user_add("test_user1", "test_group", "d:\\4.jpg", \
     //   2, "test_user_info2");
  
      std::string user11 = "18267321380";
      std::string res11 = api->user_add(user11.c_str(), "test_group", "C:\\Users\\ljhuan\\Desktop\\tmp\\100.jpeg", 2, "ljhuan");
	  std::cout << "----res is:" << res11 << std::endl; 

    // 传入base64格式的图片注册
    //  std::string base64 = ImageBase64::file2base64("d:\\2.jpg");
    //  std::string res = api->user_add("test_user6", "test_9", base64.c_str(), \
                 1, "");
    // 传入二进制图片buffer
    /*std::string out_buf;
    int buf_len = ImageBuf::get_buf("d:\\1.jpg", out_buf);
    std::string res7 = api->user_add_by_buf("test_user1", "test_group",
        (unsigned char*)out_buf.c_str(), buf_len, "test_user_info");
    std::cout << "----res is:" << res7 << std::endl;*/
}

// 批量导入注册
void Manager::user_add_list(BaiduFaceApi *api, const std::string& file_name, const std::string& file_path)
{
    //api->user_add("test_user", "test_group", "d:\\23.jpg", 2, "test_user_info");
    std::cout << "-----begin add------:" << file_path << std::endl;
    // 入参必须为字母，数字，下划线,把扩展名的.替换为_
    std::string name = file_name;
    name = name.replace(name.find("."), 1, "_");
    std::string res = api->user_add(name.c_str(), "test_group2", file_path.c_str(), 2, name.c_str());
    std::cout << "-----res is------:" << res << std::endl;
}

// 批量读取目录下面图片，导入人脸库 ,folder_path为图片目录，导入扩展名为jpg的人脸图片
void Manager::batch_user_add(BaiduFaceApi* api, const std::string& folder_path)
{
    _finddata_t file;
    long long flag = 0;
    std::string filename = folder_path + "\\*.jpg";//遍历制定文件夹内的jpg文件  
    if ((flag = _findfirst(filename.c_str(), &file)) == -1)//目录内找不到文件  
    {
        std::cout << "There is no such type file" << std::endl;
    }
    else
    {
        //通过前面的_findfirst找到第一个文件  
        std::string name = folder_path + "\\" + file.name;//file.name存放的是遍历得到的文件名  
        std::cout << name << std::endl;
        //依次寻找以后的文件  
        while (_findnext(flag, &file) == 0)
        {
            std::string name = std::string(folder_path + "\\" + std::string(file.name));
            user_add_list(api, file.name, name);
            std::cout << name << std::endl;
        }
    }
    _findclose(flag);
}


// 测试人脸更新
void Manager::user_update(BaiduFaceApi *api)
{
    api->user_add("test_user1", "test_group", "d:\\hua1.jpg", 2, "test_user_info");
    // 传入图片文件地址更新
    //std::string res = api->user_update("15717_face_image.jpg", "test_group2", "d:\\31.jpg", 2, "test_user_info2");

    // 传入base64格式的图片更新
  //  std::string base64 = ImageBase64::file2base64("d:\\14.jpg");
  //  std::string res = api->user_update("test_user1", "test_group", base64.c_str(), 1);

    // 传入二进制图片buffer
    std::string out_buf;
    int buf_len = ImageBuf::get_buf("d:\\1.jpg", out_buf);
    std::string res = api->user_update_by_buf("test_user1", "test_group", 
        (unsigned char*)out_buf.c_str(), buf_len);

    std::cout << "----res is:" << res << std::endl;
}
// 人脸删除测试
void Manager::user_face_delete(BaiduFaceApi *api)
{
    std::string res = api->user_face_delete("test_user221", "test_group", "2dfe621619ced3d327197265f8cfd339");
    std::cout << "----res is:" << res << std::endl;
}

// 用户删除测试
void Manager::user_delete(BaiduFaceApi *api)
{
    std::string res = api->user_delete("test_user221", "test_group");
    std::cout << "----res is:" << res << std::endl;
}

// 创建用户组测试
void Manager::group_add(BaiduFaceApi *api)
{
    std::string res = api->group_add("theunited");
    std::cout << "----res is:" << res << std::endl;
}

// 用户组删除测试
void Manager::group_delete(BaiduFaceApi *api)
{
    std::string res = api->group_delete("theunited");
    std::cout << "----res is:" << res << std::endl;
}

// 用户信息查询接口测试
void Manager::get_user_info(BaiduFaceApi *api)
{
    std::string res = api->get_user_info("18267321380", "test_group");
    std::cout << "---res is:" << res << std::endl;
}
// 用户组列表查询接口
void Manager::get_user_list(BaiduFaceApi *api)
{
    std::string res = api->get_user_list("test_group2");
    std::cout << "---res is:" << res << std::endl;
}

//组列表查询接口
void Manager::get_group_list(BaiduFaceApi *api)
{
    std::string res = api->get_group_list();
    std::cout << "---res is:" << res << std::endl;
}
