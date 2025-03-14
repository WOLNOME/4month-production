#pragma once

#include "Vector2.h"
#include "AppShape.h"
#include "../../application/objects/GameObject/GameObject.h"
#include "MyMath.h"

#include <vector>
#include <string>
#include <functional>
#include <list>
#include <variant>

class AppCollisionManager;

class AppCollider
{
public:

	AppCollider();
	~AppCollider();

public: // ゲッター

    inline const GameObject* GetOwner()const { return owner_; }

    inline const AppAABB* GetAABB()const { return std::get<AppAABB*>(shapeData_); }
    inline const AppOBB* GetOBB()const { return std::get<AppOBB*>(shapeData_); }
    inline const AppSphere* GetSphere()const { return std::get<AppSphere*>(shapeData_); }

    inline uint32_t GetCollisionAttribute()const { return collisionAttribute_; }
    inline uint32_t GetCollisionMask()const { return *pCollisionMask_; }
    inline AppShape GetShape()const { return shape_; }
    inline unsigned int GetRadius() const { return radiusCollider_; }
    inline const std::string& GetColliderID() const { return colliderID_; }
    inline bool GetIsEnableLighter() const { return enableLighter_; }
    inline Vector3 GetPosition() const { return position_; }
    inline const bool GetEnable() const { return isEnableCollision_; }

    const bool IsRegisteredCollidingPtr(const AppCollider* _ptr) const;
    void EraseCollidingPtr(const AppCollider* _ptr);

public: // セッター
    inline void SetOwner(GameObject* _owner) { owner_ = _owner; }
    inline void SetColliderID(const std::string& _id)
    {
        colliderID_ = _id;
    }

    template<typename T>
    void SetShapeData(T* _shapeData) { shapeData_ = _shapeData; }    
    void SetShape(AppShape _shape) { shape_ = _shape; }

    void SetAttribute(uint32_t _attribute);
    void SetMask(uint32_t* _mask);
    void SetOnCollision(const std::function<void(const AppCollider*)>& _func) { onCollisionFunction_ = _func; }
    void SetOnCollisionTrigger(const std::function<void(const AppCollider*)>& _func) { onCollisionTriggerFunction_ = _func; }
    void SetRadius(unsigned int _rad) { radiusCollider_ = _rad; }
    void SetPosition(const Vector3& _v) { position_ = _v; }
    void SetEnableLighter(bool _flag) { enableLighter_ = _flag; }
    void SetEnable(bool _flag) { isEnableCollision_ = _flag; }
    void RegisterCollidingPtr(const AppCollider* _ptr) { collidingPtrList_.push_back(_ptr); }


    inline void OnCollision(const AppCollider* _other)
    {
        if (onCollisionFunction_)
            onCollisionFunction_(_other);
        return;
    }

    void OnCollisionTrigger(const AppCollider* _other);

private:

    std::function<void(const AppCollider*)> onCollisionFunction_;
    std::function<void(const AppCollider*)> onCollisionTriggerFunction_;
    std::variant<AppOBB*, AppAABB*, AppSphere*> shapeData_ = {};

    GameObject* owner_ = nullptr;
    bool isEnableCollision_ = true; // 判定をするかどうか
    AppShape shape_ = AppShape::AppSphere; // 形状
    std::string colliderID_ = {}; // ID


    std::list<const AppCollider*> collidingPtrList_ = {}; // 現在あたっているコライダーのリスト

    // 軽量化用
    unsigned int radiusCollider_ = 0u;
    Vector3 position_ = {};
    bool enableLighter_ = false;

    // 衝突属性(自分)
    uint32_t collisionAttribute_ = 0xffffffff;
    // 衝突マスク(相手)
    uint32_t* pCollisionMask_ = new uint32_t(0xffffffff);

};

