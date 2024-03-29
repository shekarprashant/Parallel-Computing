// --*-c++-*--

#include <assert.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <algorithm>

#include "tiffio.h"
#include "image.h"

using namespace std;
 
Image::Image() 
{
   m_width = 0;
   m_height = 0;
}
 
Image::Image(const Image &other)
{
   m_width = other.m_width;
   m_height = other.m_height;
   m_image_data = other.m_image_data;  
}

Image& Image::operator=(const Image &rhs)
{
   m_width = rhs.m_width;
   m_height = rhs.m_height;
   m_image_data = rhs.m_image_data;  
   
   return *this; 
}

Image::Image(int width, int height, std::vector<uint32_t> &image_data)
{
   m_width = width;
   m_height = height;
   m_image_data = image_data;  
 
}

void Image::load_tiff(string input_filename)
{
  m_image_data.clear();

	/*m_width = 3;
	m_height = 3;

	for (int i = 0; i < m_width * m_height; i++)
		m_image_data.push_back(i);

	return;*/

  TIFF* tif = TIFFOpen(input_filename.c_str(), "r");
  if (tif) {
    uint32 w, h;
    size_t npixels;
    uint32* raster;
    
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    npixels = w * h;
    m_width = w;
    m_height = h;
    
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL) {
      if (TIFFReadRGBAImageOriented(tif, w, h, raster,
	 ORIENTATION_TOPLEFT, 0)) {
	for(size_t n=0;n<npixels;n++) m_image_data.push_back(raster[n]);
      }
      _TIFFfree(raster);
    }
    TIFFClose(tif);
  }
}

