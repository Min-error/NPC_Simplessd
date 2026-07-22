// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2019 CAMELab
 *
 * Author: Donghyun Gouk <kukdh1@camelab.org>
 */

#include "mcpat.h"

McPAT::McPAT(ParseXML *c) : setting(c) {}

McPAT::~McPAT() {}

void McPAT::getPower(Power &power) {
  Processor proc(setting);

  proc.compute();
  proc.getPower(&power);
}
