/*
 * playlisttab.cpp
 *
 * Copyright (C) 2009 Christoph Pfister <christophpfister@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "playlisttab.h"

#include <QBoxLayout>
#include <QKeyEvent>
#include <QListView>
#include <QSplitter>
#include <QToolButton>
#include <QTreeView>
#include <KAction>
#include <KActionCollection>
#include <kfilewidget.h>
#include <KLocalizedString>
#include <KMenu>
#include "../mediawidget.h"
#include "playlistmodel.h"

class PlaylistView : public QTreeView
{
public:
	explicit PlaylistView(QWidget *parent) : QTreeView(parent) { }
	~PlaylistView() { }

protected:
	void keyPressEvent(QKeyEvent *event);
};

void PlaylistView::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Delete) {
		QModelIndexList selectedRows = selectionModel()->selectedRows();
		qSort(selectedRows);

		for (int i = selectedRows.size() - 1; i >= 0; --i) {
			// FIXME compress
			model()->removeRows(selectedRows.at(i).row(), 1);
		}

		return;
	}

	QTreeView::keyPressEvent(event);
}

PlaylistTab::PlaylistTab(KMenu *menu, KActionCollection *collection, MediaWidget *mediaWidget_) :
	mediaWidget(mediaWidget_)
{
	playlistModel = new PlaylistModel(mediaWidget, this);

	KAction *repeatAction = new KAction(KIcon("media-playlist-repeat"),
					    i18nc("playlist menu", "Repeat"), this);
	repeatAction->setCheckable(true);
	connect(repeatAction, SIGNAL(triggered(bool)), playlistModel, SLOT(repeatPlaylist(bool)));
	menu->addAction(collection->addAction("playlist_repeat", repeatAction));

	KAction *shuffleAction = new KAction(KIcon("media-playlist-shuffle"),
					     i18nc("playlist menu", "Shuffle"), this);
	connect(shuffleAction, SIGNAL(triggered(bool)), playlistModel, SLOT(shufflePlaylist()));
	menu->addAction(collection->addAction("playlist_shuffle", shuffleAction));

	KAction *clearAction = new KAction(KIcon("edit-clear-list"),
					    i18nc("playlist menu", "Clear"), this);
	connect(clearAction, SIGNAL(triggered(bool)), playlistModel, SLOT(clearPlaylist()));
	menu->addAction(collection->addAction("playlist_clear", clearAction));

	menu->addSeparator();

	KAction *newAction = new KAction(KIcon("list-add"), i18nc("playlist menu", "New"), this);
	connect(newAction, SIGNAL(triggered(bool)), this, SLOT(newPlaylist()));
	menu->addAction(collection->addAction("playlist_new", newAction));

	QBoxLayout *widgetLayout = new QHBoxLayout(this);
	widgetLayout->setMargin(0);

	QSplitter *horizontalSplitter = new QSplitter(this);
	widgetLayout->addWidget(horizontalSplitter);

	QSplitter *verticalSplitter = new QSplitter(Qt::Vertical, horizontalSplitter);

	QWidget *widget = new QWidget(verticalSplitter);
	QBoxLayout *sideLayout = new QVBoxLayout(widget);
	sideLayout->setMargin(0);

	QBoxLayout *boxLayout = new QHBoxLayout();

	QToolButton *toolButton = new QToolButton(widget);
	toolButton->setDefaultAction(newAction);
	toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	boxLayout->addWidget(toolButton);

	boxLayout->addStretch();
	sideLayout->addLayout(boxLayout);

	QListView *playlistBrowserView = new QListView(widget);
	connect(playlistBrowserView, SIGNAL(activated(QModelIndex)),
		this, SLOT(playlistActivated(QModelIndex)));
	sideLayout->addWidget(playlistBrowserView);

	KFileWidget *fileWidget = new KFileWidget(KUrl(), verticalSplitter);
	fileWidget->setFilter(MediaWidget::extensionFilter());
	fileWidget->setMode(KFile::Files | KFile::ExistingOnly);
	verticalSplitter->setStretchFactor(1, 1);

	verticalSplitter = new QSplitter(Qt::Vertical, horizontalSplitter);

	widget = new QWidget(verticalSplitter);
	sideLayout = new QVBoxLayout(widget);
	sideLayout->setMargin(0);

	boxLayout = new QHBoxLayout();

	toolButton = new QToolButton(widget);
	toolButton->setDefaultAction(repeatAction);
	toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	boxLayout->addWidget(toolButton);

	toolButton = new QToolButton(widget);
	toolButton->setDefaultAction(shuffleAction);
	toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	boxLayout->addWidget(toolButton);

	toolButton = new QToolButton(widget);
	toolButton->setDefaultAction(clearAction);
	toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	boxLayout->addWidget(toolButton);

	boxLayout->addStretch();
	sideLayout->addLayout(boxLayout);

	playlistView = new PlaylistView(widget);
	playlistView->setAlternatingRowColors(true);
	playlistView->setDragDropMode(QAbstractItemView::DragDrop);
	playlistView->setRootIsDecorated(false);
	playlistView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	playlistView->setModel(playlistModel);
	playlistView->sortByColumn(0, Qt::AscendingOrder);
	playlistView->setSortingEnabled(true);
	connect(playlistView, SIGNAL(activated(QModelIndex)),
		playlistModel, SLOT(playTrack(QModelIndex)));
	sideLayout->addWidget(playlistView);

	QWidget *mediaContainer = new QWidget(verticalSplitter);
	mediaLayout = new QHBoxLayout(mediaContainer);
	mediaLayout->setMargin(0);

	verticalSplitter->setStretchFactor(1, 1);
	horizontalSplitter->setStretchFactor(1, 1);
}

PlaylistTab::~PlaylistTab()
{
}

void PlaylistTab::playUrls(const QList<KUrl> &urls)
{
	playlistModel->appendUrls(urls, false);
}

void PlaylistTab::newPlaylist()
{
	// FIXME
}

void PlaylistTab::playlistActivated(const QModelIndex &index)
{
	// FIXME
}

void PlaylistTab::activate()
{
	mediaLayout->addWidget(mediaWidget);
}
