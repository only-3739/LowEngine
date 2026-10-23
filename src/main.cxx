#include <Core/LifeCycle.h>

int main() {

    Begin();
    Start();
    while(Loop());
    Quit();
    End();
    
}
