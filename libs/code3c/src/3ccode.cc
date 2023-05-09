#include "code3c/3ccode.hh"
#include <cstring>
#include <stdexcept>
#include "code3c/pixelmap.hh"

namespace code3c
{
    char Code3C::header::operator[](uint32_t i) const
    {
        char bit(0);
        if (i < 2)
            bit = (id>>(1-i));
        else if (i < 3)
            bit = err;
        else if (i < 6)
        {
            i -= 3;
            bit = (huff>>(2-i));
        }
        else if (meta_full_bitl-i < sizeof(dlen)*8)
        {
            i -= 6;
            bit = dlen >> (meta_dlen_bitl-i-1);
        }

        return bit & 1;
    }

    const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION&
        Code3C::Code3CData::getdim(const code3c::CODE3C_MODEL_DESC & desc,
                                   uint32_t size, int err)
    {
        uint32_t errSize = (desc.hamming[err]->dim_n()-desc.hamming[err]->dim_k())
                *(size*8/desc.hamming[err]->dim_k())/8;
        uint32_t totalSize = (errSize + size)*8;

        for (const auto& dim : desc.dimensions)
        {
            if (totalSize <= desc.bitl*dim.capacity)
            {
                // Debug print
                cDebug("Capacity is: %d bits (%dB) \n",
                       desc.bitl*dim.capacity, desc.bitl*dim.capacity/8);
                cDebug("Dimension are: {%d, %d, %d, %d}\n",
                       dim.rev, dim.absRad, dim.effRad, dim.deltaRad);
                return dim;
            }
        }
        
        throw std::runtime_error("Overflowing dimension required !");
    }
    
    Code3C::Code3CData::Code3CData(Code3C* parent,
                                   const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION& dim)
                                   noexcept (false):
        matb(dim.axis_t,dim.axis_r), m_dimension(dim), m_parent(parent),
        m_hamming(parent->m_desc.hamming[parent->m_errmodel]->copy()),
        m_huffman(CODE3C_DEFAULT_HTF[parent->m_huffmodel])
    {
        int range[2] = {0, 0};
        size_t bit(0), indexbuf(0);
        size_t errOff;

        // Huffman compressing
        if (m_huffman)
        {
            m_huffdata = (char*) m_huffman->encode<char>(parent->m_data,
                                                        parent->m_datalen,
                                                        &m_hufflen);
            // Convert length from bit to byte
            m_hufflen = (m_hufflen/8) + (m_hufflen % 8 != 0);
        }
        else
        {
            m_huffdata = parent->m_data;
            m_hufflen  = parent->m_datalen;
        }

        // Build data using Hamming algorithm
        m_hamming->set_buffer(m_huffdata, m_hufflen);
        char* hamming_buf = new char[m_hamming->bitl()/8+1];
        m_hamming->build_xbuffer(hamming_buf, &errOff);
        m_hamming->build_pbuffer(&hamming_buf[errOff], nullptr);

        // For debugging
        cDebug("\nHamming data:\n\t\t- dataSeg: %d\n\t\t- errSeg: %d\n",
               dataSegSize(), errSegSize());

        // Compute specials sections positions
        int qcal1 = 1*dim.axis_t/4, // q1: rad calibration and begin angle calibration
            qcal2 = 1*dim.axis_t/2, // q2: end angle calibration
            qcal3 = 3*dim.axis_t/4; // q3: rad calibration
        int tcal1 = 3*dim.axis_t/8; // header position

        // Header vars
        const header& head(parent->m_header);
        int headi(0);
        
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
            }
            else if (i <= qcal1 || (i >= qcal2 && i <= qcal3))
            {
                // Setup Calibration (angle)
                range[0] = 1;
                range[1] = dim.axis_r;
                
                this->m_mat[i][0] = static_cast<char>(mask() * ((i + (i >= qcal2)) % 2));
            }
            else if (i == tcal1 || i == tcal1 + 1)
            {
                range[0] = 0;
                range[1] = 0;
                
                // Set-up header
                for (int j(0); j < dim.axis_r; j++, headi++)
                {
                    this->m_mat[i][j] = head[headi]*mask();
                }
            }
            else
            {
                range[0] = 0;
                range[1] = dim.axis_r;
            }
            
