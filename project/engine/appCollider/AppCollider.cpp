#include "AppCollider.h"

#include "AppColliderManager.h"

#include <sstream>

AppCollider::AppCollider()
{
}

AppCollider::~AppCollider()
{
}

void AppCollider::MakeAABBDesc(const AppColliderDesc& desc)
{
	SetOwner(desc.owner);
	SetColliderID(desc.colliderID);
	SetShape(AppShape::AppAABB);
	SetShapeData(static_cast<AppAABB*>(desc.shapeData));
	SetAttribute(desc.attribute);
	if (desc.onCollision) SetOnCollision(desc.onCollision);
	if (desc.onCollisionTrigger) SetOnCollisionTrigger(desc.onCollisionTrigger);

}

void AppCollider::MakeOBBDesc(const AppColliderDesc& desc)
{
	SetOwner(desc.owner);
	SetColliderID(desc.colliderID);
	SetShape(AppShape::AppOBB);
	SetShapeData(static_cast<AppOBB*>(desc.shapeData));
	SetAttribute(desc.attribute);
	if (desc.onCollision) SetOnCollision(desc.onCollision);
	if (desc.onCollisionTrigger) SetOnCollisionTrigger(desc.onCollisionTrigger);
}

void AppCollider::MakeSphereDesc(const AppColliderDesc& desc)
{
	SetOwner(desc.owner);
	SetColliderID(desc.colliderID);
	SetShape(AppShape::AppSphere);
	SetShapeData(static_cast<AppSphere*>(desc.shapeData));
	SetAttribute(desc.attribute);
	if (desc.onCollision) SetOnCollision(desc.onCollision);
	if (desc.onCollisionTrigger) SetOnCollisionTrigger(desc.onCollisionTrigger);
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
