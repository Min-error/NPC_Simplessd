/*
 * Copyright (C) 2017 CAMELab
 *
 * This file is part of SimpleSSD.
 *
 * SimpleSSD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleSSD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SimpleSSD.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "icl/icl.hh"
#include "dram/simple.hh"
#include "icl/generic_cache.hh"
#include "util/algorithm.hh"
#include "util/def.hh"
#include "../get_retrynum/chip_info.hh"

namespace SimpleSSD {

namespace ICL {

ICL::ICL(ConfigReader &c) : conf(c) {
  switch (conf.readInt(CONFIG_DRAM, DRAM::DRAM_MODEL)) {
    case DRAM::SIMPLE_MODEL:
      pDRAM = new DRAM::SimpleDRAM(conf);

      break;
    default:
      panic("Undefined DRAM model");

      break;
  }

  pFTL = new FTL::FTL(conf, pDRAM);

  FTL::Parameter *param = pFTL->getInfo();

  if (conf.readBoolean(CONFIG_FTL, FTL::FTL_USE_RANDOM_IO_TWEAK)) {
    totalLogicalPages =
        param->totalLogicalBlocks * param->pagesInBlock * param->ioUnitInPage;
    logicalPageSize = param->pageSize / param->ioUnitInPage;
  }
  else {
    totalLogicalPages = param->totalLogicalBlocks * param->pagesInBlock;
    logicalPageSize = param->pageSize;
  }

  pCache = new GenericCache(conf, pFTL, pDRAM);

  // 初始化输出文件
  // 使用FTLINDEX的最后一位作为test编号，生成对应的文件名
  uint32_t ftlIndex = conf.readInt(CONFIG_FTL, FTL::FTL_INDEX);

  uint32_t testNumber = ftlIndex;  // 获取最后一位数字     
  // testNumber = testNumber + 8;  // 微软的 + 8  从9 开始

  std::cout<<testNumber<<std::endl;

  auto& chipInfo = simplessd::ChipInfo::getInstance();
  std::string model_type = chipInfo.get_ModelType();
  std::string ppe = "PE" + std::to_string(chipInfo.get_PE());
  // std::string ttime = chipInfo.get_Time();
  std::string chip_name = chipInfo.get_ChipName();

  std::string outfilefolder = "data_msr";
  std::string readFileName = outfilefolder + "/" + chip_name + "/read_time" + model_type + "_" + ppe + "_" + std::to_string(testNumber) + ".csv";
  // std::string readFileName = outfilefolder + "/" + chip_name + "/read_time" + model_type + "_" + ppe + "_" + ttime + "_" + std::to_string(testNumber) + ".csv";
  // std::string writeFileName = outfilefolder + "/" + chip_name + "/write_time" + model_type + "_" + ppe + "_" + ttime + "_" + std::to_string(testNumber) + ".csv";
  // std::string readFileName = outfilefolder + "/read_time.csv";
  std::string writeFileName = outfilefolder + "/write_time.csv";


  
  std::cout<<"Output read file to: " << readFileName << std::endl;
  std::cout<<"Output write file to: " << writeFileName << std::endl;


  debugprint(LOG_ICL, 
             "ICL: FTL Index: %u, Test Number: %u, Read file: %s, Write file: %s", 
             ftlIndex, testNumber, readFileName.c_str(), writeFileName.c_str());
  
  readTimeFile.open(readFileName, std::ios::out);
  if (!readTimeFile.is_open()) {
    std::cout << "Error: Cannot open read time file: " << readFileName << std::endl;
    std::exit(1);
  }
  readTimeFile << "reqID,reqSubID,lpn,length,isWLGranularity,beginTick,endTick,readTime" << std::endl;
  
  // 打开写请求时间记录文件
  writeTimeFile.open(writeFileName, std::ios::out);
  if (!writeTimeFile.is_open()) {
    std::cout << "Error: Cannot open write time file: " << writeFileName << std::endl;
    std::exit(1);
  }
  writeTimeFile << "reqID,reqSubID,lpn,length,beginTick,endTick,writeTime" << std::endl;
}

ICL::~ICL() {
  // 关闭输出文件
  if (readTimeFile.is_open()) {
    readTimeFile.close();
  }
  if (writeTimeFile.is_open()) {
    writeTimeFile.close();
  }
  
  delete pCache;
  delete pFTL;
  delete pDRAM;
}

void ICL::read(Request &req, uint64_t &tick) {
  uint64_t beginAt;
  uint64_t finishedAt = tick;
  uint64_t reqRemain = req.length;
  Request reqInternal;
  uint64_t totalBeginTick = tick;  // 记录整个读请求的开始时间

  // 检查请求大小是否超过阈值T (128KB)
  // static uint64_t coldDataThreshold = conf.readUint(CONFIG_FTL, FTL::FTL_COLD_DATA_THRESHOLD);
  // if (req.length > coldDataThreshold) {
  //   req.isWLGranularity = true;  // 设置WL粒度标志

  //   req.isWLGranularity = false;  //  close  WL read

  //   debugprint(LOG_ICL, "READ | Large request detected: %" PRIu64 " bytes > %" PRIu64 " bytes, using WL granularity", 
  //              req.length, coldDataThreshold);
  // } else {
  //   req.isWLGranularity = false; // 设置page粒度标志
  // }

  req.isWLGranularity = false;

  reqInternal.reqID = req.reqID;
  reqInternal.offset = req.offset;
  reqInternal.isWLGranularity = req.isWLGranularity;  // 传递WL粒度标志

  // std::cout<<"icl begin: "<<tick<<std::endl;
  // uint64_t ttick = tick;

  for (uint64_t i = 0; i < req.range.nlp; i++) {
    beginAt = tick;

    reqInternal.reqSubID = i + 1;
    reqInternal.range.slpn = req.range.slpn + i;
    reqInternal.length = MIN(reqRemain, logicalPageSize - reqInternal.offset);
    pCache->read(reqInternal, beginAt);
    reqRemain -= reqInternal.length;
    reqInternal.offset = 0;

    finishedAt = MAX(finishedAt, beginAt);
  }

  debugprint(LOG_ICL,
             "READ  | LCA %" PRIu64 " + %" PRIu64 " | %" PRIu64 " - %" PRIu64
             " (%" PRIu64 ")",
             req.range.slpn, req.range.nlp, tick, finishedAt,
             finishedAt - tick);

  tick = finishedAt;
  tick += applyLatency(CPU::ICL, CPU::READ);
  // std::cout<<"icl end: "<<tick<<" spend time: "<<tick - ttick<<std::endl;
  
  // 记录读请求时间到文件
  if (readTimeFile.is_open()) {
    readTimeFile << req.reqID << "," << req.reqSubID << "," << req.range.slpn << "," 
                 << req.length << "," << (req.isWLGranularity ? "1" : "0") << "," 
                 << totalBeginTick << "," << tick << "," << (tick - totalBeginTick) << std::endl;
  }
}

void ICL::write(Request &req, uint64_t &tick) {
  uint64_t beginAt;
  uint64_t finishedAt = tick;
  uint64_t reqRemain = req.length;
  Request reqInternal;
  uint64_t totalBeginTick = tick;  // 记录整个写请求的开始时间

  reqInternal.reqID = req.reqID;
  reqInternal.offset = req.offset;

  for (uint64_t i = 0; i < req.range.nlp; i++) {
    beginAt = tick;

    reqInternal.reqSubID = i + 1;
    reqInternal.range.slpn = req.range.slpn + i;
    reqInternal.length = MIN(reqRemain, logicalPageSize - reqInternal.offset);
    pCache->write(reqInternal, beginAt);
    reqRemain -= reqInternal.length;
    reqInternal.offset = 0;

    finishedAt = MAX(finishedAt, beginAt);
  }

  debugprint(LOG_ICL,
             "WRITE | LCA %" PRIu64 " + %" PRIu64 " | %" PRIu64 " - %" PRIu64
             " (%" PRIu64 ")",
             req.range.slpn, req.range.nlp, tick, finishedAt,
             finishedAt - tick);

  tick = finishedAt;
  tick += applyLatency(CPU::ICL, CPU::WRITE);
  
  // 记录写请求时间到文件
  if (writeTimeFile.is_open()) {
    writeTimeFile << req.reqID << "," << req.reqSubID << "," << req.range.slpn << "," 
                  << req.length << "," << totalBeginTick << "," << tick << "," 
                  << (tick - totalBeginTick) << std::endl;
  }
}

void ICL::flush(LPNRange &range, uint64_t &tick) {
  uint64_t beginAt = tick;

  pCache->flush(range, tick);

  debugprint(LOG_ICL,
             "FLUSH | LCA %" PRIu64 " + %" PRIu64 " | %" PRIu64 " - %" PRIu64
             " (%" PRIu64 ")",
             range.slpn, range.nlp, beginAt, tick, tick - beginAt);

  tick += applyLatency(CPU::ICL, CPU::FLUSH);
}

void ICL::trim(LPNRange &range, uint64_t &tick) {
  uint64_t beginAt = tick;

  pCache->trim(range, tick);

  debugprint(LOG_ICL,
             "TRIM  | LCA %" PRIu64 " + %" PRIu64 " | %" PRIu64 " - %" PRIu64
             " (%" PRIu64 ")",
             range.slpn, range.nlp, beginAt, tick, tick - beginAt);

  tick += applyLatency(CPU::ICL, CPU::TRIM);
}

void ICL::format(LPNRange &range, uint64_t &tick) {
  uint64_t beginAt = tick;

  pCache->format(range, tick);

  debugprint(LOG_ICL,
             "FORMAT| LCA %" PRIu64 " + %" PRIu64 " | %" PRIu64 " - %" PRIu64
             " (%" PRIu64 ")",
             range.slpn, range.nlp, beginAt, tick, tick - beginAt);

  tick += applyLatency(CPU::ICL, CPU::FORMAT);
}

void ICL::getLPNInfo(uint64_t &t, uint32_t &s) {
  t = totalLogicalPages;
  s = logicalPageSize;
}

uint64_t ICL::getUsedPageCount(uint64_t lcaBegin, uint64_t lcaEnd) {
  uint32_t ratio = pFTL->getInfo()->ioUnitInPage;

  return pFTL->getUsedPageCount(lcaBegin / ratio, lcaEnd / ratio) * ratio;
}

void ICL::getStatList(std::vector<Stats> &list, std::string prefix) {
  pCache->getStatList(list, prefix + "icl.");
  pDRAM->getStatList(list, prefix + "dram.");
  pFTL->getStatList(list, prefix);
}

void ICL::getStatValues(std::vector<double> &values) {
  pCache->getStatValues(values);
  pDRAM->getStatValues(values);
  pFTL->getStatValues(values);
}

void ICL::resetStatValues() {
  pCache->resetStatValues();
  pDRAM->resetStatValues();
  pFTL->resetStatValues();
}

}  // namespace ICL

}  // namespace SimpleSSD
