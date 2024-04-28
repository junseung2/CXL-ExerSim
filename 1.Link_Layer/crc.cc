/*
    Author  : Junseung Lee
    email   : ljs2021@g.skku.edu
    Date    : 2024.02.21
*/

#include <iostream>
#include <vector>
#include <random>

constexpr uint16_t polynomial = 0x1021;
constexpr uint16_t initial_value = 0xFFFF;

uint16_t calculateCRC16(const std::vector<uint8_t> &data)
{
    uint16_t crc = initial_value;
    for (auto byte : data)
    {
        crc ^= (static_cast<uint16_t>(byte) << 8);
        for (int i = 0; i < 8; ++i)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ polynomial;
            else
                crc <<= 1;
        }
    }
    crc = ~crc;
    return crc;
}

bool checkCRC(const std::vector<uint8_t> &data, uint16_t receivedCRC)
{
    uint16_t calculatedCRC = calculateCRC16(data);
    return calculatedCRC == receivedCRC;
}

int main()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    std::uniform_real_distribution<> errorChance(0.0, 1.0);

    int errorTotalCount = 0;
    int errorDetectedCount = 0;

    for (int iteration = 0; iteration < 100; ++iteration)
    {
        std::vector<uint8_t> data(64);
        for (size_t i = 0; i < 64; ++i)
        {
            data[i] = static_cast<uint8_t>(dis(gen)); // Use random data for each iteration
        }

        uint16_t originalCRC = calculateCRC16(data);

        // Randomly introduce an error with a 10% chance
        if (errorChance(gen) < 0.1)
        {
            size_t errorIndex = gen() % data.size();
            data[errorIndex] ^= static_cast<uint8_t>(dis(gen)); // Introduce a random bit error
            std::cout << "Iteration " << iteration + 1 << ": Error introduced at index " << errorIndex << std::endl;
            errorTotalCount++;
        }
        else
        {
            std::cout << "Iteration " << iteration + 1 << ": No error introduced." << std::endl;
        }

        if (checkCRC(data, originalCRC))
        {
            std::cout << "CRC check passed: Data is considered correct." << std::endl;
        }
        else
        {
            std::cout << "CRC check failed: Data has errors." << std::endl;
            errorDetectedCount++;
        }
    }

    std::cout << "\nTotal iterations: " << 100 << std::endl;
    std::cout << "Number of Total Errors : " << errorDetectedCount << std::endl;
    std::cout << "Number of Detected Errors: " << errorDetectedCount << std::endl;

    return 0;
}
