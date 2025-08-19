#include "NumberStore.hxx"
#include "../utils/TimeUtils.hxx"
#include "../utils/Logger.hxx"
#include <sstream>
#include <algorithm>
#include <limits>

namespace NumberStore {
    NumberStore::NumberStore() {
    }

    ErrorCode NumberStore::insert(uint64_t number) {
        int64_t timestamp = 0;
        ErrorCode result;
        
        {
            std::unique_lock<std::shared_mutex> lock(dataMutex);
            
            if (numbers.find(number) != numbers.end()) {
                result = ErrorCode::DUPLICATE_NUMBER;
            } else {
                timestamp = TimeUtils::getCurrentUnixTimestamp();
                numbers[number] = timestamp;
                result = ErrorCode::SUCCESS;
            }
        }
        
        if (result == ErrorCode::DUPLICATE_NUMBER) {
            Logger::getInstance().info("Attempted to insert duplicate number: " + std::to_string(number));
        } else {
            Logger::getInstance().info("Inserted number: " + std::to_string(number) + " at timestamp: " + std::to_string(timestamp));
            notifyDataChanged();
        }
        
        return result;
    }

    ErrorCode NumberStore::remove(uint64_t number, int64_t& outtimestamp) {
        ErrorCode result;
        bool found = false;
        
        {
            std::unique_lock<std::shared_mutex> lock(dataMutex);
            
            auto it = numbers.find(number);
            if (it == numbers.end()) {
                result = ErrorCode::NUMBER_NOT_FOUND;
            } else {
                outtimestamp = it->second;
                numbers.erase(it);
                found = true;
                result = ErrorCode::SUCCESS;
            }
        }
        
        if (!found) {
            Logger::getInstance().info("Attempted to delete non-existent number: " + std::to_string(number));
        } else {
            Logger::getInstance().info("Deleted number: " + std::to_string(number) + " (was inserted at timestamp: " + std::to_string(outtimestamp) + ")");
            notifyDataChanged();
        }
        
        return result;
    }

    ErrorCode NumberStore::clear() {
        size_t count;
        
        {
            std::unique_lock<std::shared_mutex> lock(dataMutex);
            count = numbers.size();
            numbers.clear();
        }
        
        Logger::getInstance().info("Cleared all numbers (removed " + std::to_string(count) + " entries)");
        notifyDataChanged();
        
        return ErrorCode::SUCCESS;
    }

    std::string NumberStore::printAll() const {
        std::shared_ptr<const std::map<uint64_t, int64_t>> snapshot;
        
        {
            std::shared_lock<std::shared_mutex> lock(dataMutex);
            snapshot = snapshotManager.getSnapshot(numbers);
        }
        
        if (!snapshot || snapshot->empty()) {
            return "No numbers stored.";
        }
        
        std::ostringstream oss;
        
        for (const auto& [number, timestamp] : *snapshot) {
            oss << formatNumberEntry(number, timestamp) << "\n";
        }
        
        Logger::getInstance().debug("Printed " + std::to_string(snapshot->size()) + " numbers");
        return oss.str();
    }

    size_t NumberStore::size() const {
        std::shared_lock<std::shared_mutex> lock(dataMutex);
        return numbers.size();
    }

    bool NumberStore::contains(uint64_t number) const {
        std::shared_lock<std::shared_mutex> lock(dataMutex);
        return numbers.find(number) != numbers.end();
    }

    bool NumberStore::empty() const {
        std::shared_lock<std::shared_mutex> lock(dataMutex);
        return numbers.empty();
    }

    void NumberStore::notifyDataChanged() {
        snapshotManager.incrementVersion();
    }

    std::string NumberStore::formatNumberEntry(uint64_t number, int64_t timestamp) const {
        return std::to_string(number) + ":" + std::to_string(timestamp);
    }
}
