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
#define BLOCKSIZE 100
struct Tuple
{
    int from;
    int degree;
};
Tuple DegreeList[MAXRECORD];
int Degree[MAXNODE];
static inline int getDegree(int);
//class Node
//{
//    int value;
//    Node* Next;
//};
//class Record
//{
//public:
//    Record* next;
//    int src;
//    Node*
//};    
//class Table
//{
//public:
    
double R[MAXNODE];
double Rt[MAXNODE]={0};
double Beta=0;
int NodeCount=0;
int hash[MAXNODE]={0};
int groupCount=0;
void Prepare(const char *);
static inline double CalcDev()
{
    double dev=0;
    for (int i=0;i<MAXNODE;i++)
    {
        if (hash[i]==0) continue;
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
    char buff[1024];
    round++;
    
    printf("===========Round %d===========\n", round);
    //memset(Rt, 0, sizeof(double)*MAXNODE);
    //Check
    rj_sum=0;
    for (int i=0; i < MAXNODE; i++)
    {
        if (hash[i]==0) continue;
        rj_sum+=R[i];
    }
    printf("Now S=%lf\n", rj_sum);

    //Check end
    for (int i = 0; i < MAXNODE; i++)
    {
        Rt[i]=0.0;
    }

    //Update
    
    sprintf(buff, "R%s%d.dat", (round-1)>10?"":"0", round-1);
    ifstream bi(buff, ios::in|ios::binary);
    cout<<"open "<<buff<<endl;
    int src, dst, deg;
    for (int i = 0; i < groupCount; i++)
    {
        sprintf(buff, "./BlockFiles/Matrix%s%d.txt", i<10?"0":"", i);
        ifstream fi(buff);
        while (!fi.eof())
        {
            fi>>src>>deg;
            //cout<<src<<" "<<deg<<endl;
            //add = Beta*R[src]/(double)deg;
            bi.seekg(src*sizeof(double), ios::beg);
            bi.read((char*)&ri, sizeof(double));
            //cout<<"ri="<<ri<<endl;
            
            add = Beta*ri/(double)deg;
            
            while (1)
            {
                fi>>dst;
                if (dst==-1) break;
                Rt[dst]+=add;
                //cout<<"dst "<<dst<<"+="<<add<<endl;
            }
        }
        fi.close();
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
    printf("Dev: %f\n", Dev); 
    cout<<"R and Rt: ";
    for (int i=0; i < 5; i++)
    {
        cout<<R[i]<<"->";
        cout<<Rt[i]<<" ";

    }
    cout<<endl;
    if (Dev<STOPLOSS)
    {
        cout<<"Power Iteration Complete!\n";
        return 0;
    }

    
    sprintf(buff, "R%s%d.dat", (round)>10?"":"0", round);
    ofstream fo(buff, ios::out|ios::binary|ios::trunc);
    cout<<"open "<<buff<<endl;
    for (int i=0; i < MAXNODE; i++)
    {
        fo.write((char*)&Rt[i], sizeof(Rt[i]));
    }
    memcpy(R, Rt, sizeof(double)*MAXNODE);
    fo.close();
    
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
    double S=0;
    for (int i=0;i<MAXNODE;i++)
    {
        if (hash[i]==0) continue;
        N[it].value=R[i];
        N[it].idx=i;
        it++;
        S+=R[i];
        //printf("Get %dth Node %d, value %lf\n", it, i, R[i]);
    }
    printf("Print Check: S = %f\n", S);
    ofstream fo("./Results/FullResult.txt", ios::out|ios::trunc);
    for (int i = 0; i < NodeCount; i++)
    {
        if (hash[i]==0) continue;
        fo<<N[i].idx<<"\t"<<N[i].value<<endl;
    }
    fo.close();
    /*
    ofstream fs("./Results/SortList.txt", ios::out|ios::trunc);
    for (int i = 0; i < NodeCount; i++)
    {
        fs<<N[i].idx<<" "<<N[i].value<<endl;
    }
    fs.close();
    */
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
    Beta=argc>2?atof(argv[2]):0.85;
    const char* path = argc>1?argv[1]:"./Data/WikiData.txt";
    system("mkdir Results");
    cout<<"DataSet path:"<<path<<endl;
    cout<<"Beta value:"<<Beta<<endl;
    cout<<"==========Initializing========\n";
    int from, to;
    int pre=-1;
    int i=-1;
    cout<<"Opening Dataset.txt...";
    ifstream fin(path);
    cout<<(fin?"Success":"Cannot open file!")<<endl;
    while (!fin.eof())
    {
        fin>>from>>to;
        if (from==0&&to==0) break;
        hash[from]=1;
        hash[to]=1;
        if (from!=pre)
        {
            i++;
            DegreeList[i].from=from;
            pre=from;
        }
        DegreeList[i].degree++;
    }
    fin.close();
    for(i=0;i<MAXNODE;i++)
    {
        if(hash[i]==1) NodeCount++;
    }
    cout<<"NodeCount="<<NodeCount<<endl;
#ifdef DUMPM
    //ofstream fo("MatrixDump.txt", ios::trunc|ios::out);
    FILE* fo=fopen("Degree.txt", "w");
    for (int i = 0; i < MAXRECORD; i++)
    {
        fprintf(fo, "%d\t%d\n", DegreeList[i].from, DegreeList[i].degree);
    }
    //fo.close();
    fclose(fo);
#endif
    Prepare(path);

    //exit(-22);

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
    cout<<"Checking R[i]="<<R[30]<<endl;
    ofstream fo1("R00.dat", ios::out|ios::binary|ios::trunc);
    for (int i=0; i < MAXNODE; i++)
    {
        //cout<<"i"<<i<<" Rt[i]"<<R[i]<<endl;
        fo1.write((char*)&R[i], sizeof(R[i]));
    }
    fo1.close();
    UpdateR();

    cout << "Checking...\n";
    double S=0.0;
    for (int i=0; i < MAXNODE; i++)
    {
        if (hash[i]==1)
            S+=R[i];
    }
    cout<<"S="<<S<<endl;
    SortAndPrint();
    
    return 0;

}

void Prepare(const char* dataFile)
{
    cout<<"====Block-Stripe PageRank Algorithm====\n";
    cout<<"Preparing...\n";
    cout<<"Dataset: "<<dataFile<<endl;
    cout<<"Opening Dataset...";
    ifstream fin(dataFile);
    if(!fin)
    {
        cout<<"Fail, Dataset path is not correct!"<<endl;
        exit(-1);
    }
    cout<<"Success"<<endl;
    groupCount=MAXNODE/BLOCKSIZE + 1;
    cout<<"MAXNODE="<<MAXNODE<<endl;
    cout<<"BLOCKSIZE="<<BLOCKSIZE<<endl;
    cout<<"GroupCount="<<groupCount<<endl;
    // Create output files
    char buff[1024];
    fstream *fo=new fstream[groupCount];
    fstream *fi=new fstream[groupCount];
    int i;
    system("mkdir BlockFiles");
    cout<<"Creating Files...";
    for (i = 0; i < groupCount; i++)
    {
        sprintf(buff, "./BlockFiles/Block%s%d.txt", i<10?"0":"", i);
        fo[i].open(buff, ios::out|ios::trunc|ios::in);
        if(!fo[i])
        {
            cout<<"Open Block file %d fail."<<i<<endl;
            exit(-1);
        }
        sprintf(buff, "./BlockFiles/Matrix%s%d.txt", i<10?"0":"", i);
        fi[i].open(buff, ios::out|ios::trunc|ios::in);
        if(!fi[i])
        {
            cout<<"Open Matrix file %d fail."<<i<<endl;
            exit(-1);
        }
    }
    cout<<"Success"<<endl;
    
        
    int src, dst;
    int n=0;
    cout<<"Dividing...";
    while (!fin.eof())
    {
        fin>>src>>dst;
        if (src==0&&dst==0) break;
        n=dst/BLOCKSIZE;// Find the group
        fo[n]<<src<<" "<<dst<<endl;
    }
    cout<<"Success"<<endl;
    // Form Matrix
    cout<<"Forming Matrix...";
    int pre;
    for (i = 0; i < groupCount; i++)
    {
        fo[i].seekg(0, ios::beg);
        pre=-1;
        while (!fo[i].eof())
        {
            fo[i]>>src>>dst;
            //cout<<fo[i].tellp()<<endl;
            if (fo[i].tellp()<0) break;
            if (src==0||dst==0)break;
            //cout<<src<<" "<<dst<<endl;
            if (src!=pre)
            {
                pre=src;
                if (fi[i].tellp()>0) fi[i]<<" -1"<<endl;//Sign of end
                fi[i]<<src<<" "<<getDegree(src);
            }
            fi[i]<<" "<<dst;
        }
        fi[i]<<" -1";//marking the end of file
        //fi[i]<<src<<" "<<getDegree(src);
        fo[i].close();
        fi[i].close();
    }
    
}

static inline int getDegree(int idx)
{
    int i;
    for (i = 0; i < MAXRECORD; i++)
    {   
        if (DegreeList[i].from==idx) break;
    }
    return DegreeList[i].degree;
}
