#include "stdafx.h"
#include "ArxHelper.h"
#include "Tools.h"
namespace CadCommon
{
	std::wstring ArxHelper::GetDWGFilePath()
	{
		AcDbDatabase* m_db;//数据库文件
		m_db = acdbHostApplicationServices()->workingDatabase();
		if (m_db == NULL)
			return std::wstring();
		const ACHAR* filename;
		m_db->getFilename(filename);
                                //ceshi
		return std::wstring(filename);
	}

	std::wstring ArxHelper::GetArxPluginPath() 
	{
		wchar_t exeFullPath[MAX_PATH];
#if RDD >= 2018	
		GetModuleFileNameW(CAcModuleResourceOverride::ResourceInstance(),exeFullPath, MAX_PATH);
#else
		//Windows内存与结构管理结构体
		GetModuleFileName((HINSTANCE)&__ImageBase, exeFullPath, MAX_PATH);
#endif
		std::wstring arxPluginFullPath(exeFullPath);
		size_t pos = arxPluginFullPath.find_last_of(_T("\\"));
		if(pos != std::wstring::npos)
			arxPluginFullPath = arxPluginFullPath.substr(0, pos);
		return std::wstring(arxPluginFullPath);
	}

	std::wstring ArxHelper::GetCADPath() 
	{
		wchar_t exeFullPath[MAX_PATH];
		GetModuleFileNameW(GetModuleHandle(_T("acad.exe")),exeFullPath, MAX_PATH);
		std::wstring arxPluginFullPath(exeFullPath);
		size_t pos = arxPluginFullPath.find_last_of(_T("\\"));
		if(pos != std::wstring::npos)
			arxPluginFullPath = arxPluginFullPath.substr(0, pos);
		return std::wstring(arxPluginFullPath);
	}


	uint ArxHelper::GetColorIndexByColor(CadCommon::ARXColor _color)
	{
		long acirgb,r,g,b;
		long mindst = 2147483647L;
		long dst = 0;
		uint minndx = 0;
		for (int i = 1; i < 255; i++) {
				acirgb = acdbGetRGB(i);
				BYTE R, G, B;
				WORD LOW = LOWORD(acirgb);
				WORD HIG = HIWORD(acirgb);
				R = LOBYTE(LOW);
				G = HIBYTE(LOW);
				B = LOBYTE(HIG);
				r = R; g = G; b = B;
			dst = abs(r - _color._red) + abs(g - _color._green) + abs(b - _color._blue);
			if (dst < mindst) {
					minndx = i;
				mindst = dst;
			}
		}
		return minndx;
	}

	uint ArxHelper::GetColorIndexByColor(long red , long green, long blue)
	{
		long acirgb,r,g,b;
		long mindst = 2147483647L;
		long dst = 0;
		uint minndx = 0;
		for (int i = 1; i < 255; i++) {
			acirgb = acdbGetRGB(i);
			BYTE R, G, B;
			WORD LOW = LOWORD(acirgb);
			WORD HIG = HIWORD(acirgb);
			R = LOBYTE(LOW);
			G = HIBYTE(LOW);
			B = LOBYTE(HIG);
			r = R; g = G; b = B;
			dst = abs(r - red) + abs(g - green) + abs(b - blue);
			if (dst < mindst) {
				minndx = i;
				mindst = dst;
			}
		}
		return minndx;
	}


	bool ArxHelper::GetRGBColorByIndex(uint _colorIndex, CadCommon::ARXColor & color)
	{
		if (_colorIndex < 0 || _colorIndex > 255)
		{
			return false;
		}

		BYTE R, G, B;
		long zhi = acdbGetRGB(_colorIndex);
		WORD LOW = LOWORD(zhi);
		WORD HIG = HIWORD(zhi);
		R = LOBYTE(LOW);
		G = HIBYTE(LOW);
		B = LOBYTE(HIG);
		color._red = R;
		color._green = G;
		color._blue = B;
		return true;
	}


