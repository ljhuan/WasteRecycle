#ifndef TESTFACEAPI_TESTFACEAPI_MANAGER_H
#define TESTFACEAPI_TESTFACEAPI_MANAGER_H
#include "baidu_face_api.h"
#include <string>
// �������Ϊ����������࣬��������ע�ᣬɾ����������
class Manager
{
public:
    Manager();
    ~Manager();
public:
    // ����ע��
    void user_add(BaiduFaceApi *api);
    // ��������ע��
    void user_add_list(BaiduFaceApi *api, const std::string& file_name, 
        const std::string& file_path);
    // ������ȡĿ¼����ͼƬ������������ ,folder_pathΪͼƬĿ¼��������չ��Ϊjpg������ͼƬ
    void batch_user_add(BaiduFaceApi* api, const std::string& folder_path);
    // ������������
    void user_update(BaiduFaceApi *api);
    // ����ɾ������
    void user_face_delete(BaiduFaceApi *api);
    // �û�ɾ������
    void user_delete(BaiduFaceApi *api);
    // �����û������
    void group_add(BaiduFaceApi *api);
    // �û���ɾ������
    void group_delete(BaiduFaceApi *api);
    // �û���Ϣ��ѯ�ӿڲ���
    void get_user_info(BaiduFaceApi *api);
    // �û����б��ѯ�ӿ�
    void get_user_list(BaiduFaceApi *api);
    //���б��ѯ�ӿ�
    void get_group_list(BaiduFaceApi *api);
};

#endif
