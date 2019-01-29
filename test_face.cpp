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

// ��ȡʱ���
std::time_t get_timestamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    return timestamp;
}

//���Ի�ȡ�豸ָ��device_id
void test_get_device_id(BaiduFaceApi *api)
{
    std::string res = api->get_device_id();
    std::cout << "---res is:" << res << std::endl;
}

// �Ƿ���Ȩ
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
// ���Ի�ȡ��������
void test_face_attr(BaiduFaceApi* api)
{
    // ��ȡ�������ԣ�ͨ������ͼƬbase64(1:base64ͼƬ��2ΪͼƬ·����
   // std::string base64 = ImageBase64::file2base64("d:\\1.jpg");
   // std::string res = api->face_attr(base64.c_str(), 1);
    // ��ȡ�������ԣ�ͨ������ͼƬ·��(1:base64ͼƬ��2ΪͼƬ·����
   // std::string res = api->face_attr("d:\\1.jpg", 2);
   // ��ȡ�������ԣ�ͨ������ͼƬ������buffer
    std::string out_buf;
    // ��ȡ�ļ���������buffer
    int buf_len = ImageBuf::get_buf("d:\\1.jpg", out_buf);
    std::string res = api->face_attr_by_buf((unsigned char*)out_buf.c_str(), buf_len);
    std::cout << "res is:" << res << std::endl;
}

// ���Ի�ȡ��������
void test_face_quality(BaiduFaceApi* api)
{
    // ��ȡ�������ԣ�ͨ������ͼƬbase64(1:base64ͼƬ��2ΪͼƬ·����
    // std::string base64 = ImageBase64::file2base64("d:\\1.jpg");
    // std::string res = api->face_quality(base64.c_str(), 1);
    // ��ȡ�������ԣ�ͨ������ͼƬ·��(1:base64ͼƬ��2ΪͼƬ·����
     std::string res = api->face_quality("d:\\12.jpg", 2);
    // ��ȡ�������ԣ�ͨ������ͼƬ������buffer
  /*  std::string out_buf;
    // ��ȡ�ļ���������buffer
    std::cout << "in test_face_quality" << std::endl;
    int buf_len = ImageBuf::get_buf("d:\\12.jpg", out_buf);
    std::cout << "in test_face_quality 1111" << std::endl;
    std::string res = api->face_quality_by_buf((unsigned char*)out_buf.c_str(), buf_len); */
    std::cout << "res is:" << res << std::endl;
}

// ͨ���޸�setting����Ķ�Ӧ���������ﵽ�������������Ե�Ŀ��
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
// ������Ϣ����������ע�ᣬ���£�������
void test_face_manager(BaiduFaceApi* api)
{
    Manager *mgrptr = new Manager();
    // ����ע��
    // mgrptr->user_add(api);
    // ����ע��
  //  mgrptr->batch_user_add(api, "d:\\image");
    //��������
   //  mgrptr->user_update(api);
    // ����ɾ��
    // mgrptr->user_face_delete(api);
    // �û�ɾ��
   // mgrptr->user_delete(api);
    // �û������
    //  mgrptr->group_add(api);
    // �û���ɾ�� 
    //  mgrptr->group_delete(api);
    // �û���Ϣ��ѯ�ӿ�
    mgrptr->get_user_info(api);
    // �û����б��ѯ�ӿ�
    //  mgrptr->get_user_list(api);
    //���б��ѯ�ӿ�
    //  mgrptr->get_group_list(api);
    delete mgrptr;
}
// ���Աȶԣ�1:1�����ȶԣ�1��N�����ȶ�,����ֵ�ȶԵ�
void test_compare(BaiduFaceApi* api)
{
    Compare *comptr = new Compare();
    //1:1�Ա�
   //  comptr->match(api);
    //1:N�Ա�
     comptr->identify(api);
   //  comptr->identify_by_feature(api);
   // comptr->identify(api);
    //ͼƬ����Ƶ�Ա�
   // comptr->match_by_img_and_frame(api, "d:\\1.jpg");
    //����Ϊ����ֵ����demo
    // comptr->get_face_feature_by_img(api);
   // comptr->get_face_feature_by_frame(api);
    // comptr->get_face_feature_by_buf(api);
     //����ֵ�Ƚ�
  //  comptr->compare_feature(api);
    // ���Ի�ȡ����ֵ������ֵ�Ƚ�(�Ƚ��ۺϣ��ɸ��ݲ�����ͬ�Ƚ�)
   // comptr->get_face_feature_and_match(api);
    delete comptr;
}

// ����ɼ������
void test_liveness_track(BaiduFaceApi* api)
{
    Liveness* liveptr = new Liveness();
    // liveptr->ir_liveness_track_by_img(api);
     // usb����ͷ���������ɼ�
     liveptr->usb_track_face_info(api);
    // rgbͼ����������ɼ�
    //rgb��Ŀ������
    // liveptr->rgb_liveness_track_by_img(api);
     // irͼ����е�Ŀ������
    // liveptr->ir_liveness_track_by_img(api);
    // liveptr->usb_rgb_track(api);
    // �Ե���ͼƬ���м��������Ϣ 
    // liveptr->image_track_face_info(api);
    // ���������ͼƬbuffer
   // liveptr->image_track_face_info_by_buf(api);
    //ir��rgb˫Ŀ������
    // liveptr->ir_rgb_track(api);
    // depth��Ȼ�����
   //  liveptr->rgb_depth_track_by_buf(api);
     // �±��й�˫Ŀ����ͷ��Ȼ���
    // liveptr->rgb_depth_track_by_mat_with_orbe(api);
     // ���ܰ���˫Ŀ����ͷ��Ȼ���
    // liveptr->rgb_depth_track_by_mat_with_hjimi(api);
     delete liveptr;
}


// ��ں���
/*int main()
{
    std::cout << "in main" << std::endl;
    //apiʵ��ָ��
    BaiduFaceApi *api = new BaiduFaceApi();
    //��ʼ��sdk
    // ������֤����ģʽ�����id_card��Ϊtrue������Ϊfalse��֤����ģʽ�ͷ�֤����ģʽ��ȡ����������ֵ��ͬ��
    // ���ܻ���
    bool id_card = false;
    api->sdk_init(id_card);
    // ��ǰ���������⵽�ڴ�
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
