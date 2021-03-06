#include "../prototypes.h"


//for purpose of line drawing, the bottom left of the image is (0,0)
//draw x and y are both needed for cases of vertical and horizxontal lines
void makeLine(imageArr& pixArr, float yIntercept, float slope, Pixel lineColor, int lineSize)
{
	int yIndex, y, yLineSize, x;
	for(int xIndex = 0; xIndex < WIDTH; xIndex++)
	{
		yIndex = HEIGHT-y;
		yLineSize = lineSize + (         (        (abs(slope) /(float)1000)    * HEIGHT     )          ); //Adjust the marker line for near vertical lines
		y = (int)((slope * (float)xIndex) + yIntercept);
		
		if(yIndex > 0 && yIndex < HEIGHT); //Make sure the point is on the image
		{
			//span the lineSize on the x axis of both sides of the point
			for(int aroundX = xIndex-lineSize; aroundX < xIndex+lineSize; aroundX++) 
			{
				//span the lineSize on the y axis of both sides of the point
				for(int aroundY = yIndex-yLineSize; aroundY < yIndex+yLineSize; aroundY++) 
				{		
					//ensure the pixels to change are on the image plane	
					if(aroundX > 0 && aroundX < WIDTH && aroundY > 0 && aroundY < HEIGHT) 
					{
						pixArr[aroundY][aroundX] = lineColor; //change the color of pixels on the path of the line
					}
				}
			}
		}
	}
}



//Method: Line of Best Fit (Least Square Method)
float getAngle (imageArr& pixArr, bool drawLine, bool showStats)
{
	float slope, angle, yIntercept, xIntercept;
	float ptCount;
	float xSum, xAvg, xSqrSum;
	float ySum, yAvg, ySqrSum;
	float xMinusXAvgTimesYMinusYAvgSum, xMinusXAvgSqrSum;
	float yMinusYAvgSqrSum;
	std::vector<std::pair<int,int>> pointArray;

	//statistical analysis
	float yActual, xActual;
	float yPredicted, xPredicted;
	float residual, residualSquared;
	float yMinusYBar, yMinusYBarSquared;
	float xRes, xResSqr;
	float xMinXBar, xMinXBarSqr;
	float rSquared, xRSqr;
	float goodnessOfFit, xGOF;

	for(float y = 0; y < HEIGHT; y++) //HEIGHT
	{
		for(float x = 0; x < WIDTH; x++) //WIDTH
		{
			if(pixArr[y][x].getRed()==0) //all three colors will be 0 for black, so only one needs to be checked
			{
				int xPos = x, yPos = (HEIGHT - y); // y position has to be adjusted for top down approach
				pointArray.push_back(std::make_pair(xPos,yPos));
				xSum += x;
				ySum += yPos;
				xSqrSum += pow(x,2);
				ySqrSum += pow(yPos, 2);
				ptCount++;
			}
		}
	}

	//prevent divide by zero, and set to resonable value if ptCount is zero
	xAvg = ptCount>0?xSum/ptCount:0;
	yAvg = ptCount>0?ySum/ptCount:1;

	//determine the line of best fit for the points
	for(int i = 0; i < pointArray.size(); i++)
	{
		//y=mx+b for horizontal to 45, x=ny+p for vertical
		float x = (float)pointArray[i].first, y = (float)pointArray[i].second;
		xMinusXAvgTimesYMinusYAvgSum += ((x - xAvg)*(y - yAvg));
		xMinusXAvgSqrSum += pow((x - xAvg), 2);
		yMinusYAvgSqrSum += pow((y - yAvg), 2);
	}

	//multiply by -1 because the image is read backwards
	//angle = -1*((atan(slope) * 180) / 3.14159265); not accurate when line is close to vertical
	angle = atan2(yMinusYAvgSqrSum, xMinusXAvgSqrSum) * 180 / 3.14159265;

	//Slope determined using Least Square Method
	//if the denominator is zero, the line is vertical and a slope of infinity
	slope = xMinusXAvgSqrSum!=0?(xMinusXAvgTimesYMinusYAvgSum / xMinusXAvgSqrSum):100000000;
	yIntercept = yAvg - (slope * xAvg);
	xIntercept = -yIntercept / -slope;

	if(showStats)
	{
		//[(-inf : -1), (1 : +inf)] slopes over 1 and under -1 should use the x intercept form, vertical lines
		//(-1 : 1) slopes under 1 and over -1 should use the y intercept form, horizontal lines
		for(int i = 0; i < pointArray.size(); i++)
		{
			yActual = pointArray[i].second;
			xActual = pointArray[i].first; 
			yPredicted = (slope * xActual) + yIntercept;
			xPredicted = (yActual - yIntercept)/slope;

			residual += (yActual-yPredicted);
			xRes += (xActual-xPredicted);

			residualSquared += pow(residual,2);
			xResSqr += pow(xRes,2);

			yMinusYBar += (yActual-yAvg);
			xMinXBar += (xActual-xAvg);
			yMinusYBarSquared += pow(yMinusYBar, 2);
			xMinXBarSqr += pow(xMinXBar, 2);
		}

		//R^2 = SSR/SST || 1-SSE/SST || SSR/(SSR+SSE)
		rSquared = 1 - (residualSquared/yMinusYBarSquared);
		xRSqr = 1 - (xResSqr/xMinXBarSqr);

		goodnessOfFit = 100 * rSquared; //y intercept line, close to horizontal
		xGOF = 100 * xRSqr; //x intercept line, close to vertical
		
		//Change this to use the formula with the best fit
		//printf("Image Width(pixels): %i\n", WIDTH);
		//printf("Image Height(pixels): %i\n", HEIGHT);
		//printf("y Average: %5.0f\n", yAvg);
		//printf("x Average: %5.0f\n", xAvg);
		//printf("Sum Squared Regression(SSR): %5.2f\n",residualSquared);
		//printf("Total Sum of Squares(SST): %5.2f\n",yMinusYBarSquared);
		printf("X Intercept: %.0f\n", xIntercept);
		printf("Y Intercept: %.0f\n", yIntercept);
		printf("The xLine Fits: %3.0f Percent | R^2: %1.6f\n", xGOF, xRSqr);
		printf("The Line Fits: %3.0f Percent | R^2: %1.6f\n", goodnessOfFit, rSquared);
		printf("The Slope is: %3.4f\n", slope);
		printf("The Angle is: %3.1f\n", angle);
	}

	//this will add a line of best fit to the bmp created
	if(drawLine)
	{
		//Make Line Should be Changed to use xIntercept form when the line slope is close to +-inf
		Pixel lineColor = Pixel(0,255,0); //make the line of best fit green
		makeLine(pixArr, yIntercept, slope, lineColor, (int)1);
	}
	return angle;
};




