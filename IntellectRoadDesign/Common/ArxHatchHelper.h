#pragma once

namespace CadCommon
{
	class ArxHatchHelper
	{
	public:
		// 填充add方法：传入数据库对象ID列表、填充名字
		static AcDbObjectId Add(const AcDbObjectIdArray &objIds, 
			const ACHAR* patName ,double _pspace, AcDbObjectId layId  = AcDbObjectId::kNull, AcDbObjectId lineTypeId  = AcDbObjectId::kNull );
	};
}