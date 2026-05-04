#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <algorithm>
#include "Entity.h"

namespace Engine::ECS {

///
/// コンポーネントの基底ストレージ
///
class IComponentStorage {
public:
	virtual ~IComponentStorage() = default;
	virtual void Remove(Entity entity) = 0;
	virtual const std::vector<Entity>& GetEntities() const = 0;
};

///
/// Sparse Set を使用したコンポーネントストレージ
///
template <typename T>
class ComponentStorage final : public IComponentStorage {
public:
    static constexpr size_t kChunkSize = 1024;

	T& Add(Entity entity, T&& component) {
		if (Has(entity)) {
			*GetPtr(entityToIndex_[entity]) = std::move(component);
			return *GetPtr(entityToIndex_[entity]);
		}

		uint32_t index = static_cast<uint32_t>(count_);
        EnsureCapacity(index + 1);

		entityToIndex_[entity] = index;
		indexToEntity_.push_back(entity);
        *GetPtr(index) = std::move(component);
        count_++;
		return *GetPtr(index);
	}

	void Remove(Entity entity) override {
		if (!Has(entity)) return;

		uint32_t index = entityToIndex_[entity];
		uint32_t lastIndex = static_cast<uint32_t>(count_ - 1);
		Entity lastEntity = indexToEntity_[lastIndex];

		// 末尾要素と入れ替えて削除
		*GetPtr(index) = std::move(*GetPtr(lastIndex));
		indexToEntity_[index] = lastEntity;
		entityToIndex_[lastEntity] = index;

		count_--;
		indexToEntity_.pop_back();
		entityToIndex_.erase(entity);
	}

	bool Has(Entity entity) const {
		return entityToIndex_.find(entity) != entityToIndex_.end();
	}

	T& Get(Entity entity) {
		return *GetPtr(entityToIndex_[entity]);
	}

    T* GetPtr(uint32_t index) {
        size_t chunkIdx = index / kChunkSize;
        size_t localIdx = index % kChunkSize;
        return &chunks_[chunkIdx][localIdx];
    }

    const T* GetPtr(uint32_t index) const {
        size_t chunkIdx = index / kChunkSize;
        size_t localIdx = index % kChunkSize;
        return &chunks_[chunkIdx][localIdx];
    }

	const std::vector<Entity>& GetEntities() const override { return indexToEntity_; }

    // Interop 用: チャンクのポインタを返す
    T* GetChunkPtr(size_t chunkIndex) {
        if (chunkIndex >= chunks_.size()) return nullptr;
        return chunks_[chunkIndex].get();
    }

private:
    void EnsureCapacity(size_t capacity) {
        while (count_ < capacity) {
            if (count_ % kChunkSize == 0) {
                chunks_.push_back(std::make_unique<T[]>(kChunkSize));
            }
            break; // Once we have enough for 'capacity', we can stop if we only add one at a time
        }
        // Correct logic:
        size_t neededChunks = (capacity + kChunkSize - 1) / kChunkSize;
        while (chunks_.size() < neededChunks) {
            chunks_.push_back(std::make_unique<T[]>(kChunkSize));
        }
    }

	std::vector<std::unique_ptr<T[]>> chunks_;
    size_t count_ = 0;
	std::vector<Entity> indexToEntity_;
	std::unordered_map<Entity, uint32_t> entityToIndex_;
};

///
/// ECSのレジストリクラス
///
class Registry final {
public:
	Entity CreateEntity() {
		return ++nextEntityId_;
	}

	void DestroyEntity(Entity entity) {
		for (auto& pair : storages_) {
			pair.second->Remove(entity);
		}
	}

	template <typename T>
	T& AddComponent(Entity entity, T&& component = T()) {
		return GetStorage<T>().Add(entity, std::forward<T>(component));
	}

	template <typename T>
	void RemoveComponent(Entity entity) {
		GetStorage<T>().Remove(entity);
	}

	template <typename T>
	T& GetComponent(Entity entity) {
		return GetStorage<T>().Get(entity);
	}

	template <typename T>
	bool HasComponent(Entity entity) {
		return GetStorage<T>().Has(entity);
	}

	template <typename T>
	ComponentStorage<T>& GetStorage() {
		auto type = std::type_index(typeid(T));
		if (storages_.find(type) == storages_.end()) {
			storages_[type] = std::make_unique<ComponentStorage<T>>();
		}
		return *static_cast<ComponentStorage<T>*>(storages_[type].get());
	}

	template <typename... Components>
	class View {
	public:
		View(Registry& registry) : registry_(registry) {}

		template <typename Func>
		void Each(Func func) {
			// 最小のストレージを見つけて、それをベースにループを回す
			auto* smallestStorage = GetSmallestStorage<Components...>();
			if (!smallestStorage) return;

			const auto& entities = smallestStorage->GetEntities();
			for (Entity entity : entities) {
				if ((registry_.HasComponent<Components>(entity) && ...)) {
					func(entity, registry_.GetComponent<Components>(entity)...);
				}
			}
		}

	private:
		template <typename C, typename... Rest>
		IComponentStorage* GetSmallestStorage() {
			// 本来はサイズを比較すべきだが、一旦最初のものを返す簡易実装
			return &registry_.GetStorage<C>();
		}

		Registry& registry_;
	};

	template <typename... Components>
	View<Components...> GetView() {
		return View<Components...>(*this);
	}

private:
	Entity nextEntityId_ = 0;
	std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> storages_;
};

} // namespace Engine::ECS
