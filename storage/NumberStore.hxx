#ifndef NUMBER_STORE_HXX
#define NUMBER_STORE_HXX

#include <map>
#include <string>
#include <shared_mutex>
#include <cstdint>
#include "SnapshotManager.hxx"
#include "../utils/ErrorCodes.hxx"

namespace NumberStore {
    class NumberStore {
    private:
        std::map<uint64_t, int64_t> numbers;
        mutable std::shared_mutex dataMutex;
        mutable SnapshotManager snapshotManager;

    public:
        NumberStore();
        ~NumberStore() = default;

        NumberStore(const NumberStore&) = delete;
        NumberStore& operator=(const NumberStore&) = delete;
        NumberStore(NumberStore&&) = delete;
        NumberStore& operator=(NumberStore&&) = delete;

        ErrorCode insert(uint64_t number);
        ErrorCode remove(uint64_t number, int64_t& outtimestamp);
        ErrorCode clear();
        
        std::string printAll() const;
        size_t size() const;
        bool contains(uint64_t number) const;
        bool empty() const;
        
        
    private:
        void notifyDataChanged();
        std::string formatNumberEntry(uint64_t number, int64_t timestamp) const;
    };
}

#endif // NUMBER_STORE_HXX
