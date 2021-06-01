#ifndef CONFIG_H
#define CONFIG_H

#define SUPPORT_VIEW_K_
#define SUPPORT_MACD
#define SUPPORT_OHLC
#define SUPPORT_TICKET
#define SUPPORT_LEVEL2
#define SUPPORT_BASIC
#define SUPPORT_PER
#define SUPPORT_BATCH_SCRIPT
#define SUPPORT_BIG_SPAN
#define SUPPORT_SCRIPT_
#define SUPPORT_BID
#define SUPPORT_SEA_SCRIPT
#define SUPPORT_BOOSTING_ALGO
#define SUPPORT_hyperbola_algo
#define SUPPORT_REF
#define SUPPORT_PAIRS_TRADE
#define hb _
#define SUPPORT_HK_MARKET
#define SUPPORT_IMPLICIT_VAR_

#ifdef SUPPORT_BASIC
    #define SUPPORT_TUSHARE
#endif

#ifdef SUPPORT_BASIC
    #define SUPPORT_GPDX
#endif

#define SUPPORT_STOCK_GRID_TRADE
#define SUPPORT_STOCK_GRID_TRADE_UPDATE_STOCK

#define SUPPORT_STOCK_ANALYZE

#define SUPPORT_UPDATE_STOCK
#define SUPPORT_UPDATE_STOCK_TICK

#define SUPPORT_STOCK_VALUATION
#define SUPPORT_UPDATE_FINANCE_DATA

#define SUPPORT_STOCK_SIZE 8888


#endif