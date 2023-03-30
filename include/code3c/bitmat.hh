/*
 * 3C-CODE Library
 * Copyright (C) 2023 - Rin "madeshiro" Baudelet
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this
 * program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef HH_LIB_BITMAT
#define HH_LIB_BITMAT
#include <math.h>

namespace code3c
{
    template < typename T >
    /**
     * 
     *
     * @tparam T type of data contains in the matrix
     */
    class mat
    {
        int m_row, m_column;
    protected:
        T **m_mat;
    public:
        mat(int n);
        mat(int n, int m);
        explicit mat(int n, T**);
        explicit mat(int n, int m, T**);
        mat(const mat<T>& obj);
        virtual ~mat();

        virtual T determinant() const noexcept(false);
        virtual mat<T> invert() const noexcept(false);

        virtual mat<T> transposed() const;
        virtual mat<T> submatrix(int rows[], int nrows, int columns[], int ncolomns) const;
        
        virtual int n() const noexcept(true) final;
        virtual int m() const noexcept(true) final;
        
        virtual mat<T>& operator =(const mat<T>&);
        virtual bool operator ==(const mat<T>&) const;
        virtual bool operator !=(const mat<T>&) const;
        
        virtual mat<T>  operator +(const mat<T>&);
        virtual mat<T>& operator +=(const mat<T>&);
        virtual mat<T>  operator -(const mat<T>&);
        virtual mat<T>& operator -=(const mat<T>&);
        virtual mat<T>  operator -() const;
        
        mat<T> operator *(T) const;
        mat<T>  operator *(const mat<T>&) const;
        mat<T>& operator *=(T);
        mat<T>& operator *=(const mat<T>&);
        
        mat<T> operator /(T) const;
        mat<T>  operator /(const mat<T>&) const noexcept(false);
        mat<T>& operator /=(T);
        mat<T>& operator /=(const mat<T>&) noexcept(false);
        
        T& operator[](int i, int j);
        T operator[](int i, int j) const;

        virtual mat<T> operator ~() const noexcept(false);

        virtual operator T() const;
    };
    
    template < typename T >
    mat<T> operator *(T val, const mat<T>& mat1)
    {
        return mat1 * val;
    }
    
    template < typename T >
    mat<T> operator /(T val, const mat<T>& mat1)
    {
        return mat1 / val;
    }
    
    template < typename T >
    mat<T> matIn(int n)
    {
        mat<T> matrix(0);
        for (int i(0); i < n; i++)
            matrix[i, i] = (T) 1;
        return matrix;
    }
    
    template < typename T >
    inline mat<T> matI2()
    {
        return matIn<T>(2);
    }
    
    template < typename T >
    inline mat<T> matI3()
    {
        return matIn<T>(3);
    }
    
    template < typename T >
    inline mat<T> matI4()
    {
        return matIn<T>(4);
    }

    template < typename T >
    class vec : public mat<T>
    {
    public:
        vec(int n);
        vec(int n, T* _vec);
        vec(const mat<T>& mat1) noexcept(false);
        vec(const vec<T>& vec1);
        virtual ~vec() override = default;

        virtual vec<T>  operator +(const vec<T>&);
        virtual vec<T>& operator +=(const vec<T>&);
        virtual vec<T>  operator -(const vec<T>&);
        virtual vec<T>& operator -=(const vec<T>&);
        // virtual vec<T>  operator -() const;
    };
    
    typedef mat<float>   matf;  /*< */
    typedef mat<double>  matlf; /*< */
    typedef mat<char>    matb;  /*< */
    typedef mat<int>     matd;  /*< */
    typedef mat<long>    matld; /*< */

    typedef vec<float>   vecf;  /*< */
    typedef vec<double>  veclf; /*< */
    typedef vec<char>    vecb;  /*< */
    typedef vec<int>     vecd;  /*< */
    typedef vec<long>    vecld; /*< */

    extern template class mat<float>;
    extern template class mat<double>;
    extern template class mat<char>;
    extern template class mat<int>;
    extern template class mat<long>;

    extern template class vec<float>;
    extern template class vec<double>;
    extern template class vec<char>;
    extern template class vec<int>;
    extern template class vec<long>;
    
    template < typename T >
    mat<T> mat2translation(T dx, T dy)
    {
        return mat<T>(3, new T[3]{
            new T[3]{1, 0, dx},
            new T[3]{0, 1, dy},
            new T[3]{0, 0,  1}
        });
    }
    
    template < typename T >
    mat<T> mat2dilation(T scale)
    {
        return mat<T>(3, new T[3] {
            new T[3] {scale, 0, 0},
            new T[3] {0, scale, 0},
            new T[3] {0,     0, 1}
        });
    }
    
    template < typename T >
    mat<T> mat2dilation(T scaleX, T scaleY)
    {
        return mat<T>(3, new T[3] {
            new T[3] {scaleX, 0, 0},
            new T[3] {0, scaleY, 0},
            new T[3] {0,      0, 1}
        });
    }
    
    template < typename T >
    /**
     *
     * @tparam T
     * @param angle (rad)
     * @return
     */
    mat<T> mat2rotate(double angle)
    {
        return mat<T>(3, new T[3] {
           new T[3] {(T) cos(angle), (T) -sin(angle), (T) 0},
           new T[3] {(T) sin(angle), (T)  cos(angle), (T) 0},
           new T[3] {(T)          0, (T)           0, (T) 1}
        });
    }
}

#endif //HH_LIB_BITMAT
