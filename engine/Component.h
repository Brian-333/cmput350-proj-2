#pragma once

#include <cassert>
#include <utility>
#include <vector>

namespace ECSEngine {

/**
 * Stores components of a single type, reusing freed slots before growing.
 */
template <typename T>
class ComponentStorage {
public:
    ComponentStorage() : mValid() {
    }

    /**
     * Store a copy of a component, reusing a freed slot if one exists.
     * @param item The component to store.
     * @return The index the component was stored at.
     */
    std::size_t store(const T& item) {
        if (mFreeList.empty()) {
            mStorage.push_back(item);
            mValid.push_back(false);
            return mStorage.size() - 1;
        } else {
            std::size_t index = mFreeList.back();
            mStorage[index] = item;
            mFreeList.pop_back();
            mValid[index] = false;
            return index;
        }
    }

    /**
     * Move a component into storage, reusing a freed slot if one exists.
     * @param item The component to store.
     * @return The index the component was stored at.
     */
    std::size_t store(T&& item) {
        if (mFreeList.empty()) {
            mStorage.emplace_back(std::move(item));
            mValid.emplace_back(false);
            return mStorage.size() - 1;
        } else {
            std::size_t index = mFreeList.back();
            mStorage[index] = std::move(item);
            mFreeList.pop_back();
            mValid[index] = false;
            return index;
        }
    }

    /**
     * Check whether an index holds a live component.
     * @param index The index to check.
     * @return True if the index is in range and has not been removed.
     */
    bool valid(std::size_t index) const {
        return index < mStorage.size() && !mValid[index];
    }

    /**
     * Remove the component at an index and mark the slot as free for reuse.
     * @param index The index of the component to remove.
     */
    void remove(std::size_t index) {
        assert(valid(index));
        mFreeList.push_back(index);
        mValid[index] = true;
        // Not resetting the value to avoid unnecessary construction/destruction
        // mStorage[index] = T{};
    }

    /**
     * Get the component at an index.
     * @param index The index of the component.
     * @return The component.
     */
    T& operator[](std::size_t index) {
        assert(valid(index));
        // T& value = mData[index];
        return mStorage[index];
    }

    /**
     * Get the component at an index.
     * @param index The index of the component.
     * @return The component.
     */
    const T& operator[](std::size_t index) const {
        assert(valid(index));
        return mStorage[index];
    }

    /**
     * Get the number of live components.
     * @return The number of stored components minus the freed slots.
     */
    std::size_t size() const {
        return mStorage.size() - mFreeList.size();
    }
    std::vector<T> mStorage;
    std::vector<bool> mValid;       // Quickly track if a component is valid
    std::vector<size_t> mFreeList;  // Avoid scanning for free slots
};

}  // namespace ECSEngine
