#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

class ParticleSystem;

namespace Helper
{
  // ----------------------------------------------------------------------------

  // Constants
  const std::string cFilenameCapture = "capture"; // single file capture
  const std::string cFilenameStream = "data/capture"; // stream
  const std::string cFilenameList = "file"; // => file00, file01, ...

  // ----------------------------------------------------------------------------

  void readPLY(std::string const& filepath, ParticleSystem* ps);
  void writePLY(std::string const& filepath, ParticleSystem const* ps, std::vector<glm::vec3> const* normalsToSave=nullptr);

  // Create filename with number attached, e.g., test0000.ply, test0001.ply, ...
  std::string createFilenameById(std::string const& path, int id, std::string const& ending = ".ply");
  std::string addExtensionToPath(std::string const& path, std::string const& extension);

  class WriteToFilestream
  {
  private:
      int id;
  public:
      WriteToFilestream() { reset(); }
      void reset() { id = 0; }
      void write(ParticleSystem& ps);
  };

  class ReadFromFilestream
  {
  private:
      int id;

      int idMin;
      int idMax;

      bool exist(const std::string& name);

  public:
      ReadFromFilestream() : idMin(0), idMax(999999) { reset(); }
      void reset() { id = idMin; }
      void read(ParticleSystem& ps);

      int getLastFileIdx(); // return latest index of numbered files, 0 if none is there
      void setMinIdx(int idx) { idMin = idx; }
      void setMaxIdx(int idx) { idMax = idx; }
  };

} // namespace Helper