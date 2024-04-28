/*
    Author  : Junseung Lee
    Email   : ljs2021@g.skku.edu
    Date    : 2024.02.20
*/

#include <iostream>
#include <vector>
#include <string>
#include <map>
using namespace std;

int rollover_cnt = 0;

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
    int d2hReq;
    int d2hData;
    int d2hRsp;
    int s2mDrs;
    int s2mNdr;
};

Slot DetermineSlotType(int slotNumber, MessageCounts &counts, map<MessageType, int> &flitMessageCount, bool &hasDataHeader);
Flit CreateFlit(MessageCounts &counts);
std::string SlotTypeToString(SlotType type);
std::string MessageTypeToString(MessageType type);
void PrintFlit(const Flit &flit);

int main()
{
    MessageCounts counts = {7, 3, 5, 6, 9}; // 예시 입력

    // 모든 메시지 카운트가 0이 될 때까지 flit 생성 반복
    while (counts.d2hReq > 0 || counts.d2hData > 0 || counts.d2hRsp > 0 || counts.s2mDrs > 0 || counts.s2mNdr > 0)
    {
        std::cout << "############################" << std::endl
                  << "Before Packing MessageCounts: " << std::endl
                  << "D2H_REQ: " << counts.d2hReq << std::endl
                  << "D2H_DATA: " << counts.d2hData << std::endl
                  << "D2H_RSP: " << counts.d2hRsp << std::endl
                  << "S2M_DRS: " << counts.s2mDrs << std::endl
                  << "S2M_NDR: " << counts.s2mNdr << std::endl
                  << std::endl;

        Flit flit = CreateFlit(counts);
        PrintFlit(flit);

        std::cout << std::endl
                  << "After Packing MessageCounts: " << std::endl
                  << "D2H_REQ: " << counts.d2hReq << std::endl
                  << "D2H_DATA: " << counts.d2hData << std::endl
                  << "D2H_RSP: " << counts.d2hRsp << std::endl
                  << "S2M_DRS: " << counts.s2mDrs << std::endl
                  << "S2M_NDR: " << counts.s2mNdr << std::endl
                  << std::endl;

        std::cout << "rollover count: " << rollover_cnt << endl;
    }
    std::cout << "#########################################################" << std::endl;
    std::cout << "############## SIMULATION SUCESSFULLY DONE ##############" << std::endl;
    std::cout << "#########################################################" << std::endl;
    return 0;
}

