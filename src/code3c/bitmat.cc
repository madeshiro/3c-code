#include "include/code3c/bitmat.hh"

namespace code3c
{
    template class mat<float, 2, 1>;
    template class mat<double, 2, 1>;
    template class mat<char, 2, 1>;
    template class mat<int, 2, 1>;
    template class mat<long, 2, 1>;
    
    template class mat<float, 3, 1>;
    template class mat<double, 3, 1>;
    template class mat<char, 3, 1>;
    template class mat<int, 3, 1>;
    template class mat<long, 3, 1>;
    
    template class mat<float, 2, 2>;
    template class mat<double, 2, 2>;
    template class mat<char, 2, 2>;
    template class mat<int, 2, 2>;
    template class mat<long, 2, 2>;
    
    template class mat<float, 3, 3>;
    template class mat<double, 3, 3>;
    template class mat<char, 3, 3>;
    template class mat<int, 3, 3>;
    template class mat<long, 3, 3>;
    
    template < typename T, int n, int m >
    mat<T,n,m>::mat(T default_value):
        m_(new T*[n])
    {
        for (int i(0); i < n; i++)
        {
            m_[i] = new T[m];
            for (int j(0); j < m; j++)
                m_[i][j] = default_value;
        }
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>::mat(const T ** table):
        m_(new T*[n])
    {
        for (int i(0); i < n; i++)
        {
            m_[i] = new T[m];
            for (int j(0); j < m; j++)
                m_[i][j] = table[i][j];
        }
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>::mat(const mat<T, n, m> &obj):
        m_(new T*[n])
    {
        for (int i(0); i < n; i++)
        {
            m_[i] = new T[m];
            for (int j(0); j < m; j++)
                m_[i][j] = obj[i, j];
        }
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>::~mat()
    {
        for (int i(0); i < n; i++)
            delete[] m_[i];
        delete[] m_;
    }
    
    template < typename T, int n, int m >
    mat<T,m,n> mat<T,n,m>::transposed() const
    {
        mat<T, m, n> tr;
        for (int i(0); i < m; i++)
        {
            for (int j(0); j < n; j++)
                tr[i, j] = m_[j][i];
        }
        return tr;
    }
    
    template < typename T, int n, int m >
    T mat<T,n,m>::determinant() const noexcept(false)
    {
        if (n != m)
            throw "Invalid matrix dimension (require square dim, 2x2 or higher)";
        return (T) 0; // TODO mat::determinant
    }
    
    template < typename T, int n, int m >
    mat<T, n, m> mat<T, n, m>::invert() const noexcept(false)
    {
        T deter(determinant());
        return mat<T,n,m>((T) 0); // TODO mat::invert
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>& mat<T,n,m>::operator=(const mat<T, n, m> & mat1)
    {
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                m_[i][j] = mat1[i, j];
        return *this;
    }
    
    template < typename T, int n, int m >
    bool mat<T,n,m>::operator==(const mat<T, n, m> & mat1) const
    {
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                if (m_[i][j] != mat1[i, j])
                    return false;
        return true;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m> mat<T,n,m>::operator+(const mat<T, n, m> &mat1)
    {
        mat<T,n,m> mat2;
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                mat2.m_[i][j] = m_[i][j] + mat1[i, j];
        return mat2;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>& mat<T,n,m>::operator+=(const mat<T, n, m> & mat1)
    {
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                m_[i][j] += mat1[i, j];
        return *this;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m> mat<T,n,m>::operator-() const
    {
        mat<T,n,m> mat1(*this);
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                mat1.m_[i][j] = -mat1[i,j];
        return mat1;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m> mat<T,n,m>::operator-(const mat<T, n, m> & mat1)
    {
        mat<T,n,m> mat2;
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                mat2.m_[i][j] = m_[i][j] - mat1[i,j];
        return mat2;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>& mat<T,n,m>::operator-=(const mat<T, n, m> & mat1)
    {
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                m_[i][j] -= mat1[i,j];
        return *this;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m> mat<T,n,m>::operator*(T val) const
    {
        mat<T,n,m> mat1(*this);
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                mat1[i, j]*=val;
        return mat1;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>& mat<T,n,m>::operator*=(T val)
    {
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                m_[i][j]*=val;
        return *this;
    }
    
    template < typename T, int n, int m >
    template < int np >
    mat<T,n,np> mat<T,n,m>::operator*(const mat<T, m, np> &mat1) const
    {
        mat<T, n, np> mat2;
        for (int i(0); i < n; i++)
        {
            for (int j(0); j < np; j++)
            {
                T _sum((T) 0);
                for (int k(0); k < m; k++)
                    _sum += m_[i][k]*mat1[k, j];
                mat2.m_[i][j] = _sum;
            }
        }
        return mat2;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>& mat<T,n,m>::operator*=(const mat<T, m, m> &mat1)
    {
        mat<T, n, m> mat2(*this);
        for (int i(0); i < n; i++)
        {
            for (int j(0); j < m; j++)
            {
                T _sum((T) 0);
                for (int k(0); k < m; k++)
                    _sum += m_[i][k]*mat1[k,j];
                mat2.m_[i][j] = _sum;
            }
        }
        
        *this = mat2;
        return *this;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m> mat<T,n,m>::operator/(T val) const
    {
        mat<T,n,m> mat1(*this);
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                mat1.m_[i][j] /= val;
        return mat1;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>& mat<T,n,m>::operator/=(T val)
    {
        for (int i(0); i < n; i++)
            for (int j(0); j < m; j++)
                m_[i][j] /= val;
        return *this;
    }
    
    template < typename T, int n, int m >
    mat<T,n,m> mat<T,n,m>::operator/(const mat<T, m, m> &mat1) const noexcept(false)
    {
        return operator *(mat1.invert());
    }
    
    template < typename T, int n, int m >
    mat<T,n,m>& mat<T,n,m>::operator/=(const mat<T, m, m> &mat1) noexcept(false)
    {
        return *this *= mat1.invert();
    }
    
    template < typename T, int n, int m >
    mat<T,n,m> mat<T,n,m>::operator~() const noexcept(false)
    {
        return invert();
    }
    
    template < typename T, int n, int m >
    T& mat<T,n,m>::operator[](int i, int j)
    {
        return m_[i][j];
    }
    
    template < typename T, int n, int m >
    T mat<T,n,m>::operator[](int i, int j) const
    {
        return m_[i][j];
    }
}