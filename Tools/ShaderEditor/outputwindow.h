#ifndef EDWIDGET_OUTPUTWINDOW_H
#define EDWIDGET_OUTPUTWINDOW_H

#include <Base/Memory/Pool/TPool.h>

#include <Base/Text/Parse_Util.h>

#include <QDockWidget>

class OutputWindow
	: public QDockWidget, public mxOutputDevice
	, public TSingleton< OutputWindow >
{
	Q_OBJECT

public:
	typedef QWidget Super;

	OutputWindow(QWidget *parent = 0);
	~OutputWindow();

	virtual void Log( ELogLevel level, const char* message, UINT numChars );
	virtual void VARARGS Logf( ELogLevel level, const char* fmt, ... );

	void Log( const CompileMessage& msg );

	void Clear();

signals:
	void messageClicked(int line, int column);


protected:
	virtual void closeEvent( QCloseEvent* event );

	virtual bool eventFilter(QObject* object, QEvent* event);

private:
	struct LogEntry: public QTextBlockUserData
	{
		int line;
		int column;

	public:
		LogEntry()
		{
			line = 0;
			column = 0;
		}
	};

private:
	QPlainTextEdit *	plainTextEdit;
};

#endif // EDWIDGET_OUTPUTWINDOW_H
