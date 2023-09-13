#include "include/TEST.h"
#include "include/Event.h"
#include "include/text_input.h"



int main(int argc, char* argv[])
{
    std::string ec("");
    TEST MY_TEST;
    ec = MY_TEST.INIT("TEST TEST",1000,700,RENDERER_ACCELERATED | RENDERER_SYNC);

    if(!ec.empty())
        std::cout<<ec<<"\n";

    while(MY_TEST.is_running())
    {
        MY_TEST.RUN();
    }

    MY_TEST.TERMINATE();
    return 0;
}