//change this to not turn on relays if they are already on
void controlSwitch(float curAngle, float desiredAngle, bool& angleAchieved)
{
	bool switchOne, switchTwo;
	float deltaAngle = desiredAngle - curAngle;
	
	//determine what switch if any should be run. That way they cannot alternate back and forth if they overshoot.
	if(deltaAngle > -4 && deltaAngle < 4)
	{
		angleAchieved = true;
		switchOne = false;
		switchTwo = false;
	}
	else if(deltaAngle < 0) 
	{
		printf("\nSwitch One Is On\n");
		switchOne = true;
		switchTwo = false;
	}
	else
	{
		printf("\nSwitch Two Is On\n");
		switchOne = false;
		switchTwo = true;
	}
};





void toBlackAndWhite(imageArr& pixArr, int maxColorLimit, int trimAmount)
{
	for(int y = 0; y < HEIGHT; y++) //HEIGHT
	{
		for(int x = 0; x < WIDTH; x++) //WIDTH
		{
			pixArr[y][x];
			//when all the colors are the same value, a shade of grey will be produced
			int avgColor = ((pixArr[y][x].getRed() + pixArr[y][x].getGreen() + pixArr[y][x].getBlue())/3); // (red + green + blue)/3 = 0-255
			avgColor = avgColor<maxColorLimit?0:255; //remove all colors that don't meet the threshold, and make all other color black
			if(y > trimAmount && x > trimAmount && y < (HEIGHT-trimAmount) && x < (WIDTH-trimAmount)) //trim off 10 pixels around the border
			{
				pixArr[y][x].setRGB(avgColor, avgColor, avgColor);
			}
			else
			{
				pixArr[y][x].setRGB(255, 255, 255);
			}
		}
	}
};






