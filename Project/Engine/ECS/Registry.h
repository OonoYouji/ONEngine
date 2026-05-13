#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <algorithm>
#include <cassert>
#include <limits>
#include "Entity.h"

namespace Engine::ECS {

///
/// コンポーネントの基底ストレージ
///
class IComponentStorage {
public:
	virtual ~IComponentStorage() = default;
	virtual void Remove(Entity entity) = 0;
	virtual bool Has(Entity entity) const = 0;
	virtual uint32_t GetIndex(Entity entity) const = 0;
	virtual const std::vector<Entity>& GetEntities() const = 0;
	virtual size_t Size() const = 0;

	// 汎用データアクセスのための仮想関数
	virtual void** GetSparsePagesPtr(uint32_t* outPageCount) = 0;
	virtual void* GetChunkPtr(size_t chunkIndex) = 0;

    virtual uint32_t GetTypeId() const = 0;
    virtual void* GetRaw(Entity entity) = 0;
};

///
/// Sparse Set を使用したコンポーネントストレージ
///
template <typename T>
class ComponentStorage final : public IComponentStorage {
public:
    ComponentStorage() {
        typeId_ = (uint32_t)typeid(T).hash_code();
    }
	static constexpr size_t kChunkSize = 1024;
	static constexpr uint32_t kInvalidIndex = 0xFFFFFFFF;
	static constexpr size_t kPageSize = 4096;

    void SetTypeId(uint32_t typeId) { typeId_ = typeId; }
    uint32_t GetTypeId() const override { return typeId_; }

    void* GetRaw(Entity entity) override {
        uint32_t index = GetIndex(entity);
        if (index == kInvalidIndex) return nullptr;
        return GetPtr(index);
    }

	T& Add(Entity entity, T&& component) {
		uint32_t index = GetIndex(entity);
		if (index != kInvalidIndex) {
			*GetPtr(index) = std::move(component);
			return *GetPtr(index);
		}

		index = static_cast<uint32_t>(count_);
		EnsureCapacity(index + 1);

		EnsureSparsePage(entity);
		sparsePages_[entity / kPageSize][entity % kPageSize] = index;
		
		indexToEntity_.push_back(entity);
		*GetPtr(index) = std::move(component);
		count_++;
		return *GetPtr(index);
	}

	void Remove(Entity entity) override {
		uint32_t index = GetIndex(entity);
		if (index == kInvalidIndex) return;

		uint32_t lastIndex = static_cast<uint32_t>(count_ - 1);
		Entity lastEntity = indexToEntity_[lastIndex];

		// 末尾要素と入れ替えて削除
		if (index != lastIndex) {
			*GetPtr(index) = std::move(*GetPtr(lastIndex));
			indexToEntity_[index] = lastEntity;
			sparsePages_[lastEntity / kPageSize][lastEntity % kPageSize] = index;
		}

		count_--;
		indexToEntity_.pop_back();
		sparsePages_[entity / kPageSize][entity % kPageSize] = kInvalidIndex;
	}

	bool Has(Entity entity) const override {
		return GetIndex(entity) != kInvalidIndex;
	}

	uint32_t GetIndex(Entity entity) const override {
		size_t pageIdx = entity / kPageSize;
		if (pageIdx >= sparsePages_.size() || !sparsePages_[pageIdx]) return kInvalidIndex;
		return sparsePages_[pageIdx][entity % kPageSize];
	}

	T& Get(Entity entity) {
		uint32_t index = GetIndex(entity);
		assert(index != kInvalidIndex);
		return *GetPtr(index);
	}

