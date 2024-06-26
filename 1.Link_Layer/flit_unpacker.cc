/*
    Author  : Junseung Lee
    email   : ljs2021@g.skku.edu
*/

#include <iostream>
#include <vector>
#include <string>
#include <map>
using namespace std;

// Type of Message
enum MessageType
{
    D2H_REQ,
    D2H_DATA,
    D2H_RSP,
    S2M_DRS,
    S2M_NDR
};

// Type of Slot
enum SlotType
{
    H0, // Header format
    H1,
    H2,
    H3,
    H4,
    H5,
    G0, // Generic format
    G1,
    G2,
    G3,
    G4,
    G5,
    G6
};

// Struct of Slot
struct Slot
{
    SlotType type;
    int d2hReq = 0;
    int d2hData = 0;
    int d2hRsp = 0;
    int s2mDrs = 0;
    int s2mNdr = 0;
};

// Struct of Flit
struct Flit
{
    std::vector<Slot> slots;

    void addSlot(const Slot &slot)
    {
        if (slots.size() < 4)
        {
            slots.push_back(slot);
        }
    }
};

struct MessageCounts
{
    int d2hReq = 0;
    int d2hData = 0;
    int d2hRsp = 0;
    int s2mDrs = 0;
    int s2mNdr = 0;
};

std::string SlotTypeToString(SlotType type);
std::string MessageTypeToString(MessageType type);
void PrintFlit(const Flit &flit);
MessageCounts UnpackFlit(const Flit &flit);

int main()
{
    // 예시로 Flit 생성
    Flit flit;
    Slot slot1 = {H0, 1, 1, 1, 1, 1};
    Slot slot2 = {G1, 1, 0, 0, 0, 0};
    Slot slot3 = {G2, 1, 1, 0, 0, 0};
    Slot slot4 = {G3, 0, 1, 0, 0, 0};
    flit.addSlot(slot1);
    flit.addSlot(slot2);
    flit.addSlot(slot3);
    flit.addSlot(slot4);

    PrintFlit(flit);

    MessageCounts counts = UnpackFlit(flit);

    std::cout << "After Unpacking Flit: " << std::endl
              << "D2H_REQ: " << counts.d2hReq << std::endl
              << "D2H_DATA: " << counts.d2hData << std::endl
              << "D2H_RSP: " << counts.d2hRsp << std::endl
              << "S2M_DRS: " << counts.s2mDrs << std::endl
              << "S2M_NDR: " << counts.s2mNdr << std::endl
              << std::endl;

    return 0;
}

// SlotType 및 MessageType 열거형을 문자열로 변환하는 함수
std::string SlotTypeToString(SlotType type)
{
    switch (type)
    {
    case H0:
        return "H0";
    case H1:
        return "H1";
    case H2:
        return "H2";
    case H3:
        return "H3";
    case H4:
        return "H4";
    case H5:
        return "H5";
    case G0:
        return "G0";
    case G1:
        return "G1";
    case G2:
        return "G2";
    case G3:
        return "G3";
    case G4:
        return "G4";
    case G5:
        return "G5";
    case G6:
        return "G6";
    default:
        return "Unknown";
    }
}

std::string MessageTypeToString(MessageType type)
{
    switch (type)
    {
    case D2H_REQ:
        return "D2H_REQ";
    case D2H_DATA:
        return "D2H_DATA";
    case D2H_RSP:
        return "D2H_RSP";
    case S2M_DRS:
        return "S2M_DRS";
    case S2M_NDR:
        return "S2M_NDR";
    default:
        return "Unknown";
    }
}

// PrintFlit 함수 구현
void PrintFlit(const Flit &flit)
{
    std::cout << "Flit contains " << flit.slots.size() << " slots." << std::endl;
    for (size_t i = 0; i < flit.slots.size(); ++i)
    {
        const Slot &slot = flit.slots[i];
        std::cout << "Slot " << i << " [" << SlotTypeToString(slot.type) << "]: ";
        if (slot.d2hReq > 0)
            std::cout << MessageTypeToString(D2H_REQ) << ": " << slot.d2hReq << ", ";
        if (slot.d2hData > 0)
            std::cout << MessageTypeToString(D2H_DATA) << ": " << slot.d2hData << ", ";
        if (slot.d2hRsp > 0)
            std::cout << MessageTypeToString(D2H_RSP) << ": " << slot.d2hRsp << ", ";
        if (slot.s2mDrs > 0)
            std::cout << MessageTypeToString(S2M_DRS) << ": " << slot.s2mDrs << ", ";
        if (slot.s2mNdr > 0)
            std::cout << MessageTypeToString(S2M_NDR) << ": " << slot.s2mNdr;
        std::cout << std::endl;
    }
}

// Flit을 Unpacking하는 함수
MessageCounts UnpackFlit(const Flit &flit)
{
    MessageCounts counts = {0, 0, 0, 0, 0}; // 메시지 카운트를 저장할 구조체 초기화

    for (const Slot &slot : flit.slots)
    {
        counts.d2hReq += slot.d2hReq;
        counts.d2hData += slot.d2hData;
        counts.d2hRsp += slot.d2hRsp;
        counts.s2mDrs += slot.s2mDrs;
        counts.s2mNdr += slot.s2mNdr;
    }

    return counts;
}