//this should be its own thread
void captureImages (imageArr bmpPixels, float* angle)
{
//		Possibly move this to reduce camera startup
//###################################################################
	//The width must be divisible by 320
	//The height must be diviible by 240
	raspicam::RaspiCam Camera; //Camera object
	//getting seg fault for larger values
	Camera.setFormat(raspicam::RASPICAM_FORMAT_RGB); //can change this to bmp
	Camera.setWidth(WIDTH);
	Camera.setHeight(HEIGHT);
	Camera.setBrightness(50);
	Camera.setISO(800);
	Camera.setSharpness(80);

	//Open camera 
	std::cout<<"Opening Camera..."<<std::endl;
	if ( !Camera.open()) {throw std::runtime_error("!!!Cannot Open Camera!!!");}

//####################################################################

	//wait a while until camera stabilizes
	std::cout<<"Sleeping for 1 secs"<<std::endl;
	usleep(1*1000000);

	//capture
	std::cout<<"grabbing"<<std::endl;
	Camera.grab();

	//allocate memory
	std::cout<<"allocating"<<std::endl;
	unsigned char *data=new unsigned char[  Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB )];

	//extract the image in rgb format
	std::cout << "extracting img in rgb" << std::endl;
	Camera.retrieve ( data ); //get camera image

	std::cout<<"getting length width height"<<std::endl;
	unsigned int length = Camera.getImageBufferSize(); // Header + Image Data + Padding
	unsigned int width = Camera.getWidth();
	unsigned int height = Camera.getHeight();
	
	std::cout << "heightcam: " << height << " |widthcam: " << width << std::endl;
	unsigned char* arrayOfWhat = Camera.getImageBufferData(); //This might be an array of all the colors, but may also include the header

	/*this doesn't appear to work*/
	std::cout<<"releasing camera"<<std::endl;
	Camera.release();

	std::cout<<"getting data"<<std::endl;
	int numOfChars = (height * width * BYTES_PER_PIXEL);
	int xPos = 0;
	int yPos = 0;
	int red, green, blue;

	/*If not passing image array through parameter*/
	//imageArr newImg;
	//newImg.resize(height);
	//for(int i=0; i < height; i++) {newImg[i].resize(width);};

	for(int ct = 0; ct < numOfChars;)
	{
									//ct values at first run
		blue = (int)data[ct++]; 	//ct = 0
		green = (int)data[ct++]; 	//ct = 1
		red = (int)data[ct++]; 		//ct = 2
									//ct = 3

		//adjust pixel values
		bmpPixels[yPos][xPos ] = Pixel(red, green, blue);

		/* next position to place pixel
		there are 3 chars per pixel and height and width is in pixels
		each ct increment reperesent one char, which is one color channel of a pixel
		*/
		yPos = (ct/3)/width;
		xPos = (ct - (((ct/3)/width) * (width*3)))/3;
	}

	//clean up the image and make it black and white
	toBlackAndWhite(bmpPixels, 25);

	*angle = getAngle(bmpPixels, true, true); //returns the angle and takes input of pixel vector, draw line of best fit, and show statistics of line
	
	//create the name for the test image
	std::string strName = "NewImage.bmp";
	char* imageName = &strName[0];

	//create an image of the current capture and put a line of best fit drawn over it
	makeImage(imageName, bmpPixels); //This will save an image of what was just caputured in bmp format
		
	//free resrources    
	delete data;
	//delete imageName;
};





void makeImage (char* imageName, imageArr pixArr) 
{
	unsigned char rawImage[HEIGHT][WIDTH][BYTES_PER_PIXEL];

	for (int i = 0; i < HEIGHT; i++) 
    {
        for (int j = 0; j < WIDTH; j++) 
        {
            int xLoc = WIDTH - (j+1);
            rawImage[i][j][2] = (unsigned char)((pixArr[i][xLoc]).getRed());
            rawImage[i][j][1] = (unsigned char)((pixArr[i][xLoc]).getGreen());
            rawImage[i][j][0] = (unsigned char)((pixArr[i][xLoc]).getBlue());
        }
    }

	generateBitmapImage((unsigned char*) rawImage, imageName);
	printf("Image generated!!");
};


void generateBitmapImage (unsigned char* image, char* imageFileName)
{
    int widthInBytes = WIDTH * BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes) + paddingSize;

    FILE* imageFile = fopen(imageFileName, "wb");

    unsigned char* fileHeader = createBitmapFileHeader(stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader();
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < HEIGHT; i++) 
    {
        fwrite(image + (i*widthInBytes), BYTES_PER_PIXEL, WIDTH, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

unsigned char* createBitmapFileHeader (int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * HEIGHT);

    static unsigned char fileHeader[] = {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize      );
    fileHeader[ 3] = (unsigned char)(fileSize >>  8);
    fileHeader[ 4] = (unsigned char)(fileSize >> 16);
    fileHeader[ 5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader ()
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image WIDTH
        0,0,0,0, /// image HEIGHT
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(WIDTH      );
    infoHeader[ 5] = (unsigned char)(WIDTH >>  8);
    infoHeader[ 6] = (unsigned char)(WIDTH >> 16);
    infoHeader[ 7] = (unsigned char)(WIDTH >> 24);
    infoHeader[ 8] = (unsigned char)(HEIGHT      );
    infoHeader[ 9] = (unsigned char)(HEIGHT >>  8);
    infoHeader[10] = (unsigned char)(HEIGHT >> 16);
    infoHeader[11] = (unsigned char)(HEIGHT >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}






float adjustToAngle()
{
	//hopefully I can use an infrared remote to enter this info
	float angleToAdjustTo;
	std::cout << "What Angle Do You Desire?: ";
	std::cin >> angleToAdjustTo; 
	return angleToAdjustTo;
};