#include "get_retrynum/get_retrynum.hh"
#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>

namespace simplessd {
GetRetryNum* GetRetryNum::instance = nullptr;

// 在实现文件中定义这些常量
const std::vector<std::string> GetRetryNum::XSB_LIST = {"LSB", "CSB", "MSB"};
const std::vector<int> GetRetryNum::PE_LIST = {3000, 4000, 5000, 6000};
const std::vector<std::string> GetRetryNum::TIME_LIST = {"1m", "3m", "6m", "12m"};

std::vector<std::vector<int>> GetRetryNum::read_csv(const std::string& file_path) {
    // 去掉第一行和第一列
    std::vector<std::vector<int>> data;
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file: " << file_path << std::endl;
        return data; // 返回空vector而不是exit
    }
    
    std::string line;
    bool first_line = true;

    while (std::getline(file, line)) {
        if (first_line) {
            first_line = false;
            continue;
        }

        std::stringstream line_stream(line);
        std::string cell;
        std::vector<int> row;

        bool first_cell = true;
        while (std::getline(line_stream, cell, ',')) {
            try {
                if(first_cell){
                    first_cell = false;
                    continue;
                }
                row.push_back(static_cast<int>(std::round(std::stod(cell))));
            } catch (const std::invalid_argument& e) {
                // 处理错误
            } catch (const std::out_of_range& e) {
                // 处理错误
            }
        }

        if (!row.empty()) {
            data.push_back(row);
        }
    }

    file.close();
    return data;
}

bool GetRetryNum::read_all_blockPE(const std::string& path) {
    std::string file_path = path + "block_rt_assignment.csv";
    std::cout << "Read block to PE from: " << file_path << std::endl;

    blockPE = read_csv(file_path);
    return !blockPE.empty();
}

bool GetRetryNum::read_all_retrynum(const std::string& path, const std::string& model_type) {
    for(const auto& xsb : XSB_LIST) {
        std::string file_path = path + "all_wl_" + xsb + "_error" + model_type + "_retry_num_average_order_pe.csv";
        std::cout << "Read retry num from: " << file_path << std::endl;

        auto csv_data = read_csv(file_path);
        if (!csv_data.empty()) {
            data_.push_back(csv_data);
        }
    }
    return !data_.empty();
}

size_t GetRetryNum::get_xsb_index(const std::string& xsb) {
    for (size_t i = 0; i < XSB_LIST.size(); ++i) {
        if (XSB_LIST[i] == xsb) {
            return static_cast<int>(i);
        }
    }
    return static_cast<size_t>(-1);
}

size_t GetRetryNum::get_pe_index(int pe) {
    for (size_t i = 0; i < PE_LIST.size(); ++i) {
        if (PE_LIST[i] == pe) {
            return static_cast<int>(i);
        }
    }
    return static_cast<size_t>(-1);
}

uint32_t GetRetryNum::get_block_time(uint32_t block){
    return blockPE[block][1];
}

size_t GetRetryNum::get_time_index(const std::string& time) {
    for (size_t i = 0; i < TIME_LIST.size(); ++i) {
        if (TIME_LIST[i] == time) {
            return static_cast<int>(i);
        }
    }
    return static_cast<size_t>(-1);
}

int GetRetryNum::get_retrynum(const std::string& xsb, int wl, int pe, const std::string& time) {
    size_t xsb_idx = get_xsb_index(xsb);
    if (xsb_idx == static_cast<size_t>(-1)) {
        std::cerr << "XSB error: " << xsb << std::endl;
        return -1;
    }
    
    size_t pe_idx = get_pe_index(pe);
    // size_t pe_idx = get_pe_index(get_block_time(block));
    if (pe_idx == static_cast<size_t>(-1)) {
        std::cerr << "PE error: " << pe << std::endl;
        return -1;
    }
    
    size_t time_idx = get_time_index(time);
    // size_t time_idx = get_time_index(get_block_time(block));
    if (time_idx == static_cast<size_t>(-1)) {
        std::cerr << "Time error: " << time << std::endl;
        return -1;
    }

    size_t index = pe_idx * TIME_LIST.size() + time_idx;
    
    // 添加边界检查
    if (xsb_idx < data_.size() &&
        static_cast<size_t>(wl) < data_[xsb_idx].size() &&
        index < data_[xsb_idx][wl].size()) {
        return data_[xsb_idx][wl][index];
    }

    std::cerr << "Index error" << std::endl;
    return -1; // 索引越界
}

void GetRetryNum::set_pageIndex(uint32_t pageindex){
    pageIndex = pageindex;
}

uint32_t GetRetryNum::get_pageIndex(){
    return pageIndex;
}

void GetRetryNum::set_block(uint32_t block){
    block = block;
}

uint32_t GetRetryNum::get_block(){
    return block;
}

} // namespace simplessd