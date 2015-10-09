#pragma once

class LogWidget
	: public QDockWidget
	, public mxOutputDevice
{
public:
	LogWidget( QWidget* parent = nil );
	~LogWidget();

	void ClearLog();

//=-- mxOutputDevice
	virtual	void Log( ELogLevel level, const char* message, UINT numChars ) override;

private:

	// Output widget

	QPlainTextEdit	m_plainTextEdit;
};
