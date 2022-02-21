#ifndef Pedestrian_h
#define Pedestrian_h

#include <Bewegung.h>

#define NumOfStartPoints 2*NumOfTurnPoints  //For every turn point there are 2 possible start points in a standard crossing

#define Min_X 0
#define Max_X 15000
#define Min_Y 0
#define Max_Y 15000

class Pedestrian: public Bewegung
{
    public:
    //Constructor
    Pedestrian();

    //Set Methods
    void SetStartPoints(char x_y, int Num, int value);      //Modify the values in the start_points array

    //Get Methods
    int GetStartPoints(char x_y, int Num);                  //Read the valus in the start_points array
    int GetStartArrPos();                                   //Read the row number for the start point (is set by the SetNewPath() method)
    int GetDestinationArrPos();                             //Read the row number for the destination point (is set by the SetNewPath() method)

    //Other Methods
    void FindStartPoints();                                 //Method to find all possible start points (which equal all possible destination points)
    int CalcMaxMinCoord(char x_y, int Turn_Num);            
    //This method is used by the FindStartPoints method. Since a turn point always connects two of the possible start points (in a standard crossing)
    //every turn point holds one coordinate for two of the possible start points. The second coordinate must then either be the lower or the upper 
    //bound of the defined coordinate system
    //The CalcMaxMinCoord method decides wether it is the lower or the upper bound and then returns the value for the lower or upper bound.

    void SetNewPath();                                      //This method chooses a random combination of the start and destination points
    bool CheckDestination();                                //This method returns true if the object has readched its destination, otherwise false will be returned

    protected:
    int start_points[NumOfStartPoints][2];                  //Array to store the possible start points (which equal the possible destination points)
    int start_point;                                        //Stores the row number of the start point chosen by the SetNewPath method 
    int destination_point;                                  //Stores the row number of the destination point chosen by the SetNewPath method 
};  

#endif