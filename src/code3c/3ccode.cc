#include "code3c/3ccode.hh"
#include <string.h>

namespace code3c
{
    const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION&
        Code3C::Code3CData::getdim(const code3c::CODE3C_MODEL_DESC & desc,
                                   uint32_t size, int err)
    {
        int errSize = (int) (desc.error_margin[err]*size)+1;
        int totalSize = errSize + size;
        
        for (const auto& dim : desc.dimensions)
        {
            if (totalSize <= dim.capacity)
                return dim;
        }
        
        throw "Overflowing dimension required !";
    }
    
    Code3C::Code3CData::Code3CData(Code3C* parent,
                                   const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION& dim)
                                   noexcept (false):
        matb(dim.axis_t,dim.axis_r), m_dimension(dim), m_parent(parent)
    {
        int range[2] = {0, 0};
        size_t bit(0), indexbuf(0);
        // size_t bufsize(((size_t)dataSegSize() << ((dim.axis_t/4)-2))
        //     | ((size_t) errSegSize()));
        
        int qcal1 = dim.axis_t/4,
            qcal2 = dim.axis_t/2,
            qcal3 = 3*dim.axis_t/4;
        int tcal1 = dim.axis_t/3;
        
        uint header(((m_parent->m_desc.model_id-1) << 2) | m_parent->m_errmodel);
        switch (parent->m_desc.model_id)
        {
            case CODE3C_MODEL_1:
                header <<= 16;
                break;
            case CODE3C_MODEL_2:
                header <<= 22;
                break;
            case CODE3C_MODEL_3:
                header <<= 26;
                break;
        }
        
        for (int i(0); i < dim.axis_t; i++)
        {
            if (i == 0 || i == qcal3) {
                // Setup Calibration (radius)
                range[0] = 0;
                range[1] = 0;
                
                for (int j(0); j < dim.axis_r; j++)
                {
                    this->m_mat[i][j] = 0b111 * j % 2;
                }
            } else if (i <= qcal1 || (i >= qcal2 && i <= qcal3)) {
                // Setup Calibration (angle)
                range[0] = 1;
                range[1] = dim.axis_r;
                
                this->m_mat[i][0] = 0b111 * i%2;
            } else if (i == tcal1 || i == tcal1+1) {
                // Set-up header
                for (int j(0); j < dim.axis_r; header >>= 1, j++)
                {
                    this->m_mat[i][j] = header|0b1;
                }
            } else {
                range[0] = 0;
                
                // if (degree > 270.0f && degree < 360.0f) {
                //     // Set-up data size's header
                //     range[1] = dim.axis_r-1;
                //     this->m_mat[i][dim.axis_r-1] = bufsize & 0b1;
                //     bufsize >>= 1;
                // } else {
                    range[1] = dim.axis_r;
                // }
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
    
    Code3C::Code3CData::Code3CData(const code3c::Code3C::Code3CData &mat):
        Code3CData(mat.m_parent, mat.m_dimension)
    {
        // Nothing to do here
    }
    
    char Code3C::Code3CData::getByte(uint32_t cursor [[maybe_unused]]) const
    {
        return '\0'; // TODO getByte(uint32_t)
    }
    
    uint32_t Code3C::Code3CData::size() const
    {
        return dataSegSize() + errSegSize();
    }
    
    uint32_t Code3C::Code3CData::dataSegSize() const
    {
        return m_parent->m_datalen;
    }
    
    uint32_t Code3C::Code3CData::errSegSize() const
    {
        return 0; // TODO errSegSize()
    }
    
    
    
    Code3C::Code3C(const char *buffer, size_t bufsize, uint32_t model, int err):
        m_data(new char[bufsize]), m_datalen(bufsize), m_desc(code3c_models[model]),
        m_dataMat(this, Code3CData::getdim(code3c_models[model], bufsize, err)),
        m_errmodel(err)
    {
        strncpy(m_data, buffer, bufsize);
    }
    
    Code3C::Code3C(const char *utf8str, uint32_t model, int err):
        Code3C(utf8str, strlen(utf8str), model, err)
    {
    }
    
    Code3C::Code3C(const char32_t *unistr [[maybe_unused]], uint32_t model, int err):
        m_data(nullptr), m_desc(code3c_models[model]),
        m_dataMat(this, Code3CData::getdim(code3c_models[model], 0, err)),
        m_errmodel(err)
    {
        // TODO UTF8
    }
    
    Code3C::Code3C(const code3c::Code3C &c3c):
        Code3C(c3c.m_data, c3c.m_datalen, c3c.m_desc.model_id, c3c.m_errmodel)
    {
    }
    
    Code3C::~Code3C()
    {
        if (m_data != nullptr)
        {
            delete[] m_data;
        }
    }
    
    Drawer* Code3C::draw() const
    {
        class Code3CDrawerSample : public Code3CDrawer
        {
            const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION &modelDimension;
        public:
            Code3CDrawerSample(const Code3CData& cData): X11Drawer(
                cData.getDimension().axis_r * CODE3C_PIXEL_UNIT,
                cData.getDimension().axis_r * CODE3C_PIXEL_UNIT,
                cData
                ), modelDimension(cData.getDimension())
            {
            }
            
            void setup() override
            {
                Code3CDrawer::setTitle("Code3C Drawing Frame");
                
                // Draw data
                for (int t(0); t < modelDimension.axis_t; t++)
                    draw_angle(t);
                
                // Fill logo
                fill_circle(height()/2, width()/2, (modelDimension
                    .absRad-modelDimension.effRad)*CODE3C_PIXEL_UNIT);
            }
            
            void draw() override
            {
            }
        } *cDrawerSample = new Code3CDrawerSample(m_dataMat);
        
        return cDrawerSample;
    }
    
    char const* Code3C::data() const
    {
        return m_data;
    }
}