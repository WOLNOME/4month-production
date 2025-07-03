#include "AppCollider.h"

#include "AppCollisionManager.h"

#include <sstream>

AppCollider::AppCollider()
{
}

AppCollider::~AppCollider()
{
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
