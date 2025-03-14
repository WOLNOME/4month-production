#pragma once

#include "AppShape.h"
#include "AppCollider.h"
#include "MyMath.h"

#include <vector>
#include <string>
#include <utility>

class AppCollisionManager
{
public:

	AppCollisionManager(const AppCollisionManager&) = delete;
	AppCollisionManager(const AppCollisionManager&&) = delete;
	AppCollisionManager& operator=(const AppCollisionManager&) = delete;
	AppCollisionManager& operator=(const AppCollisionManager&&) = delete;

	static AppCollisionManager* GetInstance()
	{
		static AppCollisionManager instance;
		return &instance;
	}

	void Initialize();
    void CheckAllCollision();
    void RegisterCollider(AppCollider* _collider);
    void ClearColliderList();
    void DeleteCollider(AppCollider* _collider);
    uint32_t GetNewAttribute(std::string _id);

    template <typename... Args>
    uint32_t* GetNewMask(std::string _id, Args... _ignoreNames)
    {
        uint32_t result = 0;
        for (auto& attributePair : attributeList_)
        {
            if (_id.compare(attributePair.first) == 0)
            {
                result = ~attributePair.second;
                break;
            }
        }
        for (std::string name : std::initializer_list<std::string>{ _ignoreNames... })
        {
            bool iscompare = false;
            for (auto& attributePair : attributeList_)
            {
                if (name.compare(attributePair.first) == 0)
                {
                    result ^= attributePair.second;
                    iscompare = true;
                }
            }
        }

        if (!result) result = ~result;

        uint32_t* resultPtr = nullptr;
        bool isCompare = false;
        /// 配列の中身を変更
        for (auto& maskPair : maskList_)
        {
            if (maskPair.first.compare(_id) == 0)
            {
                maskPair.second = result;
                resultPtr = &maskPair.second;
                isCompare = true;
            }
        }
        if (!isCompare) // マスクリストに登録されていなかったら
        {
            maskList_.push_back({ _id, result });
            resultPtr = &maskList_.back().second;
        }

        return resultPtr;
    }


private:
    AppCollisionManager() = default;

    std::vector<AppCollider*> colliders_;
    std::vector<std::pair<std::string, std::string>> collisionNames_;
    std::vector<std::pair<std::string, uint32_t>> attributeList_;
    std::list<std::pair<std::string, uint32_t>> maskList_;

    void CheckCollisionPair(AppCollider* _colA, AppCollider* _colB);
    void ProjectShapeOnAxis(const std::vector<Vector3>* _v, const Vector3& _axis, float& _min, float& _max);

    bool IsCollision(const AppAABB* _aabb1, const AppAABB* _aabb2);
    bool IsCollision(const AppAABB& _aabb, const AppSphere& _sphere);

    bool IsCollision(const AppSphere* _sphere1, const AppSphere* _sphere2);
    bool IsCollision(const AppOBB* _obb1, const AppOBB* _obb2);

    // bool IsCollision(const OBB& _obb, const Sphere& _sphere);

    float ProjectOntoAxis(const AppOBB* _obb, const Vector3& axis);
    bool OverlapOnAxis(const AppOBB* _obb1, const AppOBB* _obb2, const Vector3& axis);

    uint32_t countCheckCollision_ = 0ui32;
    uint32_t countWithoutFilter_ = 0ui32;
    uint32_t countWithoutLighter = 0ui32;
};

