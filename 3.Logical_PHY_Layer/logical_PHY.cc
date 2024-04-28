/*
    Author  : Junseung Lee
    Email   : ljs2021@g.skku.edu
    Date    : 2024.02.25
*/

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <bitset>  // For std::bitset
#include <iomanip> // For std::setw

class Logical_PHY_Layer
{
public:
    // Protocol IDs
    static const uint16_t CXL_IO_FLIT = 0xFFFF;
    static const uint16_t CXL_CACHE_MEM_FLIT = 0x5555;
    static const uint16_t NULL_FLIT = 0x9999;
    static const uint16_t ALMP_FLIT = 0xCCCC;

    std::vector<uint8_t> AddProtocol_ID(const std::vector<uint8_t> &flit, uint16_t protocolID)
    {
        std::vector<uint8_t> modifiedFlit;

        // Add protocol ID at the beginning
        modifiedFlit.push_back(static_cast<uint8_t>(protocolID >> 8));
        modifiedFlit.push_back(static_cast<uint8_t>(protocolID & 0xFF));

        // Append the original flit
        modifiedFlit.insert(modifiedFlit.end(), flit.begin(), flit.end());

        return modifiedFlit;
    }

    std::vector<std::vector<std::pair<uint8_t, std::string>>> ByteStripping(const std::vector<uint8_t> &flit, int &startLane)
    {
        std::vector<std::vector<std::pair<uint8_t, std::string>>> lanes(16);
        std::string type;

        for (size_t i = 0; i < flit.size(); ++i)
        {
            if (i < 2)
            {
                type = "Protocol ID";
            }
            else if (i >= 2 && i < (flit.size() - 2))
            {
                type = "Data";
            }
            else
            {
                type = "CRC";
            }
            int laneIndex = (startLane + i) % 16;
            lanes[laneIndex].emplace_back(flit[i], type);
        }

        // Update startLane for the next Flit based on the total bytes processed
        startLane = (startLane + flit.size()) % 16;

        return lanes;
    }
};

std::string byteToBinary(uint8_t byte)
{
    return std::bitset<8>(byte).to_string();
}

int main()
{
    Logical_PHY_Layer layer;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 3);

    int startLane = 0; // 시작 레인 초기화

    for (int i = 0; i < 3; ++i)
    {
        std::vector<uint8_t> flit(64, 0xFF); // 64-byte flit with placeholder values for payload

        // Assuming 2-byte CRC is just placeholder values for demonstration
        flit.push_back(0xCC); // CRC byte 1
        flit.push_back(0xCC); // CRC byte 2

        uint16_t protocolID;

        switch (distrib(gen))
        {
        case 0:
            protocolID = Logical_PHY_Layer::CXL_IO_FLIT;
            break;
        case 1:
            protocolID = Logical_PHY_Layer::CXL_CACHE_MEM_FLIT;
            break;
        case 2:
            protocolID = Logical_PHY_Layer::NULL_FLIT;
            break;
        case 3:
            protocolID = Logical_PHY_Layer::ALMP_FLIT;
            break;
        }

        auto modifiedFlit = layer.AddProtocol_ID(flit, protocolID);

        auto lanes = layer.ByteStripping(modifiedFlit, startLane);

        std::cout << "Flit " << i + 1 << ": Completed byte stripping with protocol ID "
                  << byteToBinary(protocolID >> 8) << " " << byteToBinary(protocolID & 0xFF) << std::endl;

        int currentLane = startLane; // Adjust for visual alignment
        for (int lane = 0; lane < 16; ++lane)
        {
            std::cout << "Lane " << std::setw(2) << lane << ": ";
            if (lane < currentLane) // Adjust space for lanes before the startLane
            {
                // 현재 로직을 수정하여, 시작 레인 전까지의 레인에 대해 적절한 공백을 추가합니다.
                std::cout << std::setw(13 * (lanes[currentLane].size() - lanes[lane].size() + 1)) << "";
            }
            for (auto &[byte, type] : lanes[lane])
            {
                std::cout << std::setw(9) << byteToBinary(byte) << "(" << type.substr(0, 1) << ") ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