// Determine Slot
Slot DetermineSlotType(int slotNumber, MessageCounts &counts, map<MessageType, int> &flitMessageCount, bool &hasDataHeader)
{
    Slot slot;
    // Determine Logic for Slot 0 (Header Slot)

    if (slotNumber == 0)
    {
        // Priority: H0 > H1 > H2 > H3 > H4 > H5
        if (((counts.d2hData > 0 && flitMessageCount[D2H_DATA] < 4) ||
             (counts.d2hRsp > 0 && flitMessageCount[D2H_RSP] < 2) ||
             (counts.s2mNdr > 0 && flitMessageCount[S2M_NDR] < 2)) &&
            (hasDataHeader == false))
        {
            // Slot Type: H0
            slot.type = H0;

            // MAX Configruation for H0
            // d2h_Data: 1, d2h_Rsp: 2, s2m_Ndr: 1
            // d2hData가 있으면 추가
            if (counts.d2hData > 0 && flitMessageCount[D2H_DATA] < 4)
            {
                slot.d2hData = 1;
                counts.d2hData--;
                flitMessageCount[D2H_DATA]++;
                hasDataHeader = true;
            }

            // d2hRsp를 추가할 수 있으면 추가
            if (counts.d2hRsp > 0 && flitMessageCount[D2H_RSP] < 2)
            {
                int rspToAdd = std::min(std::min(counts.d2hRsp, 2), 2 - flitMessageCount[D2H_RSP]);
                slot.d2hRsp = rspToAdd;
                counts.d2hRsp -= rspToAdd;
                flitMessageCount[D2H_RSP] += rspToAdd;
            }

            // s2mNdr를 추가할 수 있으면 추가
            if (counts.s2mNdr > 0 && flitMessageCount[S2M_NDR] < 2)
            {
                slot.s2mNdr = 1;
                counts.s2mNdr--;
                flitMessageCount[S2M_NDR]++;
            }
        }
        else if (((counts.d2hReq > 0 && flitMessageCount[D2H_REQ] < 4) ||
                  (counts.d2hData > 0 && flitMessageCount[D2H_DATA] < 4)) &&
                 (hasDataHeader == false))
        {
            // Slot Type: H1
            slot.type = H1;

            // MAX Configruation for H1
            // d2hReq: 1, d2hData: 1

            // D2H_REQ가 있으면 추가
            if (counts.d2hReq > 0 && flitMessageCount[D2H_REQ] < 4)
            {
                slot.d2hReq = 1;
                counts.d2hReq--;
                flitMessageCount[D2H_REQ]++;
            }

            // D2H_DATA가 있으면 추가
            if (counts.d2hData > 0 && flitMessageCount[D2H_DATA] < 4)
            {
                slot.d2hData = 1;
                counts.d2hData--;
                flitMessageCount[D2H_DATA]++;
                hasDataHeader = true;
            }
        }
        else if (((counts.d2hData > 0 && flitMessageCount[D2H_DATA] < 4) ||
                  (counts.d2hRsp > 0 && flitMessageCount[D2H_RSP] < 2)) &&
                 (hasDataHeader == false))
        {
            // Slot Type: H2
            slot.type = H2;

            // Max Configuration for H2
            // d2hData: 4, d2hRsp: 1

            // Checking d2hData
            if (counts.d2hData > 0 && flitMessageCount[D2H_DATA] < 4)
            {
                int dataToAdd = std::min(std::min(counts.d2hData, 4), 4 - flitMessageCount[D2H_DATA]); // 최대 4개, flit 내 최대 개수 고려
                slot.d2hData = dataToAdd;
                counts.d2hData -= dataToAdd;
                flitMessageCount[D2H_DATA] += dataToAdd;
                hasDataHeader = true;
            }

            // Checking d2hRsp
            if (counts.d2hRsp > 0 && flitMessageCount[D2H_RSP] < 2)
            {
                slot.d2hRsp = 1; // d2hRsp는 H2에서 최대 1개
                counts.d2hRsp -= 1;
                flitMessageCount[D2H_RSP] += 1;
            }
        }
        else if (((counts.s2mDrs > 0 && flitMessageCount[S2M_DRS] < 3) ||
                  (counts.s2mNdr > 0 && flitMessageCount[S2M_NDR] < 2)) &&
                 (hasDataHeader == false))
        {
            // Slot Type: H3
            slot.type = H3;

            // Max Configuration for H3
            // s2mDrs: 1. s2mNdr: 1

            // Checking s2mDrs
            if (counts.s2mDrs > 0 && flitMessageCount[S2M_DRS] < 3)
            {
                slot.s2mDrs = 1; // s2mDrs는 H3에서 최대 1개
                counts.s2mDrs -= 1;
                flitMessageCount[S2M_DRS] += 1; // flit 내 S2M_DRS 메시지 카운트 업데이트
                hasDataHeader = true;
            }

            // Checking s2mNdr
            if (counts.s2mNdr > 0 && flitMessageCount[S2M_NDR] < 2)
            {
                slot.s2mNdr = 1; // s2mNdr는 H3에서 최대 1개
                counts.s2mNdr -= 1;
                flitMessageCount[S2M_NDR] += 1; // flit 내 S2M_NDR 메시지 카운트 업데이트
            }
        }
        else if (counts.s2mNdr > 0 && flitMessageCount[S2M_NDR] < 2)
        {
            // Slot Type: H4
            slot.type = H4;

            // Max Configuration for H4: s2mNdr: 최대 2개
            // 현재 flit 내 s2mNdr 메시지의 개수와 추가 가능한 최대 개수를 고려하여 추가할 수 있는 s2mNdr의 개수를 결정
            int NdrToAdd = std::min(std::min(counts.s2mNdr, 2), 2 - flitMessageCount[S2M_NDR]);
            slot.s2mNdr = NdrToAdd;
            counts.s2mNdr -= NdrToAdd;
            flitMessageCount[S2M_NDR] += NdrToAdd; // flit 내 S2M_NDR 메시지 카운트 업데이트
        }
        else if (counts.s2mDrs > 0 && flitMessageCount[S2M_DRS] < 3 && (hasDataHeader == false))
        {
            // Slot Type: H5
            slot.type = H5;

            // Max Configuration for H5: s2mDrs: 최대 2개
            // 현재 flit 내 s2mDrs 메시지의 개수와 추가 가능한 최대 개수를 고려하여 추가할 수 있는 s2mDrs의 개수를 결정
            int DrsToAdd = std::min(std::min(counts.s2mDrs, 2), 3 - flitMessageCount[S2M_DRS]);
            slot.s2mDrs = DrsToAdd;
            counts.s2mDrs -= DrsToAdd;
            flitMessageCount[S2M_DRS] += DrsToAdd; // flit 내 S2M_DRS 메시지 카운트 업데이트
            hasDataHeader = true;
        }
        else
        {
            cout << "[RSVD] G0 TYPE" << endl;
        }
    }
    else
    {
        // Determine Logic for Slot 1~3 (Generic Slot)
        // Priority: G1 > G2 > G3 > G4 > G5 > G6
        if ((counts.d2hReq > 0 && flitMessageCount[D2H_REQ] < 4) ||
            (counts.d2hRsp > 0 && flitMessageCount[D2H_RSP] < 2))
        {
            // Slot Type: G1
            slot.type = G1;

            // Max Configuration for G1: d2hReq: 1, d2h_rsp: 2

            // Checking d2hReq
            if (counts.d2hReq > 0 && flitMessageCount[D2H_REQ] < 4)
            {
                slot.d2hReq = 1; // D2H_REQ는 G1에서 최대 1개, flit 내 최대 4개
                counts.d2hReq -= 1;
                flitMessageCount[D2H_REQ] += 1; // flit 내 D2H_REQ 메시지 카운트 업데이트
            }

            // Checking d2hRsp
            if (counts.d2hRsp > 0 && flitMessageCount[D2H_RSP] < 2)
            {
                int rspToAdd = std::min(std::min(counts.d2hRsp, 2), 2 - flitMessageCount[D2H_RSP]); // flit 당 최대 2개 제한 고려
                slot.d2hRsp = rspToAdd;
                counts.d2hRsp -= rspToAdd;
                flitMessageCount[D2H_RSP] += rspToAdd; // flit 내 D2H_RSP 메시지 카운트 업데이트
            }
        }

        else if (((counts.d2hReq > 0 && flitMessageCount[D2H_REQ] < 4) ||
                  (counts.d2hData > 0 && flitMessageCount[D2H_DATA] < 4) ||
                  (counts.d2hRsp > 0 && flitMessageCount[D2H_RSP] < 2)) &&
                 (hasDataHeader == false))
        {
            // Slot Type: G2
            slot.type = G2;

            // Max Configuration for G2: d2hReq: 1, d2hData: 1, d2hRsp: 1

            // Checking d2hReq
            if (counts.d2hReq > 0 && flitMessageCount[D2H_REQ] < 4)
            {
                slot.d2hReq = 1;
                counts.d2hReq--;
                flitMessageCount[D2H_REQ] += 1; // flit 내 D2H_REQ 메시지 카운트 업데이트
            }

            // Checking d2hData
            if (counts.d2hData > 0 && flitMessageCount[D2H_DATA] < 4)
            {
                slot.d2hData = 1;
                counts.d2hData--;
                flitMessageCount[D2H_DATA] += 1; // flit 내 D2H_DATA 메시지 카운트 업데이트
                hasDataHeader = true;
            }

            // Checking d2hRsp
            if (counts.d2hRsp > 0 && flitMessageCount[D2H_RSP] < 2)
            {
                slot.d2hRsp = 1;
                counts.d2hRsp--;
                flitMessageCount[D2H_RSP] += 1; // flit 내 D2H_RSP 메시지 카운트 업데이트
            }
        }

        else if (counts.d2hData > 0 && flitMessageCount[D2H_DATA] < 4 && (hasDataHeader == false))
        {
            // Slot Type: G3
            slot.type = G3;

            // Max Configuration for G3: d2hData: 최대 4개
            // 현재 flit 내 d2hData 메시지의 개수와 추가 가능한 최대 개수를 고려하여 추가할 수 있는 d2hData의 개수를 결정
            int DataToAdd = std::min(std::min(counts.d2hData, 4), 4 - flitMessageCount[D2H_DATA]);
            slot.d2hData = DataToAdd;
            counts.d2hData -= DataToAdd;
            flitMessageCount[D2H_DATA] += DataToAdd; // flit 내 D2H_DATA 메시지 카운트 업데이트
            hasDataHeader = true;
        }

        else if (((counts.s2mDrs > 0 && flitMessageCount[S2M_DRS] < 3) ||
                  (counts.s2mNdr > 0 && flitMessageCount[S2M_NDR] < 2)) &&
                 (hasDataHeader == false))
        {
            // Slot Type: G4
            slot.type = G4;

            // Max Configuration for G4: s2mDrs: 최대 1개, s2mNdr: 최대 2개

            // Checking s2mDrs
            if (counts.s2mDrs > 0 && flitMessageCount[S2M_DRS] < 3)
            {
                slot.s2mDrs = 1; // s2mDrs는 G4에서 최대 1개
                counts.s2mDrs -= 1;
                flitMessageCount[S2M_DRS] += 1; // flit 내 S2M_DRS 메시지 카운트 업데이트
                hasDataHeader = true;
            }

            // Checking s2mNdr
            if (counts.s2mNdr > 0 && flitMessageCount[S2M_NDR] + slot.s2mNdr < 2)
            {
                int NdrToAdd = std::min(std::min(counts.s2mNdr, 2), 2 - flitMessageCount[S2M_NDR]); // 추가할 s2mNdr 수 결정
                slot.s2mNdr += NdrToAdd;                                                            // s2mNdr 추가
                counts.s2mNdr -= NdrToAdd;
                flitMessageCount[S2M_NDR] += NdrToAdd; // flit 내 S2M_NDR 메시지 카운트 업데이트
            }
        }

        else if (counts.s2mNdr > 0 && flitMessageCount[S2M_NDR] < 2)
        {
            // Slot Type: G5
            slot.type = G5;

            // Max Configuration for G5: s2mNdr: 최대 2개
            // 현재 flit 내 s2mNdr 메시지의 개수와 추가 가능한 최대 개수를 고려하여 추가할 수 있는 s2mNdr의 개수를 결정
            int NdrToAdd = std::min(std::min(counts.s2mNdr, 2), 2 - flitMessageCount[S2M_NDR]);
            slot.s2mNdr = NdrToAdd;
            counts.s2mNdr -= NdrToAdd;
            flitMessageCount[S2M_NDR] += NdrToAdd; // flit 내 S2M_NDR 메시지 카운트 업데이트
        }

        else if (counts.s2mDrs > 0 && flitMessageCount[S2M_DRS] < 3 && (hasDataHeader == false))
        {
            // Slot Type: G6
            slot.type = G6;

            // Max Configuration for G6: s2mDrs: 최대 3개
            // 현재 flit 내 s2mDrs 메시지의 개수와 추가 가능한 최대 개수를 고려하여 추가할 수 있는 s2mDrs의 개수를 결정
            int DrsToAdd = std::min(std::min(counts.s2mDrs, 3), 3 - flitMessageCount[S2M_DRS]);
            slot.s2mDrs = DrsToAdd;
            counts.s2mDrs -= DrsToAdd;
            flitMessageCount[S2M_DRS] += DrsToAdd; // flit 내 S2M_DRS 메시지 카운트 업데이트
            hasDataHeader = true;
        }

        else
        {
            // RSVD
            slot.type = G6;
        }
    }

    return slot;
}

