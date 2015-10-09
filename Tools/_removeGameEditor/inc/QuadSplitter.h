#pragma once

#include <QSplitter>

#include <Core/Editor.h>

MX_SWIPED("http://developer.qt.nokia.com/forums/viewthread/7308");
/*
-----------------------------------------------------------------------------
EdQuadSplitter (aka FourWaySplitter)

	a CAD-style widget which splits a window into 4 panels where the size of the panels is controlled by a ‘cross’ sash that you can drag horizontally, vertically or both at once.
-----------------------------------------------------------------------------
*/
class EdQuadSplitter : public QSplitter
{
	Q_OBJECT

private:
	QSplitter parentSplit;
	QSplitter childSplit1;
	QSplitter childSplit2;

	QWidget *widget1;
	QWidget *widget2;
	QWidget *widget3;
	QWidget *widget4;

	private slots:
		void sync1()
		{
			childSplit2.setSizes(childSplit1.sizes());
		}

		void sync2()
		{
			childSplit1.setSizes(childSplit2.sizes());
		}


public:
	EdQuadSplitter(QWidget *w1, QWidget *w2, QWidget *w3, QWidget *w4);
	void show() {parentSplit.show();}
};