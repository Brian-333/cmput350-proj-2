#pragma once

#include <vector>

namespace ECSEngine {

template <typename T>
class ComponentStorage {
public:
    ComponentStorage() : mValid() {
    }
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

    bool valid(std::size_t index) const {
        return index < mStorage.size() && !mValid[index];
    }

    void remove(std::size_t index) {
        assert(valid(index));
        mFreeList.push_back(index);
        mValid[index] = true;
        mStorage[index] = T{};
    }

    T& operator[](std::size_t index) {
        assert(valid(index));
        // T& value = mData[index];
        return mStorage[index];
    }

    const T& operator[](std::size_t index) const {
        assert(valid(index));
        return mStorage[index];
    }

    std::size_t size() const {
        return mStorage.size() - mFreeList.size();
    }
    std::vector<T> mStorage;
    std::vector<bool> mValid;       // Quickly track if a component is valid
    std::vector<size_t> mFreeList;  // Avoid scanning for free slots
};

}  // namespace ECSEngine
