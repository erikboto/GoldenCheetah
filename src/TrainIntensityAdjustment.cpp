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
    QHBoxLayout *layout = new QHBoxLayout();

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

    layout->addWidget(loadDup);
    layout->addWidget(loadUp);
    layout->addWidget(intensitySlider);
    layout->addWidget(loadDown);
    layout->addWidget(loadDdown);

    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addStretch();

    connect(loadUp, SIGNAL(clicked()), m_trainSidebar, SLOT(Higher()));
    connect(loadDup, SIGNAL(clicked()), m_trainSidebar, SLOT(HigherBigStep()));
    connect(loadDdown, SIGNAL(clicked()), m_trainSidebar, SLOT(LowerBigStep()));
    connect(loadDown, SIGNAL(clicked()), m_trainSidebar, SLOT(Lower()));
    connect(intensitySlider, SIGNAL(valueChanged(int)), m_trainSidebar, SLOT(adjustIntensity(int)));
    connect(m_trainSidebar, SIGNAL(intensityChanged(int)), intensitySlider, SLOT(setValue(int)));

    this->setContentsMargins(0,0,0,0);
    this->setFocusPolicy(Qt::NoFocus);
    this->setAutoFillBackground(false);
    this->setStyleSheet("background-color: rgba( 255, 255, 255, 0% ); border: 0px;");
    this->setLayout(layout);

}
