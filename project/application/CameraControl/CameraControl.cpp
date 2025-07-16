#include "CameraControl.h"
#include "../../application/MathUtils.h"
#include "MyMath.h"
#include "BaseCamera.h"
#include "../../application/enemy/EnemyManager.h"
#include "../../application/objects/Player/PlayerManager.h"

void CameraControl::Initialize(const Vector3& cameraRotate)
{
	cameraRotate_ = cameraRotate;
}

void CameraControl::Update(BaseCamera* camera, PlayerManager* players, EnemyManager* enemyManager)
{
	CalculateNearestPosition(players, enemyManager);

	//　カメラの演出
	if (isZoomIn_)
	{
		UpdateZoomIn(camera);
	}
	else if (waitTime_ > 0.0f)
	{
		waitTime_ -= 1.0f / 60.0f;
		if (waitTime_ <= 0.0f)
		{
			isZoomOut_ = true;
		}
	}
	else if (isZoomOut_)
	{
		UpdateZoomOut(camera);
	}
}

void CameraControl::OnGameEnd(uint32_t playerNum, uint32_t EnemyNum)
{
	isZoomIn_ = true;
	if (playerNum <= 0)
	{
		cameraEndPosition_ = nearestEnemyPos_;
	}
	else if (EnemyNum == 0)
	{
		cameraEndPosition_ = nearestPlayerPos_;
	}
}

bool CameraControl::IsGameEnd()
{
	return (!isZoomIn_ and cameraEaseTime_ >= cameraEaseDuration_ / 4.0f);
}

void CameraControl::UpdateZoomIn(BaseCamera* camera)
{
	// 時間を経過させる
	cameraEaseTime_ += 1.0f / 60.0f;
	float t = cameraEaseTime_ / cameraEaseDuration_;
	// 補完が1.0fを超えないようにする
	if (t > 1.0f) t = 1.0f;

	// 補完のイージング
	float easeT = EaseInOutQuad(t);
	//カメラのスタート地点とエンド地点を設定
	Vector3 cameraStartPosition = cameraStartPosition_;
	Vector3 cameraEndPosition = cameraEndPosition_;
	cameraEndPosition.y = 2.0f;
	cameraEndPosition.z -= 14.0f;
	// カメラの補間
	Vector3 newPosition = cameraStartPosition_ * (1.0f - easeT) + cameraEndPosition * easeT;
	Vector3 newRotation = cameraRotate_ * (1.0f - easeT) + CalculateLookAtRotation(newPosition, cameraEndPosition_) * easeT; // カメラの向きを補間
	newRotation -= 0.2f;

	// カメラに新しい位置と回転を設定
	camera->SetTranslate(newPosition);
	camera->SetRotate(newRotation);

	//補間が終わったら
	if (cameraEaseTime_ >= 3.0f)
	{
		isZoomIn_ = false;
		cameraEaseTime_ = 0.0f;
		waitTime_ = waitTimeDuration_;
	}
}

void CameraControl::UpdateZoomOut(BaseCamera* camera)
{
	// 時間を経過させる
	cameraEaseTime_ += 1.0f / 60.0f;
	float t = cameraEaseTime_ / cameraEaseDuration_;
	// 補完が1.0fを超えないようにする
	if (t > 1.0f) t = 1.0f;

	// 補完のイージング
	float easeT = EaseInOutQuad(t);
	Vector3 cameraEndPosition = cameraEndPosition_;
	cameraEndPosition.y = 2.0f;
	cameraEndPosition.z -= 14.0;
	// カメラの補間
	Vector3 newPosition = cameraEndPosition * (1.0f - easeT) + cameraStartPosition_ * easeT;
	Vector3 newRotation = CalculateLookAtRotation(newPosition, cameraEndPosition_) * (1.0f - easeT) + cameraRotate_ * easeT;
	Vector3 cameraRotate = { 0.95f, 0.0f, 0.0f };
	newRotation = newRotation * (1.0f - easeT) + cameraRotate * easeT;

	// カメラに新しい位置と回転を設定
	camera->SetTranslate(newPosition);
	camera->SetRotate(newRotation);

	//補間が終わったら
	if (t >= 1.0f)
	{
		isZoomOut_ = false;
		cameraEaseTime_ = 0.0f;
	}
}

