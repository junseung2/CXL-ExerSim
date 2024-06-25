/*
    Author  : Junseung Lee
    email   : ljs2021@g.skku.edu
*/

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <cstdlib>
#include <ctime>

enum class CacheLineState
{
    Modified,
    Exclusive,
    Shared,
    Invalid
};

std::string stateToString(CacheLineState state)
{
    switch (state)
    {
    case CacheLineState::Modified:
        return "Modified";
    case CacheLineState::Exclusive:
        return "Exclusive";
    case CacheLineState::Shared:
        return "Shared";
    case CacheLineState::Invalid:
        return "Invalid";
    default:
        return "Unknown";
    }
}

struct CacheLine
{
    int data;
    CacheLineState state;

    CacheLine(int data = 0, CacheLineState state = CacheLineState::Invalid) : data(data), state(state) {}
};

class Bus; // Forward declaration

class Core
{
public:
    std::map<int, CacheLine> cache; // Memory address to cache line mapping
    Bus *bus;
    int id;

    Core(int id, Bus *bus) : id(id), bus(bus) {}

    void read(int address);
    void write(int address, int data);
    void onBusRead(int address);
    void onBusWrite(int address);
    void printState(int address);
};

class Bus
{
public:
    std::vector<Core *> cores;

    void broadcastRead(int coreId, int address);
    void broadcastWrite(int coreId, int address);
    bool writeBack(int address, int &data);
};

void Core::read(int address)
{
    if (cache.find(address) == cache.end() || cache[address].state == CacheLineState::Invalid)
    {
        int data;
        // 메인 메모리로부터 데이터를 가져오는 것으로 가정하는 부분입니다.
        // 다른 코어가 Modified 상태에서 데이터를 제공할 수도 있습니다.
        if (bus->writeBack(address, data))
        {
            // 데이터를 메인 메모리로부터 가져왔거나, 다른 코어로부터 write-back 받았습니다.
            cache[address] = CacheLine(data, CacheLineState::Shared);
        }
        else
        {
            // 현재 코어의 ID와 주소를 전달합니다.
            bus->broadcastRead(id, address); // 수정된 호출
            // 메인 메모리로부터 데이터를 가져오는 것으로 가정하고 Exclusive 상태로 설정합니다.
            cache[address] = CacheLine(0, CacheLineState::Exclusive);
        }
    }
    printState(address);
}

void Core::write(int address, int data)
{
    bus->broadcastWrite(id, address);
    cache[address] = CacheLine(data, CacheLineState::Modified);
    printState(address);
}

void Core::onBusRead(int address)
{
    // If another core requests a read, check if we have the data in Modified state
}

void Core::onBusWrite(int address)
{
    auto it = cache.find(address);
    if (it != cache.end())
    {
        it->second.state = CacheLineState::Invalid;
    }
}

void Core::printState(int address)
{
    std::cout << "Core " << id << ": Address 0x" << std::hex << address
              << " is in " << stateToString(cache[address].state) << " state.\n";
    std::cout << std::dec; // Switch back to decimal for other outputs
}

void Bus::broadcastRead(int coreId, int address)
{
    for (Core *core : cores)
    {
        if (core->id != coreId)
        {
            core->onBusRead(address);
        }
    }
}

void Bus::broadcastWrite(int coreId, int address)
{
    for (Core *core : cores)
    {
        if (core->id != coreId)
        {
            core->onBusWrite(address);
        }
    }
}

bool Bus::writeBack(int address, int &data)
{
    for (Core *core : cores)
    {
        auto it = core->cache.find(address);
        if (it != core->cache.end() && it->second.state == CacheLineState::Modified)
        {
            data = it->second.data;
            it->second.state = CacheLineState::Shared; // After write-back, set to Shared
            std::cout << "Write-back performed for address 0x" << std::hex << address
                      << " with data " << data << "\n";
            std::cout << std::dec;
            return true;
        }
    }
    return false;
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    Bus bus;
    Core core1(1, &bus), core2(2, &bus);
    bus.cores.push_back(&core1);
    bus.cores.push_back(&core2);

    // Example loop with random operations
    for (int i = 0; i < 100; ++i)
    {
        int coreId = rand() % 2 + 1;
        int address = rand() % 10;
        Core *targetCore = (coreId == 1) ? &core1 : &core2;

        if (rand() % 2)
        {
            int data = rand() % 100;
            std::cout << "Performing write operation on Core " << coreId << "\n";
            targetCore->write(address, data);
        }
        else
        {
            std::cout << "Performing read operation on Core " << coreId << "\n";
            targetCore->read(address);
        }
    }

    return 0;
}
