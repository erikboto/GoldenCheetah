/*
 * Copyright (c) 2015 Erik Botö (erik.boto@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "TrainIntensityAdjustment.h"
#include "TrainSidebar.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>

TrainIntensityAdjustment::TrainIntensityAdjustment(TrainSidebar *trainSidebar, QWidget *parent) :
    QWidget(parent),
    m_trainSidebar(trainSidebar)
{
    QHBoxLayout *intensityControlLayout = new QHBoxLayout();

    QIcon dupIcon(":images/oxygen/up-arrow-double-bw.png");
    QPushButton *loadDup = new QPushButton(dupIcon, "", this);
    loadDup->setFocusPolicy(Qt::NoFocus);
    loadDup->setIconSize(QSize(64,64));
    loadDup->setAutoFillBackground(false);
    loadDup->setAutoDefault(false);
    loadDup->setFlat(true);
    loadDup->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");

    QIcon upIcon(":images/oxygen/up-arrow-bw.png");
    QPushButton *loadUp = new QPushButton(upIcon, "", this);
    loadUp->setFocusPolicy(Qt::NoFocus);
    loadUp->setIconSize(QSize(64,64));
    loadUp->setAutoFillBackground(false);
    loadUp->setAutoDefault(false);
    loadUp->setFlat(true);
    loadUp->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");

    QIcon downIcon(":images/oxygen/down-arrow-bw.png");
    QPushButton *loadDown = new QPushButton(downIcon, "", this);
    loadDown->setFocusPolicy(Qt::NoFocus);
    loadDown->setIconSize(QSize(64,64));
    loadDown->setAutoFillBackground(false);
    loadDown->setAutoDefault(false);
    loadDown->setFlat(true);
    loadDown->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");

    QIcon ddownIcon(":images/oxygen/down-arrow-double-bw.png");
    QPushButton *loadDdown = new QPushButton(ddownIcon, "", this);
    loadDdown->setFocusPolicy(Qt::NoFocus);
    loadDdown->setIconSize(QSize(64,64));
    loadDdown->setAutoFillBackground(false);
    loadDdown->setAutoDefault(false);
    loadDdown->setFlat(true);
    loadDdown->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");

    QSlider *intensitySlider = new QSlider(Qt::Horizontal, this);
    intensitySlider->setAutoFillBackground(false);
    intensitySlider->setFocusPolicy(Qt::NoFocus);
    intensitySlider->setMinimum(75);
    intensitySlider->setMaximum(125);
    intensitySlider->setValue(100);

    intensityControlLayout->addWidget(loadDdown);
    intensityControlLayout->addWidget(loadDown);
    intensityControlLayout->addWidget(intensitySlider);
    intensityControlLayout->addWidget(loadUp);
    intensityControlLayout->addWidget(loadDup);

    intensityControlLayout->setContentsMargins(0,0,0,0);
    intensityControlLayout->setSpacing(0);

    connect(loadUp, SIGNAL(clicked()), m_trainSidebar, SLOT(Higher()));
    connect(loadDup, SIGNAL(clicked()), m_trainSidebar, SLOT(HigherBigStep()));
    connect(loadDdown, SIGNAL(clicked()), m_trainSidebar, SLOT(LowerBigStep()));
    connect(loadDown, SIGNAL(clicked()), m_trainSidebar, SLOT(Lower()));
    connect(intensitySlider, SIGNAL(valueChanged(int)), m_trainSidebar, SLOT(adjustIntensity(int)));
    connect(m_trainSidebar, SIGNAL(intensityChanged(int)), intensitySlider, SLOT(setValue(int)));


    // Control buttons
    QHBoxLayout *toolbuttons = new QHBoxLayout;
    toolbuttons->setSpacing(0);
    toolbuttons->setContentsMargins(0,0,0,0);

    QIcon rewIcon(":images/oxygen/rewind.png");
    QPushButton *rewind = new QPushButton(rewIcon, "", this);
    rewind->setFocusPolicy(Qt::NoFocus);
    rewind->setIconSize(QSize(64,64));
    rewind->setAutoFillBackground(false);
    rewind->setAutoDefault(false);
    rewind->setFlat(true);
    rewind->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");
    rewind->setAutoRepeat(true);
    rewind->setAutoRepeatDelay(200);
#if QT_VERSION > 0x050400
    rewind->setShortcut(Qt::Key_MediaPrevious);
#endif
    toolbuttons->addWidget(rewind);

    QIcon stopIcon(":images/oxygen/stop.png");
    QPushButton *stop = new QPushButton(stopIcon, "", this);
    stop->setFocusPolicy(Qt::NoFocus);
    stop->setIconSize(QSize(64,64));
    stop->setAutoFillBackground(false);
    stop->setAutoDefault(false);
    stop->setFlat(true);
    stop->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");
#if QT_VERSION > 0x050400
    stop->setShortcut(Qt::Key_MediaStop);
#endif
    toolbuttons->addWidget(stop);

    QIcon playIcon(":images/oxygen/play.png");
    m_playButton = new QPushButton(playIcon, "", this);
    m_playButton->setFocusPolicy(Qt::NoFocus);
    m_playButton->setIconSize(QSize(64,64));
    m_playButton->setAutoFillBackground(false);
    m_playButton->setAutoDefault(false);
    m_playButton->setFlat(true);
    m_playButton->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");
    m_playButton->setShortcut(Qt::Key_MediaTogglePlayPause);
    toolbuttons->addWidget(m_playButton);

    QIcon fwdIcon(":images/oxygen/ffwd.png");
    QPushButton *forward = new QPushButton(fwdIcon, "", this);
    forward->setFocusPolicy(Qt::NoFocus);
    forward->setIconSize(QSize(64,64));
    forward->setAutoFillBackground(false);
    forward->setAutoDefault(false);
    forward->setFlat(true);
    forward->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");
    forward->setAutoRepeat(true);
    forward->setAutoRepeatDelay(200);
#if QT_VERSION > 0x050400
    forward->setShortcut(Qt::Key_MediaNext);
#endif
    toolbuttons->addWidget(forward);

    QIcon lapIcon(":images/oxygen/lap.png");
    QPushButton *lap = new QPushButton(lapIcon, "", this);
    lap->setFocusPolicy(Qt::NoFocus);
    lap->setIconSize(QSize(64,64));
    lap->setAutoFillBackground(false);
    lap->setAutoDefault(false);
    lap->setFlat(true);
    lap->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");
#if QT_VERSION > 0x050400
    lap->setShortcut(Qt::Key_0);
#endif
    toolbuttons->addWidget(lap);
    toolbuttons->addStretch();

    QHBoxLayout *allControlsLayout = new QHBoxLayout();
    allControlsLayout->addLayout(toolbuttons);
    allControlsLayout->addLayout(intensityControlLayout);

    connect(m_playButton, SIGNAL(clicked()), m_trainSidebar, SLOT(Start()));
    connect(rewind, SIGNAL(clicked()), m_trainSidebar, SLOT(Rewind()));
    connect(forward, SIGNAL(clicked()), m_trainSidebar, SLOT(FFwd()));
    connect(lap, SIGNAL(clicked()), m_trainSidebar, SLOT(newLap()));
    connect(stop, SIGNAL(clicked()), m_trainSidebar, SLOT(Stop()));
    connect(m_trainSidebar->context, SIGNAL(start()), this, SLOT(updatePlayButtonIcon()));
    connect(m_trainSidebar->context, SIGNAL(pause()), this, SLOT(updatePlayButtonIcon()));
    connect(m_trainSidebar->context, SIGNAL(unpause()), this, SLOT(updatePlayButtonIcon()));
    connect(m_trainSidebar->context, SIGNAL(stop()), this, SLOT(updatePlayButtonIcon()));

    this->setContentsMargins(0,0,0,0);
    this->setFocusPolicy(Qt::NoFocus);
    this->setAutoFillBackground(false);
    this->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");
    this->setLayout(allControlsLayout);

}

void TrainIntensityAdjustment::updatePlayButtonIcon()
{
    static QIcon playIcon(":images/oxygen/play.png");
    static QIcon pauseIcon(":images/oxygen/pause.png");

    if (m_trainSidebar->currentStatus() & RT_PAUSED)
    {
        m_playButton->setIcon(playIcon);
    }
    else if (m_trainSidebar->currentStatus() & RT_RUNNING)
    {
        m_playButton->setIcon(pauseIcon);
    }
    else // Not running or paused means stopped
    {
        m_playButton->setIcon(playIcon);
    }
}
