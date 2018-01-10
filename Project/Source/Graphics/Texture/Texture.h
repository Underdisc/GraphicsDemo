/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Texture.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 2017/10/11
\brief
  Contains the interface for the Texture class.
*/
/*****************************************************************************/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "../Color.h"

#define CHANNELS_RGB  3
#define CHANNELS_RGBA 4

// pre-declarations
class TexturePool;

/*****************************************************************************/
/*!
\class Texture
\brief
  Used for loading a texture and storing the texture in an OpenGL texture
  buffer.
*/
/*****************************************************************************/
class Texture
{
public:
  Texture(const std::string & filename, bool flip_image_vertically = false);
  ~Texture();
  void CreateNormalMap(const std::string & out_filename, float strength);
  unsigned char RedAt(int i, int j);
  const unsigned char * ImageData();
  int Width();
  int Height();
  int DataLength();
  int Channels();
  static void Write(const std::string & filename, const void * image_data, 
    int width, int height, int channels);
private:
  //! The images color data.
  unsigned char * _imageData;
  //! The width of the texture.
  int _width;
  //! The height of the texture.
  int _height;
  //! The total size of the image data in bytes.
  int _dataLength;
  //! The channels that are used by the image.
  int _channels;
  //! The name of the file that the image was loaded from.
  std::string _imageFile;
  // friends
  friend TexturePool;
};

#endif // !TEXTURE_H