#include <iostream>
#include <fstream>
using namespace std;

int singlemax = 0;


int main()
{
    int from, to;
    int count=0, UniqueCount=0;
    int pre=-1;
    int MinNode=0xFFFF, MaxNode=-0xFFFF;
    int MaxList=0, SingleList=0;
    ifstream fin("WikiData.txt");
    while (!fin.eof())
    {
        fin>>from>>to;
        if(from!=pre)
        {
            cout<<"new From:"<<from<<endl;
            pre=from;
            UniqueCount++;
            MaxList=SingleList>MaxList?SingleList:MaxList;
            SingleList=1;
        }
        else
        {
            SingleList++;
        }
        MinNode = from<MinNode?from:MinNode;
        MinNode = to<MinNode?to:MinNode;
        MaxNode = from>MaxNode?from:MaxNode;
        MaxNode = to>MaxNode?to:MaxNode;
        count++;
        
    }    
    cout<<"total record count="<<count<<endl;
    cout<<"Unique from count="<<UniqueCount<<endl;
    cout<<"MinNode="<<MinNode<<"\tMaxNode="<<MaxNode<<endl;
    cout<<"MaxList="<<MaxList<<endl;
    

   
    return 0;

}
