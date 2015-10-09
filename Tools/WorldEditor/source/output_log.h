/*
=============================================================================
	File:	console.h
	Desc:	Log, engine output.
=============================================================================
*/
#pragma once

#include "editor_system.h"

#define ED_CONSOLE_USE_LIST_WIDGET	0

/*
-----------------------------------------------------------------------------
	EdOutputLog
-----------------------------------------------------------------------------
*/
class EdOutputLog
	: public QDockWidget
	, public EdModule
	, public mxOutputDevice
	, public EdWidgetChild
	, SingleInstance< EdOutputLog >
{
	Q_OBJECT

public:
	EdOutputLog();
	~EdOutputLog();

	//--mxOutputDevice
	virtual	void Log( ELogLevel level, const char* message, UINT numChars ) override;

	//--EdModule
	virtual void SetDefaultValues() override;

	// serialize editor_app-specific settings
	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

signals:
	void signal_ConsoleChanged();

private slots:
	void slot_ScrollToBottom();

private:

	// Output widget

#if ED_CONSOLE_USE_LIST_WIDGET
	QListWidget		m_listWidget;
#else
	QPlainTextEdit	m_plainTextEdit;
#endif // ED_CONSOLE_USE_LIST_WIDGET

	// for showing console in the editor
	EdAction		m_viewConsoleAct;
};
