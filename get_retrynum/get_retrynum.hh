#ifndef GET_RETRYNUM_HH
#define GET_RETRYNUM_HH

#include <vector>
#include <string>
#include <iostream>
#include <istream>
#include <fstream>

namespace simplessd {

class GetRetryNum {
public:
    static GetRetryNum& getInstance() {
        if (!instance) {
            instance = new GetRetryNum();
        }
        return *instance;
    }

    GetRetryNum() = default;

    bool read_all_retrynum(const std::string& path, const std::string& model_type);
    bool read_all_blockPE(const std::string& path);

    int get_retrynum(const std::string& XSB, int WL, int pe, const std::string& time);

    void set_pageIndex(uint32_t pageindex);
    uint32_t get_pageIndex();
    void set_block(uint32_t block);
    uint32_t get_block();
    uint32_t get_block_time(uint32_t block);

    static const std::vector<std::string> XSB_LIST;
    static const std::vector<int> PE_LIST;
    static const std::vector<std::string> TIME_LIST;
private:
    static GetRetryNum* instance;
    std::vector<std::vector<std::vector<int>>> data_; // XSB / WL / pe&&time
    std::vector<std::vector<int>> blockPE; // block 2 PE
    uint32_t pageIndex;
    uint32_t block;

    std::vector<std::vector<int>> read_csv(const std::string& path);
    size_t get_xsb_index(const std::string& xsb);
    size_t get_pe_index(int pe);
    size_t get_time_index(const std::string& time);
};

} // namespace simplessd

#endif // GET_RETRYNUM_HH