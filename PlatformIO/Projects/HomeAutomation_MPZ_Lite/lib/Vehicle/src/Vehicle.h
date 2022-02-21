#ifndef Vehicle_h
#define Vehicle_h

/*****************************************************************************************************************************
*   This class is used to create vehicle like objects for the simulation of a "intelligent" crossing                         *                                                        *
*   The Vehicle class inherits from the Bewegung class                                                                       *
******************************************************************************************************************************/

#include <Bewegung.h>
#define NumOfTrafficLights 4           //Array size of traffic_lights
#define NumOfStartPoints 4             //Array size of start_points
//#define NumOfDestinationPoints 3
#define NumOfDestinationPoints 12      //Array size of destination_points
#define NumOfMappingsPerPoint 3        //Defines how much possible destinations a start point has
#define NumOfMappings   12 //per Start Points there are 3 possible destinations (excluding a full turn at the crossing to only switch sides)


#define Min_X 0             //Defines the minimum for the x coordinate
#define Max_X 15000         //Defines the maximum for the x coordinate
#define Min_Y 0             //Defines the minimum for the y coordinate
#define Max_Y 15000         //Defines the maximum for the y coordinate


class Vehicle: public Bewegung
{
    public:
    //Constructor
    Vehicle();

    //Set Methods
    void SetTrafficLights(int arr[NumOfTrafficLights][2]);          //Set the position of the traffic lights 
    void SetNextTrafficLights(int arr[NumOfTrafficLights-1]);      
    //The array "next_traffic_lights" is used to store the order of the traffic lights relativ to a given position
    //See method: "FindNextTrafficLight" !
    void SetCoords(char x_y, int Num);                              //Set coord1 and coord2 to x or y         
    // Num = 1: coord1 = x, coord2 = y      
    // Num = 2: coord1 = y, coord2 = x
    void SetStartPoints(int arr[NumOfStartPoints][2]);              //Set the possible start points
    void SetDestinationPoints(int arr[NumOfDestinationPoints][2]);  //Set the possible destination points
    void SetStartPoint(int Start_Num);                              //start_point is used to store which start position form the array is used
    //this variable is overwritten by the "SetNewPath" method
    void SetDestinationPoint(int Dest_Num);                         //destination_point is used to store which start position form the array is used
    //this variable is overwritten by the "SetNewPath" method

    //Replaced Set Methods
    //These methods form the "Bewegung" class are replaced to fit the "Vehicle" class
    void SetStart(int Num);                 //Choose a start point from the start_points array
    void SetDestination(int Dest_Num);      //Choose a destination point from the start_points array

    //Get Methods
    int GetTrafficLights(char x_y, int Num);        //Read traffic_lights array: Num = row, x,y = column
    int GetNextTrafficLights(int Num);              //Read the next_traffic_lightx variable with x = Num = {0,1,2}
    int GetDeltaTrafficLights(int Num);             //Read the delta_traffic_lights array: Num = row, x,y = column
    char GetCoords(int Num);                        //Read the variable coordx with x = Num = {0,1}
    int GetStartPoints(char x_y, int Num);          //Read the start_points array: Num = row, x,y = column
    int GetDestinationPoints(char x_y, int Num);    //Read the destination_points array: Num = row, x,y = column
    int GetTrafficLightMax(char x_y);               //Read the traffic_light_max_n variable with n = {x, y}    
    int GetTrafficLightMin(char x_y);               //Read the traffic_light_min_n variable with n = {x, y}    
    int GetMapping(int Start_Num, int Dest_Num);    //Read the start_destination_map array: row = Start_Num column = Dest_Num
    int GetStartPoint();                            //Read the start_point variable 
    int GetDestinationPoint();                      //Read the destination_point variable
  
    //Other Methods
    void FindStartPoints();                         
    //This Method calculates the possible start points of a vehicle based on the specified coordinate system (min, max for x and y)
    //and based on the specified turn points 

