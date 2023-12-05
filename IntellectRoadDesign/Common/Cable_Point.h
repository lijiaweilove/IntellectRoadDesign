#pragma once
#define Cable_Point_EPSILON (1e-5f)  //!<  Cable_Point_EPSILON
namespace CadCommon
{
	class Cable_Point
	{
	public:
		double x;
		double y;
		double z;
		/*! The default constructor. */
		Cable_Point() : x(0), y(0), z(0) {};

		/*! This constructor accepts floats for the x, y and z coordinates.*/
		Cable_Point(double X, double Y, double Z = 0.0f) : x(X), y(Y), z(Z) {};

		/*! This constructor accepts a pointer to an Cable_Point object. */
		Cable_Point(Cable_Point const * p) : x(p->x), y(p->y), z(p->z) {};

		/*! This constructor accepts an Cable_Point reference. */
		Cable_Point(Cable_Point const & p) : x(p.x), y(p.y), z(p.z) {};

		Cable_Point(AcGePoint3d const & p) : x(p.x), y(p.y), z(p.z) {};

		/*!  This method packs an Cable_Point object with coordinate values.  */
		void Set(double X, double Y, double Z = 0.0f) { x = X; y = Y; z = Z; };

		/*!  This method packs an Cable_Point object with coordinate values from a given Cable_Point.  */
		void Set(Cable_Point *p) { x = p->x; y = p->y; z = p->z; };

		/*!  This method packs an Cable_Point object with coordinate values from a given Cable_Point.  */
		void Set(Cable_Point const *p) { x = p->x; y = p->y; z = p->z; };

		/*!  This method packs an Cable_Point object with coordinate values from a given Cable_Point that is passed by reference.  */
		void Set(Cable_Point &p) { x = p.x; y = p.y; z = p.z; };

		/*!  This method packs an Cable_Point object with coordinate values from a given Cable_Point that is passed by reference.  */
		void Set(const Cable_Point &p) { x = p.x; y = p.y; z = p.z; };

		/*!  This method increases/decreases the coordinate values of an existing Cable_Point object.
		\param X The amount in which you want to increment or decrement the x coordinate.
		\param Y The amount in which you want to increment or decrement the y coordinate.
		\param Z The amount in which you want to increment or decrement the z coordinate.
		*/
		void Add(double X, double Y, double Z = 0.0) { x += X; y += Y; z += Z; };

		Cable_Point const operator -(const Cable_Point &p2) const
		{
			return Cable_Point(x - p2.x, y - p2.y, z - p2.z);
		}

		Cable_Point const operator +(const Cable_Point &p2) const
		{
			return Cable_Point(x + p2.x, y + p2.y, z + p2.z);
		}

		Cable_Point const operator *(float const rhs) const
		{
			return Cable_Point(x*rhs, y*rhs, z*rhs);
		}

		Cable_Point const operator /(float const rhs) const
		{
			return Cable_Point(x / rhs, y / rhs, z / rhs);
		}

		Cable_Point const & operator += (Cable_Point const & rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		Cable_Point const & operator -= (Cable_Point const & rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}

		Cable_Point const & operator *= (float const rhs)
		{
			x *= rhs;
			y *= rhs;
			z *= rhs;
			return *this;
		}

		Cable_Point const & operator /= (float const rhs)
		{
			x /= rhs;
			y /= rhs;
			z /= rhs;
			return *this;
		}

		bool operator ==(Cable_Point const & rhs) const
		{
			return Equal(&rhs);
		}

		bool operator !=(Cable_Point const & rhs) const
		{
			return !Equal(&rhs);
		}


		/*!\def Cable_Point_EPSILON */
	/*! This method checks if the xyz components of the point are equal.  */
		bool Equal(Cable_Point const *p, double epsi = Cable_Point_EPSILON) const {
			return (x - p->x < epsi) && (x - p->x > -epsi)
				&& (y - p->y < epsi) && (y - p->y > -epsi)
				&& (z - p->z < epsi) && (z - p->z > -epsi);
		};

		/*!  This method checks if the xyz components of the point are equal.  */
		bool Equal(Cable_Point const &p, double epsi = Cable_Point_EPSILON) const {
			return Equal(&p, epsi);
		};


		Cable_Point& Normalization( )
		{
			float r = this->R();
			x /= r;
			y /= r;
			z /= r;
			return *this;
		}

		float R( )
		{
			return (float)sqrt(x*x + y*y + z*z);
		}


		CString to_string() {
			CString cstrValue;
			cstrValue.Format(L"%g %g %g", x, y, z);
			return cstrValue;
		}

	};
}
