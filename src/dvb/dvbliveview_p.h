/*
 * dvbliveview_p.h
 *
 * Copyright (C) 2007-2011 Christoph Pfister <christophpfister@gmail.com>
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

#ifndef DVBLIVEVIEW_P_H
#define DVBLIVEVIEW_P_H

#include <QFile>
#include "../mediawidget.h"
#include "../osdwidget.h"
#include "dvbepg.h"
#include "dvbsi.h"
#include "dvbmanager.h"

class QSocketNotifier;

class DvbOsd : public OsdObject
{
public:
	enum OsdLevel {
		Off,
		ShortOsd,
		LongOsd
	};

	DvbOsd() : level(Off) { }
	~DvbOsd() { }

	void init(DvbManager *manager_, OsdLevel level_, const QString &channelName_,
		const QList<DvbSharedEpgEntry> &epgEntries);

	OsdLevel level;

private:
	QPixmap paintOsd(QRect &rect, const QFont &font, Qt::LayoutDirection direction);

	QString channelName;
	DvbEpgEntry firstEntry;
	DvbEpgEntry secondEntry;
	DvbManager *manager;
};

class DvbLiveViewInternal : public QObject, public DvbPidFilter, public MediaSource
{
	Q_OBJECT
public:
	explicit DvbLiveViewInternal(QObject *parent);
	~DvbLiveViewInternal();

	void resetPipe();

	bool overrideAudioStreams() const { return !audioStreams.isEmpty(); }
	QStringList getAudioStreams() const { return audioStreams; }
	QStringList getSubtitles() const { return QStringList(); }
	int getCurrentAudioStream() const { return currentAudioStream; }
	int getCurrentSubtitle() const { return currentSubtitle; }

	void setCurrentAudioStream(int currentAudioStream_)
	{
		currentAudioStream = currentAudioStream_;
		emit currentAudioStreamChanged(currentAudioStream);
	}

	void setCurrentSubtitle(int currentSubtitle_)
	{
		currentSubtitle = currentSubtitle_;
		emit currentSubtitleChanged(currentSubtitle);
	}

	bool overrideCaption() const { return true; }

	QString getDefaultCaption() const { return channelName; }

	Type getType() const { return Dvb; }

	QUrl getUrl() const { return url; }

	void updateUrl() {
		if (timeShiftFile.isOpen())
			url = QUrl::fromLocalFile(timeShiftFile.fileName());
		else
			url = QUrl::fromLocalFile(fileName);
	}

	virtual void validateCurrentTotalTime(int &currentTime, int &totalTime) const;
	bool hideCurrentTotalTime() const { return !timeshift; }

	MediaWidget *mediaWidget;
	QString channelName;
	DvbPmtFilter pmtFilter;
	QByteArray pmtSectionData;
	DvbSectionGenerator patGenerator;
	DvbSectionGenerator pmtGenerator;
	QByteArray buffer;
	QFile timeShiftFile;
	QString fileName;
	DvbOsd dvbOsd;
	bool emptyBuffer;
	QTime startTime;
	bool timeshift;
	QStringList audioStreams;
	int currentAudioStream;
	int currentSubtitle;
	int retryCounter;

signals:
	void currentAudioStreamChanged(int currentAudioStream);
	void currentSubtitleChanged(int currentSubtitle);
	void replay();
	void playbackFinished();
	void playbackStatusChanged(MediaWidget::PlaybackStatus playbackStatus);
	void previous();
	void next();

private slots:
	void writeToPipe();

private:
	void processData(const char data[188]);

	QUrl url;
	int readFd;
	int writeFd;
	QSocketNotifier *notifier;
	QList<QByteArray> buffers;
};

#endif /* DVBLIVEVIEW_P_H */
