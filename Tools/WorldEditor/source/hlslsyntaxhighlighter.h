#ifndef HLSLSYNTAXHIGHLIGHTER_H
#define HLSLSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

/*
-----------------------------------------------------------------------------
	HLSLSyntaxHighlighter
-----------------------------------------------------------------------------
*/
class HLSLSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	HLSLSyntaxHighlighter(QTextDocument* parent);
	~HLSLSyntaxHighlighter();

protected:
	virtual void highlightBlock(const QString& text);

private:

};

/*
===================================================
	Syntax highlighting, autocompletion, etc.
===================================================
*/

enum ETokenType
{
	TT_Keyword = 0,
	TT_Comment,
	TT_DataType,
	TT_BuiltinVar,
	TT_BuiltinFunction,
	TT_Number,
	TT_String,
	TT_Misc,
	TT_MAX
};

struct HighlightingRule
{
	QRegExp pattern;
	ETokenType type;
};
/*
-----------------------------------------------------------------------------
	HLSLUtil
-----------------------------------------------------------------------------
*/
class HLSLUtil
	: public TGlobal< HLSLUtil >
{
public:
	HLSLUtil();
	~HLSLUtil();

public_internal:

	QList< HighlightingRule >	rules;

	QString multiLineCommentStart;
	QString multiLineCommentEnd;

	QVector< QTextCharFormat >	formats; // idx is FormatType

private:
	void setupFormats();
	void setupRules();
	void setupComments();
};

#endif // HLSLSYNTAXHIGHLIGHTER_H
