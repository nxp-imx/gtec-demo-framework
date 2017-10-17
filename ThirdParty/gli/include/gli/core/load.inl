#include "../load_dds.hpp"
#include "../load_kmg.hpp"
#include "../load_ktx.hpp"
#include "file.hpp"

namespace gli
{
	/// Load a texture (DDS, KTX or KMG) from memory
	inline texture load(char const * Data, std::size_t Size)
	{
		{
			texture Texture = load_dds(Data, Size);
			if(!Texture.empty())
				return Texture;
		}
		{
			texture Texture = load_kmg(Data, Size);
			if(!Texture.empty())
				return Texture;
		}
		{
			texture Texture = load_ktx(Data, Size);
			if(!Texture.empty())
				return Texture;
		}

		return texture();
	}

#ifdef _WIN32
#pragma warning( push )
  // Disable the warning about unsafe method under windows 
#pragma warning( disable : 4996)
#endif

	/// Load a texture (DDS, KTX or KMG) from file
	inline texture load(char const * Filename)
	{
		FILE* File = detail::open_file(Filename, "rb");
		if(!File)
			return texture();

		long Beg = std::ftell(File);
		std::fseek(File, 0, SEEK_END);
		long End = std::ftell(File);
		std::fseek(File, 0, SEEK_SET);

		std::vector<char> Data(static_cast<std::size_t>(End - Beg));

    const auto bytesRead = std::fread(&Data[0], 1, Data.size(), File);
		std::fclose(File);

    if (bytesRead != Data.size())
      Data.resize(bytesRead);

		return load(&Data[0], Data.size());
	}

#ifdef _WIN32
#pragma warning( pop )
#endif

	/// Load a texture (DDS, KTX or KMG) from file
	inline texture load(std::string const & Filename)
	{
		return load(Filename.c_str());
	}
}//namespace gli