// Flit Generation Logic
Flit CreateFlit(MessageCounts &counts)
{
    Flit flit;
    map<MessageType, int> flitMessageCount; // flit 내 메시지 유형별 카운트를 추적하는 맵
    bool hasDataHeader = false;             // flit 내 data header 존재 여부

    if (rollover_cnt >= 4)
    {
        for (int i = 0; i < 4; ++i)
        {
            Slot slot;
            slot.type = G0;
            flit.addSlot(slot);
            rollover_cnt -= 1;
        }
        return flit;
    }
    else if (rollover_cnt == 3)
    {
        Slot slot = DetermineSlotType(0, counts, flitMessageCount, hasDataHeader);
        flit.addSlot(slot);

        for (int i = 1; i < 4; ++i)
        {
            Slot slot;
            slot.type = G0;
            flit.addSlot(slot);
            rollover_cnt -= 1;
        }

        if (hasDataHeader == true)
        {
            rollover_cnt += 4;
        }

        return flit;
    }

    else if (rollover_cnt == 2)
    {
        bool rollover_valid = true;

        Slot slot = DetermineSlotType(0, counts, flitMessageCount, hasDataHeader);
        flit.addSlot(slot);
        if (hasDataHeader == true)
        {
            rollover_cnt += 4;
        }

        for (int i = 1; i < 3; ++i)
        {
            Slot slot;
            slot.type = G0;
            flit.addSlot(slot);
            rollover_cnt -= 1;
        }

        if (hasDataHeader == true)
        {
            Slot slot;
            slot.type = G0;
            flit.addSlot(slot);
            rollover_cnt -= 1;
        }
        else
        {
            Slot slot = DetermineSlotType(3, counts, flitMessageCount, hasDataHeader);
            flit.addSlot(slot);
            if (hasDataHeader == true)
            {
                rollover_cnt += 4;
            }
        }

        return flit;
    }
    else if (rollover_cnt == 1)
    {
        bool rollover_valid = true;
        Slot slot = DetermineSlotType(0, counts, flitMessageCount, hasDataHeader);
        flit.addSlot(slot);

        Slot slot1;
        slot1.type = G0;
        flit.addSlot(slot1);
        rollover_cnt -= 1;

        for (int i = 2; i < 4; ++i)
        {
            if (hasDataHeader == true)
            {
                Slot slot;
                slot.type = G0;
                flit.addSlot(slot);
                if (rollover_valid == true)
                {
                    rollover_cnt += i;
                    rollover_valid = false;
                }

                continue;
            }
            else
            {
                Slot slot = DetermineSlotType(i, counts, flitMessageCount, hasDataHeader);
                flit.addSlot(slot);
            }
        }

        return flit;
    }
    else if (rollover_cnt == 0)
    {
        bool rollover_valid = true;
        for (int i = 0; i < 4; ++i)
        {
            if (hasDataHeader == true)
            {
                Slot slot;
                slot.type = G0;
                flit.addSlot(slot);
                if (rollover_valid == true)
                {
                    rollover_cnt += i;
                    rollover_valid = false;
                }
                continue;
            }
            else
            {
                Slot slot = DetermineSlotType(i, counts, flitMessageCount, hasDataHeader);
                flit.addSlot(slot);
            }
        }
        return flit;
    }
    else
    {
        rollover_cnt = 0;
    }
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
        if (SlotTypeToString(slot.type) == "G0")
            std::cout << "16B DATA";
        std::cout << std::endl;
    }
}