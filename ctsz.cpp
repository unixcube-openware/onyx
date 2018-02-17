
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class tSz
{
public:
    tSz(unsigned long nodecount);
    ~tSz();

    tSz* nex;
};

tSz::tSz()
{
    nex=new tSz(nodecount-1);
    
    printf("created node!\n");
}

tSz::~tSz()
{

}

int main(int argc, char** argv)
{
    tSz one(33);

    return(0);
}
