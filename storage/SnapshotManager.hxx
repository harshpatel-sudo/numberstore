#ifndef SNAPSHOT_MANAGER_HXX
#define SNAPSHOT_MANAGER_HXX

#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <cstdint>

namespace NumberStore {
    class SnapshotManager {
    private:
        mutable std::shared_ptr<std::map<uint64_t, int64_t>> cachedSnapshot;
        mutable std::atomic<uint64_t> dataVersion{0};
        mutable std::atomic<uint64_t> snapshotVersion{0};
        mutable std::mutex snapshotMutex;

    public:
        SnapshotManager();
        ~SnapshotManager() = default;

        // Disable copy constructor and assignment operator
        SnapshotManager(const SnapshotManager&) = delete;
        SnapshotManager& operator=(const SnapshotManager&) = delete;

        std::shared_ptr<const std::map<uint64_t, int64_t>> getSnapshot(const std::map<uint64_t, int64_t>& currentData) const;
        void invalidateSnapshot();
        void incrementVersion();
        
        uint64_t getCurrentVersion() const;
        bool hasValidSnapshot() const;
        
    private:
        bool needsUpdate() const;
    };
}

#endif // SNAPSHOT_MANAGER_HXX
