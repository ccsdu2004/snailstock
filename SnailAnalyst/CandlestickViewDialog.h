#ifndef CANDLESTICK_VIEW_DIALOG_H
#define CANDLESTICK_VIEW_DIALOG_H
#include <qdialog.h>
#include <QtWebEngineWidgets/qwebengineview.h>

class CandlestickViewDialog : public QDialog
{
    Q_OBJECT
public:
	CandlestickViewDialog(QWidget* parent = nullptr);

private:
	QWebEngineView* webView;
};

#endif