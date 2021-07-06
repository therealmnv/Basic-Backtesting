#include<iostream>
#include<string>
#include<fstream> //TO READ .csv
#include<iomanip> //FOR SETTING FIXED PRECISION (2 DECIMAL PLACES) FOR ANY FLOAT

using namespace std;

float stringToFloat(string s)
{   
    return (float)atof(s.c_str());
}

int getDate(string s) //RETURNS THE CURRENT DAY OF TRADE
{
    return (int)atof(s.substr(8,2).c_str()); //8 IS THE POSITION OF 'DD' IN DATE, 2 IS THE NUMBER OF CHARACTERS
}

string getTime(string s)
{
    return s.substr(11,8); //RETURNS TIME OF TRADE
}

bool isNewDay(int currentDay, int previousDay)
{
    if (currentDay != previousDay)
        return true;
    else
        return false;            
}

float updateCurrentDayHigh(float high, float todaysHigh)
{
    if (high>todaysHigh)
        todaysHigh = high;
    return todaysHigh;
}

float updateCurrentDayLow(float low, float todaysLow)
{
    if (low<todaysLow)
        todaysLow = low;
    return todaysLow;
}

float targetValue(float buyPrice, float previousDayHigh, float previousDayLow)
{
    return buyPrice + (previousDayHigh - previousDayLow);
}

float stopLossValue(float buyPrice, float previousDayHigh, float previousDayLow)
{
    return buyPrice - (previousDayHigh - previousDayLow)/4;
}

void displayDetails(int numberOfTrades, float netProfit)
{
    cout<<"Total number of trades: "<<numberOfTrades<<endl;
    cout<<"\nTotal profit made: "<<netProfit<<endl<<endl;
}

