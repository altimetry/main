/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/GUI/TextWidget.h"

#include "ShowInfoDialog.h"




void  CShowInfoDialog::CreateWidgets( const std::string &expr_value, const COperation *operation )
{
	//	Create

	std::string error_msg;
	CExpression expr;
	CFormula::SetExpression(expr_value, expr, error_msg);

	std::string field_name = "";
	std::string orignal_unit = "";
	std::string baseUnit = "";

	const CStringArray* fieldNames = expr.GetFieldNames();


	mInfoTable = new QStandardItemModel(1,2,this);
	initializeModel(mInfoTable);

	mInfoTable->setRowCount((int)fieldNames->size());
	CProductInfo pi( operation->OriginalDataset() );
	for (uint32_t i = 0 ; i < fieldNames->size() ; i++)
	{
		field_name = fieldNames->at(i);

		//    GetExprinfoGrid()->SetRowLabelValue(i, field_name.c_str());

		if (pi.IsValid() )
		{
			std::string field_error_msg;
			CField* field = pi.FindFieldByName( field_name, operation->GetRecord(), field_error_msg );
			if ( !field_error_msg.empty() )
				error_msg += ( "\n" + field_error_msg );

			if (field != nullptr)
			{
				orignal_unit = field->GetUnit().empty() ? "count" : field->GetUnit();
				// GetExprinfoGrid()->SetCellValue(i, 0, orignal_unit);

				try
				{
					CUnit unit( field->GetUnit() );
					if (unit.HasDateRef())
					{
						baseUnit = CUnit::m_DATE_REF_UNIT.c_str();
					}
					else
					{
						baseUnit = unit.BaseUnit().AsString().c_str();
					}
				}
				catch (CException& e)
				{
					e.what();
					baseUnit = "count";
				}

				//GetExprinfoGrid()->SetCellValue(i, 1, baseUnit);
			}

		}

		QString inline_fieldName = t2q(field_name);
		QString inline_originalUnit = t2q(orignal_unit);
		QString inline_baseUnit = t2q(baseUnit);
		addEntry(i, inline_fieldName, inline_originalUnit, inline_baseUnit);

	}

	if ( !error_msg.empty() )
	{
		LOG_WARN( error_msg );
	}



	//	... Help

	auto help = new CTextWidget;
	help->SetHelpProperties(
		"Provides information about the original units (the ones defined in the\n"
		"data products) and the units used during computation or selection.",
		0 , 6 );
	auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
	help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );

	//	... Buttons

	mButtonBox = new QDialogButtonBox( this );
	mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
	mButtonBox->setOrientation( Qt::Horizontal );
	mButtonBox->setStandardButtons( QDialogButtonBox::Ok );
	mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );
	mButtonBox->button( QDialogButtonBox::Ok )->setText( "Close" );

	mTableView = CreateTableView(mInfoTable, QObject::tr("Table Model (View 1)"));
	QBoxLayout *main_l =
		LayoutWidgets( Qt::Vertical,
		{
			mTableView,
			help_group,
			mButtonBox

		}, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


	//	Wrap up dimensions

	adjustSize();
	setMinimumWidth( width() );

	Wire();
}


void  CShowInfoDialog::Wire()
{
	//	Setup things

	connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
	connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


CShowInfoDialog::CShowInfoDialog( QWidget *parent, const CFormula *formula, const COperation *operation )
    : base_t( parent )
{
    CreateWidgets( formula->GetDescription(), operation );

    setWindowTitle( ( "Show '" + formula->GetName() + "' Information").c_str() );
}


CShowInfoDialog::~CShowInfoDialog()
{
    delete mInfoTable;
    delete mTableView;
}


void CShowInfoDialog::initializeModel(QStandardItemModel *model)
{
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Original unit")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Calculation unit(SI)")));
}

void CShowInfoDialog::addEntry(unsigned int i, QString& exp_name, QString& oUnit, QString& cUnit)
{
    mInfoTable->setVerticalHeaderItem(i, new QStandardItem(exp_name));
    QStandardItem *field1 = new QStandardItem(oUnit);
    QStandardItem *field2 = new QStandardItem(cUnit);
    mInfoTable->setItem(i,0,field1);
    mInfoTable->setItem(i,1,field2);
}

QTableView * CShowInfoDialog::CreateTableView(QStandardItemModel *model, const QString &title = "")
{
    Q_UNUSED(title);
    QTableView *view = new QTableView;
    view->setModel(model);
//    view->setWindowTitle(title);
    return view;
}

//virtual
void  CShowInfoDialog::accept()
{

    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ShowInfoDialog.cpp"
