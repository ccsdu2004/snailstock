#ifndef ADDON_BOLL_C_H
#define ADDON_BOLL_C_H
#include <QObject>
#include "Stock.h"
#include "Indicator.h"

bool isFit(const QVector<Stock>& list,const QVector<DataWrapper5<bool, float, float, float, float>>& bolls,float input);

#endif
