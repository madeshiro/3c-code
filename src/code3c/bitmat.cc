#include "include/code3c/bitmat.hh"

namespace code3c
{
    template class mat<float>;
    template class mat<double>;
    template class mat<char>;
    template class mat<int>;
    template class mat<long>;

    template class vec<float>;
    template class vec<double>;
    template class vec<char>;
    template class vec<int>;
    template class vec<long>;

    template < typename T >
    mat<T>::mat(int n):
        mat<T>(n, n)
    {
    }

    template < typename T >
    mat<T>::mat(int n, int m):
        m_mat(new T*[n])
    {
        this->m_row = n;
        this->m_column = m;
        for (int i(0); i < n; i++)
        {
            m_mat[i] = new T[m];
            for (int j(0); j < m; j++)
                m_mat[i][j] = (T) 0;
        }
    }
    
    template < typename T >
    mat<T>::mat(int n, T ** table):
        mat<T>(n, n, table)
    {
    }
    
    template < typename T >
    mat<T>::mat(int n, int m, T** table)
    {
        this->m_mat = table;
        this->m_row = n;
        this->m_column = m;
    }
    
    template < typename T >
    mat<T>::mat(const mat<T> &obj):
        m_mat(new T*[obj.m_row])
    {
        m_row = obj.m_row;
        m_column = obj.m_column;
        for (int i(0); i < m_row; i++)
        {
            m_mat[i] = new T[m_column];
            for (int j(0); j < m_column; j++)
                m_mat[i][j] = obj[i, j];
        }
    }
    
    template < typename T >
    mat<T>::~mat()
    {
        for (int i(0); i < n(); i++)
            delete[] m_mat[i];
        delete[] m_mat;
    }
    
    template < typename T >
    mat<T> mat<T>::transposed() const
    {
        mat<T> tr(m_column, m_row);
        for (int i(0); i < m(); i++)
        {
            for (int j(0); j < n(); j++)
                tr[i, j] = m_mat[j][i];
        }
        return tr;
    }
    
    template < typename T >
    mat<T> mat<T>::submatrix(int *rows, int nrows, int *columns, int ncolomns) const
    {
        mat<T> mat1(nrows, ncolomns);
        for (int i(0), i1(0), irow(0); i < n(); i++)
        {
            if (nrows == n() || rows[irow] == i)
            {
                for (int j(0), j1(0), jcolumn(0); j < m(); j++)
                {
                    if (ncolomns == m() || columns[jcolumn] == j)
                    {
                        mat1[i1,j1] = m_mat[i][j];
                        j1++;
                        jcolumn++;
                    }
                }
                irow++;
                i1++;
            }
        }
        return mat1;
    }
    
    template < typename T >
    int mat<T>::n() const noexcept(true)
    {
        return m_row;
    }
    
    template < typename T >
    int mat<T>::m() const noexcept(true)
    {
        return m_column;
    }
    
    template < typename T >
    mat<T>& mat<T>::operator=(const mat<T> & mat1)
    {
        if (mat1.n() != n() && mat1.m() != m())
            throw "Invalid Dimension for assignment";
        
        for (int i(0); i < n(); i++)
        {
            for (int j(0); j < m(); j++)
                m_mat[i][j] = mat1[i, j];
        }
        return *this;
    }
    
    template < typename T >
    bool mat<T>::operator==(const mat<T> & mat1) const
    {
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                if (m_mat[i][j] != mat1[i, j])
                    return false;
        return true;
    }
    
    template < typename T >
    bool mat<T>::operator!=(const mat<T> & mat1) const
    {
        return !operator==(mat1);
    }
    
    template < typename T >
    mat<T> mat<T>::operator+(const mat<T> &mat1)
    {
        mat<T> mat2(m_row, m_column);
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                mat2.m_mat[i][j] = m_mat[i][j] + mat1[i, j];
        return mat2;
    }
    
    template < typename T >
    mat<T>& mat<T>::operator+=(const mat<T> & mat1)
    {
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                m_mat[i][j] += mat1[i, j];
        return *this;
    }
    
    template < typename T >
    mat<T> mat<T>::operator-() const
    {
        mat<T> mat1(*this);
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                mat1.m_mat[i][j] = -mat1[i,j];
        return mat1;
    }
    
    template < typename T >
    mat<T> mat<T>::operator-(const mat<T> & mat1)
    {
        mat<T> mat2(m_row, m_column);
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                mat2.m_mat[i][j] = m_mat[i][j] - mat1[i,j];
        return mat2;
    }
    
    template < typename T >
    mat<T>& mat<T>::operator-=(const mat<T> & mat1)
    {
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                m_mat[i][j] -= mat1[i,j];
        return *this;
    }
    
