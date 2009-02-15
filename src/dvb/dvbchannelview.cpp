/*
 * dvbchannelview.cpp
 *
 * Copyright (C) 2007-2008 Christoph Pfister <christophpfister@gmail.com>
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

#include "dvbchannelview.h"

#include <KAction>
#include <KLocalizedString>
#include "dvbchannel.h"

const DvbSharedChannel *DvbChannelModel::channelForName(const QString &name) const
{
	foreach (const DvbSharedChannel &channel, list) {
		if (channel->name == name) {
			return &channel;
		}
	}

	return NULL;
}

int DvbChannelModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) {
		return 0;
	}

	return 2;
}

QVariant DvbChannelModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || role != Qt::DisplayRole || index.row() >= list.size()) {
		return QVariant();
	}

	switch (index.column()) {
	case 0:
		return list.at(index.row())->name;
	case 1:
		return list.at(index.row())->number;
	}

	return QVariant();
}

QVariant DvbChannelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
		return QVariant();
	}

	switch (section) {
	case 0:
		return i18n("Name");
	case 1:
		return i18n("Number");
	}

	return QVariant();
}

QList<QAction *> DvbChannelModel::createContextActions(bool enableDeleteAction)
{
	QList<QAction *> actions;

	KAction *action = new KAction(i18n("Edit channel"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(actionEdit()));
	actions.append(action);

	action = new KAction(i18n("Change icon"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(actionChangeIcon()));
	actions.append(action);

	if (enableDeleteAction) {
		action = new KAction(i18n("Delete channel"), this);
		connect(action, SIGNAL(triggered()), this, SLOT(actionDelete()));
		actions.append(action);
	}

	return actions;
}
