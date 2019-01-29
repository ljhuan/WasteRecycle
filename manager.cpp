
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
* �������Ϊ����������࣬��������ע�ᣬɾ����������
*/
Manager::Manager()
{
}

Manager::~Manager()
{
}

// ����ע��
void Manager::user_add(BaiduFaceApi *api)
{
    std::cout << "int test_user_add" << std::endl;
    // ����ͼƬ�ļ���ַע��
   // std::string res = api->user_add("test_user1", "test_group", 
      //  "d:\\zhoujielun.jpg", 2, "test_user_info");

  //  std::string res1 = api->user_add("test_user1", "test_group", "d:\\4.jpg", \
     //   2, "test_user_info2");
  
      std::string user11 = "18267321380";
      std::string res11 = api->user_add(user11.c_str(), "test_group", "C:\\Users\\ljhuan\\Desktop\\tmp\\100.jpeg", 2, "ljhuan");
	  std::cout << "----res is:" << res11 << std::endl; 

    // ����base64��ʽ��ͼƬע��
    //  std::string base64 = ImageBase64::file2base64("d:\\2.jpg");
    //  std::string res = api->user_add("test_user6", "test_9", base64.c_str(), \
                 1, "");
    // ���������ͼƬbuffer
    /*std::string out_buf;
    int buf_len = ImageBuf::get_buf("d:\\1.jpg", out_buf);
    std::string res7 = api->user_add_by_buf("test_user1", "test_group",
        (unsigned char*)out_buf.c_str(), buf_len, "test_user_info");
    std::cout << "----res is:" << res7 << std::endl;*/
}

// ��������ע��
void Manager::user_add_list(BaiduFaceApi *api, const std::string& file_name, const std::string& file_path)
{
    //api->user_add("test_user", "test_group", "d:\\23.jpg", 2, "test_user_info");
    std::cout << "-----begin add------:" << file_path << std::endl;
    // ��α���Ϊ��ĸ�����֣��»���,����չ����.�滻Ϊ_
    std::string name = file_name;
    name = name.replace(name.find("."), 1, "_");
    std::string res = api->user_add(name.c_str(), "test_group2", file_path.c_str(), 2, name.c_str());
    std::cout << "-----res is------:" << res << std::endl;
}

// ������ȡĿ¼����ͼƬ������������ ,folder_pathΪͼƬĿ¼��������չ��Ϊjpg������ͼƬ
void Manager::batch_user_add(BaiduFaceApi* api, const std::string& folder_path)
{
    _finddata_t file;
    long long flag = 0;
    std::string filename = folder_path + "\\*.jpg";//�����ƶ��ļ����ڵ�jpg�ļ�  
    if ((flag = _findfirst(filename.c_str(), &file)) == -1)//Ŀ¼���Ҳ����ļ�  
    {
        std::cout << "There is no such type file" << std::endl;
    }
    else
    {
        //ͨ��ǰ���_findfirst�ҵ���һ���ļ�  
        std::string name = folder_path + "\\" + file.name;//file.name��ŵ��Ǳ����õ����ļ���  
        std::cout << name << std::endl;
        //����Ѱ���Ժ���ļ�  
        while (_findnext(flag, &file) == 0)
        {
            std::string name = std::string(folder_path + "\\" + std::string(file.name));
            user_add_list(api, file.name, name);
            std::cout << name << std::endl;
        }
    }
    _findclose(flag);
}


// ������������
void Manager::user_update(BaiduFaceApi *api)
{
    api->user_add("test_user1", "test_group", "d:\\hua1.jpg", 2, "test_user_info");
    // ����ͼƬ�ļ���ַ����
    //std::string res = api->user_update("15717_face_image.jpg", "test_group2", "d:\\31.jpg", 2, "test_user_info2");

    // ����base64��ʽ��ͼƬ����
  //  std::string base64 = ImageBase64::file2base64("d:\\14.jpg");
  //  std::string res = api->user_update("test_user1", "test_group", base64.c_str(), 1);

    // ���������ͼƬbuffer
    std::string out_buf;
    int buf_len = ImageBuf::get_buf("d:\\1.jpg", out_buf);
    std::string res = api->user_update_by_buf("test_user1", "test_group", 
        (unsigned char*)out_buf.c_str(), buf_len);

    std::cout << "----res is:" << res << std::endl;
}
// ����ɾ������
void Manager::user_face_delete(BaiduFaceApi *api)
{
    std::string res = api->user_face_delete("test_user221", "test_group", "2dfe621619ced3d327197265f8cfd339");
    std::cout << "----res is:" << res << std::endl;
}

// �û�ɾ������
void Manager::user_delete(BaiduFaceApi *api)
{
    std::string res = api->user_delete("test_user221", "test_group");
    std::cout << "----res is:" << res << std::endl;
}

// �����û������
void Manager::group_add(BaiduFaceApi *api)
{
    std::string res = api->group_add("theunited");
    std::cout << "----res is:" << res << std::endl;
}

// �û���ɾ������
void Manager::group_delete(BaiduFaceApi *api)
{
    std::string res = api->group_delete("theunited");
    std::cout << "----res is:" << res << std::endl;
}

// �û���Ϣ��ѯ�ӿڲ���
void Manager::get_user_info(BaiduFaceApi *api)
{
    std::string res = api->get_user_info("18267321380", "test_group");
    std::cout << "---res is:" << res << std::endl;
}
// �û����б��ѯ�ӿ�
void Manager::get_user_list(BaiduFaceApi *api)
{
    std::string res = api->get_user_list("test_group2");
    std::cout << "---res is:" << res << std::endl;
}

//���б��ѯ�ӿ�
void Manager::get_group_list(BaiduFaceApi *api)
{
    std::string res = api->get_group_list();
    std::cout << "---res is:" << res << std::endl;
}
