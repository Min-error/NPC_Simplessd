#include "get_retrynum/chip_info.hh"

namespace simplessd{
ChipInfo* ChipInfo::instance = nullptr;
    
bool ChipInfo::set_info(const std::string& chipName){
    if(chipName == "3DV7"){
        ChipName = chipName;
        PageSize = 18432;
        PageNum = 4224;
        return true;
    }else if(chipName == "X3_9070"){
        ChipName = chipName;
        PageSize = 18368;
        PageNum = 4176;
        return true;
    }else{
        std::cerr << "Chip name error: " << chipName << std::endl;
        return false;
    }
}

void ChipInfo::set_PE(int pe){
    PE = pe;
}

void ChipInfo::set_Time(std::string time){
    Time = time;
}

void ChipInfo::set_ModelType(std::string modelType){
    ModelType = modelType;
}

int ChipInfo::get_PE(){
    return PE;
}

std::string ChipInfo::get_Time(){
    return Time;
}

std::string ChipInfo::get_ModelType(){
    return ModelType;
}

std::string ChipInfo::get_ChipName(){
    return ChipName;
}

int ChipInfo::get_PageSize(){
    return PageSize;
}

int ChipInfo::get_PageNum(){
    return PageNum;
}

}