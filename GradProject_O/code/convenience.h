#include "main.h"
//=============================================
//�����擾�֐�
//=============================================
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