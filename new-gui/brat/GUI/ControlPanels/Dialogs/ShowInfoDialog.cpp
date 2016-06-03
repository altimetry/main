#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/GUI/TextWidget.h"

#include "ShowInfoDialog.h"




CShowInfoDialog::CShowInfoDialog( QWidget *parent, const CFormula *formula, COperation *operation )
    : base_t( parent )
{
    CreateWidgets( formula->GetDescription(), operation );

    setWindowTitle( ( "Show '" + formula->GetName() + "' information...").c_str() );
}


CShowInfoDialog::~CShowInfoDialog()
{
    delete mInfoTable;
    delete mTableView;
}


void  CShowInfoDialog::CreateWidgets( const std::string& exprValue, COperation *operation)
{
    //	Create

    std::string errorMsg;
    CExpression expr;
    CFormula::SetExpression(exprValue, expr, errorMsg);

    std::string fieldName = "";
    std::string orignalUnit = "";
    std::string baseUnit = "";

    const CStringArray* fieldNames = expr.GetFieldNames();


    mInfoTable = new QStandardItemModel(1,2,this);
    initializeModel(mInfoTable);

    mInfoTable->setRowCount((int)fieldNames->size());
    for (uint32_t i = 0 ; i < fieldNames->size() ; i++)
    {
     fieldName = fieldNames->at(i);

  //    GetExprinfoGrid()->SetRowLabelValue(i, fieldName.c_str());

      CProduct* product = operation->GetProduct();

      if (product != nullptr)
      {
        CField* field = product->FindFieldByName(fieldName, (const char *)operation->GetRecord().c_str(), false, nullptr, true);

        if (field != nullptr)
        {
          orignalUnit = (field->GetUnit().empty() ? "count" : field->GetUnit().c_str());
         // GetExprinfoGrid()->SetCellValue(i, 0, orignalUnit);


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

      QString inline_fieldName = t2q(fieldName);
      QString inline_originalUnit = t2q(orignalUnit);
      QString inline_baseUnit = t2q(baseUnit);
      addEntry(i, inline_fieldName, inline_originalUnit, inline_baseUnit);

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