void CameraControl::CalculateNearestPosition(PlayerManager* players, EnemyManager* enemyManager)
{
	// プレイヤーが残り1人のとき
	if (players->GetPlayers().size() == 1)
	{
		//最も近い敵の座標を取得
		float closestDistance_ = (std::numeric_limits<float>::max)();

		//最も近い敵の座標を取得
		//タックルエネミー
		for (uint32_t i = 0; i < enemyManager->GetTackleEnemyCount(); i++)
		{
			//地面にいないエネミーは無視
			if (!enemyManager->GetTackleEnemy(i)->IsGround()) { continue; }
			Vector3 enemyPos = enemyManager->GetTackleEnemyPosition(i);
			float distance = (players->GetPlayerPosition(0) - enemyPos).Length();
			if (distance < closestDistance_)
			{
				closestDistance_ = distance;
				nearestEnemyPos_ = enemyPos;
				nearestEnemyType_ = "TackleEnemy";
				nearestEnemyNum_ = i;
			}
		}
		//ファンエネミー
		for (uint32_t i = 0; i < enemyManager->GetFanEnemyCount(); i++)
		{
			//地面にいないエネミーは無視
			if (!enemyManager->GetFanEnemy(i)->IsGround()) { continue; }
			Vector3 enemyPos = enemyManager->GetFanEnemyPosition(i);
			float distance = (players->GetPlayerPosition(0) - enemyPos).Length();
			if (distance < closestDistance_)
			{
				closestDistance_ = distance;
				nearestEnemyPos_ = enemyPos;
				nearestEnemyType_ = "FanEnemy";
				nearestEnemyNum_ = i;
			}
		}
		//フリーズエネミー
		for (uint32_t i = 0; i < enemyManager->GetFreezeEnemyCount(); i++)
		{
			if (!enemyManager->GetFreezeEnemy(i)->IsGround()) { continue; }
			Vector3 enemyPos = enemyManager->GetFreezeEnemyPosition(i);
			float distance = (players->GetPlayerPosition(0) - enemyPos).Length();
			if (distance < closestDistance_)
			{
				closestDistance_ = distance;
				nearestEnemyPos_ = enemyPos;
				nearestEnemyType_ = "FreezeEnemy";
			}
		}
		if (closestDistance_ == (std::numeric_limits<float>::max)())
		{
			//敵がみんな地面にいない時はフィールドの中心を向く
			nearestEnemyPos_ = { 0.0f, 1.0f, 0.0f };
			nearestEnemyType_ = "";
		}
	}

	// エネミーが残り1体のとき
	if (enemyManager->GetEnemyCount() == 1)
	{
		//最も近いプレイヤーの座標を取得
		float closestDistance_ = (std::numeric_limits<float>::max)();
		//最も近いプレイヤーの座標を取得
		for (uint32_t i = 0; i < players->GetPlayers().size(); i++)
		{
			//地面にいないプレイヤーは無視
			if (!players->IsPlayerGrounded(i)) { continue; }
			Vector3 playerPos = players->GetPlayerPosition(i);
			float distance;
			//タックルエネミー
			for (uint32_t j = 0; j < enemyManager->GetTackleEnemyCount(); j++)
			{
				Vector3 enemyPos = enemyManager->GetTackleEnemyPosition(j);
				distance = (playerPos - enemyPos).Length();
				if (distance < closestDistance_)
				{
					closestDistance_ = distance;
					nearestPlayerPos_ = playerPos;
					nearestPlayerNum_ = i;
				}
			}
			//ファンエネミー
			for (uint32_t j = 0; j < enemyManager->GetFanEnemyCount(); j++)
			{
				Vector3 enemyPos = enemyManager->GetFanEnemyPosition(j);
				distance = (playerPos - enemyPos).Length();
				if (distance < closestDistance_)
				{
					closestDistance_ = distance;
					nearestPlayerPos_ = playerPos;
					nearestPlayerNum_ = i;
				}
			}
			//フリーズエネミー
			for (uint32_t j = 0; j < enemyManager->GetFreezeEnemyCount(); j++)
			{
				Vector3 enemyPos = enemyManager->GetFreezeEnemyPosition(j);
				distance = (playerPos - enemyPos).Length();
				if (distance < closestDistance_)
				{
					closestDistance_ = distance;
					nearestPlayerPos_ = playerPos;
					nearestPlayerNum_ = i;
				}
			}
		}
		if (closestDistance_ == (std::numeric_limits<float>::max)())
		{
			//プレイヤーがみんな地面にいない時はフィールドの中心を向く
			nearestPlayerPos_ = { 0.0f, 1.0f, 0.0f };
			nearestPlayerNum_ = (std::numeric_limits<uint32_t>::max)();
		}

	}

}
