//
//  Copyright (c) 2012 Pansenti, LLC.
//	
//  This file is part of Syntro
//
//  Syntro is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Syntro is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Syntro.  If not, see <http://www.gnu.org/licenses/>.
//

#include "CameraDlg.h"
#include <qboxlayout.h>
#include <qformlayout.h>
#include "CamClient.h"

CameraDlg::CameraDlg(QWidget *parent)
    : QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	layoutWindow();
	setWindowTitle("Camera configuration");
	connect(m_buttons, SIGNAL(accepted()), this, SLOT(onOk()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void CameraDlg::onOk()
{
	bool changed = false;

	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup(CAMERA_GROUP);

	if (m_index->text() != settings->value(CAMERA_CAMERA).toString()) {
		settings->setValue(CAMERA_CAMERA, m_index->text());
		changed = true;
	}

	if (m_width->text() != settings->value(CAMERA_WIDTH).toString()) {
		settings->setValue(CAMERA_WIDTH, m_width->text());
		changed = true;
	}

	if (m_height->text() != settings->value(CAMERA_HEIGHT).toString()) {
		settings->setValue(CAMERA_HEIGHT, m_height->text());
		changed = true;
	}

	if (m_rate->text() != settings->value(CAMERA_FRAMERATE).toString()) {
		settings->setValue(CAMERA_FRAMERATE, m_rate->text());
		changed = true;
	}

	settings->endGroup();

	delete settings;

    if (changed)
		accept();
    else
        reject();
}

void CameraDlg::layoutWindow()
{
	QSettings *settings = SyntroUtils::getSettings();

    settings->beginGroup(CAMERA_GROUP);

	QVBoxLayout *centralLayout = new QVBoxLayout(this);
	centralLayout->setSpacing(20);
	centralLayout->setContentsMargins(11, 11, 11, 11);
	
	QFormLayout *formLayout = new QFormLayout();
	formLayout->setSpacing(16);
	formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	m_index = new QLineEdit(this);
	m_index->setMaximumWidth(60);
	formLayout->addRow(tr("Index of camera"), m_index);
	m_index->setText(settings->value(CAMERA_CAMERA).toString());
	m_index->setValidator(new QIntValidator(0, 64));

	m_width = new QLineEdit(this);
	m_width->setMaximumWidth(60);
	formLayout->addRow(tr("Frame width"), m_width);
	m_width->setText(settings->value(CAMERA_WIDTH).toString());
	m_width->setValidator(new QIntValidator(120, 1920));

	m_height = new QLineEdit(this);
	m_height->setMaximumWidth(60);
	formLayout->addRow(tr("Frame height"), m_height);
	m_height->setText(settings->value(CAMERA_HEIGHT).toString());
	m_height->setValidator(new QIntValidator(80, 1200));

	m_rate = new QLineEdit(this);
	m_rate->setMaximumWidth(60);
	formLayout->addRow(tr("Frame rate"), m_rate);
	m_rate->setText(settings->value(CAMERA_FRAMERATE).toString());
	m_rate->setValidator(new QIntValidator(1, 100));

	centralLayout->addLayout(formLayout);

	m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	m_buttons->setCenterButtons(true);

	centralLayout->addWidget(m_buttons);

	settings->endGroup();
	delete settings;
}
