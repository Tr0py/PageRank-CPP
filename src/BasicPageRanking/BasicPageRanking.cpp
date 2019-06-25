#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace std;

#define MAXRECORD 6110
#define MAXNODE 8298
#define STOPLOSS 1e-10
#define DUMPM
struct Tuple
{
    int from;
    int degree;
    int toList[894];
};
Tuple M[MAXRECORD]={0,0,{0}};
double R[MAXNODE];
double Rt[MAXNODE]={0};
double Beta=0;
int NodeCount=0;
int hash[MAXNODE]={0};
static inline double CalcDev()
{
    double dev=0;
    for (int i=0;i<MAXNODE;i++)
    {
        if (hash[i]==0) continue;
        //cout<<"abs:"<<(double)(R[i]-Rt[i])<<endl;
        dev+=(double)fabs(R[i]-Rt[i]);
    }
    //cout<<"DDDeeevvv in func: "<<dev<<endl;
    return dev;
}

int UpdateR()
{
    static int round=0;
    double ri=0, rj_sum=0, add=0;
    int degree;
    round++;
    
    printf("===========Round %d===========\n", round);
    //memset(Rt, 0, sizeof(double)*MAXNODE);
    //Check
    rj_sum=0;
    for (int i=0; i < MAXNODE; i++)
    {
        if (hash[i]==0) continue;
        rj_sum+=Rt[i];
    }
    //printf("Now S=%lf\n", rj_sum);

    //Check end
    for (int i = 0; i < MAXNODE; i++)
    {
        Rt[i]=0.0;
    }
    for (int i = 0; i < MAXRECORD; i++)
    {
        ri = M[i].from;
        add = R[(int)ri]/(double)M[i].degree;
        add *= Beta;
        //cout<<"ri="<<ri<<" R[i]="<<R[(int)ri]<<" degree="<<M[i].degree<<" Beta="<<Beta<<" hash[i]="<<hash[M[i].from]<<endl;
        for (int j = 0; j < M[i].degree; j++)
        {
            //cout<<"to: "<<M[i].toList[j]<<"+="<<add<<endl;
            
            Rt[M[i].toList[j]]+=add;
        }
    }
    // Insert Leaked PageRank
    rj_sum=0;
    for (int i=0; i < MAXNODE; i++)
    {
        if (hash[i]==0) continue;
        rj_sum+=Rt[i];
    }
    double Leak=(1-rj_sum);
    printf("Round %d LeakPageRank %f\n", round, Leak);
    Leak/=(double)NodeCount;
    for (int i=0; i < MAXNODE; i++)
    {
        if (hash[i]==0) continue;
        Rt[i]+=Leak;
    }
    rj_sum=0;
    for (int i=0; i < MAXNODE; i++)
    {
        if (hash[i]==0) continue;
        rj_sum+=Rt[i];
    }
    printf("Now S=%lf\n", rj_sum);
    double Dev = CalcDev();
    cout<<"Deviation: "<<Dev;
    //cout<<"R and Rt: ";
    //for (int i=0; i < 5; i++)
    //{
    //    cout<<R[i]<<"->";
    //    cout<<Rt[i]<<" ";

    //}
    //printf("4037 R=%lf\n", R[4037]);
    if (Dev<STOPLOSS)
    {
        cout<<"Power Iteration Complete!\n";
        return 0;
    }
    cout<<" < Expected Deviation("<<STOPLOSS<<"), continue..."<<endl;
    memcpy(R, Rt, sizeof(double)*MAXNODE);
    //exit(-1);
    UpdateR();
    return 0;
}
struct Nd
{
    double value;
    int idx;
};
void swapNd(Nd& a, Nd&b)
{
    Nd tmp;
    tmp.value=a.value;
    tmp.idx=a.idx;
    a.value=b.value;
    a.idx=b.idx;
    b.value=tmp.value;
    b.idx=tmp.idx;
}

void SortAndPrint()
{
    Nd *N=new Nd[NodeCount+1];
    Nd tmp;
    int it=0;
    for (int i=0;i<MAXNODE;i++)
    {
        if (hash[i]==0) continue;
        N[it].value=R[i];
        N[it].idx=i;
        it++;
        //printf("Get %dth Node %d, value %lf\n", it, i, R[i]);
    }
    ofstream fo("./Results/FullResult.txt", ios::out|ios::trunc);
    for (int i = 0; i < NodeCount; i++)
    {
        if (hash[i]==0) continue;
        fo<<N[i].idx<<"\t"<<N[i].value<<endl;
    }
    fo.close();
    for (int i=0;i<NodeCount;i++)
    {
        for (int j=i+1;j<NodeCount;j++)
        {
            if (N[i].value<N[j].value)
            {
                swapNd(N[i], N[j]);
            }
        }
    }
    ofstream f100("./Results/Top100.txt", ios::out|ios::trunc);
    for (int i = 0; i < 100; i++)
    {
        f100<<N[i].idx<<"\t"<<N[i].value<<endl;
    }
    f100.close();
}


int main(int argc, char* argv[])
{
    //TODO check argc
    Beta=argc>2?atof(argv[2]):0.85;
    const char* defaultPath=argc>1?argv[1]:"./Data/WikiData.txt";
    system("mkdir Results");
    
    cout<<"DataSet path:"<<defaultPath<<endl;
    cout<<"Beta value:"<<Beta<<endl;
    int from, to;
    int pre=-1;
    cout<<"==========Initializing========\n";
    cout<<"Opening Dataset.txt...";
    ifstream fin(defaultPath);
    cout<<(fin?"Success":"Cannot open file!")<<endl;
    int i=-1;
    while (!fin.eof())
    {
        fin>>from>>to;
        if (from==0&&to==0) break;
        hash[from]=1;
        hash[to]=1;
        if (from!=pre)
        {
            i++;
            M[i].from=from;
            pre=from;
        }
        M[i].toList[M[i].degree++]=to;
    }
    fin.close();
    for(i=0;i<MAXNODE;i++)
    {
        if(hash[i]==1) NodeCount++;
    }
    cout<<"NodeCount="<<NodeCount<<endl;
#ifdef DUMPM
    //ofstream fo("MatrixDump.txt", ios::trunc|ios::out);
    FILE* fo=fopen("MatrixDump.txt", "w");
    for (int i = 0; i < MAXRECORD; i++)
    {
        fprintf(fo, "i=%d\t%d\t%d\t", i, M[i].from, M[i].degree);
        for (int j = 0; j < M[i].degree; j++)
        {
            fprintf(fo, "%d ",M[i].toList[j]);
        }
        fprintf(fo, "\n");
    }
    //fo.close();
    fclose(fo);
#endif
    cout<<"Initializing variables...";
    for (int i=0; i < MAXNODE; i++)
    {
        if (hash[i]==1)
            R[i]=1.0/NodeCount;
        else
            R[i]=0;
        //cout<<"I:"<<i<<" "<<R[i]<<endl;
    }
    cout<<"Success\n";
    cout<<"Checking R[i]="<<R[0]<<endl;
    UpdateR();
    cout << "Checking...\n";
    double S=0;
    for (int i=0; i < MAXNODE; i++)
    {
        if (hash[i]==1)
            S+=R[i];
    }
    cout<<"S="<<S<<endl;
    SortAndPrint();
    
    return 0;

}
