#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string>
#include <sstream>
#include <assert.h>
#include <map>

struct ImageRLE
{
    size_t width;
    size_t height;
    std::map<uint32_t, uint32_t> pallete;
    std::vector<uint32_t> data;
};

void encode(std::ifstream& inFile, std::ofstream& outFile)
{
    std::string line;
    if (!getline(inFile, line))
        throw std::runtime_error("wrong input file format: missing header line");

    std::istringstream iss(line);
    size_t width, height;
    iss >> width >> height;

    std::vector<uint32_t> data;
    data.reserve(width*height);

    for (size_t i = 0; i < height; ++i)
    {
        if (!getline(inFile, line))
            throw std::runtime_error("wrong input file: missing line");

        iss.clear();
        iss.str(line);
        copy_n(std::istream_iterator<uint32_t>(iss), width, back_inserter(data));
    }
    iss.clear();
    if (data.size() != width*height)
    {
        throw std::runtime_error("wrong input file: not enough image data");
        assert(false);
    }

    ImageRLE rleData = { width, height };
    for (auto it = cbegin(data), end = cend(data); it != end;)
    {
        const auto color = *it;
        const auto rleEnd = std::find_if(it + 1, end, [color](uint32_t c) { return c != color;});

        uint32_t paletteColor;
        auto paletteIndex = rleData.pallete.lower_bound(color);
        if (paletteIndex == rleData.pallete.end())
        {
            paletteColor = rleData.pallete.size();
            if (paletteColor == 127)
            {
                assert(false);
                throw std::runtime_error("too many colors in input file");
            }
            rleData.pallete[color] = paletteColor;
        }
        else
        {
            paletteColor = paletteIndex->second;
        }

        const auto repeated = rleEnd - it;
        if (repeated < 2)
        {
            rleData.data.emplace_back(paletteColor);
        }
        else
        {
            rleData.data.emplace_back(paletteColor + 128);
            rleData.data.emplace_back(repeated - 1);
        }

        it = rleEnd;
    }

    outFile << std::to_string(rleData.pallete.size()) << std::endl;
    for (auto&& palette : rleData.pallete) outFile << palette.first << std::endl;
    outFile << width << ' ' << height << std::endl;
    copy(cbegin(rleData.data), cend(rleData.data), std::ostream_iterator<uint32_t>(outFile, " "));

}

void decode(std::ifstream& inFile, std::ofstream& outFile)
{
    ImageRLE rleData;

    size_t paletteSize;
    if (!(inFile >> paletteSize))
        throw std::runtime_error("wrong input file format: missing palette size");

    if (paletteSize > 127)
    {
        assert(false);
        throw std::runtime_error("wrong input file format: too big palette");
    }

    for (size_t i = 0; i < paletteSize; ++i)
    {
        uint32_t color;
        if (!(inFile >> color))
            throw std::runtime_error("wrong input file format: missing palette color");

        rleData.pallete[i] = color; // inverse
    }

    if (paletteSize != rleData.pallete.size())
    {
        assert(false);
        throw std::runtime_error("wrong input file format: missing palette color");
    }

    uint32_t width, height;
    if (!(inFile >> width >> height))
        throw std::runtime_error("wrong input file format: missing width and height");

    rleData.data.reserve(width * height);
    uint32_t rleColor, rleSize;
    while (inFile >> rleColor)
    {
        if (rleColor > 127)
        {
            if (inFile >> rleSize)
            {
                fill_n(back_inserter(rleData.data), rleSize+1, rleData.pallete[rleColor - 128]);
            }
            else
            {
                throw std::runtime_error("wrong input file format: missing rle data length");
            }
        }
        else
        {
            rleData.data.emplace_back(rleData.pallete[rleColor]);
        }
    }

    if (rleData.data.size() != width * height)
    {
        assert(false);
        throw std::runtime_error("wrong input file format: missing color data");
    }

    outFile << width << ' ' << height << std::endl;
    auto it = cbegin(rleData.data);
    for (uint32_t i = 0; i < height; ++i)
    {
        auto end = it + width;
        assert(end <= cend(rleData.data));

        copy(it, end, std::ostream_iterator<uint32_t>(outFile, " "));
        outFile << std::endl;
        it = end;
    }
}

int main(int argc, char* argv[])
{

    if (argc < 4)
    {
        std::cout << R"(Missing agruments:
			rle[.exe] <encode | decode> input output\n)";
        return -1;
    }

    std::ios::sync_with_stdio(false);
    std::ifstream inputFile(argv[2]);
    std::ofstream outputFile(argv[3], std::ios_base::out);

    if (inputFile.bad())
    {
        std::cout << "Cannot open input file " << argv[2] << std::endl;
        return -1;
    }

    if (outputFile.bad())
    {
        std::cout << "Cannot open output file " << argv[3] << std::endl;
        return -1;
    }

    try
    {

        if (0 == strcmp(argv[1], "encode"))
        {
            encode(inputFile, outputFile);
        }

        if (0 == strcmp(argv[1], "decode"))
        {
            decode(inputFile, outputFile);
        }
        return 0;
    }
    catch (const std::exception& error) {
        std::cout << "Error: " << error.what() << std::flush;
        return -1;
    }
}

