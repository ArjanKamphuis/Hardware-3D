#include "ChiliXM.h"

namespace ChiliXM
{
    using namespace DirectX;

    XMVECTOR XM_CALLCONV ExtractEulerAngles(FXMMATRIX matrix)
    {
        XMFLOAT4X4 mat;
        XMStoreFloat4x4(&mat, matrix);

        XMFLOAT3 euler;
        euler.x = asinf(-mat._32);

        if (cosf(euler.x) > 0.0001)
        {
            euler.y = atan2f(mat._31, mat._33);
            euler.z = atan2f(mat._12, mat._22);
        }
        else
        {
            euler.y = 0.0f;
            euler.z = atan2f(-mat._21, mat._11);
        }

        return XMLoadFloat3(&euler);
    }

    XMVECTOR XM_CALLCONV ExtractTranslation(FXMMATRIX matrix)
    {
        XMFLOAT4X4 mat;
        XMStoreFloat4x4(&mat, matrix);
        return XMVectorSet(mat._41, mat._42, mat._43, 1.0f);
    }

    XMMATRIX XM_CALLCONV ScaleTranslation(XMMATRIX matrix, float scale)
    {
        matrix.r[3].m128_f32[0] *= scale;
        matrix.r[3].m128_f32[1] *= scale;
        matrix.r[3].m128_f32[2] *= scale;
        return matrix;
    }
}