void Image::save_tiff_rgb(string output_filename) 
{
  TIFF *output_image;
  
  // Open the TIFF file
  if((output_image = TIFFOpen(output_filename.c_str(), "w")) == NULL){
    cerr << "Unable to write tif file: " << output_filename << endl;
  }
  
  // We need to set some values for basic tags before we can add any data
  TIFFSetField(output_image, TIFFTAG_IMAGEWIDTH, m_width);
  TIFFSetField(output_image, TIFFTAG_IMAGELENGTH, m_height);
  TIFFSetField(output_image, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(output_image, TIFFTAG_SAMPLESPERPIXEL, 4);
  TIFFSetField(output_image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  
  TIFFSetField(output_image, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
  TIFFSetField(output_image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  
  // Write the information to the file
  TIFFWriteEncodedStrip(output_image, 0, &m_image_data[0], m_width*m_height * 4);
  
  // Close the file
  TIFFClose(output_image);
}

void Image::save_tiff_grey_32bit(string output_filename) 
{
  TIFF *output_image;
  
  // Open the TIFF file
  if((output_image = TIFFOpen(output_filename.c_str(), "w")) == NULL){
    cerr << "Unable to write tif file: " << output_filename << endl;
  }
  
  // We need to set some values for basic tags before we can add any data
  TIFFSetField(output_image, TIFFTAG_IMAGEWIDTH, m_width);
  TIFFSetField(output_image, TIFFTAG_IMAGELENGTH, m_height);
  TIFFSetField(output_image, TIFFTAG_BITSPERSAMPLE, 32);
  TIFFSetField(output_image, TIFFTAG_SAMPLESPERPIXEL, 1);
  TIFFSetField(output_image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
 
  TIFFSetField(output_image, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
  TIFFSetField(output_image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
  
  // Write the information to the file
  TIFFWriteEncodedStrip(output_image, 0, &m_image_data[0], m_width*m_height * 4);
  
  // Close the file
  TIFFClose(output_image);
}

void Image::save_tiff_grey_8bit(string output_filename) 
{
  TIFF *output_image;
  
  // Open the TIFF file
  if((output_image = TIFFOpen(output_filename.c_str(), "w")) == NULL){
    cerr << "Unable to write tif file: " << output_filename << endl;
  }
  
  // We need to set some values for basic tags before we can add any data
  TIFFSetField(output_image, TIFFTAG_IMAGEWIDTH, m_width);
  TIFFSetField(output_image, TIFFTAG_IMAGELENGTH, m_height);
  TIFFSetField(output_image, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(output_image, TIFFTAG_SAMPLESPERPIXEL, 1);
  TIFFSetField(output_image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  
  TIFFSetField(output_image, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
  TIFFSetField(output_image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
  
  // convert data to 8bit
  vector<uint8_t> data;
  for(size_t n=0;n<m_image_data.size();n++) {
    data.push_back(255-(m_image_data[n]/(256*256*256)));
  }
  
  // Write the information to the file
  TIFFWriteEncodedStrip(output_image, 0, &data[0], m_width*m_height);
  
  // Close the file
  TIFFClose(output_image);
}

void Image::make_greyscale() 
{
  for(size_t n=0;n<m_image_data.size();n++) {
    
    double r = TIFFGetR(m_image_data[n]);
    double g = TIFFGetG(m_image_data[n]);
    double b = TIFFGetB(m_image_data[n]);
    
    // See http://en.wikipedia.org/wiki/Grayscale
    double grey = (0.3*r) + (0.59*g) + (0.11*b);
    m_image_data[n] = grey * 256 * 256 * 256;
  }
}


void Image::image_filter_mean(int windowSize)
{
   // make copy of the existing image -- with borders
   std::vector<uint32_t> tmp_data = m_image_data;

   int edgex = windowSize/2;
   int edgey = windowSize/2;

   for (int x = 0; x < m_width; x++) {
      for (int y = 0; y < m_height; y++) {
         double sum = 0;
         int count = 0;
         for (int fx = 0; fx < windowSize; fx++) {
           for (int fy = 0; fy < windowSize; fy++) {
             int yy = y+fy-edgey;
             int xx = x+fx-edgex;
             if (yy < 0 || yy >= m_height || xx < 0 || xx >= m_width)
               continue;
             sum += (double)P(tmp_data,yy,xx);
             count++;
           }
         }
         assert(count != 0);
         P(m_image_data,y,x) = (uint32_t)(sum/count);
      }
   }
}

void Image::image_filter_median(int windowSize)
{
   // make copy of the existing image
   std::vector<uint32_t> tmp_data = m_image_data;
   int edgex = windowSize/2;
   int edgey = windowSize/2;

   for (int x = 0; x < m_width; x++) {
      for (int y = 0; y < m_height; y++) {
         std::vector<uint32_t> colorArray;
         for (int fx = 0; fx < windowSize; fx++) {
           for (int fy = 0; fy < windowSize; fy++) {
             int yy = y+fy-edgey;
             int xx = x+fx-edgex;
             if (yy < 0 || yy >= m_height || xx < 0 || xx >= m_width)
               continue;
             colorArray.push_back ( P(tmp_data,y+fy-edgey,x+fx-edgex) );
           } 
         }
         sort (colorArray.begin(), colorArray.end());
         assert(colorArray.size() != 0);
         P(m_image_data,y,x) = colorArray[colorArray.size()/2];
      }
   }
}

void Image::get_part(std::vector<uint32_t> &vec, int startrow, int endrow, int startcol, int endcol)
{

   for (int r = startrow; r < endrow; r++)
   {
      for (int c = startcol; c < endcol; c++)
      {   
         vec.push_back(P(r, c));
      }
   }
}


void Image::image_filter_mean(int windowSize, int startx, int starty, int endrow, int endcol)
{
   // make copy of the existing image -- with borders
   std::vector<uint32_t> tmp_data = m_image_data;

   int edgex = windowSize/2;
   int edgey = windowSize/2;

   for (int x = startx; x < endrow; x++) {
      for (int y = starty; y < endcol; y++) {
         double sum = 0;
         int count = 0;
         for (int fx = 0; fx < windowSize; fx++) {
           for (int fy = 0; fy < windowSize; fy++) {
             int yy = y+fy-edgey;
             int xx = x+fx-edgex;
             if (yy < 0 || yy >= m_height || xx < 0 || xx >= m_width)
               continue;
             sum += (double)P(tmp_data,yy,xx);
             count++;
           }
         }
         assert(count != 0);
         P(m_image_data,y,x) = (uint32_t)(sum/count);
      }
   }
}


void Image::image_filter_median(int windowSize, int startx, int starty, int endrow, int endcol)
{
   // make copy of the existing image
   std::vector<uint32_t> tmp_data = m_image_data;
   int edgex = windowSize/2;
   int edgey = windowSize/2;

   for (int x = startx; x < m_width; x++) {
      for (int y = starty; y < m_height; y++) {
         std::vector<uint32_t> colorArray;
         for (int fx = 0; fx < windowSize; fx++) {
           for (int fy = 0; fy < windowSize; fy++) {
             int yy = y+fy-edgey;
             int xx = x+fx-edgex;
             if (yy < 0 || yy >= m_height || xx < 0 || xx >= m_width)
               continue;
             colorArray.push_back ( P(tmp_data,y+fy-edgey,x+fx-edgex) );
           } 
         }
         sort (colorArray.begin(), colorArray.end());
         assert(colorArray.size() != 0);
         P(m_image_data,y,x) = colorArray[colorArray.size()/2];
      }
   }
}


void Image::print_image()
{
	for (int r = 0; r < m_height; r++)
	{
		for (int c = 0; c < m_width; c++)
		{
			std::cout<<P(r, c) <<" ";
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
	std::cout<<std::endl;
}

void Image::resize(int width, int height)
{
	m_width = width;
	m_height = height;
	m_image_data.resize(m_width*m_height);
}

void Image::set_part(std::vector<uint32_t> &vec, int startrow, int startcol, int endrow, int endcol)
{

	size_t index = 0;
	m_width = m_width > endcol ? m_width : endcol;
   m_height = m_height > endrow ? m_height : endrow;


   for (int r = 0; r < endrow - startrow; r++)
   {
      for (int c = 0; c < endcol - startcol; c++)
      {  

			index = (endcol - startcol) * r + c;
         P(r + startrow, c + startcol) = vec[index];
      }
   }

}


void Image::compareImages(Image& rhs)
{
	if (m_width != rhs.m_width)
	{
		std::cout<<"Widths different"<<std::endl;
		return;
	}
	else if (m_height != rhs.m_height)
	{
		std::cout<<"Heights different"<<std::endl;
		return;
	}

	for (int r = 0; r < m_height; r++)
	{
		for (int c = 0; c < m_width; c++)
		{
			if (P(r,c) != rhs.P(r,c))
			{
				std::cout<<"Point:"<<r<<","<<c<<" different"<<std::endl;
				std::cout<<"LHS:"<<P(r,c)<<std::endl;
				std::cout<<"RHS:"<<rhs.P(r,c)<<std::endl;
				return;
			}

		}
	}
	std::cout<<"No differences!!"<<std::endl;
}