    template < typename T >
    mat<T> mat<T>::operator*(T val) const
    {
        mat<T> mat1(*this);
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                mat1[i, j]*=val;
        return mat1;
    }
    
    template < typename T >
    mat<T>& mat<T>::operator*=(T val)
    {
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                m_mat[i][j]*=val;
        return *this;
    }
    
    template < typename T >
    mat<T> mat<T>::operator*(const mat<T> &mat1) const
    {
        if (m() != mat1.n())
            throw "Invalid dimension for matrix multiplication";

        mat<T> mat2(m_row, mat1.m_column);
        for (int i(0); i < n(); i++)
        {
            for (int j(0); j < mat1.m(); j++)
            {
                T _sum((T) 0);
                for (int k(0); k < m(); k++)
                    _sum += m_mat[i][k] * mat1[k, j];
                mat2.m_mat[i][j] = _sum;
            }
        }
        return mat2;
    }
    
    template < typename T >
    mat<T>& mat<T>::operator*=(const mat<T> &mat1)
    {
        return (*this = *this * mat1);
    }
    
    template < typename T >
    mat<T> mat<T>::operator/(T val) const
    {
        mat<T> mat1(*this);
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                mat1.m_mat[i][j] /= val;
        return mat1;
    }
    
    template < typename T >
    mat<T>& mat<T>::operator/=(T val)
    {
        for (int i(0); i < n(); i++)
            for (int j(0); j < m(); j++)
                m_mat[i][j] /= val;
        return *this;
    }
    
    template < typename T >
    mat<T> mat<T>::operator/(const mat<T> &mat1) const noexcept(false)
    {
        return operator *(mat1.invert());
    }
    
    template < typename T >
    mat<T>& mat<T>::operator/=(const mat<T> &mat1) noexcept(false)
    {
        return *this *= mat1.invert();
    }
    
    template < typename T >
    T& mat<T>::operator[](int i, int j)
    {
        return m_mat[i][j];
    }
    
    template < typename T >
    T mat<T>::operator[](int i, int j) const
    {
        return m_mat[i][j];
    }
    
    template < typename T >
    T mat<T>::determinant() const noexcept(false)
    {
        if (this->n() != this->m())
            throw "Invalid matrix dimension (require square dim, 2x2 or higher)";
        return (T) 0; // TODO mat::determinant
    }

    template < typename T >
    mat<T> mat<T>::invert() const noexcept(false)
    {
        return mat<T>(n(), m()); // TODO mat::invert
    }

    template < typename T >
    mat<T> mat<T>::operator~() const noexcept(false)
    {
        return invert();
    }

    template < typename T >
    mat<T>::operator T() const
    {
        return m_mat[0][0];
    }
    
    template < typename T >
    vec<T>::vec(int n):
        mat<T>(n, 1)
    {
    }
    
    template < typename T >
    vec<T>::vec(int n, T *_vec):
        mat<T>(n, 1, &_vec)
    {
    }
    
    template < typename T >
    vec<T>::vec(const mat<T> &mat1) noexcept(false):
        mat<T>(mat1)
    {
        if (mat1.m())
            throw "Invalid matrix dimension";
    }
    
    template < typename T >
    vec<T>::vec(const vec<T> &vec1):
        mat<T>(vec1)
    {
    }
    
    template < typename T >
    vec<T> vec<T>::operator+(const vec<T> & vec1)
    {
        if (vec1.n() != this->n())
            throw "Invalid vector dimension";
        vec<T> vec2(*this);
        for (int i = 0; i < this->n(); i++)
            vec2[i, 0] += vec1[i, 0];
        return vec2;
    }
    
    template < typename T >
    vec<T>& vec<T>::operator+=(const vec<T> & vec1)
    {
        if (vec1.n() != this->n())
            throw "Invalid vector dimension";
        for (int i = 0; i < this->n(); i++)
            this->m_mat[i][0] += vec1[i, 0];
        return *this;
    }
    
    template < typename T >
    vec<T> vec<T>::operator-(const vec<T> & vec1)
    {
        if (vec1.n() != this->n())
            throw "Invalid vector dimension";
        vec<T> vec2(*this);
        for (int i = 0; i < this->n(); i++)
            vec2[i, 0] -= vec1[i, 0];
        return vec2;
    }
    
    template < typename T >
    vec<T>& vec<T>::operator-=(const vec<T> & vec1)
    {
        if (vec1.n() != this->n())
            throw "Invalid vector dimension";
        for (int i = 0; i < this->n(); i++)
            this->m_mat[i][0] -= vec1[i, 0];
        return *this;
    }
}