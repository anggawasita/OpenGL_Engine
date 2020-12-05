#pragma once
#include "pch.h"
#include <random>

using namespace Doom;

class Coin: public GameObject{
public:
	RectangleCollider2D* col = nullptr;
	Transform* tr = nullptr;
	bool playanim = false;
	bool secondphase = false;
	float prevscale[2];
	Sound* coinSound = nullptr;

	Coin(const std::string name = "Coin", float x = 0, float y = 15) :GameObject(name, x, y) {
		EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, (GameObject*)this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONCOLLSION, (GameObject*)this);
		col = GetComponentManager()->AddComponent<RectangleCollider2D>();
		tr = GetComponentManager()->GetComponent<Transform>();
		col->SetOwner(this);
		tr->Scale(3, 3);
		col->IsTrigger = true;
		col->SetTag("Coin");
		prevscale[0] = GetScale()[0];
		prevscale[1] = GetScale()[1];
	}

	~Coin() {
	}

	virtual void OnUpdate() override {

		if (playanim) {

			if (!secondphase) {
				tr->Scale(GetScale()[0] *  (1 + (5.2f * DeltaTime::s_Deltatime)), GetScale()[1] * (1 + (5.2f * DeltaTime::s_Deltatime)));
				if (GetScale()[0] > prevscale[0] * 1.5f)
					secondphase = true;
			}
			else if (secondphase) {
				tr->Scale(GetScale()[0] * (1 - (5.f * DeltaTime::s_Deltatime)), GetScale()[1] * (1 - (5.f * DeltaTime::s_Deltatime)));
			}

			if (GetScale()[0] < prevscale[0] * 0.5f) {
				auto rand = std::bind(&Coin::Randomize, this);
				rand();
				playanim = false;
				secondphase = false;
				tr->Scale(3,3);
			}
		}
		col->IsCollidedDIAGS();
		if (col->isCollided) {
			EventSystem::GetInstance()->SendEvent(EventType::ONCOLLSION, this, col->collidedObject);
		}
		tr->Move(0,-5,0);
		if (GetPosition().y <= -18) {
			Randomize();
			EventSystem::GetInstance()->SendEvent(EventType::ONMISS, nullptr);
		}
		
	}

	virtual void OnCollision(void* _col)override {
		RectangleCollider2D* col = static_cast<RectangleCollider2D*>(_col);
		if (col->GetTag() == "Player") {
			SoundManager::Play(coinSound);
			this->col->Enable = false;
			Player* player = dynamic_cast<Player*>(col->GetOwnerOfComponent());
			player->scores++;
			player = nullptr;
			playanim = true;
		}
		else if(col->GetTag() == "Coin"){
			Randomize();
		}
		col = nullptr;
		
	}

	void Randomize() {
		m_Enable = true;
		this->col->Enable = true;
		std::random_device rd;
		std::mt19937 e2(rd());
		std::uniform_real_distribution<float> distribution(-30.f, 30.f);
		float x = distribution(e2);
		std::uniform_real_distribution<float> distribution1(0.f, 5.f);
		float y = distribution1(e2);
		tr->Translate(x, 30 + y);
		tr->Scale(3, 3);
	}
};