            // Write data
            for (int j(range[0]); j < range[1] /*&& indexbuf < size()*/; j++)
            {
                char _byte = 0;
                for (size_t b(0); b < bitl() /*&& indexbuf < size()*/; b++)
                {
                    _byte <<= 1;
                    _byte |= (hamming_buf[indexbuf%size()] >> bit % 8) & 0b1;
                    
                    bit++;
                    indexbuf = bit / 8;
                }
                
                this->m_mat[i][j] = _byte;
            }
        }

        delete[] hamming_buf;
    }
    
    Code3C::Code3CData::Code3CData(const code3c::Code3C::Code3CData &mat):
        Code3CData(mat.m_parent, mat.m_dimension)
    {
        // Nothing to do here
    }

    Code3C::Code3CData::~Code3CData()
    {
        if (m_huffdata && m_huffdata != m_parent->m_data)
        {
            delete[] m_huffdata;
        }
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
        return m_hamming->xbitl()/8;
    }

    uint32_t Code3C::Code3CData::errSegSize() const
    {
        return m_hamming->pbitl()/8+1;
    }

    Code3C::Code3C(const char *buffer, size_t bufsize, uint32_t model,
                   int err, int huff):
        m_data(strncpy(new char[bufsize], buffer, bufsize)),
        m_datalen(bufsize),
        m_desc(code3c_models[model]),
        m_dim(Code3CData::getdim(code3c_models[model], bufsize, err)),
        m_header({model+1, static_cast<uint64_t>(err), static_cast<uint64_t>(huff),
                  bufsize, static_cast<uint32_t>(2*m_dim.axis_r-6), 6}),
        m_errmodel(err),
        m_huffmodel(huff),
        m_dataMat(this, m_dim)
    {
    }

    Code3C::Code3C(const char *utf8str, uint32_t model,
                   int err, int huff):
        Code3C(utf8str, strlen(utf8str), model, err, huff)
    {
    }

    Code3C::Code3C(const char32_t *unistr [[maybe_unused]], uint32_t model,
                   int err, int huff):
        m_data(nullptr),
        m_datalen(/* todo data len */ 0),
        m_desc(code3c_models[model]),
        m_dim(Code3CData::getdim(code3c_models[model], m_datalen, err)),
        m_header({model+1, static_cast<uint64_t>(err),
                  static_cast<uint64_t>(huff), m_datalen,
                  static_cast<uint32_t>(2*(m_dim.effRad*m_dim.axis_r)-6), 6}),
        m_errmodel(err),
        m_huffmodel(huff),
        m_dataMat(this, Code3CData::getdim(code3c_models[model], 0, err))
    {
        // TODO UTF8
    }
    
    Code3C::Code3C(const code3c::Code3C &c3c):
        Code3C(c3c.m_data, c3c.m_datalen, c3c.m_desc.model_id,
               c3c.m_errmodel, c3c.m_huffmodel)
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

            PixelMap *logo, *marker;

            void save_ui()
            {
                char fname[256];
                sprintf(fname, "3ccode-generated-%llx.png", hash());
                savePNG(fname);
            }
        public:
            Code3CDrawerSample(const Code3C* parent, const Code3CData& cData):
            parent(parent), logo(nullptr), marker(nullptr),
            Code3CDrawer(
                40+2 * cData.getDimension().absRad * CODE3C_PIXEL_UNIT,
                40+2 * cData.getDimension().absRad * CODE3C_PIXEL_UNIT,
                cData
                ), modelDimension(cData.getDimension())
            {
                bindKey((DRAWER_KEY_CTRL | 's'),
                        reinterpret_cast<delegate>(&Code3CDrawerSample::save_ui));
            }

            ~Code3CDrawerSample()
            {
                delete logo;
                delete marker;
            }
            
            unsigned long bit_to_color(char _byte)
            {
                switch (parent->m_desc.model_id)
                {
                    case CODE3C_MODEL_WB:   // WB -- 1bit
                    {
                        return 0xffffff & ~((_byte & 0b1) * 0xffffff);
                    }
                    case CODE3C_MODEL_WB2C: // WB2C -- 2bits
                    {
                        int cyan = 0xffff * ((_byte >> 1) & 1);
                        int red  = 0xff   * ((_byte >> 0) & 1);
                        return ~rgb(red, cyan>>8, cyan);
                    }
                    case CODE3C_MODEL_WB6C: // WB6C -- 3bits
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
                    
                    int offRad(modelDimension.absRad-modelDimension.effRad
                        +modelDimension.deltaRad);
                    int currentRad((offRad+(r*modelDimension.deltaRad))
                        *CODE3C_PIXEL_UNIT);
                    
                    foreground(bit_to_color(_byte));
                    draw_slice(width() / 2, height() / 2, currentRad,
                               180/modelDimension.rev,
                               t*180/(modelDimension.rev));
                }
            }
            
            void setup() override
            {
                // Setup window
                setTitle("Code3C Drawing Frame");

                // Load marker
                {
                    PixelMap map = PixelMap::loadFromPNG("resources/code3c-marker.png");
                    marker = new PixelMap(map.resize(width(), height()));
                }

                // Load logo
                {
                    int logoDiameter = (modelDimension.absRad - modelDimension.effRad) *
                                   CODE3C_PIXEL_UNIT * 2;
                    // Todo define var to contain custom file
                    PixelMap map = PixelMap::loadFromPNG(parent->m_desc.default_logo);
                    logo = new PixelMap(map.resize(logoDiameter, logoDiameter));
                }
            }

            void draw() override
            {
                // white background
                background(0xffffff);

                // Draw marker
                draw_pixelmap(*marker, 0, 0);

                // Draw colour calibration
                {
                    int offRad(modelDimension.absRad-modelDimension.effRad
                        +modelDimension.deltaRad);
                    int currentRad((offRad+(m_data.m()*modelDimension.deltaRad))
                        *CODE3C_PIXEL_UNIT);
                    int tcal1 = 3*modelDimension.axis_t/8; // header position

                    // Left part from the header
                    for (int bit(0), t=tcal1+1;
                        bit < (1<<parent->m_desc.bitl)/2;
                        bit++,t++)
                    {
                        foreground(bit_to_color(bit));
                        draw_slice(width()/2, height()/2, currentRad+5,
                                   180/modelDimension.rev,
                                   t*180/(modelDimension.rev));
                    }
                    // Right part from the header
                    for (int bit(0b111&parent->m_desc.mask), i(0),t=tcal1;
                            i < (1<<parent->m_desc.bitl)/2;
                            bit--,t--, i++)
                    {
                        foreground(bit_to_color(bit));
                        draw_slice(width()/2, height()/2, currentRad+5,
                                   180/modelDimension.rev,
                                   t*180/(modelDimension.rev));
                    }
                }

#ifdef CODE3C_DEBUG
                // Draw 3ccode outline
                foreground(0);
                fill_circle(width()/2, height()/2, 2+(width()-40)/2);
                draw_line(0, height()/2, width(), height()/2);
                draw_line(width()/2, 0, width()/2, height());

                // Debug : header landmark
                foreground(0xff0000);
                draw_line(0, 0, width()/2, height()/2);
#endif // CODE3C_DEBUG

                // Draw data
                for (int t(0); t < modelDimension.axis_t; t++)
                    draw_angle(t);

                // Fill logo
                foreground(0xbe55ab);
                fill_circle(width()/2, height()/2, (modelDimension
                    .absRad-modelDimension.effRad)*CODE3C_PIXEL_UNIT);
                
                // Draw logo
                int logoDiameter = (modelDimension.absRad - modelDimension.effRad) *
                                   CODE3C_PIXEL_UNIT * 2;
                int origX = (width() - logoDiameter) / 2;
                int origY = (height() - logoDiameter) / 2;

                int rlogo = logoDiameter / 2;
                for (int x = 0, xx=origX; x < logoDiameter; x++, xx++)
                {
                    int rx = abs(rlogo-x);
                    for (int y = 0, yy=origY; y < logoDiameter; y++, yy++)
                    {
                        int ry = abs(rlogo-y);
                        if (((rx*rx)+(ry*ry)) < (rlogo*rlogo))
                            draw_pixel((*logo)[x,y].color, xx, yy);
                    }
                }
            }
        } *cDrawerSample = new Code3CDrawerSample(this, m_dataMat);
        
        return cDrawerSample;
    }
    
    char const* Code3C::data() const
    {
        return m_data;
    }
}