    void FindNextTrafficLight(int Start_Num);
    //This Method sorts the traffic lights (stored in traffic_lights[][] from nearest to farthest relative to the choosen
    //start point, which is defined by Start_Num (relates to a row of the start_point array).
    //The row number of the sorted traffic lights are stored in the next_traffic_light_n (with n = {0,1,2}) variable. 

    void CalcCoords();
    //This method defines wether the x or the y coordinate directs to the next traffic light
    //The direction towards the next traffic light is used to determine the possible destination points for a given start point

    /*****************************************************************************************************************************
    *   For the next explainations the following defintion is used:                                                              *
    *   First coordinate relates to the coordinate which defines the vertical position (in relation to the street) based on      *
    *  a given start point.                                                                                                      *
    *  Second coordinate relates to the horizontal position based on a given start point                                         *
    *  e.g if you start in the north or south the first coordinate will be 'y' and the second coordinate will be 'x'.            *
    *  But if you start at west most or east most point the first coordinate will be 'x' and the second coordinta will be 'y'    *
    ******************************************************************************************************************************/


    int CalcMaxMinCoord(int TrafficLightNum, int coord_num);
    //This Method is used to determine the first coordinate of a possible destination point
    //In a crossing the first coordinate of a possible destination point is always one coordinate of the traffic light position
    //The value of the first coordinate is determined by "CalcDestinatnionPoints"

    void CalcDestinationPoints(int Start_Num);
    //This Method is used to find the possible destination points for a given start point
    //The idea is that for a vehicle, in a standard crossing, some of the traffic lights hold the first coordinate for
    //the possible destination points.
    //For example:
    //The first possibility is to just drive straight, in that case the nearest traffic light holds the first coordinate of
    //the possible destination point. The second coordinate, which is the coordinate for the direction of travel, is eitehr given by
    //the greatest or lowest point in the coordinate system.
    //The last to possibilities are to turn left or right.
    //For a right turn the first coordinate is given by the second nearest traffic light in the crossing. The second 
    //coordinate is again given by the greatest or lowest point of the coordinate system.
    //For a left turn only the first coordinate is different and in that case given by the first coordinate of the 
    //third nearest traffic light.
    //The method also maps the possible destination for the given start point in the start_destination_map array! 
    
    void MapStartDestinationPoints();
    //This method is used to find all possible destination points for all the defined start points.

    void SetNewPath();
    //This method is used to choose a random start point and a random destination point for the vehicle

    bool CheckDestination();
    //This method is used to check wether the vehicle has reached the destination
    //This can for example be used as trigger for the "SetNewPath" method

    protected:
    int traffic_lights[NumOfTrafficLights][2];
    int next_traffic_lights[NumOfTrafficLights-1];      //-1 since we only care for the first three traffic lights
    int delta_traffic_lights[NumOfTrafficLights];
    int start_points[NumOfStartPoints][2];
    int destination_points[NumOfDestinationPoints][3];  // Columns: x, y, start point mapping
    int start_destination_map[NumOfStartPoints][NumOfMappingsPerPoint+1];
    int map_pos;
    int start_point;                                //stores the row of the start point calculated by SetNewPath
    int destination_point;                          //stores the row of the start point calculated by SetNewPath
    // int next_traffic_light0;                        
    // int next_traffic_light1;
    // int next_traffic_light2;
    char coord1;                                    //Vertical coordinate of the start position in the relation to the street 
    char coord2;                                    //Horizontal coordinate of the start position in the relation to the street
    int traffic_light_max_x;                        //Stores the highest x coordinate value of the given traffic light positions
    int traffic_light_min_x;                        //Stores the lowest x coordinate value of the given traffic light positions
    int traffic_light_max_y;                        //Stores the highest y coordinate value of the given traffic light positions
    int traffic_light_min_y;                        //Stores the lowest y coordinate value of the given traffic light positions

};


#endif