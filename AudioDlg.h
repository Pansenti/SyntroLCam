//
//  Copyright (c) 2012, 2013 Pansenti, LLC.
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


#ifndef AUDIODLG_H
#define AUDIODLG_H

#include <QDialog>
#include <qvalidator.h>
#include <qsettings.h>
#include <qlineedit.h>
#include <qdialogbuttonbox.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include "syntrogui_global.h"

#include "SyntroLib.h"
#include "SyntroValidators.h"

class AudioDlg : public QDialog
{
	Q_OBJECT

public:
	AudioDlg(QWidget *parent);

public slots:
	void onOk();

private:
	void layoutWindow();

	QCheckBox *m_enable;
    QLineEdit *m_inputDevice;
    QLineEdit *m_inputCard;
    QComboBox *m_channels;
	QComboBox *m_sampleRate;
	QDialogButtonBox *m_buttons;

	int m_channelMap[2];
    int m_rateMap[2];
};

#endif // AUDIODLG_H
