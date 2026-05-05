/**
 * @file SparseSet.h
 * @brief Component storage implementation using sparse sets with address-stable chunks.
 */

#pragma once
#include <vector>
#include <memory>
#include <cassert>
#include <type_traits>
#include <functional>
#include <algorithm>
#include "EcsCore.h"

/**
 * @class ISparseSet
 * @brief Type-erased interface for component storage.
 */
class ISparseSet {
public:
    virtual ~ISparseSet() = default;
    
    virtual void remove(Entity entity) = 0;
    virtual bool contains(Entity entity) const = 0;
    virtual size_t size() const = 0;
    virtual const std::vector<Entity>& get_entities() const = 0;

    // Callbacks
    virtual void set_on_remove_callback(std::function<void(Entity)> callback) = 0;

    // Raw data access for Interop/Tooling
    virtual void add_raw(Entity entity, const void* data) = 0;
    virtual void* get_raw(Entity entity) = 0;

    // Sparse Array Paging (for C# side direct access)
    virtual uint32_t** get_sparse_pages_ptr() = 0;
    virtual size_t get_sparse_page_count() const = 0;

    // Chunked Storage info (for C# side direct access)
    virtual void* get_chunk_ptr(size_t index) = 0;
    virtual size_t get_chunk_count() const = 0;
    
    // Change tracking
    virtual uint32_t get_version() const = 0;
};

/**
 * @class SparseSet
 * @brief Implementation of ISparseSet with address-stable chunked data storage.
 * @tparam T Component type.
 */
template<typename T>
class SparseSet : public ISparseSet {
public:
    static constexpr size_t CHUNK_SIZE = 1024;
    static constexpr size_t CHUNK_MASK = CHUNK_SIZE - 1;
    static constexpr size_t CHUNK_SHIFT = 10;
    
    static constexpr size_t PAGE_SIZE = 4096;
    static constexpr size_t PAGE_SHIFT = 12;
    static constexpr size_t PAGE_MASK = PAGE_SIZE - 1;

private:
    static constexpr bool IS_TAG = std::is_empty_v<T>;
    using StorageType = std::conditional_t<IS_TAG, char, T>;
    
    std::vector<std::unique_ptr<StorageType[]>> chunks;
    std::vector<Entity> dense_entities;
    std::vector<uint32_t*> sparse_pages;

    uint32_t storage_version = 0;

    uint32_t get_sparse_index(uint32_t entity_index) const {
        size_t page_idx = entity_index >> PAGE_SHIFT;
        size_t offset = entity_index & PAGE_MASK;
        if (page_idx >= sparse_pages.size() || !sparse_pages[page_idx]) return 0xFFFFFFFF;
        return sparse_pages[page_idx][offset];
    }

    void set_sparse_index(uint32_t entity_index, uint32_t dense_index) {
        size_t page_idx = entity_index >> PAGE_SHIFT;
        size_t offset = entity_index & PAGE_MASK;
        if (page_idx >= sparse_pages.size()) {
            sparse_pages.resize(page_idx + 1, nullptr);
        }
        if (!sparse_pages[page_idx]) {
            uint32_t* new_page = new uint32_t[PAGE_SIZE];
            std::fill(new_page, new_page + PAGE_SIZE, 0xFFFFFFFF);
            sparse_pages[page_idx] = new_page;
        }
        sparse_pages[page_idx][offset] = dense_index;
    }

    StorageType& get_by_dense_index(uint32_t dense_index) {
        size_t chunk_idx = dense_index >> CHUNK_SHIFT;
        size_t offset = dense_index & CHUNK_MASK;
        if (chunk_idx >= chunks.size()) {
            chunks.push_back(std::make_unique<StorageType[]>(CHUNK_SIZE));
            storage_version++;
        }
        return chunks[chunk_idx][offset];
    }

public:
    SparseSet() = default;
    ~SparseSet() override {
        for (auto page : sparse_pages) {
            delete[] page;
        }
    }

    // Callbacks for internal/scripting use
    std::function<void(Entity, T&)> on_add;
    std::function<void(Entity, T&)> on_remove_raw; // With data
    std::function<void(Entity)> on_remove_event;  // Event only

    void set_on_remove_callback(std::function<void(Entity)> callback) override {
        on_remove_event = std::move(callback);
    }

    uint32_t get_version() const override { return storage_version; }

    // --- ISparseSet Overrides ---

    uint32_t** get_sparse_pages_ptr() override { return sparse_pages.data(); }
    size_t get_sparse_page_count() const override { return sparse_pages.size(); }

    void add_raw(Entity entity, const void* data) override {
        if (contains(entity)) {
            if constexpr (!IS_TAG) {
                get(entity) = *static_cast<const T*>(data);
            }
            return;
        }

        if constexpr (IS_TAG) {
            add(entity, T{});
        } else {
            add(entity, *static_cast<const T*>(data));
        }
    }

    void* get_raw(Entity entity) override {
        if constexpr (IS_TAG) return nullptr;
        return &get(entity);
    }

    void* get_chunk_ptr(size_t index) override {
        if (index >= chunks.size()) return nullptr;
        return chunks[index].get();
    }

    size_t get_chunk_count() const override { return chunks.size(); }

    size_t size() const override { return dense_entities.size(); }
    const std::vector<Entity>& get_entities() const override { return dense_entities; }

    // --- Typed Operations ---

    T& add(Entity entity, T value) {
        assert(!contains(entity));
        uint32_t dense_index = (uint32_t)dense_entities.size();
        dense_entities.push_back(entity);
        set_sparse_index(entity.index(), dense_index);

        if constexpr (IS_TAG) {
            static T dummy_tag{};
            if (on_add) on_add(entity, dummy_tag);
            return dummy_tag;
        } else {
            T& slot = get_by_dense_index(dense_index);
            slot = std::move(value);
            if (on_add) on_add(entity, slot);
            return slot;
        }
    }

    void remove(Entity entity) override {
        uint32_t dense_index = get_sparse_index(entity.index());
        if (dense_index == 0xFFFFFFFF) return;

        Entity last_entity = dense_entities.back();
        uint32_t last_dense_index = (uint32_t)dense_entities.size() - 1;

        if (on_remove_event) on_remove_event(entity);

        if constexpr (!IS_TAG) {
            T& target = get_by_dense_index(dense_index);
            if (on_remove_raw) on_remove_raw(entity, target);
            if (dense_index != last_dense_index) {
                target = std::move(get_by_dense_index(last_dense_index));
            }
        } else {
            static T dummy_tag{};
            if (on_remove_raw) on_remove_raw(entity, dummy_tag);
        }

        dense_entities[dense_index] = last_entity;
        set_sparse_index(last_entity.index(), dense_index);
        set_sparse_index(entity.index(), 0xFFFFFFFF);
        dense_entities.pop_back();
    }

    bool contains(Entity entity) const override {
        uint32_t dense_index = get_sparse_index(entity.index());
        if (dense_index == 0xFFFFFFFF) return false;
        return dense_entities[dense_index] == entity;
    }

    T& get(Entity entity) {
        assert(contains(entity));
        if constexpr (IS_TAG) {
            static T dummy_tag{};
            return dummy_tag;
        } else {
            return get_by_dense_index(get_sparse_index(entity.index()));
        }
    }
};
