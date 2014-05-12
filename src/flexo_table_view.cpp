#include "flexo_table_view.h"
#include <QContextMenuEvent>

FlexoTableView::FlexoTableView(QWidget * parent /* = 0 */)
    : QTableView(parent)
{}

FlexoTableView::~FlexoTableView()
{}

void FlexoTableView::currentChanged(const QModelIndex & current, 
	const QModelIndex & previous)
{
	emit currentCellChanged(current, previous);
	emit cellSelected(current);
	base::currentChanged(current, previous);
}


void FlexoTableView::contextMenuEvent(QContextMenuEvent * event)
{
	event->accept();
}
#include "flexo_table_view.moc"
