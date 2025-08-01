#pragma once

#include <BaseCamera.h>

#include <Vector3.h>
#include <string>

class GameObject
{
public:
    GameObject();
    virtual ~GameObject() {};

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw(BaseCamera _camera) = 0;
    virtual void Finalize() = 0;
    

public: // ゲッター
    virtual Vector3 GetScale() const { return scale_; };
    virtual Vector3 GetRotation() const { return rotation_; };
    virtual Vector3 GetPosition() const { return position_; };
    virtual float   GetHP() const { return hp_; };
    virtual bool IsAttack() const { return isAttack_; };
	virtual Vector3 GetVelocity() const { return moveVel_; };

public: // セッター
    virtual void SetScale(const Vector3& _scale) { scale_ = _scale; };
    virtual void SetRotation(const Vector3& _rotation) { rotation_ = _rotation; };
    virtual void SetPosition(const Vector3& _position) { position_ = _position; };
    virtual void SetHP(float _hp) { hp_ = _hp; };

protected: // メンバー
    std::string objectName_;
    Vector3 scale_;
    Vector3 rotation_;
    Vector3 position_;
    float   hp_;
	bool isAttack_;
	Vector3 moveVel_ = { 0.01f,0.01f,0.01f };


};
