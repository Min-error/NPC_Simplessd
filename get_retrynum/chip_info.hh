#ifndef SIMPLESSD_CHIP_INFO_HH
#define SIMPLESSD_CHIP_INFO_HH

#include <string>
#include <iostream>

namespace simplessd {

// Basic description of an NAND/flash chip used by the controller.
// Keep fields minimal and serializable-friendly.
class ChipInfo {
public:
    static ChipInfo& getInstance() {
        if (!instance) {
            instance = new ChipInfo();
        }
        return *instance;
    }

    ChipInfo() = default;

    bool set_info(const std::string& chipName);
    std::string get_ChipName();
    int get_PageSize();
    int get_PageNum();

    void set_PE(int pe);
    void set_Time(std::string time);
    void set_ModelType(std::string modelType);
    int get_PE();
    std::string get_Time();
    std::string get_ModelType();
private:
    static ChipInfo* instance;
    std::string ChipName;
    int PageSize;
    int PageNum;
    int PE;
    std::string Time;
    std::string ModelType;
};

} // namespace simplessd

#endif // SIMPLESSD_CHIP_INFO_HH