#include "main.h"
#include <memory>
#include <mutex>
//=============================================
//�֗��֐�
//=============================================
using namespace std;
#ifndef CONVENIENCE
#define CONVENIENCE		// ��d�C���N���[�h�h�~�p�}�N�����`
float GetDistance(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2);
D3DXVECTOR3 VectorToAngles(const D3DXVECTOR3& vector);
D3DXVECTOR3 AnglesToVector(const D3DXVECTOR3& angles);
void Adjust(D3DXVECTOR3& rot);
void Adjust(float& rot);

#define my_max(a,b)            (((a) > (b)) ? (a) : (b))// �傫�����l��Ԃ�

#define my_min(a,b)            (((a) < (b)) ? (a) : (b))// ���������l��Ԃ�

#define SAFE_DELETE(p)		if ((p) != nullptr) { delete (p);		(p) = nullptr; }	// �j��

#define SAFE_DELETEARRAY(p)		if ((p) != nullptr) { delete[] (p);		(p) = nullptr; }	// �j��

#define SAFE_UNINIT(p)		if ((p) != nullptr) { (p)->Uninit();	(p) = nullptr; }	// Uninit���g�p�����j��

#define SAFE_UNINIT_DELETE(p)		if ((p) != nullptr) { (p)->Uninit();	 delete (p);	(p) = nullptr; }	// Uninit���g�p�����j��

#define VECTOR3_ZERO (D3DXVECTOR3(0.0f,0.0f,0.0))
#define VECTOR3_ONE (D3DXVECTOR3(1.0f,1.0f,1.0f))

//�ėp�e���v���[�g
template <typename T> class CMaster
{
public:
	CMaster() : value(make_shared<T>()) {};

	//�e��A�N�Z�T���\�b�h
	T Get() { return *value.get(); };										//�Q�b�g
	T Get(mutex mut) { lock_guard<mutex> lock(mut); return value.get(); };	//�Q�b�g(�X���b�h�Z�[�t)
	shared_ptr<T> GetPtr() { return value; };								//�{�̎擾

	void Set(T in) { *value = in; };										//�Z�b�g
	void Set(T in, mutex mut) { lock_guard<mutex> lock(mut); *value = in; };//�Z�b�g(�X���b�h�Z�[�t)
	void SetPtr(shared_ptr<T> in) { value = in; };							//�{�̑��
																			
private:
	shared_ptr<T> value;//�{��
};


#endif