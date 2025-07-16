#include "BaseEnemy.h"
#include "GameCamera.h"

void BaseEnemy::RegistShake(float time, float power) {
	camera_->RegistShake(time, power);
}
