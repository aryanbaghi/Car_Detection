/// Aryan Baghi
/// 90521058


#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <iterator>


using namespace std;
using namespace cv;


class cars     //car class
{
	public:	    
	
	Mat image_input;          //main input image
	Mat image_main_result;    //the final result
	Mat storage;              //introduced to stop detection of same car more than once

	CascadeClassifier cascade;    //the main cascade classifier
	CascadeClassifier checkcascade;        //a test classifier,car detected by both main and test is stated as car

	int num;

	void getimage(Mat src) //getting the input image
    {              
 
        if(! src.data )                             
        {
            cout <<  "src not filled" << endl ;
        }
 
        else
        {
            image_input = src.clone();
			storage = src.clone();             
			image_main_result = src.clone();    
			
            cout << "got image" <<endl;
        }
    }

	
	void cascade_load(string cascade_string)            
	{
		cascade.load(cascade_string);

		if( !cascade.load(cascade_string) )
    	{
        	cout << endl << "Could not load classifier cascade" << endl;
        	
    	}
		else
		{
			
			cout << "cascade : " << cascade_string << " loaded" << endl;
		}

	}


	void checkcascade_load(string checkcascade_string)               //loading the test/check cascade
	{
		checkcascade.load(checkcascade_string);

		if( !checkcascade.load(checkcascade_string) )
    	{
        	cout << endl << "Could not load classifier checkcascade" << endl;
        	
    	}
		else
		{
			cout<< "checkcascade : " << checkcascade_string << " loaded" << endl;
		}
	}

	
	void display_input()             // function to display input
	{
		namedWindow("display_input");
		imshow("display_input",image_input);
		waitKey(0);
	}

	void display_output()            //function to display output
	{
		
		if(!image_main_result.empty() )
        {
			namedWindow("display_output");
			imshow("display_output",image_main_result);
			waitKey(0);
		}
	}

	void setnum()                      
	{
		num = 0;
	}


	void findcars()                 //main function
	{
    	int i = 0;
    	
		Mat img = storage.clone();
		Mat temp;                    
		
		if(img.empty() )
        {
			cout << endl << "detect not successful" << endl;
		}
		int cen_x;                         
		int cen_y;
    	vector<Rect> cars;
    	const static Scalar colors[] =  { CV_RGB(0,0,255),CV_RGB(0,255,0),CV_RGB(255,0,0),CV_RGB(255,255,0),CV_RGB(255,0,255),CV_RGB(0,255,255),CV_RGB(255,255,255),CV_RGB(128,0,0),CV_RGB(0,128,0),CV_RGB(0,0,128),CV_RGB(128,128,128),CV_RGB(0,0,0)};                   
        	
    	Mat gray; 

    	cvtColor( img, gray, CV_BGR2GRAY );

		Mat resize_image(cvRound (img.rows), cvRound(img.cols), CV_8UC1 );		

    	resize( gray, resize_image, resize_image.size(), 0, 0, INTER_LINEAR );
    	equalizeHist( resize_image, resize_image );

    	
    	cascade.detectMultiScale( resize_image, cars,1.1,2,0,Size(10,10));                 
        	
		
		for( vector<Rect>::const_iterator main = cars.begin(); main != cars.end(); main++, i++ )
    	{
       		Mat resize_image_reg_of_interest;
        	vector<Rect> nestedcars;
        	Point center;
        	Scalar color = colors[i%8];
        	
	        	
			//getting points for bouding a rectangle over the car detected by main
			int x0 = cvRound(main->x);                              
			int y0 = cvRound(main->y);
			int x1 = cvRound((main->x + main->width-1));
			int y1 = cvRound((main->y + main->height-1));

		
			
        	if( checkcascade.empty() )
            	continue;
        	resize_image_reg_of_interest = resize_image(*main);
        	checkcascade.detectMultiScale( resize_image_reg_of_interest, nestedcars,1.1,2,0,Size(30,30));
            	
        	for( vector<Rect>::const_iterator sub = nestedcars.begin(); sub != nestedcars.end(); sub++ )      //testing the detected car by main using checkcascade
        	{
           		center.x = cvRound((main->x + sub->x + sub->width*0.5));        //getting center points for bouding a circle over the car detected by checkcascade
				cen_x = center.x;
			   	center.y = cvRound((main->y + sub->y + sub->height*0.5));
				cen_y = center.y;
				if(cen_x>(x0+15) && cen_x<(x1-15) && cen_y>(y0+15) && cen_y<(y1-15))         //if centre of bounding circle is inside the rectangle boundary over a threshold the the car is certified
				{
                								
					rectangle( image_main_result, cvPoint(x0,y0),
                    	   		cvPoint(x1,y1),
                     	  		color, 3, 8, 0);               //detecting boundary rectangle over the final result
					

					
					//masking the detected car to detect second car if present

					Rect region_of_interest = Rect(x0, y0, x1-x0, y1-y0);
					temp = storage(region_of_interest);
					temp = Scalar(255,255,255);

					num = num+1;     //num if number of cars detected
				
				}
			}	
				
		}		
				
                    	   
	if(image_main_result.empty() )
    {
		cout << endl << "result storage not successful" << endl;
	}			
			
    }      
			
};

       

		    	
int main( int argc, const char** argv )
{
		
	double t = 0;
    Mat image1 = imread(argv[1],1);
	Mat image;
	resize(image1,image,Size(300,150),0,0,INTER_LINEAR);        //resizing image to get best experimental results
	cars detectcars;                      //creating a object

	
	string checkcas = argv[2];

	detectcars.getimage(image);           //get the image
	detectcars.setnum();                  //set number of cars detected as 0
	detectcars.checkcascade_load(checkcas);      //load the test cascade
	
	//Applying various cascades for a finer search.
	if(argc > 3)
	{
		for(int i = 3;i<argc;i++)
		{
			string cas = argv[i];			
			detectcars.cascade_load(cas);            
			detectcars.findcars();
		}
	}
	else
	{
		cout << endl << "Please provide atleast one main cascade xml file" << endl;
	}
	
    
	if(detectcars.num!=0)
	{
		cout << endl << detectcars.num << " cars got detected !" << endl << endl;
	}
	else
	{
		cout << endl << "cars not found" << endl;
	}
	
	detectcars.display_output();          //displaying the final result
	
	return 0;
}




	




	
