#ifndef CONSOLEPANEL_H
#define CONSOLEPANEL_H

#include <QPlainTextEdit>

class ConsolePanel : public QPlainTextEdit
{
	Q_OBJECT

public:
	ConsolePanel(QWidget *parent);
	~ConsolePanel();

	virtual void keyPressEvent(QKeyEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void contextMenuEvent(QContextMenuEvent *);

	void scrollDown();

public slots:
	void onEnter();
	void onCommand(const QString&);

public:
	void insertPrompt(bool insertNewBlock);

	void output(const QString& s);

	void historyAdd(const QString& cmd);
	void historyBack();
	void historyForward();

private:
	QString prompt;
	bool isLocked;

	QStringList *history;
	int historyPos;
};

class ConsolePanel_DockWidget : public QDockWidget
{
public:
	ConsolePanel_DockWidget(QWidget *parent);

public:
	ConsolePanel* p;
};

#endif // CONSOLEPANEL_H
