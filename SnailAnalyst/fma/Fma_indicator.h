#ifndef FMA_FMA_H
#define FMA_FMA_H
#include "common/common.h"
#include "../SnailAddonHost.h"

bool isFMA(const QVector<Stock>& stocks,QVector<DataWrapper5<bool,float,float,float,float>>& flags);

#endif