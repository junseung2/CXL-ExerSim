/*
    Author  : Junseung Lee
    email   : ljs2021@g.skku.edu
    Date    : 2024.06.25
*/

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>

// Forward declarations
class HostMemory;
class DeviceMemory;
class HostCache;
class DeviceCache;
class DLSF;
class DRSF;
class HRSF;

class Memory {
public:
    virtual void read(int address) = 0;
    virtual void write(int address, int data) = 0;
};

class Cache {
public:
    virtual bool snoop(int address) = 0;
    virtual void update(int address, int data) = 0;
    virtual int getData(int address) = 0;
    virtual void writeBack(int address, Memory* memory) = 0;
};

class SnoopFilter {
public:
    virtual bool check(int address) = 0;
    virtual void registerAddress(int address) = 0;
};

class HostMemory : public Memory {
    std::unordered_map<int, int> storage;
public:
    void read(int address) override {
        std::cout << "Host Memory: Read from address 0x" << std::hex << address << std::dec << std::endl;
    }
    void write(int address, int data) override {
        std::cout << "Host Memory: Write to address 0x" << std::hex << address << std::dec << " data: " << data << std::endl;
        storage[address] = data;
    }
};

class DeviceMemory : public Memory {
    std::unordered_map<int, int> storage;
public:
    void read(int address) override {
        std::cout << "Device Memory: Read from address 0x" << std::hex << address << std::dec << std::endl;
    }
    void write(int address, int data) override {
        std::cout << "Device Memory: Write to address 0x" << std::hex << address << std::dec << " data: " << data << std::endl;
        storage[address] = data;
    }
};

class HostCache : public Cache {
    std::unordered_map<int, int> cache;
public:
    bool snoop(int address) override {
        std::cout << "Host Cache: Snoop address 0x" << std::hex << address << std::dec << std::endl;
        return cache.find(address) != cache.end();
    }
    void update(int address, int data) override {
        std::cout << "Host Cache: Update address 0x" << std::hex << address << std::dec << " with data: " << data << std::endl;
        cache[address] = data;
    }
    int getData(int address) override {
        return cache[address];
    }
    void writeBack(int address, Memory* memory) override {
        std::cout << "Host Cache: Write back to Memory at address 0x" << std::hex << address << std::dec << std::endl;
        memory->write(address, cache[address]);
    }
};

class DeviceCache : public Cache {
    std::unordered_map<int, int> cache;
public:
    bool snoop(int address) override {
        std::cout << "Device Cache: Snoop address 0x" << std::hex << address << std::dec << std::endl;
        return cache.find(address) != cache.end();
    }
    void update(int address, int data) override {
        std::cout << "Device Cache: Update address 0x" << std::hex << address << std::dec << " with data: " << data << std::endl;
        cache[address] = data;
    }
    int getData(int address) override {
        return cache[address];
    }
    void writeBack(int address, Memory* memory) override {
        std::cout << "Device Cache: Write back to Memory at address 0x" << std::hex << address << std::dec << std::endl;
        memory->write(address, cache[address]);
    }
};

class DLSF : public SnoopFilter {
    std::vector<int> addresses;
public:
    bool check(int address) override {
        std::cout << "DLSF: Check address 0x" << std::hex << address << std::dec << std::endl;
        return std::find(addresses.begin(), addresses.end(), address) != addresses.end();
    }
    void registerAddress(int address) override {
        std::cout << "DLSF: Register address 0x" << std::hex << address << std::dec << std::endl;
        addresses.push_back(address);
    }
};

class DRSF : public SnoopFilter {
    std::vector<int> addresses;
public:
    bool check(int address) override {
        std::cout << "DRSF: Check address 0x" << std::hex << address << std::dec << std::endl;
        return std::find(addresses.begin(), addresses.end(), address) != addresses.end();
    }
    void registerAddress(int address) override {
        std::cout << "DRSF: Register address 0x" << std::hex << address << std::dec << std::endl;
        addresses.push_back(address);
    }
};

