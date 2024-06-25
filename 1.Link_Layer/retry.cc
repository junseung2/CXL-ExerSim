/*
    Author  : Junseung Lee
    email   : ljs2021@g.skku.edu
*/

#include <iostream>
#include <vector>
#include <random>
#include <cassert>

const int LLRB_SIZE = 64;
const int TIMEOUT = 5;

/**
 * Flit struct. .
 *
 */
struct Flit
{
    int data; // 예시로 단순한 데이터를 포함; 실제 구현에서는 필요한 모든 필드를 포함해야 함
    // 다른 필드들 (헤더 정보, CRC 등) 추가 가능

    Flit(int data = 0) : data(data) {}
};

class DeviceTX
{
public:
    int WrPtr = 0;
    int RdPtr = 0;
    int AckPtr = 0;
    int NumFreeBuf = LLRB_SIZE;
    std::vector<Flit> LLRB; // Flit 객체를 저장하는 벡터

    DeviceTX() : LLRB(LLRB_SIZE) {}

    void writeFlit(const Flit &flit)
    {
        assert(NumFreeBuf > 0); // 사용 가능한 버퍼가 있는지 확인
        LLRB[WrPtr] = flit;
        WrPtr = (WrPtr + 1) % LLRB_SIZE;
        NumFreeBuf--;
    }

    void sendFlit()
    {
        incrementRdPtr();
    }

    void receiveAck(int numAcks)
    {
        AckPtr = (AckPtr + numAcks) % LLRB_SIZE;
        NumFreeBuf += numAcks;
    }

    void incrementRdPtr()
    {
        RdPtr = (RdPtr + 1) % LLRB_SIZE;
    }

    void decrementRdPtr()
    {
        RdPtr = (RdPtr - 1) % LLRB_SIZE;
    }
};

class DeviceRX
{
public:
    int ESeq = 0;
    int NumAcks = 0;
    int Num_Retry = 0;

    void receiveFlit(const Flit &flit)
    {
        ESeq = (ESeq + 1) % LLRB_SIZE;
        NumAcks++;
    }

    bool shouldSendAck()
    {
        return NumAcks >= 8;
    }

    void sendAck(DeviceTX &tx)
    {
        if (shouldSendAck())
        {
            tx.receiveAck(NumAcks);
            NumAcks = 0;
        }
    }

    void retry(DeviceTX &tx, bool decrement_valid)
    {
        for (int i = 0; i < 5; i++)
            std::cout << "RETRY.Frame" << std::endl;

        std::cout << "RETRY.Req(Eseq= " << ESeq << ", Num_Retry= " << Num_Retry << ")" << std::endl;

        if (decrement_valid)
            tx.decrementRdPtr();
    }
};

int main()
{
    DeviceTX tx;
    DeviceRX rx;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disFor10Percent(1, 10);
    std::uniform_int_distribution<> disFor50Percent(1, 3);

    bool wirtevalid_n = 0;
    bool retry_complete = 0;

    // 예시: device_TX가 100개의 flit을 전송
    for (int i = 0; i < 100; i++)
    {

        bool CRC_check = (disFor10Percent(gen) != 1); // 10% 확률로 CRC 체크 실패
        bool decrement_valid = 1;
        rx.Num_Retry = 0;

        std::cout << "Before Send Flit: "
                  << "WrPtr: " << tx.WrPtr << ", RdPtr: " << tx.RdPtr << ", AckPtr: "
                  << tx.AckPtr << ", NumFreeBuf: " << tx.NumFreeBuf << std::endl;
        Flit flit(i); // Flit 객체 생성

        if (!wirtevalid_n)
            tx.writeFlit(flit); // flit 전송

        tx.sendFlit(); // RdPtr 증가

        if (CRC_check)
        {
            std::cout << "### [Flit " << i << "] CRC Check Sucessfully ###" << std::endl;
            rx.receiveFlit(flit); // device_RX가 flit 수신

            if (rx.shouldSendAck())
            {
                rx.sendAck(tx); // 조건 충족 시 ack 전송
            }
            wirtevalid_n = 0;
        }
        else
        {
            std::cout << "### [Flit " << i << "] CRC Check ERROR. Retry System [START]  ###" << std::endl;

            while (true)
            {

                rx.retry(tx, decrement_valid);
                decrement_valid = 0;
                retry_complete = (disFor50Percent(gen) == 1);

                if (retry_complete)
                    break;

                else
                {
                    std::cout << "### RETRY FAIL. TRY TO RETRY AGAIN ###" << std::endl;
                    rx.Num_Retry++;
                }

                if (rx.Num_Retry == TIMEOUT)
                {
                    std::cout << "###  NEED TO LINK RETRAINING  ###" << std::endl;
                    break;
                }
            }

            wirtevalid_n = 1;
            i -= 1;
        }

        std::cout << "After Send Flit: "
                  << "WrPtr: " << tx.WrPtr << ", RdPtr: " << tx.RdPtr << ", AckPtr: "
                  << tx.AckPtr << ", NumFreeBuf: " << tx.NumFreeBuf << std::endl
                  << std::endl;
    }

    if (tx.WrPtr == tx.RdPtr)
    {
        std::cout << "#########################################################" << std::endl;
        std::cout << "############## SIMULATION SUCESSFULLY DONE ##############" << std::endl;
        std::cout << "#########################################################" << std::endl;
    }
    else
    {

        std::cout << "#########################################################" << std::endl;
        std::cout << "##############       SIMULATION Fail       ##############" << std::endl;
        std::cout << "#########################################################" << std::endl;
    }
    return 0;
}
