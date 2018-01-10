/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Texture.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 2017/10/11
\brief
  Contains the implementation of the Texture class.
*/
/*****************************************************************************/

// using stb's image functions
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <STB\stb_image.h>
#include <STB\stb_image_write.h>
#include "../../Math/Reals.h"
#include "../../Math/Vector3.h"
#include "../../Utility/Error.h"
#include "Texture.h"

/*****************************************************************************/
/*!
\brief
  Constructor for a Texture.

\param filename
  The name of the file being loaded.
*/
/*****************************************************************************/
Texture::Texture(const std::string & filename, bool flip_image_vertically) :
  _imageFile(filename)
{
  // flip image
  stbi_set_flip_vertically_on_load(flip_image_vertically);
  // loading image
  _imageData = stbi_load(filename.c_str(), &_width, 
    &_height, &_channels, 0);
  if (!_imageData) {
    Error error("Texture.cpp", "Texture Constructor");
    error.Add("Image file failed to load.");
    error.Add("> Image File");
    error.Add(filename.c_str());
    throw(error);
  }
  _dataLength = _width * _height * _channels;
}

/*****************************************************************************/
/*!
\brief Simply frees the image data allocated by stb.
*/
/*****************************************************************************/
Texture::~Texture()
{
  stbi_image_free(_imageData);
}

void Texture::CreateNormalMap(const std::string & out_filename, float strength)
{
  // buffer with normal map image data
  unsigned char * normal_map_data = new unsigned char[_dataLength];
  for (int i = 0; i < _width; ++i) {
    for (int j = 0; j < _height; ++j) {
      // find heights on the bump map
      unsigned char left_height = RedAt(i - 1, j);
      unsigned char right_height = RedAt(i + 1, j);
      unsigned char bottom_height = RedAt(i, j - 1);
      unsigned char top_height = RedAt(i, j + 1);
      // find the change in heights
      float sdz = strength * (float)(right_height - left_height);
      float tdz = strength * (float)(bottom_height - top_height);
      Math::Vector3 s(1.0f, 0.0f, sdz);
      Math::Vector3 t(0.0f, 1.0f, tdz);
      // find the normal and translate the normal into rgb
      Math::Vector3 normal = Math::Cross(s, t).Normalized();
      unsigned char r = (unsigned char)(255.0f * 0.5f * (normal.x + 1.0f));
      unsigned char g = (unsigned char)(255.0f * 0.5f * (normal.y + 1.0f));
      unsigned char b = (unsigned char)(255.0f * 0.5f * (normal.z + 1.0f));
      // offset into new image array
      unsigned offset = (j * _width + i) * CHANNELS_RGB;
      // save the calculated rgb values
      normal_map_data[offset + 0] = r;
      normal_map_data[offset + 1] = g;
      normal_map_data[offset + 2] = b;
    }
  }
  // save the normal_map to file
  Texture::Write(out_filename, normal_map_data,
    _width, _height, CHANNELS_RGB);
  delete [] normal_map_data;
}

unsigned char Texture::RedAt(int i, int j)
{
  // clamp i and j to edges
  i = Math::Clamp(i, 0, _width -  1);
  j = Math::Clamp(j, 0, _height - 1);
  // get red at location
  unsigned offset = (j * _width + i) * _channels;
  return _imageData[offset];
}


const unsigned char * Texture::ImageData()
{
  return _imageData;
}

int Texture::Width()
{
  return _width;
}

int Texture::Height()
{
  return _height;
}

int Texture::DataLength()
{
  return _dataLength;
}

int Texture::Channels()
{
  return _channels;
}

// saves to png by default for now
void Texture::Write(const std::string & filename, const void * image_data, 
  int width, int height, int channels)
{
  // write image to file
  // allow for specification of filetype
  int result = stbi_write_png(filename.c_str(), width, height, channels, 
    image_data, width * sizeof(unsigned char) * channels);
  if(result == 0)
    return;
}