	bool ArxHelper::Zoom(AcGePoint2d & p1, AcGePoint2d & p2)
	{
		AcDbViewTableRecord view;
		AcGePoint2d max_2d(p2[X], p2[Y]);
		AcGePoint2d min_2d(p1[X], p1[Y]);
		// now set the view centre point
		view.setCenterPoint(GetMidPoint(min_2d,max_2d));
		// now height and width of view
// 		view.setHeight(max_2d[Y] - min_2d[Y]);
// 		view.setWidth(max_2d[X] - min_2d[X]);
		// set the view
		//当前视口的视图高度（图形单位）
		double h1 = fabs(max_2d[Y] - min_2d[Y] )+ 50;
 		struct resbuf rb;
 		acedGetVar(L"VIEWSIZE", &rb);
 		double h2= rb.resval.rreal;
		acedGetVar(TEXT("SCREENSIZE"), &rb);
		double w2 = rb.resval.rpoint[X] / rb.resval.rpoint[Y] * h2;

		double w1 = fabs(max_2d[X] - min_2d[X] )+ 50;/*max_2d[X] / min_2d[X] * h2 ;*/
	
		if(h1 - h2 < -0.0000001)
			h1 = h2;
		if(w1 - w2 < -0.0000001){
			w1 = w2;
		}
		view.setHeight(h1);
		view.setWidth(w1);
// 		view.setHeight();
// 		view.setWidth();

		acedSetCurrentView(&view, NULL);
		// updates the extents
		//acdbHostApplicationServices()->workingDatabase()->updateExt(TRUE);
		return 0;
	}

	bool ArxHelper::Zoom(CadCommon::Cable_Box & pBox)
	{
		AcDbViewTableRecord view;
		AcGePoint2d max_2d(pBox.max.x, pBox.max.y);
		AcGePoint2d min_2d(pBox.min.x, pBox.min.y);
		// now set the view centre point
		view.setCenterPoint(min_2d + (max_2d - min_2d) / 2.0);
		// now height and width of view
		//view.setHeight(max_2d[Y] - min_2d[Y]);
		//view.setWidth(max_2d[X] - min_2d[X]);
		// set the view
		struct resbuf rb;
		acedGetVar(L"VIEWSIZE", &rb);
		view.setHeight(rb.resval.rreal);
		double height = rb.resval.rreal;
		//以像素为单位的当前视口的大小（X和Y值）
		acedGetVar(TEXT("SCREENSIZE"), &rb);
		view.setWidth(rb.resval.rpoint[X] / rb.resval.rpoint[Y] * height);
		acedSetCurrentView(&view, NULL);
		// updates the extents
		//acdbHostApplicationServices()->workingDatabase()->updateExt(TRUE);
		return 0;
	}

