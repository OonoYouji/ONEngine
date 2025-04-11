#include "ComponentArray.h"

//#include "../Component.h"
//
//template<>
//inline size_t ComponentArray<CustomMeshRenderer>::AddComponent() {
//	if (removedIndices_.size() > 0) { ///< 削除されたインデックスがある場合
//		size_t index = removedIndices_.back();
//		removedIndices_.pop_back();
//		usedIndices_.push_back(index);
//		components_[index] = CustomMeshRenderer(); ///< 今までのデータを上書き
//		return index;
//	}
//
//	components_.emplace_back();
//	size_t index = components_.size() - 1;
//	usedIndices_.push_back(index);
//	return index;
//}