class HRSF : public SnoopFilter {
    std::vector<int> addresses;
public:
    bool check(int address) override {
        std::cout << "HRSF: Check address 0x" << std::hex << address << std::dec << std::endl;
        return std::find(addresses.begin(), addresses.end(), address) != addresses.end();
    }
    void registerAddress(int address) override {
        std::cout << "HRSF: Register address 0x" << std::hex << address << std::dec << std::endl;
        addresses.push_back(address);
    }
};

class ProcessorCore {
public:
    void accessHostMemory(int address, HostMemory* memory, HostCache* cache, HRSF* hrsf);
    void accessDeviceMemory(int address, DeviceMemory* memory, DeviceCache* cache, DRSF* drsf);
};

class DeviceCore {
public:
    void accessDeviceMemory(int address, DeviceMemory* memory, DeviceCache* cache, DRSF* drsf);
    void accessHostMemory(int address, HostMemory* memory, HostCache* cache, DLSF* dlsf);
};

void ProcessorCore::accessHostMemory(int address, HostMemory* memory, HostCache* cache, HRSF* hrsf) {
    std::cout << "Processor Core: Accessing Host Memory at address 0x" << std::hex << address << std::dec << std::endl;
    if (hrsf->check(address)) {
        if (cache->snoop(address)) {
            std::cout << "Processor Core: Address found in Host Cache with data: " << cache->getData(address) << std::endl;
        } else {
            std::cout << "Processor Core: Address not found in Host Cache, reading from Host Memory" << std::endl;
            memory->read(address);
        }
    } else {
        std::cout << "Processor Core: Address not found in HRSF, reading from Host Memory" << std::endl;
        memory->read(address);
    }
}

void ProcessorCore::accessDeviceMemory(int address, DeviceMemory* memory, DeviceCache* cache, DRSF* drsf) {
    std::cout << "Processor Core: Accessing Device Memory at address 0x" << std::hex << address << std::dec << std::endl;
    if (drsf->check(address)) {
        if (cache->snoop(address)) {
            std::cout << "Processor Core: Address found in Device Cache with data: " << cache->getData(address) << std::endl;
        } else {
            std::cout << "Processor Core: Address not found in Device Cache, reading from Device Memory" << std::endl;
            memory->read(address);
        }
    } else {
        std::cout << "Processor Core: Address not found in DRSF, reading from Device Memory" << std::endl;
        memory->read(address);
    }
}

void DeviceCore::accessDeviceMemory(int address, DeviceMemory* memory, DeviceCache* cache, DRSF* drsf) {
    std::cout << "Device Core: Accessing Device Memory at address 0x" << std::hex << address << std::dec << std::endl;
    if (drsf->check(address)) {
        if (cache->snoop(address)) {
            std::cout << "Device Core: Address found in Device Cache with data: " << cache->getData(address) << std::endl;
            // Handle snoop (could involve more logic here)
        } else {
            std::cout << "Device Core: Address not found in Device Cache, reading from Device Memory" << std::endl;
            memory->read(address);
        }
    } else {
        std::cout << "Device Core: Address not found in DRSF, reading from Device Memory" << std::endl;
        memory->read(address);
    }
}

void DeviceCore::accessHostMemory(int address, HostMemory* memory, HostCache* cache, DLSF* dlsf) {
    std::cout << "Device Core: Accessing Host Memory at address 0x" << std::hex << address << std::dec << std::endl;
    if (dlsf->check(address)) {
        if (cache->snoop(address)) {
            std::cout << "Device Core: Address found in Host Cache with data: " << cache->getData(address) << std::endl;
        } else {
            std::cout << "Device Core: Address not found in Host Cache, reading from Host Memory" << std::endl;
            memory->read(address);
        }
    } else {
        std::cout << "Device Core: Address not found in DLSF, reading from Host Memory" << std::endl;
        memory->read(address);
    }
}

