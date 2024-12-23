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

//============================================================
//	�l�̍ő�l��������
//============================================================
template<class T> bool LimitMaxNum
(
	T& rNum,	// �������l
	const T max	// �ő�͈�
)
{
	if (rNum > max)
	{ // �������l���ő�͈͂𒴂��Ă����ꍇ

		// �͈͓��ɕ␳
		rNum = max;
		return true;
	}

	return false;
}
void PathToBaseName(std::string* pPath);
std::wstring MultiByteToWide(const std::string& rSrcStr);
float GetTexWidthFromAspect(const float fHeight, const int nTexIdx);
std::string WideToMultiByte(const std::wstring& rSrcStr);
void Adjust(D3DXVECTOR3* rot);
void Adjust(float* rot);

#define my_max(a,b)            (((a) > (b)) ? (a) : (b))// �傫�����l��Ԃ�
#define my_min(a,b)            (((a) < (b)) ? (a) : (b))// ���������l��Ԃ�

#define NUM_ARRAY(a)	(sizeof((a)) / sizeof((a)[0]))	// �z��̗v�f���v�Z

#define SAFE_DELETE(p)		if ((p) != nullptr) { delete (p);		(p) = nullptr; }	// �j��
#define SAFE_DELETEARRAY(p)		if ((p) != nullptr) { delete[] (p);		(p) = nullptr; }	// �j��
#define SAFE_UNINIT(p)		if ((p) != nullptr) { (p)->Uninit();	(p) = nullptr; }	// Uninit���g�p�����j��
#define SAFE_UNINIT_DELETE(p)		if ((p) != nullptr) { (p)->Uninit();	 delete (p);	(p) = nullptr; }	// Uninit���g�p�����j��
#define SAFE_RELEASE(p)		if ((p) != nullptr) { (p)->Release();		(p) = nullptr; }	// Release�֐��̔j���}�N��
#define SAFE_REF_RELEASE(p)	if ((p) != nullptr) { (p)->Release((p)); }						// �Q�ƃ|�C���^�t��Release�֐��̔j��
#define SAFE_DEL_OBJECT(p)	if ((p) != nullptr) { DeleteObject((p));	(p) = nullptr; }	// DeleteObject�֐��̔j���}�N��
#define VECTOR3_ZERO (D3DXVECTOR3(0.0f,0.0f,0.0))
#define VECTOR2_ZERO (D3DXVECTOR2(0.0f,0.0f))
#define VECTOR3_ONE (D3DXVECTOR3(1.0f,1.0f,1.0f))
#define VECTOR2_ONE (D3DXVECTOR2(1.0f,1.0f))


// POSGRID2�֌W
#define GRID2_ZERO	(POSGRID2())	// 0�N���A
#define GRID2_ONE	(POSGRID2(1))	// 1�N���A
//************************************************************
//	�񋓌^��`
//************************************************************
// ���z�u��
enum EAlignX
{
	XALIGN_LEFT = 0,	// ������
	XALIGN_CENTER,		// ��������
	XALIGN_RIGHT,		// �E����
	XALIGN_MAX,			// ���̗񋓌^�̑���
};

// �c�z�u��
enum EAlignY
{
	YALIGN_TOP = 0,	// �㑵��
	YALIGN_CENTER,	// ��������
	YALIGN_BOTTOM,	// ������
	YALIGN_MAX,		// ���̗񋓌^�̑���
};
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
// 2�������W
struct POSGRID2
{
	// �f�t�H���g�R���X�g���N�^
	POSGRID2() : x(0), y(0) {}

	// �����t���R���X�g���N�^
	POSGRID2(const int XY) : x(XY), y(XY) {}
	POSGRID2(const int X, const int Y) : x(X), y(Y) {}

	// �f�X�g���N�^
	~POSGRID2() {}

	// �����o�ϐ�
	int x;	// ������W (x)
	int y;	// ������W (y)

	//********************************************************
	//	���Z�q�I�[�o�[���[�h
	//********************************************************
	// ���Z
	inline POSGRID2 operator+(const POSGRID2& rGrid) const { return POSGRID2(x + rGrid.x, y + rGrid.y); }
	inline POSGRID2 operator+(const int nVal) const { return POSGRID2(x + nVal, y + nVal); }
	inline POSGRID2 operator+() const { return *this; }

	// ���Z
	inline POSGRID2 operator-(const POSGRID2& rGrid) const { return POSGRID2(x - rGrid.x, y - rGrid.y); }
	inline POSGRID2 operator-(const int nVal) const { return POSGRID2(x - nVal, y - nVal); }
	inline POSGRID2 operator-() const { return POSGRID2(-x, -y); }

	// ��Z
	inline POSGRID2 operator*(const int nVal) const { return POSGRID2(x * nVal, y * nVal); }

	// ���Z
	inline POSGRID2 operator/(const int nVal) const { return POSGRID2(x / nVal, y / nVal); }

	// ���Z���
	inline POSGRID2& operator+=(const int nVal)
	{
		x += nVal;
		y += nVal;
		return *this;
	}
	inline POSGRID2& operator+=(const POSGRID2& rGrid)
	{
		x += rGrid.x;
		y += rGrid.y;
		return *this;
	}

	// ���Z���
	inline POSGRID2& operator-=(const int nVal)
	{
		x -= nVal;
		y -= nVal;
		return *this;
	}
	inline POSGRID2& operator-=(const POSGRID2& rGrid)
	{
		x -= rGrid.x;
		y -= rGrid.y;
		return *this;
	}

	// ��Z���
	inline POSGRID2& operator*=(const int nVal)
	{
		x *= nVal;
		y *= nVal;
		return *this;
	}
	inline POSGRID2& operator*=(const POSGRID2& rGrid)
	{
		x *= rGrid.x;
		y *= rGrid.y;
		return *this;
	}

	// ���Z���
	inline POSGRID2& operator/=(const int nVal)
	{
		x /= nVal;
		y /= nVal;
		return *this;
	}
	inline POSGRID2& operator/=(const POSGRID2& rGrid)
	{
		x /= rGrid.x;
		y /= rGrid.y;
		return *this;
	}

	/*
		@brief	���W����v���邩�̌���
		@param	const POSGRID2& [in] ���肷�������W
		@return	���茋��
	*/
	inline bool operator==(const POSGRID2& rGrid) const
	{
		if (rGrid.x == x && rGrid.y == y) { return true; }
		return false;
	}

	/*
		@brief	���W����v���Ȃ����̌���
		@param	const POSGRID2& [in] ���肷�������W
		@return	���茋��
	*/
	inline bool operator!=(const POSGRID2& rGrid) const
	{
		if (rGrid.x != x || rGrid.y != y) { return true; }
		return false;
	}

	//********************************************************
	//	�����o�֐�
	//********************************************************
	/* @brief ������W�̔��] */
	inline void Invert() { *this = POSGRID2(-x, -y); }

	/* @brief ������W�̐�Βl */
	inline void Absolute() { *this = POSGRID2(std::abs(x), std::abs(y)); }

	/*
		@brief	�v�f�����ׂă[����
		@return	���茋��
	*/
	inline bool IsZero() const
	{
		if (x == 0 && y == 0) { return true; }
		return false;
	}
};

#endif