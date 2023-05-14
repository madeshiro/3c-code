#include "code3c/3ccode.hh"
#include <cstring>
#include <stdexcept>
#include "code3c/pixelmap.hh"

namespace code3c
{
    Code3C::data::data(Code3C *parent):
            mat8_t(parent->dimension().axis_t, parent->dimension().axis_r),
            m_parent(parent),
            m_hamming(parent->model().hamming[parent->m_errmodel]->copy())
    {
        // Comfort variables
        const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION& dim(parent->dimension());
        size_t range[2] = {0, 0}, bit(0), index(0), erroff(0), ihead(0);
        char8_t* data3c;
        char* data;
        uint32_t ldata3c;
        auto set_range = [&range](size_t lindex, size_t hindex) -> void {
            range[0] = lindex; range[1] = hindex;
        };

        // Huffman compression
        HuffmanTable* huffman = code3c_default_htf[parent->m_huffmodel];
        if (huffman)
        {
            data3c = huffman->encode<char8_t>(parent->m_rawdata,
                                              parent->m_datalen,
                                              &ldata3c);
            data    = (char*) data3c;
            ldata3c = (ldata3c/8) + (ldata3c % 8 != 0);
        }
        else
        {
            data3c  = (char8_t*) strncpy(new char[parent->m_datalen],
                    (char*)parent->m_rawdata, parent->m_datalen);
            data    = (char*) data3c;
            ldata3c = parent->m_datalen;
        }

        // Build data using Hamming algorithm
        m_hamming->set_buffer(data, ldata3c);
        char* hammingbuf = new char[m_hamming->bitl()/8+1];
        m_hamming->build_xbuffer(hammingbuf, &erroff);
        m_hamming->build_pbuffer(&hammingbuf[erroff], nullptr);

        // Re-assignation of buffers
        delete[] data3c;
        data3c = (char8_t *) hammingbuf;

        // Compute specials sections positions
        int qcal1 = 1*dim.axis_t/4, // q1: rad calibration and begin angle calibration
            qcal2 = 1*dim.axis_t/2, // q2: end angle calibration
            qcal3 = 3*dim.axis_t/4; // q3: rad calibration
        int tcal1 = 3*dim.axis_t/8; // header position

        // Header
        const header& head(parent->m_header);

        for (int i(0); i < dim.axis_t; i++)
        {
            if (i == 0 || i == qcal3)
            {
                // Setup Calibration (radius)
                set_range(0, 0);

                for (int j(0); j < dim.axis_r; j++)
                {
                    m_mat[i][j] = static_cast<char8_t>(mask() * (j%2));
                }
            }
            else if (i <= qcal1 ||  (i >= qcal2 && i <= qcal3))
            {
                // Setup Calibration (angle)
                set_range(1, dim.axis_r);
                m_mat[i][0] = static_cast<char8_t>(mask() * ((i + (i >= qcal2)) % 2));
            }
            else if (i == tcal1 || i == tcal1 + 1)
            {
                set_range(0, 0);

                // Setup header
                for (int j(0); j < dim.axis_r; j++, ihead++)
                {
                    m_mat[i][j] = head[ihead]* mask();
                }
            }
            else set_range(0, dim.axis_r);

            // Write data
            for (int j(range[0]); j < range[1]; j++)
            {
                char8_t _byte = 0;
                for (size_t b(0); b < bitl(); b++)
                {
                    _byte <<= 1;
                    _byte |= (data3c[index % size()] >> bit % 8) & 0b1;
                    bit++;
                    index = bit / 8;
                }

                m_mat[i][j] = _byte;
            }
        }

        delete[] data3c;
    }

    Code3C::data::data(code3c::Code3C *parent, const code3c::mat8_t &in_data):
            mat8_t(in_data), m_parent(parent),
            m_hamming(parent->model().hamming[parent->m_errmodel]->copy())
    {
    }

    Code3C::data::data(const data &obj):
            mat8_t(obj), m_parent(obj.m_parent),
            m_hamming(obj.m_hamming->copy())
    {
    }

    Code3C::data::~data()
    {
        delete m_hamming;
    }

    char8_t Code3C::data::getByte(size_t index) const
    {
        return m_parent->m_rawdata[index];
    }

