/*
 * manager.cpp
 *
 * Copyright (C) 2007 Christoph Pfister <christophpfister@gmail.com>
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

#include <config.h>

#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>

#include <KAction>
#include <KActionCollection>
#include <KLocalizedString>

#include "kaffeine.h"
#include "mediawidget.h"
#include "manager.h"
#include "manager.moc"

void TabBase::activate()
{
	if (!ignoreActivate) {
		ignoreActivate = true;
		emit activating(this);
		ignoreActivate = false;
		internalActivate();
	}
}

class StartTab : public TabBase
{
public:
	explicit StartTab(Manager *manager_);
	~StartTab() { }

private:
	void internalActivate() { }
};

StartTab::StartTab(Manager *manager_) : TabBase(manager_)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	QLabel *label = new QLabel(i18n("<font size=\"+4\"><b>[Kaffeine Player]</b><br>caffeine for your desktop!</font>"));
	label->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	label->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum));
	QPalette pal = label->palette();
	pal.setColor(label->backgroundRole(), QColor(127, 127, 255));
	label->setPalette(pal);
	label->setAutoFillBackground(true);
	layout->addWidget(label);
	QWidget *widget = new QWidget(this);
	pal = widget->palette();
	pal.setColor(widget->backgroundRole(), QColor(255, 255, 255));
	widget->setPalette(pal);
	widget->setAutoFillBackground(true);
	layout->addWidget(widget);
}

class PlayerTab : public TabBase
{
public:
	PlayerTab(Manager *manager_, MediaWidget *mediaWidget_);
	~PlayerTab() { }

private:
	void internalActivate()
	{
		layout()->addWidget(mediaWidget);
	}

	MediaWidget *mediaWidget;
};

PlayerTab::PlayerTab(Manager *manager_, MediaWidget *mediaWidget_)
	: TabBase(manager_), mediaWidget(mediaWidget_)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(mediaWidget);
}

Manager::Manager(Kaffeine *kaffeine) : QWidget(kaffeine),
	currentState(~stateAlways)
{
	mediaWidget = new MediaWidget(this);

	stackedLayout = new QStackedLayout(this);
	buttonGroup = new QButtonGroup(this);

	startTab = new StartTab(this);
	playerTab = new PlayerTab(this, mediaWidget);

	KActionCollection *collection = kaffeine->actionCollection();
	KAction *action;

	action = KStandardAction::open(kaffeine, SLOT(actionOpen()), collection);
	addAction(collection, "file_open", stateAlways, action);

	action = KStandardAction::quit(kaffeine, SLOT(close()), collection);
	addAction(collection, "file_quit", stateAlways, action);

	action = new KAction(KIcon("media-skip-backward"), i18n("Previous"), collection);
	addAction(collection, "controls_previous", statePrevNext | statePlaying, action);

	actionPlayPause = new KAction(collection);
	QObject::connect(actionPlayPause, SIGNAL(triggered(bool)), kaffeine, SLOT(actionPlayPause(bool)));
	textPlay = i18n("Play");
	textPause = i18n("Pause");
	iconPlay = KIcon("media-playback-start");
	iconPause = KIcon("media-playback-pause");
	addAction(collection, "controls_play_pause", stateAlways, actionPlayPause);

	action = new KAction(KIcon("media-playback-stop"), i18n("Stop"), collection);
	QObject::connect(action, SIGNAL(triggered(bool)), mediaWidget, SLOT(stop()));
	addAction(collection, "controls_stop", statePlaying, action);

	action = new KAction(KIcon("media-skip-forward"), i18n("Next"), collection);
	addAction(collection, "controls_next", statePrevNext, action);

	action = new KAction(collection);
	action->setDefaultWidget(mediaWidget->newVolumeSlider());
	addAction(collection, "controls_volume", stateAlways, action);

	action = new KAction(collection);
	widgetPositionSlider = mediaWidget->newPositionSlider();
	action->setDefaultWidget(widgetPositionSlider);
	addAction(collection, "position_slider", stateAlways, action);

	action = new KAction(collection);
	action->setDefaultWidget(addTab(i18n("Start"), startTab));
	addAction(collection, "tabs_start", stateAlways, action);

	action = new KAction(collection);
	action->setDefaultWidget(addTab(i18n("Player"), playerTab));
	addAction(collection, "tabs_player", stateAlways, action);

	startTab->activate();
	setState(stateAlways);
}

void Manager::activating(TabBase *tab)
{
	stackedLayout->setCurrentWidget(tab);
}

void Manager::addAction(KActionCollection *collection, const QString &name,
	Manager::stateFlags flags, KAction *action)
{
	collection->addAction(name, action);
	if (flags != stateAlways)
		actionList.append(qMakePair(flags, action));
}

void Manager::setState(stateFlags newState)
{
	if (currentState == newState)
		return;

	if (((currentState ^ newState) & statePlaying) == statePlaying) {
		if ((newState & statePlaying) == statePlaying) {
			actionPlayPause->setText(textPause);
			actionPlayPause->setIcon(iconPause);
			actionPlayPause->setCheckable(true);
			widgetPositionSlider->setEnabled(true);
		} else {
			actionPlayPause->setText(textPlay);
			actionPlayPause->setIcon(iconPlay);
			actionPlayPause->setCheckable(false);
			widgetPositionSlider->setEnabled(false);
		}
	}

	QPair<stateFlags, KAction *> action;
	foreach (action, actionList)
		action.second->setEnabled((action.first & newState) != stateAlways);

	currentState = newState;
}

QPushButton *Manager::addTab(const QString &name, TabBase *tab)
{
	QPushButton *pushButton = new QPushButton(name);
	pushButton->setCheckable(true);
	pushButton->setFocusPolicy(Qt::NoFocus);
	connect(pushButton, SIGNAL(clicked(bool)), tab, SLOT(activate()));
	connect(tab, SIGNAL(activating(TabBase *)), pushButton, SLOT(click()));
	connect(tab, SIGNAL(activating(TabBase *)), this, SLOT(activating(TabBase *)));
	buttonGroup->addButton(pushButton);
	stackedLayout->addWidget(tab);
	return pushButton;
}
