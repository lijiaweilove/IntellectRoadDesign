#include "StdAfx.h"
#include "ArxHatchHelper.h"
#include "ArxHelper.h"
namespace CadCommon
{
	AcDbObjectId ArxHatchHelper::Add(const AcDbObjectIdArray &objIds, 
							const ACHAR* patName,double _pspace, AcDbObjectId layId, AcDbObjectId lineTypeId )
	{
		// �쳣����
		Acad::ErrorStatus es;
		// �յ����ݿ�������
		AcDbHatch *pHatch = new AcDbHatch();
		pHatch->setPattern(AcDbHatch::kPreDefined, patName);
		pHatch->setDatabaseDefaults();
		pHatch->setAssociative(Adesk::kFalse);  
		if(!layId.isNull())
			pHatch->setLayer(layId);	

		if(!lineTypeId.isNull())
			pHatch->setLinetype(lineTypeId);

		// ������ķ�������������ı�� 
		AcGeVector3d normal(0, 0, 1);
		pHatch->setNormal(normal);
		// �Ƿ�����߽磨�����߽��ߣ����ͼ�����ű仯��,��Ҫ��Ӧ��������if���
		pHatch->setAssociative(false);
		// ���ͼ�����˴�ѡ��ϵͳԤ���壨kPreDefined��ͼ����
		// ���߽磺����������ӱ߽�
		es = pHatch->appendLoop(AcDbHatch::kExternal, objIds);
		// ��ʾ���
		es = pHatch->evaluateHatch(false);
		pHatch->setHatchStyle(AcDbHatch::kOuter);
		pHatch->setPatternSpace(_pspace);
		pHatch->setPatternScale(_pspace);

		// ���϶����ڴ��в�������������ǽ�����д�����ݿ��ļ�dwg
		AcDbObjectId hatchId = ArxHelper::PostToModelSpace(pHatch);
// 
// 		// ����������߽��
// 		// ����������pHatch������ ����ʽ �򿪵Ķ���ID  hatchId ���صĶ���ָ��
 		if (acdbOpenObject(pHatch, hatchId, AcDb::kForRead) == Acad::eOk)
 		{	// ���󶨱߽����ID�б�
 			AcDbObjectIdArray assocEntids;
 			// ��pHatch������ ���󶨱߽����ID�б�
 			pHatch->getAssocObjIds(assocEntids);
 			// ��ȡÿ��ID
 			for (int i = 0; i < assocEntids.length(); i++)
 			{	// ��ȡÿ�����󶨱߽����ID���Ӧ��ָ��
 				AcDbEntity *pEnt = NULL;
 				// ��ȡÿ�� ���󶨱߽����ID ���Ӧ�� ָ�� ��
 				// ��ӵ����ķ�Ӧ���У�����ƪ�½���
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