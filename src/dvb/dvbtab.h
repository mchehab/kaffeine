/*
 * dvbtab.h
 *
 * Copyright (C) 2007-2009 Christoph Pfister <christophpfister@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef DVBTAB_H
#define DVBTAB_H

#include <QSharedDataPointer>
#include "../tabbase.h"

class QModelIndex;
class QSplitter;
class KAction;
class KActionCollection;
class KMenu;
class DvbChannel;
class DvbChannelView;
class DvbDevice;
class DvbLiveStream;
class DvbManager;
class DvbTimeShiftCleaner;
class MediaWidget;

class DvbTab : public TabBase
{
	Q_OBJECT
public:
	DvbTab(KMenu *menu, KActionCollection *collection, MediaWidget *mediaWidget_);
	~DvbTab();

	DvbManager *getDvbManager() const
	{
		return dvbManager;
	}

	DvbDevice *getLiveDevice() const;
	QSharedDataPointer<DvbChannel> getLiveChannel() const;

	void playChannel(const QString &name);
	void playLastChannel();

private slots:
	void showChannelDialog();
	void showEpgDialog();
	void showRecordingDialog();
	void instantRecord(bool checked);
	void instantRecordingRemoved();
	void configureDvb();
	void playLive(const QModelIndex &index);
	void previousChannel();
	void nextChannel();
	void prepareTimeShift();
	void startTimeShift();
	void changeAudioChannel(int index);
	void changeSubtitle(int index);
	void liveStopped();
	void osdKeyPressed(int key);
	void tuneOsdChannel();
	void fastRetuneTimeout();
	void cleanTimeShiftFiles();

private:
	void activate();
	void playChannel(const QSharedDataPointer<DvbChannel> &channel);

	MediaWidget *mediaWidget;
	DvbManager *dvbManager;
	KAction *instantRecordAction;
	QSplitter *splitter;
	DvbChannelView *channelView;
	QLayout *mediaLayout;

	QString osdChannel;
	QTimer *osdChannelTimer;
	QTimer *fastRetuneTimer;
	DvbLiveStream *liveStream;

	DvbTimeShiftCleaner *timeShiftCleaner;
};

#endif /* DVBTAB_H */
