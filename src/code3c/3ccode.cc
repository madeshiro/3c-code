#include "code3c/3ccode.hh"
#include <string.h>

namespace code3c
{
    const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION&
        Code3C::Code3CData::getdim(const code3c::CODE3C_MODEL_DESC & desc, uint32_t size)
    {
        /* NOLINT */
    }
    
    Code3C::Code3CData::Code3CData(Code3C* parent,
                                   const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION& dim)
                                   noexcept (false):
        matb(dim.axis_t,dim.axis_r), m_dimension(dim), m_parent(parent)
    {
        int range[2] = {0, 0};
        size_t bit(0), indexbuf(0);
        for (int i(0); i < dim.axis_t; i++) {
            float degree((((float) i) / ((float) dim.axis_t))*360.0f);
            
            if (degree == 0.0f || degree == 270.0f) {
                // Setup Calibration (radius)
                range[0] = 0;
                range[1] = 0;
                
                for (int j(0); j < dim.axis_r; j++)
                {
                    this->m_mat[i][j] = 0b111 * j % 2;
                }
            } else if (degree <= 90.0f || (degree >= 180.0 && degree <= 270.0f)) {
                // Setup Calibration (angle)
                range[0] = 1;
                range[1] = dim.axis_r;
                
                this->m_mat[i][0] = 0b111 * i%2;
            } else if (degree == 135.0f) {
                // Set-up header
                // TODO header writing
            } else {
                range[0] = 0;
                range[1] = dim.axis_r;
            }
            
            // Write data
            for (int j(range[0]); j < range[1]; j++)
            {
                char _byte(0);
                for (size_t b(0); b < bitl(); b++, _byte <<= 1)
                {
                    _byte |= ((m_parent->m_data[indexbuf] & (0b1 << (bit%8))) << b);
                    
                    bit++;
                    indexbuf = bit/8;
                }
                
                this->m_mat[i][j] = _byte;
            }
        }
    }
    
    Code3C::Code3C(const char *buffer, size_t bufsize, uint32_t model):
        m_data(new char[bufsize]), m_datalen(bufsize), m_desc(code3c_models[model]),
        m_dataMat(this, Code3CData::getdim(code3c_models[model], bufsize))
    {
        strncpy(m_data, buffer, bufsize);
    }
    
    Code3C::Code3C(const char *utf8str, uint32_t model):
        Code3C(utf8str, strlen(utf8str), model)
    {
    }
    
    Code3C::Code3C(const char32_t *unistr [[maybe_unused]], uint32_t model):
        m_data(nullptr), m_desc(code3c_models[model]),
        m_dataMat(this, Code3CData::getdim(code3c_models[model], 0))
    {
        // TODO UTF8
    }
    
    Code3C::Code3C(const code3c::Code3C &c3c):
        Code3C(c3c.m_data, c3c.m_datalen, c3c.m_desc.model_id)
    {
    }
    
    Code3C::~Code3C()
    {
        if (m_data != nullptr)
        {
            delete[] m_data;
        }
    }
    
    DisplayManager* Code3C::draw() const
    {
        return nullptr; // TODO Code3C::draw()
    }
    
    char const* Code3C::data() const
    {
        return m_data;
    }
}