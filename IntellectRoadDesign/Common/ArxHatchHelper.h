#pragma once

namespace CadCommon
{
	class ArxHatchHelper
	{
	public:
		// ���add�������������ݿ����ID�б��������
		static AcDbObjectId Add(const AcDbObjectIdArray &objIds, 
			const ACHAR* patName ,double _pspace, AcDbObjectId layId  = AcDbObjectId::kNull, AcDbObjectId lineTypeId  = AcDbObjectId::kNull );
	};
}