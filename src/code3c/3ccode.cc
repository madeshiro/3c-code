#include "code3c/3ccode.hh"
#include <cstring>
#include <stdexcept>

namespace code3c
{
    const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION&
        Code3C::Code3CData::getdim(const code3c::CODE3C_MODEL_DESC & desc,
                                   uint32_t size, int err)
    {
        auto errSize = static_cast<uint32_t>((desc.error_margin[err]*(float)size)+1);
        uint32_t totalSize = (errSize + size)*8;
        
        for (const auto& dim : desc.dimensions)
        {
            if (totalSize <= desc.bitl*dim.capacity)
            {
                // Debug print
                printf("Capacity is: %d bits (%dB) ",
                       desc.bitl*dim.capacity, desc.bitl*dim.capacity/8);
                return dim;
            }
        }
        
        throw std::runtime_error("Overflowing dimension required !");
    }
    
    Code3C::Code3CData::Code3CData(Code3C* parent,
                                   const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION& dim)
                                   noexcept (false):
        matb(dim.axis_t,dim.axis_r), m_dimension(dim), m_parent(parent)
    {
        // Debug print
        // printf("\ndata: [");
        
        int range[2] = {0, 0};
        size_t bit(0), indexbuf(0);
        // size_t bufsize(((size_t)dataSegSize() << ((dim.axis_t/4)-2))
        //     | ((size_t) errSegSize()));
        
        int qcal1 = 1*dim.axis_t/4, // q1: rad calibration and begin angle calibration
            qcal2 = 1*dim.axis_t/2, // q2: end angle calibration
            qcal3 = 3*dim.axis_t/4; // q3: rad calibration
        int tcal1 = 3*dim.axis_t/8; // header position
        
        uint64_t header(((m_parent->m_desc.model_id-1) << 2) | m_parent->m_errmodel);
        header <<= ((2*m_dimension.axis_r) - 4);
        header |= dataSegSize();
        
        for (int i(0); i < dim.axis_t; i++)
        {
            if (i == 0 || i == qcal3)
            {
                // Setup Calibration (radius)
                range[0] = 0;
                range[1] = 0;
                
                for (int j(0); j < dim.axis_r; j++)
                {
                    this->m_mat[i][j] = static_cast<char>(mask() * (j % 2));
                }
            } else if (i <= qcal1 || (i >= qcal2 && i <= qcal3))
            {
                // Setup Calibration (angle)
                range[0] = 1;
                range[1] = dim.axis_r;
                
                this->m_mat[i][0] = static_cast<char>(mask() * (i%2));
            } else if (i == tcal1 || i == tcal1+1) {
                range[0] = 0;
                range[1] = 0;
                
                // Set-up header
                for (int j(0); j < dim.axis_r; j++, header>>=1)
                {
                    this->m_mat[i][j] = static_cast<char>(mask() & ~((header&0b1)*mask()));
                }
            } else {
                range[0] = 0;
                range[1] = dim.axis_r;
            }
            
            // Write data
            for (int j(range[0]); j < range[1] && indexbuf < size(); j++)
            {
                char _byte = 0;
                // printf(" "); // Debug print
                for (size_t b(0); b < bitl() && indexbuf < size(); b++)
                {
                    // Debug print
                    // printf("%d", ((m_parent->m_data[indexbuf] >> bit%8) & 0b1));
                    
                    _byte <<= 1;
                    _byte |= (m_parent->m_data[indexbuf] >> bit % 8) & 0b1;
                    
                    bit++;
                    indexbuf = bit/8;
                }
                
                this->m_mat[i][j] = _byte;
            }
        }
        // Debug print
        // printf("]\n");
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
        m_data(strncpy(new char[bufsize], buffer, bufsize)),
        m_datalen(bufsize), m_desc(code3c_models[model]),
        m_dataMat(this, Code3CData::getdim(code3c_models[model], bufsize, err)),
        m_errmodel(err)
    {
    }
    
    Code3C::Code3C(const char *utf8str, uint32_t model, int err):
        Code3C(utf8str, strlen(utf8str), model, err)
    {
    }
    
    Code3C::Code3C(const char32_t *unistr [[maybe_unused]], uint32_t model, int err):
        m_data(nullptr), m_desc(code3c_models[model]),
        m_dataMat(this, Code3CData::getdim(code3c_models[model], 0, err)),
        m_errmodel(err),
        m_datalen(/* todo data len */ 0)
    {
        // TODO UTF8
    }
    
    Code3C::Code3C(const code3c::Code3C &c3c):
        Code3C(c3c.m_data, c3c.m_datalen, c3c.m_desc.model_id, c3c.m_errmodel)
    {
    }
    
    Code3C::~Code3C()
    {
        delete[] m_data;
    }
    
    Drawer* Code3C::draw() const
    {
        class Code3CDrawerSample : public Code3CDrawer
        {
            const Code3C* parent;
            const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION &modelDimension;
        public:
            Code3CDrawerSample(const Code3C* parent, const Code3CData& cData):
            parent(parent),
            X11Drawer(
                2 * cData.getDimension().absRad * CODE3C_PIXEL_UNIT,
                2 * cData.getDimension().absRad * CODE3C_PIXEL_UNIT,
                cData
                ), modelDimension(cData.getDimension())
            {
            }
            
            unsigned long bit_to_color(char _byte)
            {
                switch (parent->m_desc.model_id)
                {
                    case CODE3C_MODEL_1: // WB
                        return 0xffffff&~((_byte&0b1)*0xffffff);
                    case CODE3C_MODEL_2: // WB2C
                    {
                        int cyan = 0xffff * ((_byte >> 1) & 1);
                        int red  = 0xff   * ((_byte >> 0) & 1);
                        return ~rgb(red, cyan>>8, cyan);
                    }
                    case CODE3C_MODEL_3: // WB-RGB
                    {
                        int red   = 0xff * ((_byte >> 2) & 1);
                        int green = 0xff * ((_byte >> 1) & 1);
                        int blue  = 0xff * ((_byte >> 0) & 1);
                        return ~rgb(red, green, blue);
                    }
                    default:
                        throw std::runtime_error("Unsupported model");
                }
            }

            void draw_angle(int t)
            {
                for (int r(m_data.m()-1); r >= 0; r--)
                {
                    char _byte(m_data[t, r]);
                    
                    // Debug print
                    // printf(" ");
                    // for (int i = parent->m_desc.bitl-1; i >= 0; i--)
                    //     printf("%d", (_byte >> i)&1);
                    
                    int offRad(modelDimension.absRad-modelDimension.effRad
                        +modelDimension.deltaRad);
                    int currentRad((offRad+(r*modelDimension.deltaRad))
                        *CODE3C_PIXEL_UNIT);
                    
                    set_color(bit_to_color(_byte));
                    draw_slice(width() / 2, height() / 2, currentRad,
                               180/modelDimension.rev,
                               t*180/(modelDimension.rev));
                }
                // printf("\n"); // Debug print
            }
            
            void setup() override
            {
                Code3CDrawer::setTitle("Code3C Drawing Frame");
                Code3CDrawer::background(0xbe55ab);
                set_color(0);
                Code3CDrawer::fill_circle(width()/2, height()/2, 8+width()/2);

                // Draw data
                for (int t(0); t < modelDimension.axis_t; t++)
                    draw_angle(t);
                
                set_color(0xff0000);
                draw_line(0, 0, width()/2, height()/2);
                
                // Fill logo
                set_color(0xbe55ab);
                fill_circle(width()/2, height()/2, (modelDimension
                    .absRad-modelDimension.effRad)*CODE3C_PIXEL_UNIT);
            }
            
            void draw() override
            {
            }
        } *cDrawerSample = new Code3CDrawerSample(this, m_dataMat);
        
        return cDrawerSample;
    }
    
    char const* Code3C::data() const
    {
        return m_data;
    }
}