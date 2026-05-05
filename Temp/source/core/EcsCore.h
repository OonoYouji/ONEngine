/**
 * @file EcsCore.h
 * @brief ECSの基本型と、型ごとの一意なID生成器を定義します。
 */

#pragma once
#include <cstdint>
#include <atomic>
#include <functional>
#include <vector>
#include <memory>

/**
 * @struct Entity
 * @brief エンティティ識別子（世代付きインデックス）。
 */
struct Entity {
    uint64_t id;
    
    static constexpr uint32_t INVALID_INDEX = 0xFFFFFFFF;
    static constexpr uint64_t NULL_ID = 0xFFFFFFFFFFFFFFFF;

    Entity() : id(NULL_ID) {}
    Entity(uint32_t index, uint32_t generation) {
        id = ((uint64_t)generation << 32) | index;
    }

    uint32_t index() const { return (uint32_t)(id & 0xFFFFFFFF); }
    uint32_t generation() const { return (uint32_t)(id >> 32); }
    
    bool operator==(const Entity& other) const { return id == other.id; }
    bool operator!=(const Entity& other) const { return id != other.id; }
    bool operator<(const Entity& other) const { return id < other.id; }
    
    explicit operator bool() const { return id != NULL_ID; }
    bool is_valid() const { return id != NULL_ID; }
    
    static Entity Null() { return Entity(); }
};

class ISparseSet;

/**
 * @class TypeRegistry
 * @brief 型IDとストレージ生成関数を管理します。
 */
class TypeRegistry {
public:
    using Factory = std::function<std::unique_ptr<ISparseSet>()>;
    
    static void Register(uint32_t id, Factory factory) {
        auto& list = get_factories();
        if (id >= list.size()) list.resize(id + 1);
        list[id] = std::move(factory);
    }
    
    static std::unique_ptr<ISparseSet> Create(uint32_t id) {
        auto& list = get_factories();
        if (id < list.size() && list[id]) return list[id]();
        return nullptr;
    }

private:
    static std::vector<Factory>& get_factories() {
        static std::vector<Factory> factories;
        return factories;
    }
};

/**
 * @class TypeID
 * @brief コンポーネント型ごとに一意な連続した整数IDを割り振ります。
 */
class TypeID {
public:
    template<typename T>
    static uint32_t get() {
        static const uint32_t id = next_id++;
        return id;
    }

private:
    static inline std::atomic<uint32_t> next_id{ 0 };
};
