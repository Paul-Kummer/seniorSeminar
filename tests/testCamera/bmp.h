#ifndef BMP
#define BMP

#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <stdio.h>


class Pixel
{
    public:
        //enforce that colors are in a valid range
        int validateColorVal(int value)
        {
            if(value < 0){value = 0;};
            if(value > 255){value = value % 255;};
            return value;
        };
        //Default Constructor
        Pixel()   :red(0),     green(0),    blue(0){};
        //Parameter Constructor
        Pixel(  int rVal,    int gVal,  int bVal)
                :red(rVal),  green(gVal), blue(bVal){};
        //Overloaded Parameter Constructor
        Pixel(  int rVal,    int gVal,  int bVal,  int aVal)
                :red(rVal),  green(gVal), blue(bVal),alpha(aVal){};
		//Overloaded Paramter Constructor
		Pixel(int pixelAsInt)
		{
			size_t bytesPerInt = sizeof(int);
			for (int i = 0; i < bytesPerInt; i++)
			{
				size_t shift = 8 * (bytesPerInt - 1 - i);
				switch(i)
				{
					case 0: //0-7 bits are alpha
						this->alpha = (pixelAsInt >> shift) & 0xff;
					break;
					case 1: //8-15 bits are red
						this->red = (pixelAsInt >> shift) & 0xff;
					break;
					case 2: //16-23 bits are green
						this->green = (pixelAsInt >> shift) & 0xff;
					break;
					case 3: //24-31 bits are blue
						this->blue = (pixelAsInt >> shift) & 0xff;
					break;
				}
			}
		}
		Pixel operator=(Pixel otherPix){this->setRGB(otherPix.getRed(),otherPix.getGreen(), otherPix.getBlue()); return *this;};
        int getRed()                   				{return (int)this->red;};
        int getGreen()                 				{return (int)this->green;};
        int getBlue()                  				{return (int)this->blue;};
        int getAlpha()                 				{return (int)this->alpha;};
		void setRGB(int newR, int newG, int newB)	{red=validateColorVal(newR);green=validateColorVal(newG);blue=validateColorVal(newB);};
        void setRed(int newRed)        				{this->red=validateColorVal(newRed);};
        void setGreen(int newGreen)    				{this->green=validateColorVal(newGreen);};
        void setBlue(int newBlue)      				{this->blue=validateColorVal(newBlue);};
        void setAlpha(int newAlpha)    				{this->alpha=validateColorVal(newAlpha);};
        void setPixelSize(int newSize)  			{this->pixelSize=newSize;};

    private:
        //values range from 0-255. 0 is absence of color 255 is saturated with color
        //char is used because they are all limited to 8bits, which is the max value of a color
        int alpha = 255;
        int red;
        int green;
        int blue;
        int pixelSize = 3;
};

//This typedef must come after Pixel is defined
typedef std::vector<std::vector<Pixel> > imageArr;




class BitMap 
{

    private:
        unsigned char m_bmpFileHeader[14];
        unsigned int m_pixelArrayOffset;
        unsigned char m_bmpInfoHeader[40];

        int m_height;
        int m_width;
        int m_bitsPerPixel;

        int m_rowSize;
        int m_pixelArraySize;

        unsigned char* m_pixelData;

        char * m_copyname;
        const char * m_filename;
    public:
        BitMap(const char * filename);
        ~BitMap();

        std::vector<unsigned int> getPixel(int i,int j);

        void makeCopy(char * filename);
        void writePixel(int i,int j, int R, int G, int B);

        void swapPixel(int i, int j, int i2, int j2);

        void dispPixelData();

        int width() {return m_width;}
        int height() {return m_height;}

        int vd(int i, int j);
        int hd(int i, int j);

        bool isSorted();
};

