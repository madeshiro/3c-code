/* 3C-CODE Library
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

namespace code3c
{
    template < typename T, int n, int m >
    class mat
    {
        T **m_;
    public:
        mat(T default_value = 0);
        explicit mat(const T **);
        mat(const mat<T, n, m>& obj);
        virtual ~mat();
        
        mat<T, m, n> transposed() const;
        
        T determinant() const noexcept(false);
        mat<T, n, m> invert() const noexcept(false);
        
        mat<T,n,m>& operator =(const mat<T,n,m>&);
        bool operator ==(const mat<T,n,m>&) const;
        
        mat<T, n, m>  operator +(const mat<T, n, m>&);
        mat<T, n, m>& operator +=(const mat<T, n, m>&);
        mat<T, n, m>  operator -(const mat<T, n, m>&);
        mat<T, n, m>& operator -=(const mat<T, n, m>&);
        mat<T, n, m>  operator -() const;
        
        mat<T, n, m> operator *(T) const;
        template < int np > mat<T, n, np>  operator *(const mat<T, m, np>&) const;
        mat<T, n, m>& operator *=(T);
        mat<T, n, m>& operator *=(const mat<T, m, m>&);
        
        mat<T, n, m> operator /(T) const;
        mat<T, n, m>  operator /(const mat<T, m, m>&) const noexcept(false);
        mat<T, n, m>& operator /=(T);
        mat<T, n, m>& operator /=(const mat<T, m, m>&) noexcept(false);
        
        mat<T, n, m> operator ~() const noexcept(false);
        
        T& operator[](int i, int j);
        T operator[](int i, int j) const;
    };
    
    template < typename T, int n >
    using vec = mat<T, n ,1>;
    
    typedef vec<float, 2>   vec2f;  /*< */
    typedef vec<double, 2>  vec2lf; /*< */
    typedef vec<char, 2>    vec2b;  /*< */
    typedef vec<int, 2>     vec2d;  /*< */
    typedef vec<long, 2>    vec2ld; /*< */
    
    typedef vec<float, 3>   vec3f;  /*< */
    typedef vec<double, 3>  vec3lf; /*< */
    typedef vec<char, 3>    vec3b;  /*< */
    typedef vec<int, 3>     vec3d;  /*< */
    typedef vec<long, 3>    vec3ld; /*< */
    
    template < int n > using vecf  = vec<float, n>  ; /*< */
    template < int n > using veclf = vec<double, n> ; /*< */
    template < int n > using vecb  = vec<char, n>   ; /*< */
    template < int n > using vecd  = vec<int, n>    ; /*< */
    template < int n > using vecld = vec<long, n>   ; /*< */
    
    typedef mat<float, 2, 2>   mat2f;  /*< */
    typedef mat<double, 2, 2>  mat2lf; /*< */
    typedef mat<char, 2, 2>    mat2b;  /*< */
    typedef mat<int, 2, 2>     mat2d;  /*< */
    typedef mat<long, 2, 2>    mat2ld; /*< */
    
    typedef mat<float, 3, 3>   mat3f;  /*< */
    typedef mat<double, 3, 3>  mat3lf; /*< */
    typedef mat<char, 3, 3>    mat3b;  /*< */
    typedef mat<int, 3, 3>     mat3d;  /*< */
    typedef mat<long, 3, 3>    mat3ld; /*< */
    
    template < typename T, int n >
    mat<T, n, n> matIn()
    {
        mat<T, n, n> matrix(0);
        for (int i(0); i < n; i++)
            matrix[i, i] = (T) 1;
        return matrix;
    }
    
    template < typename T >
    mat<T, 2, 2> matI2()
    {
        return matIn<T, 2>();
    }
    
    template < typename T >
    mat<T, 3, 3> matI3()
    {
        return matIn<T, 3>();
    }
    
    template < int n, int m > using matf  = mat<float, n, m>  ; /*< */
    template < int n, int m > using matlf = mat<double, n, m> ; /*< */
    template < int n, int m > using matb  = mat<char, n, m>   ; /*< */
    template < int n, int m > using matd  = mat<int, n, m>    ; /*< */
    template < int n, int m > using matld = mat<long, n, m>   ; /*< */
}

#endif //HH_LIB_BITMAT
