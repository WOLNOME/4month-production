#include "AppCollider.h"

#include "AppColliderManager.h"

#include <sstream>

AppCollider::AppCollider()
{
}

AppCollider::~AppCollider()
{
}

AppCollider::AppColliderDesc AppCollider::MakeAABBDesc(GameObject* owner, const std::string& colliderID, AppAABB* aabb, AppColliderManager* mgr, std::function<void(const AppCollider*)> onCollision, std::function<void(const AppCollider*)> onCollisionTrigger)
{
	AppColliderDesc desc;
	desc.owner = owner;
	desc.colliderID = colliderID;
	desc.shape = AppShape::AppAABB;
	desc.shapeData = aabb;
	desc.attribute = mgr->GetNewAttribute(colliderID);
	desc.onCollision = onCollision;
	desc.onCollisionTrigger = onCollisionTrigger;
	return desc;
}

AppCollider::AppColliderDesc AppCollider::MakeOBBDesc(GameObject* owner, const std::string& colliderID, AppOBB* obb, AppColliderManager* mgr, std::function<void(const AppCollider*)> onCollision, std::function<void(const AppCollider*)> onCollisionTrigger)
{
	AppColliderDesc desc;
	desc.owner = owner;
	desc.colliderID = colliderID;
	desc.shape = AppShape::AppOBB;
	desc.shapeData = obb;
	desc.attribute = mgr->GetNewAttribute(colliderID);
	desc.onCollision = onCollision;
	desc.onCollisionTrigger = onCollisionTrigger;
	return desc;
}

AppCollider::AppColliderDesc AppCollider::MakeSphereDesc(GameObject* owner, const std::string& colliderID, AppSphere* sphere, AppColliderManager* mgr, std::function<void(const AppCollider*)> onCollision, std::function<void(const AppCollider*)> onCollisionTrigger)
{
	AppColliderDesc desc;
	desc.owner = owner;
	desc.colliderID = colliderID;
	desc.shape = AppShape::AppSphere;
	desc.shapeData = sphere;
	desc.attribute = mgr->GetNewAttribute(colliderID);
	desc.onCollision = onCollision;
	desc.onCollisionTrigger = onCollisionTrigger;
	return desc;
}

const bool AppCollider::IsRegisteredCollidingPtr(const AppCollider* _ptr) const
{
	for (auto itr = collidingPtrList_.begin(); itr != collidingPtrList_.end(); ++itr)
	{
		if (_ptr == *itr) return true;
	}
	return false;
}

void AppCollider::EraseCollidingPtr(const AppCollider* _ptr)
{
	collidingPtrList_.remove_if([_ptr](const AppCollider* _pCollider) {
		return _pCollider == _ptr;
		});
	return;
}

void AppCollider::SetAttribute(uint32_t _attribute)
{
	collisionAttribute_ = _attribute;
}

void AppCollider::SetMask(uint32_t* _mask)
{
	pCollisionMask_ = _mask;
}

void AppCollider::OnCollisionTrigger(const AppCollider* _other)
{
	if (onCollisionTriggerFunction_)
		onCollisionTriggerFunction_(_other);
	return;
}
