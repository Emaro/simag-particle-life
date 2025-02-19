#include "HelperReadPLY.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <filesystem>

#include <tinyply.h>
#include <glm/vec3.hpp>

#include "../ParticleSystem/ParticleSystem.h"
#include "../Context/GlobalEnvironment.h"

using namespace tinyply;

#define IS_SIMAG_LECTURE

namespace Helper
{

    void readPLY(std::string const& filepath, ParticleSystem* ps)
    {
        const bool preload_into_memory = false; // gk disabled

        //std::cout << "........................................................................\n";
        //if (gEnv->logDebug)
        std::cout << "Now Reading: " << filepath << std::endl;

        std::unique_ptr<std::istream> file_stream;
        std::vector<uint8_t> byte_buffer;

        try
        {
            // For most files < 1gb, pre-loading the entire file upfront and wrapping it into a 
            // stream is a net win for parsing speed, about 40% faster. 
            if (preload_into_memory)
            {
                assert(false && "preload not implemented atm");
                //byte_buffer = read_file_binary(filepath);
                //file_stream.reset(new memory_stream((char*)byte_buffer.data(), byte_buffer.size()));
            }
            else
            {
                file_stream.reset(new std::ifstream(filepath, std::ios::binary));
            }

            if (!file_stream || file_stream->fail()) throw std::runtime_error("file_stream failed to open " + filepath);

            file_stream->seekg(0, std::ios::end);
            const float size_mb = file_stream->tellg() * float(1e-6);
            file_stream->seekg(0, std::ios::beg);

            PlyFile file;
            file.parse_header(*file_stream);

            if (gEnv->logDebug)
            {
                std::cout << "\t[ply_header] Type: " << (file.is_binary_file() ? "binary" : "ascii") << std::endl;
                for (const auto& c : file.get_comments()) std::cout << "\t[ply_header] Comment: " << c << std::endl;
                for (const auto& c : file.get_info()) std::cout << "\t[ply_header] Info: " << c << std::endl;

                for (const auto& e : file.get_elements())
                {
                    std::cout << "\t[ply_header] element: " << e.name << " (" << e.size << ")" << std::endl;
                    for (const auto& p : e.properties)
                    {
                        std::cout << "\t[ply_header] \tproperty: " << p.name << " (type=" << tinyply::PropertyTable[p.propertyType].str << ")";
                        if (p.isList) std::cout << " (list_type=" << tinyply::PropertyTable[p.listType].str << ")";
                        std::cout << std::endl;
                    }
                }
            }

            // Because most people have their own mesh types, tinyply treats parsed data as structured/typed byte buffers. 
            // See examples below on how to marry your own application-specific data structures with this one. 
            std::shared_ptr<PlyData> vertices, normals, colors, texcoords, faces, tripstrip;

            // The header information can be used to programmatically extract properties on elements
            // known to exist in the header prior to reading the data. For brevity of this sample, properties 
            // like vertex position are hard-coded: 
            try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
            catch (const std::exception& e) { if (gEnv->logDebug) std::cerr << "tinyply exception: " << e.what() << std::endl; }

            try { normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
            catch (const std::exception& e) { if (gEnv->logDebug) std::cerr << "tinyply exception: " << e.what() << std::endl; }

            // GK read fix 
            bool colorRGB_isChar = false;
            bool colorRGB_hasAlpha = false;
            try {
                colors = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" });
                if (colors->t == tinyply::Type::FLOAT32) {}
                else { colorRGB_isChar = true; }
                colorRGB_hasAlpha = true;
                if (gEnv->logDebug)
                {
                    std::cout << "Color size: " << colors << "\n";
                }
            }
            catch (const std::exception& e) { if (gEnv->logDebug) std::cerr << "tinyply exception: " << e.what() << std::endl; }

            try {
                colors = file.request_properties_from_element("vertex", { "r", "g", "b", "a" });
                if (colors->t == tinyply::Type::FLOAT32) {}
                else { colorRGB_isChar = true; }
                colorRGB_hasAlpha = true;
                if (gEnv->logDebug)
                {
                    std::cout << "Read RGB - Color size: " << colors << "\n";
                }
            }
            catch (const std::exception& e) { if (gEnv->logDebug) std::cerr << "tinyply exception: " << e.what() << std::endl; }

            // GK
            try {
                colors = file.request_properties_from_element("vertex", { "red", "green", "blue" });
                if (colors->t == tinyply::Type::FLOAT32) {}
                else { colorRGB_isChar = true; }
                if (gEnv->logDebug)
                {
                    std::cout << "Read RGB - Color size: " << colors << "\n";
                }
            }
            catch (const std::exception& e) { if (gEnv->logDebug) std::cerr << "tinyply exception: " << e.what() << std::endl; }


            try { texcoords = file.request_properties_from_element("vertex", { "u", "v" }); }
            catch (const std::exception& e) { if (gEnv->logDebug) std::cerr << "tinyply exception: " << e.what() << std::endl; }

            // Providing a list size hint (the last argument) is a 2x performance improvement. If you have 
            // arbitrary ply files, it is best to leave this 0. 
            try { faces = file.request_properties_from_element("face", { "vertex_indices" }, 3); }
            catch (const std::exception& e) { if (gEnv->logDebug) std::cerr << "tinyply exception: " << e.what() << std::endl; }

            // Tristrips must always be read with a 0 list size hint (unless you know exactly how many elements
            // are specifically in the file, which is unlikely); 
            try { tripstrip = file.request_properties_from_element("tristrips", { "vertex_indices" }, 0); }
            catch (const std::exception& e) { if (gEnv->logDebug) std::cerr << "tinyply exception: " << e.what() << std::endl; }

            // manual_timer read_timer; // gk

            // read_timer.start(); // gk
            file.read(*file_stream);
            // read_timer.stop(); // gk

            // const float parsing_time = static_cast<float>(read_timer.get()) / 1000.f; // gk
            const float parsing_time = 1.0f; // gk
            if (gEnv->logDebug)
            {

                std::cout << "\tparsing " << size_mb << "mb in " << parsing_time << " seconds [" << (size_mb / parsing_time) << " MBps]" << std::endl;

                if (vertices)   std::cout << "\tRead " << vertices->count << " total vertices " << std::endl;
                if (normals)    std::cout << "\tRead " << normals->count << " total vertex normals " << std::endl;
                if (colors)     std::cout << "\tRead " << colors->count << " total vertex colors " << std::endl;
                if (texcoords)  std::cout << "\tRead " << texcoords->count << " total vertex texcoords " << std::endl;
                if (faces)      std::cout << "\tRead " << faces->count << " total faces (triangles) " << std::endl;
                if (tripstrip)  std::cout << "\tRead " << (tripstrip->buffer.size_bytes() / tinyply::PropertyTable[tripstrip->t].stride) << " total indicies (tristrip) " << std::endl;
            }

            // Example One: converting to your own application types
            if (vertices.get())
            {
                assert(vertices->t == tinyply::Type::FLOAT32);

                //const size_t numVerticesBytes = vertices->buffer.size_bytes();
                //std::vector< glm::vec<3, float> > verts(vertices->count);
                //std::memcpy(verts.data(), vertices->buffer.get(), numVerticesBytes);

                if (colors.get())
                {
                    if (colorRGB_hasAlpha && colorRGB_isChar)
                    {
                        // make rgb / skip alpha
                        for (int i = 0; i < (int)colors->count; i++)
                        {
                            colors->buffer.get()[i * 3 + 0] = colors->buffer.get()[i * 4 + 0];
                            colors->buffer.get()[i * 3 + 1] = colors->buffer.get()[i * 4 + 1];
                            colors->buffer.get()[i * 3 + 2] = colors->buffer.get()[i * 4 + 2];
                        }
                    }
                    ps->set(vertices->count, vertices->buffer.get(), colors->count > 0 ? colors->buffer.get() : nullptr, colorRGB_isChar);
                }
                else
                {
                    ps->set(vertices->count, vertices->buffer.get(), nullptr);
                }
            }

#ifdef IS_SIMAG_LECTURE

            if (normals.get() && (normals->count == vertices->count))
            {
                ps->setVelocities(normals->count, normals->buffer.get());
            }
            else
            {
                ps->setAllVelocitiesToZero();
            }

#else
            if (normals.get() && (normals->count == vertices->count))
            {
                ps->setNormals(normals->count, normals->buffer.get());
            }
#endif



            // Example Two: converting to your own application type
            /*
            {
                std::vector< glm::vec<3, float> > verts_floats;
                std::vector< glm::vec<3, double> > verts_doubles;
                if (vertices->t == tinyply::Type::FLOAT32) { } // as floats ...
                if (vertices->t == tinyply::Type::FLOAT64) { } // as doubles ...
            }
            */
        }
        catch (const std::exception& e)
        {
            std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
        }

    }

#ifdef IS_SIMAG_LECTURE
    void writePLY(std::string const& filepath, ParticleSystem const* ps, std::vector<glm::vec3> const* normalsToSave)
#else
    void writePLY(std::string const& filepath, ParticleSystem const* ps)
#endif
    {
        std::cout << "Now Writing: " << filepath << std::endl;

        std::filebuf fb_binary;
        fb_binary.open(filepath, std::ios::out | std::ios::binary);
        std::ostream outstream_binary(&fb_binary);
        if (outstream_binary.fail()) throw std::runtime_error("failed to open " + filepath);

        PlyFile filePly;

        filePly.add_properties_to_element("vertex", { "x", "y", "z" },
            Type::FLOAT32, ps->positions().size(), reinterpret_cast<uint8_t const*>(&ps->positions().front()), Type::INVALID, 0);

#ifdef IS_SIMAG_LECTURE
        if (normalsToSave && normalsToSave->size() == ps->size())
        {
            filePly.add_properties_to_element("vertex", { "nx", "ny", "nz" },
                Type::FLOAT32, normalsToSave->size(), reinterpret_cast<uint8_t const*>(&normalsToSave->front()), Type::INVALID, 0);
        }

        // GK
        // Create color3
        std::vector<glm::vec3> col;
        {
            //col.resize(ps->colors().size());
            for (int i = 0; i < (int)ps->colors().size(); i++)
            {
                //                col[i][0] = ps->colors()[i][0];
                //                col[i][1] = ps->colors()[i][1];
                //                col[i][2] = ps->colors()[i][2];
                col.emplace_back(ps->colors()[i]);
            }

            filePly.add_properties_to_element("vertex", { "red", "green", "blue" },
                Type::FLOAT32, col.size(), reinterpret_cast<uint8_t const*>(&col.front()), Type::INVALID, 0);
        }
#else
        if (ps->hasNormals())
        {
            filePly.add_properties_to_element("vertex", { "nx", "ny", "nz" },
                Type::FLOAT32, ps->normals().size(), reinterpret_cast<uint8_t const*>(&ps->normals().front()), Type::INVALID, 0);
        }


        filePly.add_properties_to_element("vertex", { "red", "green", "blue" },
            Type::FLOAT32, ps->colors().size(), reinterpret_cast<uint8_t const*>(ps->colors().data()), Type::INVALID, 0);
#endif

        filePly.get_comments().emplace_back("particle system");

        // Write a binary file
        filePly.write(outstream_binary, true);
    }


#ifdef IS_SIMAG_LECTURE

    std::string createFilenameById(std::string const& path, int id, std::string const& ending)
    {
        assert(id < 10000);

        std::stringstream ss;
        if (id < 10) { ss << path << "000" << id << ending; }
        else if (id < 100) { ss << path << "00" << id << ending; }
        else if (id < 1000) { ss << path << "0" << id << ending; }
        else if (id < 10000) { ss << path << "" << id << ending; }
        return ss.str();
    }

    std::string addExtensionToPath(std::string const& path, std::string const& extension)
    {
        if (!std::filesystem::path(path).has_extension())
        {
            std::stringstream ss;
            ss << path << extension;
            return ss.str();
        }

        return path;
    }

    void WriteToFilestream::write(ParticleSystem& ps)
    {
        std::string filename = createFilenameById(cFilenameStream, id);
        ps.save(filename);
        std::cout << "Writing file: " << filename << "\n";
        id++;
    }

    bool ReadFromFilestream::exist(const std::string& name)
    {
        std::ifstream f(name.c_str());
        return f.good();
    }

    void ReadFromFilestream::read(ParticleSystem& ps)
    {
        id = std::max(id, idMin);
        if (id > idMax) id = idMin;

        std::string filename = createFilenameById(cFilenameStream, id);
        std::string filenamePly = addExtensionToPath(filename, ".ply");
        std::string filenameXML = addExtensionToPath(filename, ".xml");

        if (!exist(filenamePly) || !exist(filenameXML))
        {
            reset();
            std::string filename = createFilenameById(cFilenameStream, id);
            std::string filenamePly = addExtensionToPath(filename, ".ply");
            std::string filenameXML = addExtensionToPath(filename, ".xml");
            if (!exist(filenamePly) || !exist(filenameXML)) return;
        }

        ps.load(filename);
        std::cout << "Reading file: " << filename << "\n";
        id++;
    }

    int ReadFromFilestream::getLastFileIdx()
    {
        int idx = 0;
        std::string filename = createFilenameById(cFilenameStream, idx);

        while (exist(filename))
        {
            idx++;
            filename = createFilenameById(cFilenameStream, idx);
        }
        return idx;
    }


#endif

}