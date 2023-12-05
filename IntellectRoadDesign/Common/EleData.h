#pragma once
#include <string.h>
#include <string>
#include <iostream>
#if _MSC_VER > 1800
#include <unordered_map>
#define BIMHASH_MAP std::unordered_map
#else
#include <map>
#define BIMHASH_MAP std::map
#endif


#include "ArxMathHelper.h"
#include "Cable_Point.h"


namespace CadCommon
{
#define  DIR_MAX_PATH 256
#define SAFE_DELETE(p) if(p != NULL) {  try \
	{delete p;p = NULL;\
	}\
	catch(...){}}
#define SAFE_DELETE_ARRAY(p) if(p != NULL) {try\
{delete []p;p = NULL;\
}\
catch(...){}}


#define ISVALIDOBJECTID(p) (p.isValid() && !p.isErased())

#define MAINFESTFILEMAP BIMHASH_MAP<std::wstring, std::wstring> 

#define OuptPutEnter {				OutputDebugString(L"\r\n");}

	template<class T>
	void ClaerVector(std::vector<T>& t) { t.clear(); t.swap(std::vector<T>()); }

	template<class T>
	void SafeClaerVector(std::vector<T*>& t) {
		if (!t.empty()) {
			for (int index = 0;index < (int)t.size();++index)
			{
				T* pT = t[index];
				ASSERT(pT);
				SAFE_DELETE(pT);		
			}
			ClaerVector(t);
		}
	}

	template<class T,class B>
	void ClaerUnorderMap(BIMHASH_MAP<T, B>& t) {
		t.clear();
		t.swap(BIMHASH_MAP<T, B>());
	}

	template<class  T, class B>
	void SafeClaerUnorderedMap(BIMHASH_MAP<T, B*>& t) {
		if (!t.empty())
		{
			for (BIMHASH_MAP<T, B*>::iterator it = t.begin(); it != t.end(); ++it)
			{
				B* pt = (*it).second;
				ASSERT(pt);
				SAFE_DELETE(pt);
			}
			ClaerUnorderMap(t);
		}
	}

	template<class  T, class B>
	void SafeClaerVectorUnorderedMap(BIMHASH_MAP<T, std::vector<B*>>& t) {
		if (!t.empty())
		{
			for (BIMHASH_MAP<T, std::vector< B*>>::iterator it = t.begin(); it != t.end(); ++it)
			{
				std::vector<B*>  vec_pt = (*it).second;
				SafeClaerVector(vec_pt);
			}
			ClaerUnorderMap(t);
		}
	}


	enum DrawingType {
		schematic_diagram = 0,//原理图
		wiring_diagram = 1, //接线图
		undefine = 2
	};

	struct  ARXColor
	{
		int _red;
		int _green;
		int _blue;
		ARXColor(): _red(-1),_green(-1),_blue(-1) {

		}

		ARXColor(int red, int green, int blue) :_red(red),_green(green),_blue(blue) {

		}

		ARXColor(ARXColor& _color) :
			_red(_color._red), 
			_green(_color._green), 
			_blue(_color._blue)
		{

		}
		bool operator ==(ARXColor const & rhs) const
		{
			return Equal(&rhs);
		}

		bool operator !=(ARXColor const & rhs) const
		{
			return !Equal(&rhs);
		}


		/*!\def Cable_Point_EPSILON */
	/*! This method checks if the xyz components of the point are equal.  */
		bool Equal(ARXColor const *p) const {
			return (_red == p->_red) && (_green == p->_green) && (_blue == p->_blue);
		};

		/*!  This method checks if the xyz components of the point are equal.  */
		bool Equal(ARXColor const &p) const {
			return Equal(&p);
		};
	};

	class Cable_Box
	{
	public:
		Cable_Point min;
		Cable_Point max;
	public:
		inline bool PointInBox(Cable_Point& point) {
			if (point.x - min.x > nweps && point.y - min.y > nweps&& point.z - min.z > nweps
				&& max.x - point.x > nweps && max.y - point.y > nweps&& max.z - point.z > nweps)
				return true;
			return false;
		}

		double GetTransverseLength()
		{
			AcGePoint3d p1(min.x,max.y,0.0);
			AcGePoint3d p2(max.x,max.y,0.0);
			double _horLength1 = p1.distanceTo(p2);
			return _horLength1;
		}

		double GetVerticalLength()
		{
			AcGePoint3d p1(min.x,max.y,0.0);
			AcGePoint3d p2(min.x,min.y,0.0);
			double _vetLength1 = p1.distanceTo(p2);
			return _vetLength1;
		}
			
		bool UpdateValue(Cable_Box box)
		{
			bool bRet = false;
			if(min.x - box.min.x > Cable_Point_EPSILON)
			{
				min.x = box.min.x;
				bRet = true;
			}
			if(min.y - box.min.y > Cable_Point_EPSILON)
			{
				min.y = box.min.y;
				bRet = true;
			}
			if(box.max.x - max.x  > Cable_Point_EPSILON)
			{
				max.x = box.max.x;
				bRet = true;
			}
			if(box.max.y - max.y > Cable_Point_EPSILON)
			{
				max.y = box.max.y;
				bRet = true;
			}
			return bRet;
		}


		bool operator <(Cable_Box const & rhs) const
		{
			double xEvalue = (abs(max.x - min.x) + abs(rhs.max.x - rhs.min.x)) / 2.0;
			double yEvalue = (abs(max.y - min.y) + abs(rhs.max.y - rhs.min.y)) / 2.0;
			AcGePoint2d _mnidPoint1((min.x + max.x) / 2.0, (min.y + max.y) / 2.0);
			AcGePoint2d _midPoint((rhs.min.x + rhs.max.x) / 2.0, (rhs.min.y + rhs.max.y) / 2.0);
			if ((_midPoint.y - _mnidPoint1.y) > yEvalue)
			{
				return true;
			}
			if ((_mnidPoint1.y - _midPoint.y) > yEvalue)
			{
				return false;
			}

			if ((_midPoint.x - _mnidPoint1.x) > xEvalue)
			{
				return true;
			}
			return false;
		}


		bool operator >(Cable_Box const & rhs) const
		{
			double xEvalue = (abs(max.x - min.x) + abs(rhs.max.x - rhs.min.x)) / 2.0;
			double yEvalue = (abs(max.y - min.y) + abs(rhs.max.y - rhs.min.y)) / 2.0;
			AcGePoint2d _mnidPoint1((min.x + max.x) / 2.0, (min.y + max.y) / 2.0);
			AcGePoint2d _midPoint((rhs.min.x + rhs.max.x) / 2.0, (rhs.min.y + rhs.max.y) / 2.0);
			if ((_midPoint.y - _mnidPoint1.y) > yEvalue)
			{
				return false;
			}
			if ((_mnidPoint1.y - _midPoint.y) > yEvalue)
			{
				return true;
			}

			if ((_mnidPoint1.x - _midPoint.x) > xEvalue)
			{
				return true;
			}
			return false;

		}
	};

 
	struct text_position
	{
		double x;
		double	y;
		double	height;
		double	excursion;
	};

	struct drawing_config
	{
		std::wstring name;
		text_position locationcode;
		text_position framename;

		//默认为A3横初始化数据
		drawing_config() {
			name = L"A3横";
			locationcode.x = 25;
			locationcode.y = 6;
			locationcode.height = 6;
			locationcode.excursion = 15;

			framename.x = 85;
			framename.y = 18;
			framename.height = 0;
			framename.excursion = 180;
		}
	};
}