class TransactionLog {
    std::vector<std::string> logs;
public:
    void log(const std::string& entry) {
        logs.push_back(entry);
    }
    void displayLogs() {
        for (const auto& log : logs) {
            std::cout << log << std::endl;
        }
    }
};

std::string formatLogEntry(const std::string& component, const std::string& action, int address, const std::string& result) {
    std::ostringstream oss;
    oss << std::setw(15) << std::left << component 
        << std::setw(25) << std::left << action 
        << "Address: 0x" << std::hex << address << std::dec 
        << " | " << result;
    return oss.str();
}

int main() {
    // Create instances of each component
    HostMemory hostMemory;
    DeviceMemory deviceMemory;
    HostCache hostCache;
    DeviceCache deviceCache;
    DLSF dlsf;
    DRSF drsf;
    HRSF hrsf;
    ProcessorCore processorCore;
    DeviceCore deviceCore;
    TransactionLog transactionLog;

    // Register some addresses in Snoop Filters to simulate previous accesses
    drsf.registerAddress(0x2000);
    dlsf.registerAddress(0x4000);
    hrsf.registerAddress(0x1000);

    // Simulate some transactions
    std::cout << "==== Processor Core Transactions ====" << std::endl;
    transactionLog.log(formatLogEntry("Processor Core", "Accessing Host Memory", 0x1000, "Initiated"));
    processorCore.accessHostMemory(0x1000, &hostMemory, &hostCache, &hrsf);
    transactionLog.log(formatLogEntry("Processor Core", "Accessing Host Memory", 0x1000, "Completed"));

    transactionLog.log(formatLogEntry("Processor Core", "Accessing Device Memory", 0x2000, "Initiated"));
    processorCore.accessDeviceMemory(0x2000, &deviceMemory, &deviceCache, &drsf);
    transactionLog.log(formatLogEntry("Processor Core", "Accessing Device Memory", 0x2000, "Completed"));

    std::cout << "==== Device Core Transactions ====" << std::endl;
    transactionLog.log(formatLogEntry("Device Core", "Accessing Device Memory", 0x3000, "Initiated"));
    deviceCore.accessDeviceMemory(0x3000, &deviceMemory, &deviceCache, &drsf);
    transactionLog.log(formatLogEntry("Device Core", "Accessing Device Memory", 0x3000, "Completed"));

    transactionLog.log(formatLogEntry("Device Core", "Accessing Host Memory", 0x4000, "Initiated"));
    deviceCore.accessHostMemory(0x4000, &hostMemory, &hostCache, &dlsf);
    transactionLog.log(formatLogEntry("Device Core", "Accessing Host Memory", 0x4000, "Completed"));

    // Additional Transactions to demonstrate more complex interactions
    std::cout << "==== Additional Transactions ====" << std::endl;
    transactionLog.log(formatLogEntry("Host Cache", "Updating", 0x5000, "with data 42"));
    hostCache.update(0x5000, 42);
    dlsf.registerAddress(0x5000);

    transactionLog.log(formatLogEntry("Device Core", "Accessing Host Memory", 0x5000, "Initiated"));
    deviceCore.accessHostMemory(0x5000, &hostMemory, &hostCache, &dlsf);
    transactionLog.log(formatLogEntry("Device Core", "Accessing Host Memory", 0x5000, "Completed"));

    transactionLog.log(formatLogEntry("Device Cache", "Updating", 0x6000, "with data 84"));
    deviceCache.update(0x6000, 84);
    drsf.registerAddress(0x6000);

    transactionLog.log(formatLogEntry("Processor Core", "Accessing Device Memory", 0x6000, "Initiated"));
    processorCore.accessDeviceMemory(0x6000, &deviceMemory, &deviceCache, &drsf);
    transactionLog.log(formatLogEntry("Processor Core", "Accessing Device Memory", 0x6000, "Completed"));

    // Display the transaction log
    std::cout << "==== Transaction Log ====" << std::endl;
    transactionLog.displayLogs();

    return 0;
}