    size_t Code3C::data::size() const
    {
        return dataSegSize() + errSegSize();
    }

    size_t Code3C::data::dataSegSize() const
    {
        return m_hamming->xbitl() / 8 + (m_hamming->xbitl() % 8 == 1);
    }

    size_t Code3C::data::errSegSize() const
    {
        return m_hamming->pbitl() / 8 + (m_hamming->pbitl() % 8 == 1);
    }

    char8_t Code3C::header::operator[](size_t i) const
    {
        char bit(0);
        if (i < 2)
            bit = (desc>>(1-i));
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

    void Code3C::header::from_buffer(char8_t *buf, size_t len)
    {
        size_t i(0);
        for (size_t k(0); k < 2; k++, i++, desc <<= 1)
            desc |= buf[i] & 1;
        err |= buf[i++] & 1;
        for (size_t k(0); k < 3; k++, i++, huff <<= 1)
            huff |= buf[i] & 1;

        for (; i < len; i++, dlen <<= 1)
            dlen |= buf[i] & 1;
    }

    Code3C::Code3C(const char *utf8buf):
            Code3C(utf8buf, strlen(utf8buf))
    {
    }

    Code3C::Code3C(const char *utf8buf, size_t buflen):
            m_rawdata((char8_t *) strcpy(new char[buflen+1], utf8buf)),
            m_datalen(buflen),
            m_logo(nullptr),
            m_header({0,0,0,0,0}),
            m_drawer(nullptr),
            m_outfile(nullptr)
    {
    }

    Code3C::Code3C(const mat8_t &in_data):
            m_header({0,0,0,0,0}),
            m_data(new data(this, in_data)),
            m_logo(nullptr),
            m_outfile(nullptr)
    {
        size_t i, j, r, t, bit(0);
        size_t range[2] = {0, 0};
        auto set_range = [&range](size_t lindex, size_t hindex) -> void {
            range[0] = lindex; range[1] = hindex;
        };

        // Compute specials sections positions
        int qcal1 = 1*dimension().axis_t/4, // q1: rad and angle calibration begin
            qcal2 = 1*dimension().axis_t/2, // q2: end angle calibration
            qcal3 = 3*dimension().axis_t/4; // q3: rad calibration
        int tcal1 = 3*dimension().axis_t/8; // header position

        // Read header
        char8_t *headbuf = new char8_t[2*in_data.m()];
        for (i = 0, t = 0; t < 2; t++)
            for (r = 0; r < in_data.m(); r++, i++)
                headbuf[i] = in_data[t, r];
        m_header.from_buffer(headbuf, 2*in_data.m());

        // Set-up descriptor values
        m_desc = m_header.desc - 1;
        m_huffmodel = m_header.huff;
        m_errmodel = m_header.err;

        // Set-up dimension
        for (auto dim : model().dimensions)
            if (dim.axis_t != in_data.n() || dim.axis_r != in_data.m())
                m_dim++;
        if (m_dim > sizeof(model().dimensions) /
            sizeof(CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION))
        {
            throw std::runtime_error("Invalid matrix (unable to find dimension)");
        }

        for (i = 0; i < dimension().axis_t; i++)
        {
            if (i == 0 || i == qcal3)
            {
                // Setup Calibration (radius)
                set_range(0, 0);
            }
            else if (i <= qcal1 ||  (i >= qcal2 && i <= qcal3))
            {
                // Setup Calibration (angle)
                set_range(1, dimension().axis_r);
            }
            else if (i == tcal1 || i == tcal1 + 1)
            {
                set_range(0, 0);
            }
            else set_range(0, dimension().axis_r);

            // Read data
            for (j = range[0]; j < range[1]; j++, bit++)
            {
                // TODO
            }
        }

        // Free memory
        delete[] headbuf;
    }

    Code3C::Code3C(const Code3C &code3C):
            m_rawdata((char8_t *) strcpy(new char[code3C.m_datalen+1], (char*)
                                         code3C.m_rawdata)),
            m_datalen(code3C.m_datalen),
            m_header(code3C.m_header),
            m_logo(code3C.m_logo),
            m_drawer(nullptr)
    {
    }

    Code3C::~Code3C() noexcept
    {
        delete[] m_rawdata;
        delete m_data;
    }

    void Code3C::setModel(uint8_t model)
    {
        if (model <= CODE3C_MODEL_WB6C)
        {
            m_desc = model;
        }
    }

    void Code3C::setErrorModel(uint8_t model)
    {
        if (model <= CODE3C_ERRLVL_B)
        {
            m_errmodel = model;
        }
    }

    void Code3C::setHuffmanTable(uint8_t model)
    {
        if (model <= CODE3C_HUFFMAN_LATIN)
        {
            m_huffmodel = model;
        }
    }

    void Code3C::setLogo(const char *fname)
    {
        m_logo = fname;
    }

    bool Code3C::generate()
    {
        // Check dimensions
        const HuffmanTable* huffman = code3c_default_htf[m_huffmodel];
        const Hamming* hamming = model().hamming[m_errmodel];

        size_t buflen = huffman ? huffman->lengthOf<char8_t>(m_rawdata, m_datalen)
                                : m_datalen;
        size_t errlen = (hamming->dim_n()-hamming->dim_k())*(buflen*8/hamming->dim_k())/8;
        size_t total  = (errlen + buflen) * 8;

        for (const auto& dim : model().dimensions)
        {
            if (total > model().bitl * dim.capacity)
                m_dim++;
        }
        if (m_dim >= sizeof(model().dimensions) /
            sizeof(CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION))
            return false;

        // Set-up header
        m_header = {
                .desc = static_cast<uint8_t>(m_desc + 1),
                .err  = m_errmodel,
                .huff = m_huffmodel,
                .dlen = buflen,

                .meta_dlen_bitl = static_cast<uint32_t>(
                        2*(dimension().axis_r)-6),
                .meta_head_bitl = 6
        };
        m_header.meta_full_bitl = m_header.meta_head_bitl+m_header.meta_dlen_bitl;

        // Generate code3c data
        delete m_data;
        delete m_drawer;

        m_data = new data(this);
        m_drawer = new Code3CDrawer(this, *m_data);

        return m_data != nullptr;
    }

    // #### Display / Save #### //

    void Code3C::display() const
    {
        m_drawer->run();
    }

    Drawer* Code3C::drawer() const
    {
        return m_drawer;
    }

    void Code3C::set_output(const char *dest)
    {
        m_outfile = dest;
    }

    // #### Util functions #### //

    const char* Code3C::begin() const
    {
        return reinterpret_cast<const char *>(m_rawdata);
    }

    const char* Code3C::end() const
    {
        return reinterpret_cast<const char *>(&m_rawdata[m_datalen]);
    }

    size_t Code3C::rawSize() const
    {
        return m_datalen;
    }

    size_t Code3C::dataSegSize() const
    {
        return m_data->dataSegSize();
    }

    size_t Code3C::errSegSize() const
    {
        return m_data->errSegSize();
    }

    const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION& Code3C::dimension() const
    {
        return model().dimensions[m_dim];
    }

    const CODE3C_MODEL_DESC& Code3C::model() const
    {
        return code3c_models[m_desc];
    }

    __dlgt Code3CDrawer::save_ui()
    {
        savePNG(parent->m_outfile ? parent->m_outfile : "code3c.png");
    }

    Code3CDrawer::Code3CDrawer(const code3c::Code3C *parent, const Code3C::data &cData) :
            parent(parent), logo(nullptr), marker(nullptr),
            SimpleDrawer(
                    40 + 2 * parent->dimension().absRad * CODE3C_PIXEL_UNIT,
                    40 + 2 * parent->dimension().absRad * CODE3C_PIXEL_UNIT,
                    cData
            ), modelDimension(parent->dimension())
    {
        bindKey((DRAWER_KEY_CTRL | 's'),
                reinterpret_cast<delegate>(&Code3CDrawer::save_ui));
    }

    Code3CDrawer::~Code3CDrawer()
    {
        delete logo;
        delete marker;
    }

    unsigned long Code3CDrawer::bit_to_color(char _byte)
    {
        switch (parent->model().model_id)
        {
            case CODE3C_MODEL_WB:   // WB -- 1bit
            {
                return 0xffffff & ~((_byte & 0b1) * 0xffffff);
            }
            case CODE3C_MODEL_WB2C: // WB2C -- 2bits
            {
                int cyan = 0xffff * ((_byte >> 1) & 1);
                int red = 0xff * ((_byte >> 0) & 1);
                return ~rgb(red, cyan >> 8, cyan);
            }
            case CODE3C_MODEL_WB6C: // WB6C -- 3bits
            {
                int red = 0xff * ((_byte >> 2) & 1);
                int green = 0xff * ((_byte >> 1) & 1);
                int blue = 0xff * ((_byte >> 0) & 1);
                return ~rgb(red, green, blue);
            }
            default:
                throw std::runtime_error("Unsupported model");
        }
    }

    void Code3CDrawer::draw_angle(int t)
    {
        for (int r(m_data.m() - 1); r >= 0; r--)
        {
            char _byte(m_data[t, r]);

            int offRad(modelDimension.absRad - modelDimension.effRad
                       + modelDimension.deltaRad
            );
            int currentRad((offRad + (r * modelDimension.deltaRad))
                           * CODE3C_PIXEL_UNIT
            );

            foreground(bit_to_color(_byte));
            draw_slice(width() / 2, height() / 2, currentRad,
                       180 / modelDimension.rev,
                       t * 180 / (modelDimension.rev));
        }
    }

    void Code3CDrawer::setup()
    {
        // Setup window
        setTitle("Code3C Drawing Frame");

        // Load marker
        {
            PixelMap map = PixelMap::loadFromPNG(C3CRC("code3c-marker.png"));
            marker = new PixelMap(map.resize(width(), height()));
        }

        // Load logo
        {
            int logoDiameter = (modelDimension.absRad - modelDimension.effRad) *
                               CODE3C_PIXEL_UNIT * 2;
            PixelMap map = PixelMap::loadFromPNG(parent->m_logo
                                                 ? parent->m_logo
                                                 : parent->model().default_logo
            );
            logo = new PixelMap(map.resize(logoDiameter, logoDiameter));
        }
    }

    void Code3CDrawer::draw()
    {
        {
            // white background
            background(0xffffff);

            // Draw marker
            draw_pixelmap(*marker, 0, 0);

            // Draw colour calibration
            {
                int offRad(modelDimension.absRad - modelDimension.effRad
                           + modelDimension.deltaRad
                );
                int currentRad((offRad + (m_data.m() * modelDimension.deltaRad))
                               * CODE3C_PIXEL_UNIT
                );
                int tcal1 = 3 * modelDimension.axis_t / 8; // header position

                // Left part from the header
                for (int bit(0), t = tcal1 + 1;
                     bit < (1 << parent->model().bitl) / 2;
                     bit++, t++)
                {
                    foreground(bit_to_color(bit));
                    draw_slice(width() / 2, height() / 2, currentRad + 5,
                               180 / modelDimension.rev,
                               t * 180 / (modelDimension.rev));
                }
                // Right part from the header
                for (int bit(0b111 & parent->model().mask), i(0), t = tcal1;
                     i < (1 << parent->model().bitl) / 2;
                     bit--, t--, i++)
                {
                    foreground(bit_to_color(bit));
                    draw_slice(width() / 2, height() / 2, currentRad + 5,
                               180 / modelDimension.rev,
                               t * 180 / (modelDimension.rev));
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
            {
                draw_angle(t);
            }

            // Fill logo
            foreground(0xbe55ab);
            fill_circle(width() / 2, height() / 2, (modelDimension
                                                            .absRad -
                                                    modelDimension.effRad) *
                                                   CODE3C_PIXEL_UNIT
            );

            // Draw logo
            int logoDiameter = (modelDimension.absRad - modelDimension.effRad) *
                               CODE3C_PIXEL_UNIT * 2;
            int origX = (width() - logoDiameter) / 2;
            int origY = (height() - logoDiameter) / 2;

            int rlogo = logoDiameter / 2;
            for (int x = 0, xx = origX; x < logoDiameter; x++, xx++)
            {
                int rx = abs(rlogo - x);
                for (int y = 0, yy = origY; y < logoDiameter; y++, yy++)
                {
                    int ry = abs(rlogo - y);
                    if (((rx * rx) + (ry * ry)) < (rlogo * rlogo))
                        draw_pixel((*logo)[x, y].color, xx, yy);
                }
            }
        }
    }
}