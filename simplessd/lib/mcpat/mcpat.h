// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2019 CAMELab
 *
 * Author: Donghyun Gouk <kukdh1@camelab.org>
 */

#pragma once

#ifndef __LIB_MCPAT__
#define __LIB_MCPAT__

#include "XML_Parse.h"
#include "processor.h"

class McPAT {
 private:
  ParseXML *setting;

 public:
  McPAT(ParseXML *);
  ~McPAT();

  void getPower(Power &);
};

#endif
