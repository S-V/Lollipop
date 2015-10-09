#include <stdafx.h>
#pragma hdrstop
#include <QtSupport/common.h>
#include "editor_common.h"
#include "app.h"
#include "QuadSplitter.h"

/*
-----------------------------------------------------------------------------
	EdQuadSplitter
-----------------------------------------------------------------------------
*/
EdQuadSplitter::EdQuadSplitter(QWidget *w1, QWidget *w2, QWidget *w3, QWidget *w4)
: widget1(w1),
widget2(w2),
widget3(w3),
widget4(w4)
{
	parentSplit.setOrientation(Qt::Vertical);

	childSplit1.addWidget(w1);
	childSplit1.addWidget(w2);
	childSplit2.addWidget(w3);
	childSplit2.addWidget(w4);

	parentSplit.addWidget(&childSplit1);
	parentSplit.addWidget(&childSplit2);

	connect (&childSplit1,
		SIGNAL(splitterMoved(int,int)),
		this,
		SLOT(sync1()));

	connect (&childSplit2,
		SIGNAL(splitterMoved(int,int)),
		this,
		SLOT(sync2()));
}

/*
#include <QApplication>
#include <QTextEdit>
#include "EdQuadSplitter.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QTextEdit *t1 = new QTextEdit;
	QTextEdit *t2 = new QTextEdit;
	QTextEdit *t3 = new QTextEdit;
	QTextEdit *t4 = new QTextEdit;

	EdQuadSplitter quadSplit(t1, t2, t3, t4);
	quadSplit.show();

	return app.exec();
}
*/
