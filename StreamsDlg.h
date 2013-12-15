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


#ifndef STREAMSDLG_H
#define STREAMSDLG_H

#include <QDialog>
#include <qvalidator.h>
#include <qsettings.h>
#include <qlineedit.h>
#include <qdialogbuttonbox.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include "syntrogui_global.h"

#include "SyntroLib.h"
#include "SyntroValidators.h"

class StreamsDlg : public QDialog
{
	Q_OBJECT

public:
	StreamsDlg(QWidget *parent);
	~StreamsDlg();

public slots:
	void onOk();
	void onCancel();
	void lowRateStateChange(int state);

signals:
	void newStream();

private:
	void layoutWindow();
	void enableLowRate(bool enable);

	QLineEdit *m_highRateMinInterval;
	QLineEdit *m_highRateMaxInterval;
	QLineEdit *m_highRateNullInterval;
	QLineEdit *m_lowRateMinInterval;
	QLineEdit *m_lowRateMaxInterval;
	QLineEdit *m_lowRateNullInterval;
	QCheckBox *m_generateLowRate;
	QCheckBox *m_lowRateHalfRes;
	QDialogButtonBox *m_buttons;

};

#endif // STREAMSDLG_H
