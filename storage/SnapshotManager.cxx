#include "SnapshotManager.hxx"
#include "../utils/Logger.hxx"

namespace NumberStore {
    SnapshotManager::SnapshotManager() {
    }

    std::shared_ptr<const std::map<uint64_t, int64_t>> SnapshotManager::getSnapshot(const std::map<uint64_t, int64_t>& currentData) const {
        // Check if we need a fresh snapshot
        if (needsUpdate()) {
            std::lock_guard<std::mutex> lock(snapshotMutex);
            
            // Double-check after acquiring lock (another thread might have updated)
            if (needsUpdate()) {
                Logger::getInstance().debug("Creating new snapshot with " + std::to_string(currentData.size()) + " items");
                cachedSnapshot = std::make_shared<std::map<uint64_t, int64_t>>(currentData);
                snapshotVersion.store(dataVersion.load());
            }
        }
        
        // Return the cached snapshot
        std::lock_guard<std::mutex> lock(snapshotMutex);
        return std::static_pointer_cast<const std::map<uint64_t, int64_t>>(cachedSnapshot);
    }

    void SnapshotManager::invalidateSnapshot() {
        std::lock_guard<std::mutex> lock(snapshotMutex);
        cachedSnapshot.reset();
        Logger::getInstance().debug("Snapshot invalidated");
    }

    void SnapshotManager::incrementVersion() {
        dataVersion.fetch_add(1);
        Logger::getInstance().debug("Data version incremented to " + std::to_string(dataVersion.load()));
    }

    uint64_t SnapshotManager::getCurrentVersion() const {
        return dataVersion.load();
    }

    bool SnapshotManager::hasValidSnapshot() const {
        std::lock_guard<std::mutex> lock(snapshotMutex);
        return cachedSnapshot != nullptr && snapshotVersion.load() == dataVersion.load();
    }

    bool SnapshotManager::needsUpdate() const {
        return !cachedSnapshot || snapshotVersion.load() != dataVersion.load();
    }
}
