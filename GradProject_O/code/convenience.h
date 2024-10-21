#include "main.h"
#include <memory>
#include <mutex>
//=============================================
//�����擾�֐�
//=============================================
using namespace std;
#ifndef CONVENIENCE
float GetDistance(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2);
#ifndef my_max
#define my_max(a,b)            (((a) > (b)) ? (a) : (b))// �傫�����l��Ԃ�
#endif

#ifndef my_min
#define my_min(a,b)            (((a) < (b)) ? (a) : (b))// ���������l��Ԃ�
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)		if ((p) != nullptr) { delete (p);		(p) = nullptr; }	// �j��
#endif

#ifndef SAFE_UNINIT
#define SAFE_UNINIT(p)		if ((p) != nullptr) { (p)->Uninit();	(p) = nullptr; }	// Uninit���g�p�����j��
#endif



//�ėp�e���v���[�g
template <typename T> class CMaster
{
public:
	CMaster() : value(make_shared<T>()) {};

	//�e��A�N�Z�T���\�b�h
	T Get() { return value.get(); };										//�Q�b�g
	T Get(mutex mut) { lock_guard<mutex> lock(mut); return value.get(); };	//�Q�b�g(�X���b�h�Z�[�t)
	shared_ptr<T> GetPtr() { return value; };								//�{�̎擾

	void Set(T in) { *value = in; };										//�Z�b�g
	void Set(T in, mutex mut) { lock_guard<mutex> lock(mut); *value = in; };//�Z�b�g(�X���b�h�Z�[�t)
	void SetPtr(shared_ptr<T> in) { value = in; };							//�{�̑��
																			
private:
	shared_ptr<T> value;//�{��
};



#endif