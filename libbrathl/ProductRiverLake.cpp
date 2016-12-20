/*
 * 
 *
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

#include <cstdlib>
#include <cstdio>
#include <cstring> 
#include <typeinfo> 
#include <string> 


#include "common/tools/brathl_error.h"
#include "brathl_c/brathlc.h"
#include "brathl.h" 

#include "common/tools/TraceLog.h"
#include "Tools.h" 
#include "common/tools/Exception.h"
#include "ProductRiverLake.h" 



namespace brathl
{
    const std::string CProductRiverLake::m_TIME_NAME = "time";
    const std::string CProductRiverLake::m_TIME_DESC = "Time in seconds since 1950-01-01T00:00:00";
    const std::string CProductRiverLake::m_TIME_UNIT = "seconds since 1950-01-01T00:00:00";

    const std::string CProductRiverLake::m_YEAR_NAME = "year";
    const std::string CProductRiverLake::m_MONTH_NAME = "month";
    const std::string CProductRiverLake::m_DAY_NAME = "day";
    const std::string CProductRiverLake::m_HOUR_NAME = "hour";
    const std::string CProductRiverLake::m_MINUTE_NAME = "minute";

    const std::string CProductRiverLake::m_PROD_TYPE_RLH = "RLH";
    const std::string CProductRiverLake::m_PROD_TYPE_RLA = "RLA";


	void CProductRiverLake::Init()
	{
		mLabel = "River & Lake product";

		InitDateRef();
		InitCriteriaInfo();
	}

	CProductRiverLake::CProductRiverLake()
	{
		Init();
	}


	//----------------------------------------

	CProductRiverLake::CProductRiverLake( const std::string& fileName )
		: CProduct( fileName )
	{
		Init();
	}


	//----------------------------------------
	CProductRiverLake::CProductRiverLake( const CStringList& fileNameList, bool check_only_first_file )
		: CProduct( fileNameList, check_only_first_file )

	{
		Init();
	}

	//----------------------------------------

	CProductRiverLake::~CProductRiverLake()
	{
	}

	//----------------------------------------
	//----------------------------------------
	void CProductRiverLake::InitDateRef()
	{
        m_refDate = REF19500101;
	}

	//----------------------------------------
	void CProductRiverLake::InitCriteriaInfo()
	{
		CProduct::InitCriteriaInfo();
	}

	//----------------------------------------
	void CProductRiverLake::Dump( std::ostream& fOut /* = std::cerr */ )
	{
		if ( CTrace::IsTrace() == false )
		{
			return;
		}


		fOut << "==> Dump a CProductRiverLake Object at " << this << std::endl;

		//------------------
		CProduct::Dump( fOut );
		//------------------

		fOut << "==> END Dump a CProductRiverLake Object at " << this << std::endl;

		fOut << std::endl;

	}

    //----------------------------------------
    void CProductRiverLake::AddCombinedVariableToTree()
    {
        CFieldBasic *field = new CFieldBasic(-1,
                                             m_TIME_NAME,
                                             m_TIME_DESC,
                                             m_TIME_UNIT);

        SetIndex(field);
        SetTypeClass(field);
        SetNativeType(field);

        field->SetDateRef(m_refDate);
        field->SetFieldHasDefaultValue(m_fieldsHaveDefaultValue);

        // GetChildren at position 2, this way the field is inside dataset group "chd"
        m_tree.AddChild(m_tree.GetWalkCurrent()->GetChildren().at(2), m_TIME_NAME, field, true);
    }

    //----------------------------------------
    void CProductRiverLake::initInternalFieldNamesForCombinedVariable(CStringList& listField)
    {
        bool timePresent = false;
        CStringList::iterator itField;

        /*
         * In this product, the "time" field is a combined variable.
         * If it is in the list, the variables that will combine to create the
         * "time" value will be added to the list
         */
        for (itField = listField.begin() ; itField != listField.end() ; itField++)
        {
            /*
             * If "time" is present, then the fields that will combine are
             * added to the list, and "time" is moved back of the list
             */
            if (*itField == m_TIME_NAME)
            {
              timePresent = true;

              *itField = m_YEAR_NAME;
            }
        }

        if (timePresent)
        {
            listField.push_back(m_MONTH_NAME);

            listField.push_back(m_DAY_NAME);

            if (m_fileList.m_productType == m_PROD_TYPE_RLH)
                listField.push_back(m_HOUR_NAME);

            listField.push_back(m_MINUTE_NAME);

            listField.push_back(m_TIME_NAME);
        }
    }

    //----------------------------------------
    bool CProductRiverLake::computeCombinedField(CField *field)
    {
       if (field->GetName() == m_TIME_NAME) {

            CFieldSet *fieldSet = m_dataSet.GetFieldSet(field->GetKey());

            if (fieldSet == NULL)
            {
              fieldSet = field->CreateFieldSet(m_listFields);

              // If m_listFields contains only one item, there is no record, all fields are at the first level.
              // So recordset does not exist yet ==> create it
              if (m_dataSet.GetCurrentRecordSet() == NULL)
              {
                //InsertRecord(field->GetCurrentPos());
                InsertRecord(0);
              }

              m_dataSet.InsertFieldSet(field->GetKey(), fieldSet); //insert new CFieldSet in CRecordSet
            }

            /*
             * Gets the year, month, day, (hour), minute values of the current record
             */
            CFieldSet *fieldSetYear =
                    m_dataSet.GetCurrentRecordSet()->GetFieldSet(field->GetRecordName(), m_YEAR_NAME);
            CFieldSetDbl* fieldSetYearDbl = dynamic_cast<CFieldSetDbl*>(fieldSetYear);
            double yearValue = fieldSetYearDbl->GetData();

            CFieldSet *fieldSetMonth =
                    m_dataSet.GetCurrentRecordSet()->GetFieldSet(field->GetRecordName(), m_MONTH_NAME);
            CFieldSetDbl* fieldSetMonthDbl = dynamic_cast<CFieldSetDbl*>(fieldSetMonth);
            double monthValue = fieldSetMonthDbl->GetData();

            CFieldSet *fieldSetDay =
                    m_dataSet.GetCurrentRecordSet()->GetFieldSet(field->GetRecordName(), m_DAY_NAME);
            CFieldSetDbl* fieldSetDayDbl = dynamic_cast<CFieldSetDbl*>(fieldSetDay);
            double dayValue = fieldSetDayDbl->GetData();

            double hourValue = 0;
            if (m_fileList.m_productType == m_PROD_TYPE_RLH)
            {
                CFieldSet *fieldSetHour =
                        m_dataSet.GetCurrentRecordSet()->GetFieldSet(field->GetRecordName(), m_HOUR_NAME);
                CFieldSetDbl* fieldSetHourDbl = dynamic_cast<CFieldSetDbl*>(fieldSetHour);
                hourValue = fieldSetHourDbl->GetData();
            }

            CFieldSet *fieldSetMinute =
                    m_dataSet.GetCurrentRecordSet()->GetFieldSet(field->GetRecordName(), m_MINUTE_NAME);
            CFieldSetDbl* fieldSetMinuteDbl = dynamic_cast<CFieldSetDbl*>(fieldSetMinute);
            double minuteValue = fieldSetMinuteDbl->GetData();

            /*
             * The value for the combined time is computed (it is different for
             * RLH and for RLA)
             */
            brathl_DateSecond dateSecond;
            brathl_DateYMDHMSM dateYMDHMSM;

            if (m_fileList.m_productType == m_PROD_TYPE_RLH)
            {
                dateYMDHMSM.year = yearValue;
                dateYMDHMSM.month = monthValue;
                dateYMDHMSM.day = dayValue;
                dateYMDHMSM.hour = hourValue;
                dateYMDHMSM.minute = minuteValue;
                dateYMDHMSM.second = 0;
                dateYMDHMSM.muSecond = 0;

                brathl_YMDHMSM2Seconds(&dateYMDHMSM, m_refDate, &dateSecond);
            }
            else
            {
                dateYMDHMSM.year = yearValue;
                dateYMDHMSM.month = monthValue;
                dateYMDHMSM.day = dayValue;
                dateYMDHMSM.hour = 0;
                dateYMDHMSM.minute = 0;
                dateYMDHMSM.second = 0;
                dateYMDHMSM.muSecond = 0;

                brathl_YMDHMSM2Seconds(&dateYMDHMSM, m_refDate, &dateSecond);

                dateSecond.nbSeconds += minuteValue * 60;
            }

            /*
             * Finally, the combined time value is set into the fieldSet
             */
            CDoubleArray vectTime;
            vectTime.push_back(dateSecond.nbSeconds);

            fieldSet->Insert(vectTime, true);

            return true;
        }

       return false;
    }

} // end namespace