	int ArxHelper::CenterObject(AcDbObjectId _objID)
	{
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk == acdbOpenObject(pEnt, _objID, AcDb::kForRead))
		{
			CenterObject(pEnt);
			pEnt->close();
		}
		return 0;
	}

	int ArxHelper::CenterObject(std::vector<AcDbObjectId> _objOldIDs)
	{
		if (_objOldIDs.size() == 0)
			return 0;
		AcGePoint2d _minPonit(0, 0);
		AcGePoint2d _maxPonit(0, 0);
		for (int index = 0; index < _objOldIDs.size(); ++index)
		{
			AcDbObjectId objId = _objOldIDs[index];
			AcDbEntity *pEnt = NULL;
			if (Acad::eOk == acdbOpenObject(pEnt, objId, AcDb::kForRead))
			{
				AcDbExtents _extents;
				if (Acad::eOk == pEnt->getGeomExtents(_extents))
				{
					if (index == 0) {
						_minPonit.set(_extents.minPoint().x, _extents.minPoint().y);
						_maxPonit.set(_extents.maxPoint().x, _extents.maxPoint().y);
					}
					else {
						if (_minPonit.x > _extents.minPoint().x) {
							_minPonit.x = _extents.minPoint().x;
						}
						if (_minPonit.y > _extents.minPoint().y) {
							_minPonit.y = _extents.minPoint().y;
						}
						if (_maxPonit.x < _extents.maxPoint().x) {
							_maxPonit.x = _extents.maxPoint().x;
						}
						if (_maxPonit.y < _extents.maxPoint().y) {
							_maxPonit.y = _extents.maxPoint().y;
						}
					}
				}
				pEnt->close();
			}
		}
		Zoom(_minPonit, _maxPonit);
		return 0;
	}

	int ArxHelper::CenterObject(std::set<AcDbObjectId> objectList)
	{
		AcGePoint2d _minPonit(0, 0);
		AcGePoint2d _maxPonit(0, 0);
		int index = 0;
		for each (AcDbObjectId objId in objectList)
		{
			AcDbEntity *pEnt = NULL;
			Acad::ErrorStatus Es = acdbOpenObject(pEnt, objId, AcDb::kForRead);
			if (Acad::eOk == Es)
			{
				AcDbExtents _extents;
				if (Acad::eOk == pEnt->getGeomExtents(_extents))
				{
					if (index == 0) {
						_minPonit.set(_extents.minPoint().x, _extents.minPoint().y);
						_maxPonit.set(_extents.maxPoint().x, _extents.maxPoint().y);
					}
					else {
						if (_minPonit.x > _extents.minPoint().x) {
							_minPonit.x = _extents.minPoint().x;
						}
						if (_minPonit.y > _extents.minPoint().y) {
							_minPonit.y = _extents.minPoint().y;
						}
						if (_maxPonit.x < _extents.maxPoint().x) {
							_maxPonit.x = _extents.maxPoint().x;
						}
						if (_maxPonit.y < _extents.maxPoint().y) {
							_maxPonit.y = _extents.maxPoint().y;
						}
					}
				}
				pEnt->close();
				++index;
			}
		}
		Zoom(_minPonit, _maxPonit);
		return 0;
	}

	int ArxHelper::CenterObject(AcDbEntity * _obj)
	{
		AcDbExtents _extents;
		if (Acad::eOk == _obj->getGeomExtents(_extents))
		{
			AcGePoint2d _minPonit(_extents.minPoint().x, _extents.minPoint().y);
			AcGePoint2d _maxPonit(_extents.maxPoint().x, _extents.maxPoint().y);
			Zoom(_minPonit, _maxPonit);
		}
		return 0;
	}

	AcGePoint2d ArxHelper::GetMidPoint(AcGePoint2d & p1, AcGePoint2d & p2)
	{
		return AcGePoint2d((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
	}
	AcGePoint2d ArxHelper::GetMidPoint(CadCommon::Cable_Box & pBox)
	{
		return AcGePoint2d((pBox.min.x + pBox.max.x) / 2.0, (pBox.min.y + pBox.max.y) / 2.0);
	}

	//获得元素包围盒
	bool ArxHelper::GetEntityBox(AcDbEntity* entity, CadCommon::Cable_Box& box)
	{
		bool bRet = false;
		AcDbExtents _extents;
		if (Acad::eOk == entity->getGeomExtents(_extents))
		{
			box.min.Set(Cable_Point(_extents.minPoint().x, _extents.minPoint().y, 0.0));
			box.max.Set(Cable_Point(_extents.maxPoint().x, _extents.maxPoint().y, 0.0));
			bRet = true;
		}
		return bRet;
	}
	//获得元素中心点
	 bool ArxHelper::GetMidPoint(AcDbEntity* entity, CadCommon::Cable_Point& point)
	 {
		 bool bRet = false;
		 CadCommon::Cable_Box box;
		 if (GetEntityBox(entity, box)) {
			 AcGePoint2d point2D = GetMidPoint(box);
			 point.x = point2D.x;
			 point.y = point2D.y;
			 point.z = 0.0;
			 bRet = true;
		 }
		 return bRet;
	 }

	 AcDbObjectId ArxHelper::PostToModelSpace(AcDbEntity* pEnt)
	 {
		 ASSERT(pEnt != NULL);
		 AcDbBlockTable *pBlockTable = NULL;
		 Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()
			 ->getBlockTable(pBlockTable, AcDb::kForRead);
		ASSERT(pBlockTable != NULL);
		 //打开块表记录
		 AcDbBlockTableRecord *pRc = NULL;
		 es = pBlockTable->getAt(ACDB_MODEL_SPACE, pRc, AcDb::kForWrite);
		 pBlockTable->close();//块表记录打开后及时关闭块表

		 ASSERT(pRc != NULL);
		 //将实体加入模块空间
		 AcDbObjectId entId;
		 es = pRc->appendAcDbEntity(entId, pEnt);
		 //实体加入成功后关闭块表记录和实体
		 pRc->close();
		 pEnt->close();
		 return entId;
	 }

	 AcDbObjectId ArxHelper::PostToModelSpace(AcDbDatabase* pDb,AcDbEntity* pEnt)
	 {
		 ASSERT(pEnt != NULL);
		 AcDbBlockTable *pBlockTable = NULL;
		 Acad::ErrorStatus es = pDb->getBlockTable(pBlockTable, AcDb::kForRead);
		 ASSERT(pBlockTable != NULL);
		 //打开块表记录
		 AcDbBlockTableRecord *pRc = NULL;
		 es = pBlockTable->getAt(ACDB_MODEL_SPACE, pRc, AcDb::kForWrite);
		 pBlockTable->close();//块表记录打开后及时关闭块表

		 ASSERT(pRc != NULL);
		 //将实体加入模块空间
		 AcDbObjectId entId;
		 es = pRc->appendAcDbEntity(entId, pEnt);
		 //实体加入成功后关闭块表记录和实体
		 pRc->close();
		 pEnt->close();
		 return entId;
	 }

	 void ArxHelper::SetEntityColor(AcDbObjectId objId,uint colorIndex)
	 {
		AcDbEntity* pEnt = NULL;
		Acad::ErrorStatus Es = acdbOpenObject(pEnt, objId, AcDb::kForWrite);
		if (Acad::eOk == Es)
		{
			pEnt->setColorIndex(colorIndex);
			pEnt->close();
		}
	 }
	 void ArxHelper::AcgePoint3dToads_point(const AcGePoint3d& pt, ads_point& adsPoint)
	 {
		 adsPoint[X] = pt.x;
		 adsPoint[Y] = pt.y;
		 adsPoint[Z] = pt.z;
	 }

	 double ArxHelper::Max(double a, double b)
	 {
		 if (a > b)
		 {
			 return a;
		 }
		 else
		 {
			 return b;
		 }
	 }
	 double ArxHelper::Min(double a, double b)
	 {
		 if (a < b)
		 {
			 return a;
		 }
		 else
		 {
			 return b;
		 }
	 }
	 //旋转
	
	 //移动
	 Acad::ErrorStatus ArxHelper::Move(AcDbObjectId entId, AcGePoint3d ptBase,
		 AcGePoint3d ptDest)
	 {
		 // 设置变换矩阵的参数
		 AcGeMatrix3d xform;
		 AcGeVector3d vec(ptDest.x - ptBase.x, ptDest.y - ptBase.y,ptDest.z - ptBase.z);
		 xform.setToTranslation(vec);
		 AcDbEntity *pEnt;
		 Acad::ErrorStatus es = acdbOpenObject(pEnt, entId, AcDb::kForWrite,false);
		 pEnt->transformBy(xform);
		 pEnt->close();
		 return es;
	 }
	 //缩放
	 Acad::ErrorStatus ArxHelper::Scale(AcDbObjectId entId,
		 AcGePoint3d ptBase, double scaleFactor)
	 {
		 // 设置变换矩阵的参数
		 AcGeMatrix3d xform;
		 xform.setToScaling(scaleFactor, ptBase);
		 AcDbEntity *pEnt;
		 Acad::ErrorStatus es = acdbOpenObject(pEnt, entId, AcDb::kForWrite,false);
		 pEnt->transformBy(xform);
		 pEnt->close();
		 return es;
	 }
	 AcDbObjectId ArxHelper::CreateRectangle(AcGePoint2d pt1, AcGePoint2d pt2,
		 double width)
	 {
		 // 提取两个角点的坐标值
		 double x1 = pt1.x, x2 = pt2.x;
		 double y1 = pt1.y, y2 = pt2.y;

		 // 计算矩形的角点
		 AcGePoint2d ptLeftBottom(ArxHelper::Min(x1, x2),
			 ArxHelper::Min(y1, y2));
		 AcGePoint2d ptRightBottom(ArxHelper::Max(x1, x2),
			 ArxHelper::Min(y1, y2));
		 AcGePoint2d ptRightTop(ArxHelper::Max(x1, x2),
			 ArxHelper::Max(y1, y2));
		 AcGePoint2d ptLeftTop(ArxHelper::Min(x1, x2),
			 ArxHelper::Max(y1, y2));

		 // 创建对应的多段线
		 AcDbPolyline *pPoly = new AcDbPolyline(4);
		 pPoly->addVertexAt(0, ptLeftBottom, 0, width, width);
		 pPoly->addVertexAt(1, ptRightBottom, 0, width, width);
		 pPoly->addVertexAt(2, ptRightTop, 0, width, width);
		 pPoly->addVertexAt(3, ptLeftTop, 0, width, width);
		 pPoly->setClosed(Adesk::kTrue);

		 // 将多段线添加到模型空间
		 AcDbObjectId polyId;
		 polyId = ArxHelper::PostToModelSpace(pPoly);
		 return polyId;
	 }
	 AcDbObjectId ArxHelper:: WhCHAP2CreateLine(AcGePoint3d pt1, AcGePoint3d pt2)
	 {
		 // TODO: Implement the command
		 //在内存上创建一个新的AcDbLine对象
		 AcDbLine *pLine=new AcDbLine(pt1,pt2);
		 //pLine->setColorIndex(1); //设置直线为红色
		 AcDbObjectId polyId;
		 polyId = ArxHelper::PostToModelSpace(pLine);
		 return polyId;
	 }

	 bool ArxHelper::CopyDBEntity(std::wstring filePath,AcDbDatabase*& pDB,AcGePoint3d mPoint)
	 {
		 bool bRet = false;
		 AcDbDatabase* pExternalDb = new AcDbDatabase(Adesk::kFalse);
		 std::wstring wstrExtern = BIM::Tools::getFileExtention(filePath);
		 BIM::Tools::lowerWstring(wstrExtern);
		 Acad::ErrorStatus es = Acad::eNullHandle ;
		if(wstrExtern == L".dxf")
		{
			es = pExternalDb->dxfIn(filePath.c_str());
			
		}
		else if(wstrExtern == L".dwg")
		{
			es = pExternalDb->readDwgFile(filePath.c_str());
		}

		if(es == Acad::eOk)
		{
			 AcDbDatabase *pTempDb;
			 if (Acad::eOk==pExternalDb->wblock(pTempDb))
			 {
				 AcGeMatrix3d XForm;
				 AcGeVector3d fromPt(mPoint.x,mPoint.y,mPoint.z);
				 XForm.setToTranslation(fromPt);
				 pDB->insert(XForm,pTempDb);
				 SAFE_DELETE(pTempDb);
				 bRet = true;
			 }
		 }
		 pExternalDb->closeInput();
		 SAFE_DELETE(pExternalDb);
		 return bRet;
	 }


	 AcDbObjectId ArxHelper::CreateLine(AcGePoint3d point1,AcGePoint3d point2,AcDbObjectId _layerId,int colorIndex)
	 {
		 AcDbLine* pLine = new AcDbLine(point1,point2);
		 pLine->setLayer(_layerId);
		 pLine->setColorIndex(colorIndex);
		 return CadCommon::ArxHelper::PostToModelSpace(pLine);
	 }

	 AcDbObjectId ArxHelper::CreateLine(AcGePoint3d point1,AcGePoint3d point2,AcDbObjectId _layerId,int colorIndex,AcDbObjectId _lineTypeId,double _linetypeScale)
	 {
		 AcDbLine* pLine = new AcDbLine(point1,point2);
		 pLine->setLayer(_layerId);
		 pLine->setColorIndex(colorIndex);
		 pLine->setLinetypeScale(_linetypeScale);
		 if(_lineTypeId != AcDbObjectId::kNull)
		 {
			 pLine->setLinetype(_lineTypeId);
		 }
		 return CadCommon::ArxHelper::PostToModelSpace(pLine);

	 }


	 AcDbObjectId ArxHelper::CreateText(AcGePoint3d point,std::wstring txtStr,int colorIndex,double _height, double widFactor,AcDbObjectId _layId ,AcDbObjectId txtStyleId)
	 {
		 AcDbText* pText1 = new AcDbText(point,txtStr.c_str());
		 if(!_layId.isNull())
			 pText1->setLayer(_layId);
		 if(!txtStyleId.isNull())
			 pText1->setTextStyle(txtStyleId);
		 pText1->setHeight(_height);
		 pText1->setWidthFactor(widFactor);
		 if(colorIndex != -1)
		 {
			 pText1->setColorIndex(colorIndex);
		 }
		return CadCommon::ArxHelper::PostToModelSpace(pText1);
	 }

	 AcDbObjectId ArxHelper::CreateCircle(double radius,const AcGePoint3d& _centerPoint,AcDbObjectId _layId,int colorIndex)
	 {
		 AcDbCircle* pCircle = new AcDbCircle();
		 pCircle->setRadius(radius);
		 pCircle->setCenter(_centerPoint);
		 pCircle->setLayer(_layId);
		 pCircle->setColorIndex(colorIndex);
		return CadCommon::ArxHelper::PostToModelSpace(pCircle);
	 }

	 AcDbObjectId ArxHelper::CreateCircle(double radius,const AcGePoint3d& _centerPoint,AcDbObjectId _layId,int colorIndex,AcDbObjectId _lineTypeId,double _linetypeScale)
	 {
		 AcDbCircle* pCircle = new AcDbCircle();
		 pCircle->setRadius(radius);
		 pCircle->setCenter(_centerPoint);
		 pCircle->setLayer(_layId);
		 pCircle->setColorIndex(colorIndex);
		 pCircle->setLinetypeScale(_linetypeScale);
		 if(_lineTypeId != AcDbObjectId::kNull)
		 {
			 pCircle->setLinetype(_lineTypeId);
		 }
		 return CadCommon::ArxHelper::PostToModelSpace(pCircle);

	 }

	 AcDbObjectId ArxHelper::CreateDiametricDimension(double txtradius,double radius,const AcGePoint3d& _centerPoint,AcGeVector3d vDir,
		 AcDbObjectId _dimStyleId)
	 {
		 vDir.normalize();
		 AcGePoint3d txtpos(_centerPoint + (vDir * txtradius));
		 AcDbDiametricDimension* dim = new AcDbDiametricDimension();
		 dim->setChordPoint(_centerPoint + (vDir * radius));
		 dim->setFarChordPoint(_centerPoint + (-vDir * radius));
		 dim->setTextPosition(txtpos);
		 dim->useSetTextPosition();    // make text go where user picked
		 dim->setDatabaseDefaults();
		 if(!_dimStyleId.isNull())
			 dim->setDimensionStyle(_dimStyleId);
		return CadCommon::ArxHelper::PostToModelSpace(dim);
	 }

}