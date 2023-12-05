#include "StdAfx.h"
#include "ArxHatchHelper.h"
#include "ArxHelper.h"
namespace CadCommon
{
	AcDbObjectId ArxHatchHelper::Add(const AcDbObjectIdArray &objIds, 
							const ACHAR* patName,double _pspace, AcDbObjectId layId, AcDbObjectId lineTypeId )
	{
		// 异常变量
		Acad::ErrorStatus es;
		// 空的数据库填充对象
		AcDbHatch *pHatch = new AcDbHatch();
		pHatch->setPattern(AcDbHatch::kPreDefined, patName);
		pHatch->setDatabaseDefaults();
		pHatch->setAssociative(Adesk::kFalse);  
		if(!layId.isNull())
			pHatch->setLayer(layId);	

		if(!lineTypeId.isNull())
			pHatch->setLinetype(lineTypeId);

		// 填充对象的法向量、填充对象的标高 
		AcGeVector3d normal(0, 0, 1);
		pHatch->setNormal(normal);
		// 是否关联边界（拉动边界线，填充图案跟着变化）,需要反应器，下面if语句
		pHatch->setAssociative(false);
		// 填充图案：此处选择系统预定义（kPreDefined）图案，
		// 填充边界：给填充对象添加边界
		es = pHatch->appendLoop(AcDbHatch::kExternal, objIds);
		// 显示填充
		es = pHatch->evaluateHatch(false);
		pHatch->setHatchStyle(AcDbHatch::kOuter);
		pHatch->setPatternSpace(_pspace);
		pHatch->setPatternScale(_pspace);

		// 以上都在内存中操作，下面这句是将对象写入数据库文件dwg
		AcDbObjectId hatchId = ArxHelper::PostToModelSpace(pHatch);
// 
// 		// 将填充对象与边界绑定
// 		// 方法参数：pHatch接收由 读方式 打开的对象ID  hatchId 返回的对象指针
 		if (acdbOpenObject(pHatch, hatchId, AcDb::kForRead) == Acad::eOk)
 		{	// 待绑定边界对象ID列表
 			AcDbObjectIdArray assocEntids;
 			// 给pHatch对象传入 待绑定边界对象ID列表
 			pHatch->getAssocObjIds(assocEntids);
 			// 读取每个ID
 			for (int i = 0; i < assocEntids.length(); i++)
 			{	// 获取每个待绑定边界对象ID相对应的指针
 				AcDbEntity *pEnt = NULL;
 				// 获取每个 待绑定边界对象ID 相对应的 指针 ，
 				// 添加到填充的反应器中（后面篇章讲）
 				if (acdbOpenObject(pEnt, 
 					assocEntids.at(i), 
 					AcDb::kForWrite) == Acad::eOk)
 				{	
 					pEnt->addPersistentReactor(hatchId);
 					pEnt->close();
 				}
 			}
 			pHatch->close();
 		}
		return hatchId;
	}
}