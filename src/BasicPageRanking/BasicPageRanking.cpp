#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace std;

#define MAXRECORD 6111
#define MAXNODE 8298
#define STOPLOSS 0.00000001
struct Tuple
{
    int from;
    int degree;
    int toList[894];
};
Tuple M[MAXRECORD]={0,0,{0}};
double R[MAXNODE]={1.0/MAXNODE};
double Rt[MAXNODE]={0};
double Beta=0;
static inline double CalcDev()
{
    double dev=0;
    for (int i=0;i<MAXNODE;i++)
    {
        //cout<<"abs:"<<(double)(R[i]-Rt[i])<<endl;
        dev+=(double)fabs(R[i]-Rt[i]);
    }
    cout<<"DDDeeevvv in func: "<<dev<<endl;
    return dev;
}

int UpdateR()
{
    static int round=0;
    double ri=0, rj_sum=0, add=0;
    int degree;
    round++;
    
    printf("===========Round %d===========\n", round);
    memset(Rt, 0, sizeof(double)*MAXNODE);
    for (int i = 0; i < MAXRECORD; i++)
    {
        ri = M[i].from;
        add = R[i]/M[i].degree;
        add *= Beta;
        for (int j = 0; j < M[i].degree; j++)
        {
            
            Rt[M[i].toList[j]]+=add;
        }
    }
    // Insert Leaked PageRank
    rj_sum=0;
    for (int i=0; i < MAXNODE; i++)
    {
        rj_sum+=Rt[i];
    }
    double Leak=(1-rj_sum)/MAXNODE;
    printf("Round %d LeakPageRank %f\n", round, Leak);
    for (int i=0; i < MAXNODE; i++)
    {
        Rt[i]+=Leak;
    }
    double Dev = CalcDev();
    printf("Dev: %f\n", Dev); 
    cout<<"R and Rt: ";
    for (int i=0; i < 50; i++)
    {
        cout<<R[i]<<"-";
        cout<<Rt[i]<<" ";
    }
    cout<<endl;
    if (Dev<STOPLOSS)
    {
        cout<<"Power Iteration Complete!\n";
        return 0;
    }
    memcpy(R, Rt, sizeof(double)*MAXNODE);
    UpdateR();
    return 0;
}

int main(int argc, char* argv[])
{
    //TODO check argc
    Beta=atof(argv[2]);
    cout<<"DataSet path:"<<argv[1]<<endl;
    cout<<"Beta value:"<<Beta<<endl;
    int from, to;
    int pre=-1;
    ifstream fin(argv[1]);
    int i=-1;
    while (!fin.eof())
    {
        fin>>from>>to;
        if (from!=pre)
        {
            i++;
            cout<<from<<endl;
            M[i].from=from;
            pre=from;
        }
        M[i].toList[M[i].degree++]=to;
        //cout<<M[i].degree;
        //cout<<"  "<<from<<"-->"<<to<<endl;
        

    }
    cout<<"wow"<<M[0].from<<"degree"<<M[0].degree<<"to0"<<M[0].toList[0]<<endl;
    cout<<"RRRRRR"<<R[10]<<"2222"<<1.0/MAXNODE<<endl;
    printf("test %lf\n", 1/9000);
    
    for (int i=0; i < MAXNODE; i++)
    {
        R[i]=1.0/MAXNODE;
    }
    UpdateR();
    cout << "Checking...\n";
    double S=0;
    for (int i=0; i < MAXNODE; i++)
    {
        S+=R[i];
    }
    cout<<"S="<<S<<endl;
    
    return 0;

}