BitMap::BitMap( const char * filename) {

    using namespace std;

    m_filename = filename;

    
    ifstream inf(filename);
    if(!inf) {
        cerr<<"Unable to open file: "<<filename<<"\n";
    }


    //unsigned char m_bmpFileHeader[14];
    unsigned char a;
    for(int i =0;i<14;i++) {
        inf>>hex>>a;
        m_bmpFileHeader[i] = a;
        m_bmpFileHeader[i] = filename[i];
    }
    if(m_bmpFileHeader[0]!='B' || m_bmpFileHeader[1]!='M') {
        cerr<<"Your info header might be different!\nIt should start with 'BM'.\n";
    }

    /*
        THE FOLLOWING LINE ONLY WORKS IF THE OFFSET IS 1 BYTE!!!!! (it can be 4 bytes max)
        That should be fixed now. 
        old line was
        m_pixelArrayOffset = m_bmpFileHeader[10];
    */
    unsigned int * array_offset_ptr = (unsigned int *)(m_bmpFileHeader + 10);
    m_pixelArrayOffset = *array_offset_ptr;


    if( m_bmpFileHeader[11] != 0 || m_bmpFileHeader[12] !=0 || m_bmpFileHeader[13] !=0 ) {
        std::cerr<< "You probably need to fix something. bmp.h("<<__LINE__<<")\n";
    }



    //unsigned char m_bmpInfoHeader[40];
    for(int i=0;i<40;i++) {
        inf>>hex>>a;
        m_bmpInfoHeader[i]=a;
        m_bmpInfoHeader[i] = filename[i];
    }

    int * width_ptr = (int*)(m_bmpInfoHeader+4);
    int * height_ptr = (int*)(m_bmpInfoHeader+8);

    m_width = *width_ptr;
    m_height = *height_ptr;

    printf("W: %i, H: %i", m_width, m_height);

    m_bitsPerPixel = m_bmpInfoHeader[14];
    if(m_bitsPerPixel!=24) {
        cerr<<"This program is for 24bpp files. Your bmp is not that\n";
    }
    int compressionMethod = m_bmpInfoHeader[16];
    if(compressionMethod!=0) {
        cerr<<"There's some compression stuff going on that we might not be able to deal with.\n";
        cerr<<"Comment out offending lines to continue anyways. bpm.h line: "<<__LINE__<<"\n";
    }


    m_rowSize = int( floor( (m_bitsPerPixel*m_width + 31.)/32 ) ) *4;
    m_pixelArraySize = m_rowSize* abs(m_height);

    m_pixelData = new unsigned char [m_pixelArraySize];

    inf.seekg(m_pixelArrayOffset,ios::beg);
    for(int i=0;i<m_pixelArraySize;i++) {
        inf>>hex>>a;
        m_pixelData[i]=a; 
    }



}

BitMap::~BitMap() {
    delete[] m_pixelData;
}

void BitMap::dispPixelData() {
    for(int i=0;i<m_pixelArraySize;i++) {
        std::cout<<(unsigned int)m_pixelData[i]<<" ";   
    }
    std::cout<<"\n";
}

// output is in rgb order.
std::vector<unsigned int> BitMap::getPixel(int x, int y) {
    if(x<m_width && y<m_height) {
        std::vector<unsigned int> v;
        v.push_back(0);
        v.push_back(0);
        v.push_back(0);

        y = m_height -1- y; //to flip things
        //std::cout<<"y: "<<y<<" x: "<<x<<"\n";
        v[0] = (unsigned int) ( m_pixelData[ m_rowSize*y+3*x+2 ] ); //red
        v[1] = (unsigned int) ( m_pixelData[ m_rowSize*y+3*x+1 ] ); //greed
        v[2] = (unsigned int) ( m_pixelData[ m_rowSize*y+3*x+0 ] ); //blue


        return v;
    }
    else {throw std::out_of_range("!!!Bad Index!!!");}
}

void BitMap::makeCopy(char * filename) {
    std::ofstream copyfile(filename);
    std::ifstream infile(m_filename);
    m_copyname = filename;

    unsigned char c;
    while(infile) {
        infile>>c;
        copyfile<<c;
    }
}

// changes the file
void BitMap::writePixel(int x,int y, int R, int G, int B) {
    std::fstream file(m_filename);
    y = m_height -1- y; // to flip things.
    int blueOffset = m_pixelArrayOffset+m_rowSize*y+3*x+0;

    // writes to the file
    file.seekg(blueOffset,std::ios::beg);
    file<< (unsigned char)B;
    file.seekg(blueOffset+1,std::ios::beg);
    file<< (unsigned char)G;
    file.seekg(blueOffset+2,std::ios::beg);
    file<< (unsigned char)R;

    // edits data in pixelData array 
    m_pixelData[m_rowSize*y+3*x+2] = (unsigned char)R;
    m_pixelData[m_rowSize*y+3*x+1] = (unsigned char)G;
    m_pixelData[m_rowSize*y+3*x+0] = (unsigned char)B;
}

// changes the file
void BitMap::swapPixel(int i, int j, int i2, int j2) {
    std::vector<unsigned int> p1 = (*this).getPixel(i,j);

    std::vector<unsigned int> p2 = (*this).getPixel(i2,j2);

    (*this).writePixel(i,j,p2[0],p2[1],p2[2]);
    (*this).writePixel(i2,j2,p1[0],p1[1],p1[2]);

}

#endif 