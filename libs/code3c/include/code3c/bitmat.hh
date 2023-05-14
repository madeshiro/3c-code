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
#include <cmath>

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
        explicit mat(int n);
        mat(int n, int m);
        explicit mat(int n, T**);
        explicit mat(int n, int m, T**);
        mat(const mat<T>& obj);
        virtual ~mat();

        virtual T determinant() const noexcept(false);
        virtual mat<T> invert() const noexcept(false);

        virtual mat<T> transposed() const;
        virtual mat<T> submatrix(int rows[], int nrows,
                                 int columns[], int ncolomns) const;
        
        virtual int n() const noexcept(true) final;
        virtual int m() const noexcept(true) final;
        
        virtual mat<T>& operator =(const mat<T>&); /* NOLINT */
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

        virtual explicit operator T() const;
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
        explicit vec(int n);
        vec(int n, T** _vec);
        explicit vec(const mat<T>& mat1) noexcept(false);
        vec(const vec<T>& vec1);
        ~vec() override = default;

        vec<T>& operator =(const vec<T>&);
        vec<T>& operator =(const mat<T>&) override;

        virtual vec<T>  operator +(const vec<T>&);
        virtual vec<T>& operator +=(const vec<T>&);
        virtual vec<T>  operator -(const vec<T>&);
        virtual vec<T>& operator -=(const vec<T>&);
        
        virtual T& operator[](int i);
        virtual T operator[](int i) const;
    };
    
    typedef mat<float>   matf;   /*< */
    typedef mat<double>  matlf;  /*< */
    typedef mat<char>    matb;   /*< */
    typedef mat<int>     matd;   /*< */
    typedef mat<long>    matld;  /*< */
    typedef mat<char8_t> mat8_t; /*< */

    typedef vec<float>   vecf;   /*< */
    typedef vec<double>  veclf;  /*< */
    typedef vec<char>    vecb;   /*< */
    typedef vec<int>     vecd;   /*< */
    typedef vec<long>    vecld;  /*< */
    typedef vec<char8_t> vec8_t; /*< */

    extern template class mat<float>;
    extern template class mat<double>;
    extern template class mat<char>;
    extern template class mat<int>;
    extern template class mat<long>;
    extern template class mat<char8_t>;

    extern template class vec<float>;
    extern template class vec<double>;
    extern template class vec<char>;
    extern template class vec<int>;
    extern template class vec<long>;
    extern template class vec<char8_t>;
}

#endif //HH_LIB_BITMAT
