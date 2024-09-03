#include "CollisionManager.h"
#include <cassert>

#include "BaseCollider.h"
#include "BoxCollider.h"

#include <ImGuiManager.h>

void CollisionManager::AddGameObject(BaseGameObject* collider) {
	gameObjects_.push_back(collider);
}

void CollisionManager::SubGameObject(BaseGameObject* collider) {
	auto itr = std::find(gameObjects_.begin(), gameObjects_.end(), collider);
	assert(itr != gameObjects_.end());
	gameObjects_.erase(itr);

	collidedPairs_.remove_if([&collider](const CollidedPair& pair) {
		return pair.first == collider || pair.second == collider;
	});

	currentCollidedPairs_.remove_if([&collider](const CollidedPair& pair) {
		return pair.first == collider || pair.second == collider;
	});

}


void CollisionManager::Update() {

	currentCollidedPairs_.clear();

	for(auto& object : gameObjects_) {
		if(object->GetCollider()) {
			object->GetCollider()->Update();
		}
	}

	for(auto& objectA : gameObjects_) {
		for(auto& objectB : gameObjects_) {

			if(objectA == objectB) { continue; }

			auto it = std::find_if(currentCollidedPairs_.begin(), currentCollidedPairs_.end(), [&objectA, &objectB](const CollidedPair& pair) {
				return (pair.first == objectA && pair.second == objectB)
					|| (pair.first == objectB && pair.second == objectA);
			});

			if(it != currentCollidedPairs_.end()) {
				continue;
			}

			if(objectA->GetTag() != objectB->GetTag()) {
				CheckCollision(objectA, objectB);
			}
		}
	}

}


void CollisionManager::DrawHitBoxALL() {
	for(auto& gameObject : gameObjects_) {
		if(!gameObject->isDrawActive) { continue; }
		BaseCollider* collider = gameObject->GetCollider();
		if(collider) {
			collider->Draw();
		}
	}
}

void CollisionManager::CheckCollision(BaseGameObject* a, BaseGameObject* b) {
	BaseCollider* aCollider = a->GetCollider();
	BaseCollider* bCollider = b->GetCollider();

	if(!aCollider || !bCollider) {
		return;
	}

	CollidedPair pairA = std::make_pair(a, b);
	CollidedPair pairB = std::make_pair(b, a);

	if(aCollider->IsCollision(bCollider)) {

		/// Listないのpairの数を数える
		int64_t aCount = std::count(collidedPairs_.begin(), collidedPairs_.end(), pairA);
		int64_t bCount = std::count(collidedPairs_.begin(), collidedPairs_.end(), pairB);

		if(aCount == 0 && bCount == 0) {
			/// listないになければ衝突した瞬間なのでEnterを呼ぶ
			a->OnCollisionEnter(b);
			b->OnCollisionEnter(a);
#ifdef _DEBUG
			pairNames_.push_back("Enter :  " + a->GetName() + "  to  " + b->GetName());
#endif // _DEBUG

			/// Listに追加する
			collidedPairs_.push_back(pairA);


		} else {
			/// あったら衝突しているのでStayを呼ぶ
			a->OnCollisionStay(b);
			b->OnCollisionStay(a);
#ifdef _DEBUG
			pairNames_.push_back("Stay  :  " + a->GetName() + "  to  " + b->GetName());
#endif // _DEBUG
		}


		/// Listに追加する
		currentCollidedPairs_.push_back(pairA);


	} else {

		/// List内にpairが何個あるか数える
		int64_t aCount = std::count(collidedPairs_.begin(), collidedPairs_.end(), pairA);
		int64_t bCount = std::count(collidedPairs_.begin(), collidedPairs_.end(), pairB);

		/// List内にあったらExitをよんでListからpairを削除
		if(aCount != 0 || bCount != 0) {
			a->OnCollisionExit(b);
			b->OnCollisionExit(a);
			collidedPairs_.remove_if([&pairA, &pairB](const CollidedPair& elem) {
				return elem.first == pairA.first && elem.second == pairA.second
					|| elem.first == pairB.first && elem.second == pairB.second;
			});

#ifdef _DEBUG
			pairNames_.push_back("Exit  :  " + a->GetName() + "  to  " + b->GetName());
#endif // _DEBUG

		}

	}

}

void CollisionManager::ImGuiDebug() {
#ifdef _DEBUG
	if(!ImGui::Begin("collision manager")) {
		ImGui::End();
		return;
	}

	if(ImGui::Button("clear : hit checker")) {
		pairNames_.clear();
	}

	if(ImGui::TreeNodeEx("hit checker")) {
		for(auto& pairName : pairNames_) {
			ImGui::Text(pairName.c_str());
		}
		ImGui::TreePop();
	}

	ImGui::Separator();

	if(ImGui::TreeNodeEx("collision pair")) {
		for(auto& pair : collidedPairs_) {
			std::string str = pair.first->GetName() + "  to  " + pair.second->GetName();
			ImGui::Text(str.c_str());
		}
		ImGui::TreePop();
	}

	ImGui::Separator();

	if(ImGui::TreeNodeEx("current collision pair")) {
		for(auto& pair : currentCollidedPairs_) {
			std::string str = pair.first->GetName() + "  to  " + pair.second->GetName();
			ImGui::Text(str.c_str());
		}
		ImGui::TreePop();
	}

	ImGui::End();


	while(pairNames_.size() >= 100) {
		pairNames_.pop_front();
	}

#endif // _DEBUG
}

void CollisionManager::Reset() {
	gameObjects_.clear();
}
