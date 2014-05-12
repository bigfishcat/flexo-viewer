#ifndef FLEXO_TABLE_VIEW_H
#define FLEXO_TABLE_VIEW_H

#include <QTableView>

class FlexoTableView : public QTableView
{
Q_OBJECT

	typedef QTableView base;

public:
    FlexoTableView(QWidget *parent = 0);
    ~FlexoTableView();

signals:
	void currentCellChanged(const QModelIndex & current, 
		const QModelIndex & previous);
	void cellSelected(const QModelIndex & current);

protected slots:
	void currentChanged(const QModelIndex & current, 
		const QModelIndex & previous);

protected:
	void contextMenuEvent(QContextMenuEvent * event);
};

#endif
