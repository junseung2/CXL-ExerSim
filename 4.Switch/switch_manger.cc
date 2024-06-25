#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

class Device
{
public:
    virtual std::string getType() = 0;
};

class SLDDevice : public Device
{
public:
    std::string getType() override
    {
        return "SLD";
    }
};

class MLDDevice : public Device
{
private:
    int numLDs;

public:
    MLDDevice(int numLDs) : numLDs(numLDs) {}
    std::string getType() override
    {
        return "MLD";
    }
    int getNumLDs()
    {
        return numLDs;
    }
};

class VCS
{
public:
    int id;
    std::unordered_map<int, std::pair<int, int>> virtualDownstreamPorts; // key: virtual port, value: (downstream port, LD)

    VCS(int id) : id(id) {}
    void mapPort(int virtualPort, int downstreamPort, int ld = -1)
    {
        virtualDownstreamPorts[virtualPort] = std::make_pair(downstreamPort, ld);
    }
};

class FM
{
private:
    std::unordered_map<int, VCS *> vcsMap;
    std::unordered_map<int, Device *> downstreamDevices;

public:
    void addVCS(VCS *vcs)
    {
        vcsMap[vcs->id] = vcs;
    }

    void addDownstreamDevice(int port, Device *device)
    {
        downstreamDevices[port] = device;
    }

    void updateMapping(int vcsId, int virtualPort, int downstreamPort, int ld = -1)
    {
        if (vcsMap.find(vcsId) != vcsMap.end())
        {
            vcsMap[vcsId]->mapPort(virtualPort, downstreamPort, ld);
        }
    }

    void showMapping()
    {
        for (const auto &vcsPair : vcsMap)
        {
            VCS *vcs = vcsPair.second;
            std::cout << "VCS " << vcs->id << " Mapping:" << std::endl;
            for (const auto &portPair : vcs->virtualDownstreamPorts)
            {
                int virtualPort = portPair.first;
                int downstreamPort = portPair.second.first;
                int ld = portPair.second.second;
                std::cout << "  Virtual Port " << virtualPort << " -> Downstream Port " << downstreamPort;
                if (ld != -1)
                {
                    std::cout << ", LD " << ld;
                }
                std::cout << std::endl;
            }
        }
    }
};

int main()
{
    // Create VCS instances
    VCS vcs0(0);
    VCS vcs1(1);

    // Create FM instance
    FM fabricManager;
    fabricManager.addVCS(&vcs0);
    fabricManager.addVCS(&vcs1);

    // Create devices
    SLDDevice sld0;
    SLDDevice sld1;
    MLDDevice mld2(16);

    // Add devices to FM
    fabricManager.addDownstreamDevice(0, &sld0);
    fabricManager.addDownstreamDevice(1, &sld1);
    fabricManager.addDownstreamDevice(2, &mld2);

    // Initial mapping
    fabricManager.updateMapping(0, 0, 0); // VCS0 -> SLD0
    for (int i = 0; i < 8; ++i)
    {
        fabricManager.updateMapping(0, i + 1, 2, i); // VCS0 -> MLD2 LD0-LD7
    }
    fabricManager.updateMapping(1, 0, 1); // VCS1 -> SLD1
    for (int i = 0; i < 8; ++i)
    {
        fabricManager.updateMapping(1, i + 1, 2, i + 8); // VCS1 -> MLD2 LD8-LD15
    }

    // Show initial mapping
    std::cout << "Initial Mapping:" << std::endl;
    fabricManager.showMapping();

    // Update mapping (example of runtime change)
    std::cout << "\nUpdating Mapping at Runtime:" << std::endl;
    fabricManager.updateMapping(0, 0, 1); // Change VCS0 to use SLD1
    fabricManager.updateMapping(1, 0, 0); // Change VCS1 to use SLD0

    // Add more complexity: map VCS0 to a new virtual port with MLD
    fabricManager.updateMapping(0, 9, 2, 9);  // VCS0 -> MLD2 LD9
    fabricManager.updateMapping(1, 9, 2, 10); // VCS1 -> MLD2 LD10

    // Further mapping updates
    fabricManager.updateMapping(0, 10, 2, 11); // VCS0 -> MLD2 LD11
    fabricManager.updateMapping(1, 10, 2, 12); // VCS1 -> MLD2 LD12

    // Show updated mapping
    std::cout << "\nUpdated Mapping:" << std::endl;
    fabricManager.showMapping();

    return 0;
}
