#ifndef GPDX_H
#define GPDX_H
#include "Stock.h"
#include "GPDX_config.h"
#ifdef SUPPORT_GPDX

bool isGPDX_FTMR(const QVector<Stock>& stocks,int end);
bool isGPDX_SPMR(const QVector<Stock>& stocks,int end);

#endif
#endif