	const T& Get(Entity entity) const {
		uint32_t index = GetIndex(entity);
		assert(index != kInvalidIndex);
		return *GetPtr(index);
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
	size_t Size() const override { return count_; }

	// Interop 用: 
	void* GetChunkPtr(size_t chunkIndex) override {
		if (chunkIndex >= chunks_.size()) return nullptr;
		return chunks_[chunkIndex].get();
	}

	void** GetSparsePagesPtr(uint32_t* outPageCount) override {
		*outPageCount = static_cast<uint32_t>(sparsePages_.size());
		return (void**)sparsePages_.data();
	}

private:
	void EnsureCapacity(size_t capacity) {
		size_t neededChunks = (capacity + kChunkSize - 1) / kChunkSize;
		while (chunks_.size() < neededChunks) {
			chunks_.push_back(std::make_unique<T[]>(kChunkSize));
		}
	}

	void EnsureSparsePage(Entity entity) {
		size_t pageIdx = entity / kPageSize;
		if (pageIdx >= sparsePages_.size()) {
			sparsePages_.resize(pageIdx + 1, nullptr);
		}
		if (!sparsePages_[pageIdx]) {
			sparsePages_[pageIdx] = new uint32_t[kPageSize];
			std::fill(sparsePages_[pageIdx], sparsePages_[pageIdx] + kPageSize, kInvalidIndex);
		}
	}

	std::vector<std::unique_ptr<T[]>> chunks_;
	size_t count_ = 0;
	std::vector<Entity> indexToEntity_;
	std::vector<uint32_t*> sparsePages_; // Paged sparse array
    uint32_t typeId_;
};

///
/// ECSのレジストリクラス
///
class Registry final {
public:
	Entity CreateEntity() {
		if (!freeEntities_.empty()) {
			Entity entity = freeEntities_.back();
			freeEntities_.pop_back();
			return entity;
		}
		return ++nextEntityId_;
	}

	void DestroyEntity(Entity entity) {
		for (auto& pair : storages_) {
			pair.second->Remove(entity);
		}
		freeEntities_.push_back(entity);
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

    /// @brief TypeID指定でコンポーネントを取得 (Editor用)
    void* GetComponent(Entity entity, uint32_t typeId) {
        for (auto& [typeIndex, storage] : storages_) {
            if (storage->GetTypeId() == typeId) {
                return storage->GetRaw(entity);
            }
        }
        return nullptr;
    }

    /// @brief TypeID指定でコンポーネントの有無を確認 (Editor用)
    bool HasComponent(Entity entity, uint32_t typeId) {
        for (auto& [typeIndex, storage] : storages_) {
            if (storage->GetTypeId() == typeId) {
                return storage->Has(entity);
            }
        }
        return false;
    }

	template <typename T>
	ComponentStorage<T>& GetStorage() {
		auto type = std::type_index(typeid(T));
		auto it = storages_.find(type);
		if (it == storages_.end()) {
			it = storages_.emplace(type, std::make_unique<ComponentStorage<T>>()).first;
		}
		return *static_cast<ComponentStorage<T>*>(it->second.get());
	}

	template <typename... Components>
	class View {
	public:
		View(Registry& registry) : registry_(registry) {}

		template <typename Func>
		void Each(Func func) {
			IComponentStorage* smallestStorage = GetSmallestStorage();
			if (!smallestStorage) return;

			const auto& entities = smallestStorage->GetEntities();
			for (Entity entity : entities) {
				if ((registry_.HasComponent<Components>(entity) && ...)) {
					func(entity, registry_.GetComponent<Components>(entity)...);
				}
			}
		}

	private:
		IComponentStorage* GetSmallestStorage() {
			IComponentStorage* smallest = nullptr;
			size_t minSize = (std::numeric_limits<size_t>::max)();

			auto check = [&](auto* storage) {
				if (storage->Size() < minSize) {
					minSize = storage->Size();
					smallest = storage;
				}
			};

			(check(&registry_.GetStorage<Components>()), ...);
			return smallest;
		}

		Registry& registry_;
	};

	template <typename... Components>
	View<Components...> GetView() {
		return View<Components...>(*this);
	}

	uint32_t GetMaxEntityId() const { return nextEntityId_; }

	void Clear() {
		storages_.clear();
		freeEntities_.clear();
		nextEntityId_ = 0;
	}

private:
	Entity nextEntityId_ = 0;
	std::vector<Entity> freeEntities_;
	std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> storages_;
};

} // namespace Engine::ECS
