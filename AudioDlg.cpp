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

#include "AudioDlg.h"

#include <qlist.h>
#include <qboxlayout.h>
#include <qformlayout.h>
#include "AudioDriver.h"

AudioDlg::AudioDlg(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Audio configuration");

	m_channelMap[0] = 1;
	m_channelMap[1] = 2;

	m_rateMap[0] = 8000;
    m_rateMap[1] = 48000;

	layoutWindow();

	connect(m_buttons, SIGNAL(accepted()), this, SLOT(onOk()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(onCancel()));
}

AudioDlg::~AudioDlg()
{

}

void AudioDlg::onOk()
{
	bool changed = false;

	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup(AUDIO_GROUP);

	if ((m_enable->checkState() == Qt::Checked) != settings->value(AUDIO_ENABLE).toBool()) {
		settings->setValue(AUDIO_ENABLE, m_enable->checkState() == Qt::Checked);
		changed = true;
	}

    if (m_inputCard->text() != settings->value(AUDIO_INPUT_CARD).toString()) {
        settings->setValue(AUDIO_INPUT_CARD, m_inputCard->text());
        changed = true;
    }
    if (m_inputDevice->text() != settings->value(AUDIO_INPUT_DEVICE).toString()) {
		settings->setValue(AUDIO_INPUT_DEVICE, m_inputDevice->text());
		changed = true;
	}

	if (m_channels->currentIndex() != -1) {
		if (m_channelMap[m_channels->currentIndex()] != settings->value(AUDIO_CHANNELS).toInt()) {
			settings->setValue(AUDIO_CHANNELS, m_channelMap[m_channels->currentIndex()]);
			changed = true;
		}
	}

	if (m_sampleRate->currentIndex() != -1) {
		if (m_rateMap[m_sampleRate->currentIndex()] != settings->value(AUDIO_SAMPLERATE).toInt()) {
			settings->setValue(AUDIO_SAMPLERATE, m_rateMap[m_sampleRate->currentIndex()]);
			changed = true;
		}
	}

	settings->endGroup();

	delete settings;

	if (changed) {
		emit newAudio();
		accept();
	}
	reject();
}

void AudioDlg::onCancel()
{
	reject();
}

void AudioDlg::layoutWindow()
{

	QSettings *settings = SyntroUtils::getSettings();

    setModal(true);
	settings->beginGroup(AUDIO_GROUP);

	QVBoxLayout *centralLayout = new QVBoxLayout(this);
	centralLayout->setSpacing(20);
	centralLayout->setContentsMargins(11, 11, 11, 11);
	
	QFormLayout *formLayout = new QFormLayout();
	formLayout->setSpacing(16);
	formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	m_enable = new QCheckBox(this);
    m_enable->setMinimumWidth(100);
	formLayout->addRow(tr("Enable audio"), m_enable);
	m_enable->setCheckState(settings->value(AUDIO_ENABLE).toBool() ? Qt::Checked : Qt::Unchecked);

    m_inputCard = new QLineEdit(this);
    m_inputCard->setMaximumWidth(60);
    formLayout->addRow(tr("Audio card"), m_inputCard);
    m_inputCard->setText(settings->value(AUDIO_INPUT_CARD).toString());
    m_inputCard->setValidator(new QIntValidator(0, 64));

	m_inputDevice = new QLineEdit(this);
	m_inputDevice->setMaximumWidth(60);
	formLayout->addRow(tr("Audio device"), m_inputDevice);
	m_inputDevice->setText(settings->value(AUDIO_INPUT_DEVICE).toString());
    m_inputDevice->setValidator(new QIntValidator(0, 64));

	m_channels = new QComboBox(this);
	m_channels->setMaximumWidth(60);
	
	for (int i = 0; i < 2; i++) {
		m_channels->addItem(QString::number(m_channelMap[i]));
		if (m_channelMap[i] == settings->value(AUDIO_CHANNELS).toInt())
			m_channels->setCurrentIndex(i);
	}
	formLayout->addRow(tr("Channels"), m_channels);

	m_sampleRate = new QComboBox(this);
    m_sampleRate->setMaximumWidth(80);
	
    for (int i = 0; i < 2; i++) {
		m_sampleRate->addItem(QString::number(m_rateMap[i]));
		if (m_rateMap[i] == settings->value(AUDIO_SAMPLERATE).toInt())
			m_sampleRate->setCurrentIndex(i);
	}
	formLayout->addRow(tr("Sample rate (samples per second)"), m_sampleRate);

	centralLayout->addLayout(formLayout);

	m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	m_buttons->setCenterButtons(true);

	centralLayout->addWidget(m_buttons);

	settings->endGroup();
	delete settings;
}