main()
{
    string line,time;

    cout<<setprecision(2)<<fixed; //SETTING PRECISION OF FLOAT VARIABLES TO 2 DECIMAL PLACES

    float open,high,low,close;
 
    float todaysHigh = -1.00f,todaysLow = 1000000.00f; //INITIALIZING todaysHigh AND todaysLow IS REQUIRED FOR...
    //...FIRST ITERATION AS WE NEED A NUMERIC VALUE TO COMPARE IN "updateCurrentDayHigh" FUNCTION
    
    float previousDayHigh,previousDayLow;
    float buyPrice,target,stopLoss;
    
    float netProfit = 0.00f;

    int currentDay,previousDay;
    int numberOfTrades = 0;

    bool tradeActive = false;

     
    ifstream data;
    data.open("NSE_NIFTY, 1Min Data 26oct2020-18jan2021_DatasetForAssignment.csv");

    //GETTING RID OF 5 COLUMN NAMES 
    for(int i=0;i<4;++i)
        getline(data, line, ',');
    getline(data, line, '\n');


    //FOR DAY 1 WE FIND THE DAY HIGH AND DAY LOW 
    getline(data, line, ',');
    previousDay = getDate(line);

    do
    {
        getline(data, line, ','); //open
    
        getline(data, line, ',');
        high = stringToFloat(line);

        getline(data, line, ',');
        low = stringToFloat(line);

        getline(data, line, '\n'); //close

        todaysHigh = updateCurrentDayHigh(high, todaysHigh);
        todaysLow = updateCurrentDayLow(low, todaysLow);

        getline(data, line, ',');
        currentDay = getDate(line);
    
    } while (!isNewDay(currentDay, previousDay));
    
    time = getTime(line);

    //AS NEW DATA COMES IN, *ALL* CURRENT DAY VALUES BECOME PREVIOUS DAY VALUES AND...
    //...CURRENT DAY VALUES ARE *RESET* AND ARE UPDATED AS NEW VALUES COME IN
    previousDay = currentDay; 

    previousDayHigh = todaysHigh;
    previousDayLow = todaysLow;

    todaysHigh = -1.00f,todaysLow = 1000000.00f;

    while(data.good())
    {
        getline(data, line, ',');
        open = stringToFloat(line);
        
        //ACCOUNTING FOR GAP OPENING *DURING BUYING*
        //IF THE MARKET OPENS AT A PRICE GREATER THAN previousHigh AND...
        //...THERE'S NO ACTIVE TRADE, WE BUY AT THE OPEN PRICE!
        if (open>previousDayHigh && tradeActive == false) 
        {   
            buyPrice = open;
            tradeActive = true;
            
            target = targetValue(buyPrice, previousDayHigh, previousDayLow);
            stopLoss = stopLossValue(buyPrice, previousDayHigh, previousDayLow);

            cout<<"ID: "<<numberOfTrades<<" | TIME: "<<time<<" | BOUGHT @"<<buyPrice<<" | TARGET "<<target<<" | STOP LOSS "<<stopLoss<<endl;
        }

        //ACCOUNTING FOR GAP OPENING *DURING SELLING*
        //IF THE MARKET OPENS AT A PRICE MORE THAN target AND...
        //...THERE IS AN ACTIVE TRADE, WE SELL AT THE OPEN PRICE!
        else if (open>=target && tradeActive == true) 
        {
            cout<<"ID: "<<numberOfTrades<<" | TIME: "<<time<<" | SOLD @"<<open<<"   | TRADE PROFIT: "<<(open-buyPrice)<<endl<<endl;

            numberOfTrades+=1;
            tradeActive = false;

            netProfit += (open-buyPrice);
        }

        getline(data, line, ',');
        high = stringToFloat(line);
        
        //*DURING BUYING*, ACCOUNTING FOR OPENING LESS THAN OR EQUAL TO previousDayHigh AND... 
        //...DAY HIGH IS GREATER THAN THE previousDayHigh === WHICH MEANS THAT THE STOCK PRICE WILL MOVE UP AND...
        //...OUR "BUY AT PRICE" (previousDayHigh+0.05) GETS TRIGGERED IF... 
        //...THERE'S NO ACTIVE TRADE!
        if (high>previousDayHigh && open <= previousDayHigh && tradeActive == false)
        {   
            buyPrice = previousDayHigh+0.05; //HERE 0.05 == TICK (ACCORDING TO THE GIVEN SCENARIO)
            tradeActive = true;
            
            target = targetValue(buyPrice, previousDayHigh, previousDayLow);
            stopLoss = stopLossValue(buyPrice, previousDayHigh, previousDayLow);

            cout<<"ID: "<<numberOfTrades<<" | TIME: "<<time<<" | BOUGHT @"<<buyPrice<<" | TARGET "<<target<<" | STOP LOSS "<<stopLoss<<endl;
        }
        
        //IF THE HEIGHEST VALUE REACHED FOR THE MINUTE IS LESS THAN OR EQUAL TO THE TARGET AND...
        //...THERE IS AN ACTIVE TRADE, WE SELL AT THE TARGET PRICE!
        if (high>=target && tradeActive == true) 
        {
            cout<<"ID: "<<numberOfTrades<<" | TIME: "<<time<<" | SOLD @"<<target<<"   | TRADE PROFIT: "<<(target-buyPrice)<<endl<<endl;

            numberOfTrades+=1;
            tradeActive = false;

            netProfit += (target-buyPrice);
        }

        getline(data, line, ',');
        low = stringToFloat(line); 

        //IF THE LOWEST PRICE REACHED FOR THE MINUTE IS LESS THAN OR EQUAL TO THE STOPLOSS AND...
        //...THERE IS AN ACTIVE TRADE, OUR STOPLOSS GETS TRIGGERED!  
        if (low <= stopLoss && tradeActive == true)
        {
            cout<<"ID: "<<numberOfTrades<<" | TIME: "<<time<<" | SOLD @"<<stopLoss<<"   | TRADE PROFIT: "<<(stopLoss-buyPrice)<<endl<<endl;

            numberOfTrades+=1;
            tradeActive = false;

            netProfit += (stopLoss-buyPrice);
        }

        getline(data, line, '\n'); //close

        todaysHigh = updateCurrentDayHigh(high, todaysHigh);
        todaysLow = updateCurrentDayLow(low, todaysLow);

        if(isNewDay(currentDay, previousDay))
        {
            previousDay = currentDay;
            previousDayHigh = todaysHigh;
            previousDayLow = todaysLow; 

            todaysHigh = -1.00f,todaysLow = 1000000.00f; //RESET THE DAY HIGH AND LOW VALUES
        }

        //WE MUST SECURE THE FOLLOWING IN try-catch, AS THE LAST LINE OF THE DATASET HAS NOTHING WRITTEN...
        //...THE getDate FUNCTION CAN NOT PROCESS EMPTY STRINGS, HENCE WE DISPLAY THE FINAL OUTPUT IN THE CATCH BLOCK
        try
        {
            getline(data, line, ',');
            currentDay = getDate(line);
            time = getTime(line);
        }

        catch(const exception& e)
        {
            displayDetails(numberOfTrades, netProfit);
            break;
        }
    }
}
