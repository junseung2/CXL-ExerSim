/*
    Author  : Junseung Lee
    Email   : ljs2021@g.skku.edu
    Date    : 2024.02.23
*/

#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <vector>

class Flit
{
public:
    std::string data;
    Flit(std::string data) : data(data) {}
};

class ArbMuxLayer
{
private:
    std::queue<Flit> ioQueue;
    std::queue<Flit> cacheMemQueue;
    int ioWeight;
    int cacheMemWeight;
    int totalCycles;
    int currentCycle;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> vLSMChance;

public:
    ArbMuxLayer(int ioWeight, int cacheMemWeight) : ioWeight(ioWeight), cacheMemWeight(cacheMemWeight), currentCycle(0), gen(rd()), vLSMChance(0, 9)
    {
        totalCycles = ioWeight + cacheMemWeight;
    }

    void addFlitToIOQueue(Flit flit)
    {
        ioQueue.push(flit);
        std::cout << "Added to IO Queue: " << flit.data << std::endl;
    }

    void addFlitToCacheMemQueue(Flit flit)
    {
        cacheMemQueue.push(flit);
        std::cout << "Added to Cache/Mem Queue: " << flit.data << std::endl
                  << std::endl;
    }

    bool generateVLSMFlit()
    {
        // 10% chance to generate a vLSM flit
        return vLSMChance(gen) == 0;
    }

    void insertVLSMFlitIfNeeded()
    {
        if (generateVLSMFlit())
        {
            std::cout << "##vLSM flit[ALMP] generated and transmitted.##" << std::endl;
        }
    }

    Flit selectFlit()
    {
        insertVLSMFlitIfNeeded();

        if (!ioQueue.empty() && (currentCycle < ioWeight || cacheMemQueue.empty()))
        {
            Flit selectedFlit = ioQueue.front();
            ioQueue.pop();
            std::cout << "Selected IO flit: " << selectedFlit.data << std::endl;
            currentCycle = (currentCycle + 1) % totalCycles;
            return selectedFlit;
        }
        else if (!cacheMemQueue.empty())
        {
            Flit selectedFlit = cacheMemQueue.front();
            cacheMemQueue.pop();
            std::cout << "Selected Cache/Mem flit: " << selectedFlit.data << std::endl;
            currentCycle = (currentCycle + 1) % totalCycles;
            return selectedFlit;
        }
        else if (!ioQueue.empty())
        {
            Flit selectedFlit = ioQueue.front();
            ioQueue.pop();
            std::cout << "Selected IO flit due to empty Cache/Mem queue: " << selectedFlit.data << std::endl;
            currentCycle = (currentCycle + 1) % totalCycles;
            return selectedFlit;
        }

        std::cout << "No flits available to select." << std::endl;
        return Flit("Empty");
    }

    void transmitFlits()
    {
        std::cout << std::endl
                  << "#############################"
                  << "Starting flit transmission..."
                  << std::endl
                  << "#############################"
                  << std::endl;
        while (!ioQueue.empty() || !cacheMemQueue.empty())
        {
            selectFlit();
            insertVLSMFlitIfNeeded(); // Check for vLSM flit insertion after each flit transmission
        }
        std::cout << std::endl
                  << "All flits have been transmitted. Simulation completed." << std::endl;
    }
};

void generateRandomFlits(ArbMuxLayer &arbMux, int count)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);
    std::uniform_int_distribution<> dataDis(1, 100);

    for (int i = 0; i < count; ++i)
    {
        if (dis(gen) <= 8)
        {
            Flit newFlit = Flit("Cache/Mem Flit " + std::to_string(dataDis(gen)));
            arbMux.addFlitToCacheMemQueue(newFlit);
            std::cout << "Generated and added to Cache/Mem: " << newFlit.data << std::endl;
        }
        else
        {
            Flit newFlit = Flit("IO Flit " + std::to_string(dataDis(gen)));
            arbMux.addFlitToIOQueue(newFlit);
            std::cout << "Generated and added to IO: " << newFlit.data << std::endl;
        }
    }
}

int main()
{
    ArbMuxLayer arbMux(2, 8); // CXL.io에 대한 가중치는 2, CXL.cache/CXL.mem에 대한 가중치는 8

    std::cout << "Generating random flits..." << std::endl;
    generateRandomFlits(arbMux, 100); // 20개의 flit을 무작위로 생성

    arbMux.transmitFlits(); // flits 전송 시뮬레이션

    return 0;
}
