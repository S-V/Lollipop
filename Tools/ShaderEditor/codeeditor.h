#ifndef DOCUMENTEDITOR_H
#define DOCUMENTEDITOR_H

#include <QTextEdit>

class CodeEditor
	: public QPlainTextEdit//QTextEdit
	, public TSingleton< CodeEditor >
{
	Q_OBJECT

public:

	CodeEditor(QWidget *parent = 0);
	~CodeEditor();

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

protected:
	virtual void keyPressEvent( QKeyEvent* event );

	virtual void closeEvent( QCloseEvent* event );
	virtual void resizeEvent(QResizeEvent *event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);

public slots:

	bool loadDocument(const QString& name);
	void saveDocument();

private:
	void setShaderName(const QString& name);
	const char* getShaderName() const;

private:
	QWidget *lineNumberArea;
	String	shaderName;
};

class LineNumberArea : public QWidget
{
public:
	LineNumberArea(CodeEditor *editor) : QWidget(editor) {
		codeEditor = editor;
	}

	QSize sizeHint() const {
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent *event) {
		codeEditor->lineNumberAreaPaintEvent(event);
	}

private:
	CodeEditor *codeEditor;
};

#endif // DOCUMENTEDITOR_H
